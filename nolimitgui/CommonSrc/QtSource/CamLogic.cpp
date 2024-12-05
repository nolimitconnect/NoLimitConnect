//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "CamLogic.h"
#include "AppCommon.h"

#include "GuiHelpers.h"
#include "GuiParams.h"

#include <QMediaDevices>
#include <QAudioInput>
#include <QAudioOutput>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

//============================================================================
CamLogic::CamLogic( AppCommon& myApp )
    : QWidget(&myApp)
    , m_MyApp( myApp )
    , m_VideoSinkGrabber( this )
    , m_VideoFrameProcessor( myApp, this )
{
    memset( m_WantCamInput, 0, sizeof( m_WantCamInput ) );
    connect( &m_VideoSinkGrabber, SIGNAL(videoFrameChanged(const QVideoFrame&)), &m_VideoFrameProcessor, SLOT(slotVideoFrameChanged(const QVideoFrame&)) );
}

//============================================================================
void CamLogic::camLogicStartup( void )
{
    m_StartupWasCompleted = true;
    initializeCam();
}

//============================================================================
void CamLogic::toGuiWantVideoCapture( EAppModule appModule, bool wantVidCapture )
{
    m_WantCamInput[appModule] = wantVidCapture;

    if( getCamStartupCompleted() )
    {
        bool wasRunning = isCamCaptureRunning();
        bool isRunning = false;
        for( int i = 0; i < eMaxAppModule; i++ )
        {
            if( m_WantCamInput[i] )
            {
                isRunning = true;
                break;
            }
        }

        if( wasRunning != isRunning )
        {
            cameraEnable( isRunning );
            m_CamIsStarted = isCamCaptureRunning();
        }
    }

    LogModule( eLogWebCam, LOG_INFO, "CamLogic::%s %s wantCapture %d cam running ? %d", __func__,
               DescribeAppModule(appModule), wantVidCapture, m_CamIsStarted );
}

//============================================================================
bool CamLogic::isCamAvailable( void )
{
    if( getCamStartupCompleted() )
    {
        if( !GuiHelpers::requestPermission( QLatin1String("android.permission.CAMERA") ) )
        {
            QMessageBox( QMessageBox::Information, QObject::tr( "Camera Permission" ), QObject::tr( "Cannot use camera without user permission" ), QMessageBox::Ok );
            return false;
        }

        return !m_camera.isNull() && m_camera->isAvailable();
    }

    return false;
}

//============================================================================
bool CamLogic::updateCamAvailable( void )
{
    if( getCamStartupCompleted() && isCamAvailable() )
    {
        // force update.. a camera device may have become available
        if( m_CamIsStarted )
        {
            stopCamera();
        }
        else
        {
            setCamera( QMediaDevices::defaultVideoInput() );
        }

        if( !m_CamInitiated )
        {
            m_CamInitiated = true;
        }

        return !m_camera.isNull() && m_camera->isAvailable();
    }

    return false;
}

//============================================================================
bool CamLogic::isCamCaptureRunning( void )
{
    if( getCamStartupCompleted() )
    {
        return m_CamIsStarted && !m_camera.isNull() && m_camera->isAvailable();
    }

    return false;
}

//============================================================================
void CamLogic::cameraEnable( bool wantVidCapture )
{
    if( getCamStartupCompleted() )
    {
        if( wantVidCapture != m_CamIsStarted )
        {
            if( wantVidCapture )
            {
                startCamera();
            }
            else
            {
                stopCamera();
            }
        }
    }
}

//============================================================================
bool CamLogic::initializeCam( void )
{
    int startTime = GetApplicationAliveMs();
    LogMsg( LOG_DEBUG, "CamLogic::%s begin at %d", __func__, startTime );
    if( VxIsAppShuttingDown() )
    {
        if( m_CamIsStarted )
        {
            stopCamera();
        }

        return false;
    }

    bool isRunning = false;
    bool isValid = true;
    if( getCamStartupCompleted() )
    {
        if( !m_CamInitiated )
        {
            setCamera( QMediaDevices::defaultVideoInput() );
        }

        // there may have been want cam calls before was initialized
        bool wasRunning = isCamCaptureRunning();

        for( int i = 0; i < eMaxAppModule; i++ )
        {
            if( m_WantCamInput[i] )
            {
                isRunning = true;
                break;
            }
        }

        if( wasRunning != isRunning )
        {
            cameraEnable( isRunning );
            m_CamIsStarted = isCamCaptureRunning();
        }

        isValid = !m_camera.isNull() && m_camera->isAvailable();
    }
    
    int endTime = GetApplicationAliveMs();
    LogMsg( LOG_DEBUG, " CamLogic::%s took %d ms at %d", __func__, endTime - startTime, endTime );

    return isValid;
}

//============================================================================
bool CamLogic::getCamStartupCompleted( void )                          
{ 
    if( VxIsAppShuttingDown() )
    {
        if( m_CamIsStarted )
        {
            stopCamera();
        }

        return false;
    }

    return m_StartupWasCompleted; 
}

//============================================================================
void CamLogic::setCamera( const QCameraDevice& cameraDevice )
{
    int timeStart = GetApplicationAliveMs();
    m_DesiredFrameSize = GuiParams::getSnapshotDesiredSize();
    bool isStarted = m_CamIsStarted;
    cameraEnable( false );
    m_camera.reset( new QCamera( cameraDevice ) );
    m_captureSession.setCamera( m_camera.data() );

    connect( m_camera.data(), &QCamera::activeChanged, this, &CamLogic::updateCameraActive );
    connect( m_camera.data(), &QCamera::errorOccurred, this, &CamLogic::displayCameraError );

    m_mediaRecorder.reset( new QMediaRecorder );
    m_captureSession.setRecorder( m_mediaRecorder.data() );
    connect( m_mediaRecorder.data(), &QMediaRecorder::recorderStateChanged, this, &CamLogic::updateRecorderState );


    m_imageCapture = new QImageCapture( m_mediaRecorder.data() );
    m_captureSession.setImageCapture( m_imageCapture );

    connect( m_mediaRecorder.data(), &QMediaRecorder::durationChanged, this, &CamLogic::updateRecordTime );
    connect( m_mediaRecorder.data(), &QMediaRecorder::errorChanged, this, &CamLogic::displayRecorderError );

    m_captureSession.setVideoSink(&m_VideoSinkGrabber);

    updateCameraActive( m_camera->isActive() );
    updateRecorderState( m_mediaRecorder->recorderState() );

    connect( m_imageCapture, &QImageCapture::errorOccurred, this, &CamLogic::displayCaptureError );

    readyForCapture( m_imageCapture->isReadyForCapture() );

    selectVideoFormat( cameraDevice );

    if( m_captureSession.audioInput() )
    {
        m_captureSession.audioInput()->setMuted(true);
    }

    if( m_captureSession.audioOutput() )
    {
        m_captureSession.audioOutput()->setMuted(true);
    }

    m_CamDescription = cameraDevice.description();
    emit signalCameraDescription( m_CamDescription );

    m_CamInitiated = true;
    if( isStarted )
    {
        cameraEnable( true );
    }

    int timeEnd = GetApplicationAliveMs();
    LogModule( eLogWebCam, LOG_DEBUG, "%s took %d ms at %d", __func__, timeEnd - timeStart, timeEnd );
}

//============================================================================
void CamLogic::selectVideoFormat( const QCameraDevice& cameraDevice )
{
    if( m_camera->cameraFormat().isNull() ) 
    {
        // Setting default settings.
        // The biggest resolution and the max framerate
        QSize targetSize( GuiParams::getSnapshotDesiredSize() );
        auto formats = cameraDevice.videoFormats();
        if( !formats.isEmpty() ) 
        {
            auto defaultFormat = formats.first();
            bool defaultFormatInvalid = true;
            LogModule( eLogWebCam, LOG_VERBOSE, "Default camera resolution w %d h %d min fps %3.1f max fps %3.1f", defaultFormat.resolution().width(),
                defaultFormat.resolution().height(), defaultFormat.minFrameRate(), defaultFormat.maxFrameRate() );
            if( defaultFormat.resolution().width() >= targetSize.width() && defaultFormat.resolution().height() >= targetSize.height() )
            {
                defaultFormatInvalid = false;
            }

            int formatNum = 0;

            for( const auto& format : formats ) 
            {
                formatNum++;
                //LogMsg( LOG_VERBOSE, "Format %d camera resolution w %d h %d min fps %3.1f max fps %3.1f", formatNum, format.resolution().width(),
                //    format.resolution().height(), format.minFrameRate(), format.maxFrameRate() );
                if( format.resolution().width() >= targetSize.width() && format.resolution().height() >= targetSize.height() )
                {
                    if( defaultFormatInvalid || ( format.resolution().width() - targetSize.width() < defaultFormat.resolution().width() - targetSize.width() )
                        || ( format.resolution().height() - targetSize.height() < defaultFormat.resolution().height() - targetSize.height() ) )
                    {
                        //LogMsg( LOG_VERBOSE, "Found better camera resolution %d w %d h %d min fps %3.1f max fps %3.1f", formatNum, format.resolution().width(),
                        //    format.resolution().height(), format.minFrameRate(), format.maxFrameRate() );
                        defaultFormat = format;
                        if( defaultFormat.resolution().width() >= targetSize.width() && defaultFormat.resolution().height() >= targetSize.height() )
                        {
                            defaultFormatInvalid = false;
                       }
                    }
                }
            }

            float desiredFps = 1000 / ( VideoFrameProcessor::CAM_SNAPSHOT_INTERVAL_MS / 2 ); // request frame rate toughly twice as fast a snapshot interval
#if defined(TARGET_OS_ANDROID)
            if( desiredFps > 15 )
            {
                // android devices may have trouble keeping up
                desiredFps = 15;
            }
#endif // defined(TARGET_OS_ANDROID)
            if( desiredFps > defaultFormat.maxFrameRate() )
            {
                desiredFps = defaultFormat.maxFrameRate();
            }

            LogMsg( LOG_VERBOSE, "Setting Format %d resolution w %d h %d min fps %3.1f max fps %3.1f desired fps %3.1f", formatNum, defaultFormat.resolution().width(),
                defaultFormat.resolution().height(), defaultFormat.minFrameRate(), defaultFormat.maxFrameRate(), desiredFps);

            m_camera->setCameraFormat( defaultFormat );

            if( desiredFps >= defaultFormat.minFrameRate() && desiredFps <= defaultFormat.maxFrameRate() )
            {
                m_mediaRecorder->setVideoFrameRate( desiredFps );
            }
            else if( desiredFps >= defaultFormat.maxFrameRate() )
            {
                m_mediaRecorder->setVideoFrameRate( defaultFormat.maxFrameRate() );
            }
            else if( desiredFps <= defaultFormat.minFrameRate()  )
            {
                m_mediaRecorder->setVideoFrameRate( defaultFormat.minFrameRate() );
            }
        }
    }
}

//============================================================================
void CamLogic::keyPressEvent( QKeyEvent * event )
{
    if( event->isAutoRepeat() )
        return;

    switch( event->key() ) {
    case Qt::Key_CameraFocus:
        //displayViewfinder();
        //m_camera->searchAndLock();
        event->accept();
        break;
    case Qt::Key_Camera:
        //if( m_camera->captureMode() == QCamera::CaptureStillImage ) 
        //{
        //    slotTakeSnapshot();
        //}
        event->accept();
        break;
    default:
        QWidget::keyPressEvent( event );
    }
}

//============================================================================
void CamLogic::keyReleaseEvent( QKeyEvent *event )
{
    if( event->isAutoRepeat() )
        return;

    switch( event->key() ) {
    case Qt::Key_CameraFocus:
        //m_camera->unlock();
        break;
    default:
        QWidget::keyReleaseEvent( event );
    }
}

//============================================================================
void CamLogic::updateRecordTime()
{
    //QString str = QString( "Recorded %1 sec" ).arg( m_mediaRecorder->duration() / 1000 );
    //ui->statusbar->showMessage(str);
}

//============================================================================
void CamLogic::record()
{
    //m_mediaRecorder->record();
    //updateRecordTime();
}

//============================================================================
void CamLogic::pause()
{
//    m_mediaRecorder->pause();
}

//============================================================================
void CamLogic::stop()
{
//    m_mediaRecorder->stop();
}

//============================================================================
void CamLogic::setMuted( bool muted )
{
//    m_mediaRecorder->setMuted( muted );
}

//============================================================================
void CamLogic::updateCameraActive( bool active )
{
    if( active ) 
    {
        LogMsg( LOG_VERBOSE, "updateCameraActive is active " );
    }
    else 
    {
        LogMsg( LOG_VERBOSE, "updateCameraActive is NOT active " );
    }
}

//============================================================================
void CamLogic::updateRecorderState( QMediaRecorder::RecorderState state )
{
    switch( state ) 
    {
    case QMediaRecorder::StoppedState:
        LogMsg( LOG_VERBOSE, "QMediaRecorder stopped " );
        break;
    case QMediaRecorder::PausedState:
        LogMsg( LOG_VERBOSE, "QMediaRecorder paused " );
        break;
    case QMediaRecorder::RecordingState:
        LogMsg( LOG_VERBOSE, "QMediaRecorder recording " );
        break;
    }
}

//============================================================================
void CamLogic::displayCaptureError( int id, const QImageCapture::Error error, const QString& errorString )
{
    Q_UNUSED( id );
    //m_ReadyForCapture = false;
    //m_isCapturingImage = false;

    //LogModule(eLogVideo, LOG_VERBOSE, "displayCaptureError %d %s", error, errorString.toUtf8().constData());
    LogMsg( LOG_VERBOSE, "displayCaptureError %d %s", error, errorString.toUtf8().constData() );
    if( QImageCapture::NotReadyError != error )
    {
        QMessageBox::warning( this, QObject::tr( "Image Capture Error" ), errorString );
    }
}

//============================================================================
void CamLogic::startCamera()
{
    if( getCamStartupCompleted() )
    {
        if( !m_camera.isNull() && !m_CamIsStarted )
        {
            m_CamIsStarted = true;
            m_VideoFrameProcessor.enableProcessing( true );
            m_camera->start();
        }
        else if( m_camera.isNull() )
        {
            LogMsg( LOG_ERROR, "CamLogic::startCamera camera is null" );
        }
    }
}

//============================================================================
void CamLogic::stopCamera()
{
    if( getCamStartupCompleted() )
    {
        if( m_CamIsStarted )
        {
            m_CamIsStarted = false;
            if( !m_camera.isNull() )
            {
                m_camera->stop();
            }
        }
    }
}

//============================================================================
void CamLogic::displayRecorderError()
{
    //QMessageBox::warning( this, tr( "Capture Error" ), m_mediaRecorder->errorString() );
}

//============================================================================
void CamLogic::displayCameraError()
{
    //LogModule(eLogVideo, LOG_ERROR, "CamLogic Error %d %s", m_camera->error(), m_camera->errorString().toUtf8().constData() );
    LogMsg( LOG_ERROR, "CamLogic Error %d %s", m_camera->error(), m_camera->errorString().toUtf8().constData() );

    // QMessageBox::warning( this, tr( "CamLogic Error" ), m_camera->errorString() );
}

//============================================================================
void CamLogic::updateCameraDevice( QAction *action )
{
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    setCamera( qvariant_cast< QCameraInfo >( action->data() ) );
#endif // #if QT_VERSION < QT_VERSION_CHECK(6,0,0)
}

//============================================================================
void CamLogic::displayViewfinder()
{
    //ui->stackedWidget->setCurrentIndex(0);
}

//============================================================================
void CamLogic::displayCapturedImage()
{
    //ui->stackedWidget->setCurrentIndex(1);
}

//============================================================================
void CamLogic::readyForCapture( bool ready )
{
    m_ReadyForCapture = ready;
    LogMsg( LOG_ERROR, "CamLogic::readyForCapture ? %d", ready );
}

//============================================================================
void CamLogic::imageSaved( int id, const QString &fileName )
{
    Q_UNUSED( id );

    m_isCapturingImage = false;
}

//============================================================================
void CamLogic::closeEvent( QCloseEvent *event )
{
    if( m_isCapturingImage ) 
    {
        LogMsg( LOG_VERBOSE, "CamLogic::closeEvent while capturing image" );
        event->ignore();
    }
    else 
    {
        event->accept();
    }
}

//============================================================================
void CamLogic::nextCamera( void )
{
    const QList<QCameraDevice> availableCameras = QMediaDevices::videoInputs();
    bool foundDevice = false;
    bool setNewDevice = false;
    if( availableCameras.size() > 1 )
    {
        for( const QCameraDevice& cameraDevice : availableCameras )
        {
            if( cameraDevice.description() == m_CamDescription )
            {
                foundDevice = true;
            }

            if( foundDevice && !( m_CamDescription == cameraDevice.description() ) )
            {
                setCamera( cameraDevice );
                setNewDevice = true;
                return;
            }
        }

        if( !foundDevice || !setNewDevice )
        {
            setCamera( availableCameras.front() );
        }
    }
}
