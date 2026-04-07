//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "CamLogic.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiParams.h"
#include "GuiPlayerMgr.h"

#include <P2PEngine/P2PEngine.h>
#include <MediaProcessor/MediaProcessor.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#include <QMessageBox>

//============================================================================
CamLogic::CamLogic(AppCommon& myApp, QObject *parent )
    : QObject(parent)
    , m_MyApp(myApp)
    , m_MediaProcessor( GetPtoPEngine().getMediaProcessor() )
    , m_CamProcessor(*this)
#if defined(ENABLE_JAVA_CAM)
    , m_CamJavaClient( myApp, *this, this )
#elif defined(ENABLE_V4L2_CAM)
    , m_CamV4L2( *this )
#else
    , m_CamFrameProcessor( myApp, *this, this )
#endif // defined(ENABLE_JAVA_CAM)
{
    memset( m_WantCamInput, 0, sizeof( m_WantCamInput ) );
}

//============================================================================
CamLogic::~CamLogic() {
#if defined(ENABLE_JAVA_CAM)
#elif defined(ENABLE_V4L2_CAM)
    m_CamV4L2.closeDevice();
#else
    if (m_Camera) 
    {
        m_Camera->stop();
    }
#endif // defined(ENABLE_JAVA_CAM)
}

//============================================================================
bool CamLogic::isCamCaptureRequested( void )
{
    bool isRequested{ false };
    for( int i = 0; i < eMaxMediaModule; i++ )
    {
        if( m_WantCamInput[i] )
        {
            isRequested = true;
            break;
        }
    }

    return isRequested;
}

//============================================================================
void CamLogic::startupCamLogic( void )
{
#if defined(TARGET_OS_ANDROID)    
    if( !GuiParams::requestPermission("android.permission.CAMERA") )
    {
        LogMsg( LOG_ERROR, "%s Do Not have camera permission", __func__ );
        return;
    }
#endif // defined(TARGET_OS_ANDROID)

#if defined(ENABLE_JAVA_CAM)
    m_CamJavaClient.startupCamLogic();
    // onCamCaptureReady will get called after camera service starts
#elif defined(ENABLE_V4L2_CAM)
    onCamCaptureReady( true );
#else
    onCamCaptureReady( true );
#endif // defined(ENABLE_JAVA_CAM)
}

//============================================================================
void CamLogic::shutdownCamLogic( void )
{
#if defined(ENABLE_JAVA_CAM)
    m_CamJavaClient.shutdownCamLogic();
#elif defined(ENABLE_V4L2_CAM)
    m_CamV4L2.closeDevice();
#else
    m_CamFrameProcessor.enableProcessing( false );

    if( m_Camera )
    {
        m_Camera->stop();
    }

    if( m_CaptureSession )
    {
        m_CaptureSession->setCamera( nullptr );
        m_CaptureSession->setVideoSink( nullptr );
    }


    delete m_CamFrameSink;
    m_CamFrameSink = nullptr;

    delete m_CaptureSession;
    m_CaptureSession = nullptr;

    delete m_Camera;
    m_Camera = nullptr;

#endif // defined(ENABLE_JAVA_CAM)
}

//============================================================================
void CamLogic::onCamCaptureReady( bool isReady )
{
    updateCameraDevices();

    m_CamId = selectLastUsedCamera();
    if( m_CamId.empty() )
    {
        LogMsg( LOG_WARN, "%s NO AVAILABLE CAMERAS", __func__ );
    }
    else
    {
        LogMsg( LOG_VERBOSE, "%s last used camera %s", __func__, m_CamId.c_str() );
    }

    m_MyApp.fromGuiCameraEnable( m_MyApp.getAppSettings().getCamEnable() );

    emit signalCamCaptureReady();
}

//============================================================================
std::string CamLogic::selectLastUsedCamera( void )
{
    std::string camId = m_MyApp.getAppSettings().getCamSourceId();
    if( !camId.empty() && cameraExists( camId ) )
    {
        return camId;
    }

#if defined(ENABLE_JAVA_CAM)
    // try back facing camera first
    for( auto device : m_CamIdList )
    {
        if( device.first )
        {
            return device.second;
        }
    }

    // select first camera if exists
    if( m_CamIdList.size() )
    {
        return m_CamIdList.front().second;
    }

#elif defined(ENABLE_V4L2_CAM)
    if( m_V4L2DeviceMap.size() )
    {
        return m_V4L2DeviceMap.begin()->first;
    }
#else
    // try back facing camera first
    for( auto device : m_AvailableCameras )
    {
        if( device.position() == device.BackFace )
        {
            return device.description().toUtf8().constData();
        }
    }
    // try front facing camera next
    for( auto device : m_AvailableCameras )
    {
        if( device.position() == device.FrontFace )
        {
            return device.description().toUtf8().constData();
        }
    }

    // select first available if exists
    if( m_AvailableCameras.size() )
    {
        return m_AvailableCameras.front().description().toUtf8().constData();
    }

#endif // defined(ENABLE_JAVA_CAM)
    return "";
}

//============================================================================
bool CamLogic::canProcessCamCapture( void )
{
    if( VxIsAppShuttingDown() || !isCamCaptureRequested() || m_CamProcessor.isStalled() )
    {
        if( isCamCaptureRequested() )
        {
            if( LogEnabled( eLogWebCam ) )LogModule( eLogWebCam, LOG_WARN, "CamLogic::%s cannot process cam que rgb %zu jpg %zu", __func__, m_CamProcessor.getRgbQueueSize(), m_CamProcessor.getJpgQueueSize() );
        }

        return false;
    }

    return true;
}

//============================================================================
void CamLogic::processCamCapture( std::shared_ptr<CamJpgVideo>& jpgVideo )
{
    m_CamImageInTransitCnt++;
    m_MediaProcessor.processCamCaptureJpgVideo( jpgVideo );
}

//============================================================================
void CamLogic::updateCameraDevices( void )
{
#if defined(ENABLE_JAVA_CAM)
    m_CamJavaClient.getCameraDevices( m_CamIdList );
#elif defined(ENABLE_V4L2_CAM)
    m_V4L2DeviceMap.clear();
    std::vector<std::pair<std::string, std::string>> devices;
    CamV4L2::enumerateDevices( devices );
    for( const auto& dev : devices )
    {
        m_V4L2DeviceMap[dev.first] = dev.second;
    }
#else
    m_AvailableCameras = QMediaDevices::videoInputs();
#endif // defined(ENABLE_JAVA_CAM)
}

//============================================================================
void CamLogic::getAvailableCameras( std::vector<QString>& retCamList )
{
#if defined(ENABLE_JAVA_CAM)
    for( auto device : m_CamIdList )
    {
        retCamList.emplace_back( device.second.c_str() );
    }
#elif defined(ENABLE_V4L2_CAM)
    for( const auto& device : m_V4L2DeviceMap )
    {
        retCamList.emplace_back( device.first.c_str() );
    }
#else
    for( auto device : m_AvailableCameras )
    {
        retCamList.emplace_back( device.description() );
    }
#endif // defined(ENABLE_JAVA_CAM)
}

//============================================================================
bool CamLogic::startCamCapture( std::string camId )
{
    if( camId.empty() || !cameraExists( camId ) )
    {
        return false;
    }

    m_CamId = camId;
    m_LastCamUsed.clear();
    m_MyApp.getAppSettings().setCamSourceId( camId );
    m_MyApp.setCamCaptureRotation( m_MyApp.getAppSettings().getCamRotation( camId ) );
    
    return enableCamCapture( true );
}

//============================================================================
#if defined(ENABLE_V4L2_CAM)
bool CamLogic::setV4L2Camera( const std::string& devPath )
{
    if( !m_CameraEnabled )
    {
        LogModule( eLogWebCam, LOG_VERBOSE, "CamLogic::%s called but cam is disabled", __func__ );
        return false;
    }

    QSize targetSize = GuiParams::getSnapshotDesiredSize();
    m_CamV4L2.closeDevice();
    bool opened = m_CamV4L2.openDevice( devPath, targetSize.width(), targetSize.height() );
    if( !opened )
    {
        LogMsg( LOG_ERROR, "%s failed opening %s", __func__, devPath.c_str() );
    }

    return opened;
}
#endif

//============================================================================
#if !defined(ENABLE_JAVA_CAM) && !defined(ENABLE_V4L2_CAM)
bool CamLogic::setCamera( const QCameraDevice& cameraDevice )
{
    if( m_Camera )
    {
        m_Camera->stop();
        m_Camera->deleteLater();
        m_Camera = nullptr;
    }

    if( m_CaptureSession )
    {
        m_CaptureSession->deleteLater();
        m_CaptureSession = nullptr;
    }

    if( m_CamFrameSink )
    {
        disconnect( m_CamFrameSink, SIGNAL(videoFrameChanged(const QVideoFrame&)), &m_CamFrameProcessor, SLOT(slotVideoFrameChanged(const QVideoFrame&)) );
        m_CamFrameProcessor.enableProcessing( false );
        m_CamFrameSink->deleteLater();
        m_CamFrameSink = nullptr;
    }

    if( !m_CameraEnabled )
    {
        LogModule( eLogWebCam, LOG_VERBOSE, "CamLogic::%s called but cam is disabled", __func__ );
        return false;
    }

    //ProcessQtEvents( 300 ); // give qt time to clean up old capture

    m_Camera = new QCamera(cameraDevice);
    if( !selectVideoFormat( cameraDevice ) )
    {
        QString statusMsg = QString( "Camera '%1' has no supported format. JPEG and YUYV are disabled." )
            .arg( cameraDevice.description() );
        LogMsg( LOG_ERROR, "%s %s", __func__, statusMsg.toUtf8().constData() );

        m_Camera->deleteLater();
        m_Camera = nullptr;
        QMessageBox::information( nullptr, "Camera Not Supported", statusMsg );
        return false;
    }

    m_CaptureSession = new QMediaCaptureSession();
    m_CaptureSession->setCamera(m_Camera);
    if( m_CaptureSession->audioInput() )
    {
        m_CaptureSession->setAudioInput(nullptr);
    }

    if( m_CaptureSession->audioOutput() )
    {
        m_CaptureSession->setAudioOutput(nullptr);
    }

    m_CamFrameSink = new QVideoSink();
    m_CaptureSession->setVideoSink( m_CamFrameSink );
    connect( m_CamFrameSink, SIGNAL(videoFrameChanged(const QVideoFrame&)), &m_CamFrameProcessor, SLOT(slotVideoFrameChanged(const QVideoFrame&)) );
    m_CamFrameProcessor.enableProcessing( true );
    if( m_CameraEnabled )
    {
        m_Camera->start();
    }

    return m_Camera->isActive();
}
#endif

//============================================================================
void CamLogic::toGuiWantVideoCapture( EMediaModule mediaModule, bool wantVidCapture )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    bool isWanted = isCamCaptureRequested();
    m_WantCamInput[mediaModule] = wantVidCapture;
    bool isWantedAfter = isCamCaptureRequested();
    if( isWanted != isWantedAfter )
    {
        enableCamCapture( isWantedAfter );
    }
}

//============================================================================
bool CamLogic::cameraExists( std::string camId )
{
    if( camId.empty() )
    {
        return false;
    }

#if defined(ENABLE_JAVA_CAM)
    for( auto camPair : m_CamIdList )
    {
        if( camId == camPair.second )
        {
            return true;
        }
    }

    return false;
#elif defined(ENABLE_V4L2_CAM)
    return m_V4L2DeviceMap.find( camId ) != m_V4L2DeviceMap.end();
#else
    QString camDescription = camId.c_str();
    for( auto device : m_AvailableCameras )
    {
        if( device.description() == camDescription )
        {
            return true;
        }
    }

    return false;
#endif // defined(ENABLE_JAVA_CAM)
}

//============================================================================
void CamLogic::setCameraEnable( bool camEnable )
{
    if( camEnable == m_CameraEnabled )
    {
        return;
    }

    m_CameraEnabled = camEnable;
    if( !m_CameraEnabled )
    {
         enableCamCapture( false );
    }
    else
    {
        if( isCamCaptureRequested() )
        {
            enableCamCapture( true );
        }
    }

    m_MyApp.getAppSettings().setCamEnable( camEnable );
    m_MyApp.fromGuiCameraEnable( camEnable );
}

//============================================================================
QString CamLogic::getCameraBackgroundFile( void )
{
    if( m_CameraEnabled )
    {
        return ":/AppRes/Resources/ic_cam_black.png";
    }
    else
    {
        return ":/AppRes/Resources/ic_cam_disabled.png";
    }
}

//============================================================================
#if !defined(ENABLE_JAVA_CAM) && !defined(ENABLE_V4L2_CAM)
#if 0
bool CamLogic::selectVideoFormat( const QCameraDevice& cameraDevice )
{
    if( VxIsAppShuttingDown() )
    {
        return false;
    }

#if defined(ENABLE_JAVA_CAM)
    return false;
#else

    auto isPreferredFormat = []( QVideoFrameFormat::PixelFormat pixelFormat )
    {
        return pixelFormat == QVideoFrameFormat::Format_YUV420P ||
               pixelFormat == QVideoFrameFormat::Format_NV12 ||
               pixelFormat == QVideoFrameFormat::Format_NV21 ||
               pixelFormat == QVideoFrameFormat::Format_YV12;
    };

    auto isRejectedFormat = []( QVideoFrameFormat::PixelFormat pixelFormat )
    {
        return pixelFormat == QVideoFrameFormat::Format_Jpeg ||
               pixelFormat == QVideoFrameFormat::Format_YUYV;
    };

    QSize targetSize = GuiParams::getSnapshotDesiredSize();
    auto formats = cameraDevice.videoFormats();
    if( !formats.isEmpty() ) 
    {
        auto chooseFormat = formats.first();
        bool chooseFormatInvalid = true;
        //LogModule( eLogWebCam, LOG_VERBOSE, "Default camera resolution w %d h %d min fps %3.1f max fps %3.1f", chooseFormat.resolution().width(),
        //    chooseFormat.resolution().height(), chooseFormat.minFrameRate(), chooseFormat.maxFrameRate() );
        if( !isRejectedFormat( chooseFormat.pixelFormat() ) &&
            chooseFormat.resolution().width() >= targetSize.width() &&
            chooseFormat.resolution().height() >= targetSize.height() )
        {
            chooseFormatInvalid = false;
        }

        int formatNum = 0;

        for( const auto& format : formats ) 
        {
            formatNum++;
            if( formatNum == 1 )
            {
                // no need to check the first format
                continue;
            }

            if( format.resolution().width() >= targetSize.width() && format.resolution().height() >= targetSize.height() )
            {
                QVideoFrameFormat::PixelFormat newPixelFormat = format.pixelFormat();
                if(LogEnabled( eLogWebCam ) ) LogModule( eLogWebCam, LOG_VERBOSE, "%s format resolution w %d h %d min fps %3.1f max fps %3.1f pix format %d", __func__, format.resolution().width(),
                       format.resolution().height(), format.minFrameRate(), format.maxFrameRate(), newPixelFormat );

                if( isRejectedFormat( newPixelFormat ) )
                {
                    continue;
                }

                if( chooseFormatInvalid )
                {
                    chooseFormat = format;
                    chooseFormatInvalid = false;
                    continue;
                }

                QVideoFrameFormat::PixelFormat pixelFormat = chooseFormat.pixelFormat();
                if( isPreferredFormat( newPixelFormat ) && !isPreferredFormat( pixelFormat ) )
                {
                    chooseFormat = format;
                    continue;
                }

                if( !isPreferredFormat( newPixelFormat ) && isPreferredFormat( pixelFormat ) )
                {
                    continue;
                }

                if( isBetterVideoFormat( targetSize, format, chooseFormat ) )
                {
                    chooseFormat = format;
                    chooseFormatInvalid = false;
                }
            }
        }

        if( chooseFormatInvalid )
        {
            LogMsg( LOG_ERROR, "%s no supported camera format for %s target w %d h %d", __func__, cameraDevice.description().toUtf8().constData(), targetSize.width(), targetSize.height() );
            return false;
        }

        QVideoFrameFormat::PixelFormat pixelFormat = chooseFormat.pixelFormat();
        if(LogEnabled( eLogWebCam ) ) LogModule( eLogWebCam, LOG_VERBOSE, "%s Setting Format resolution w %d h %d min fps %3.1f max fps %3.1f pix format %d", __func__, chooseFormat.resolution().width(),
            chooseFormat.resolution().height(), chooseFormat.minFrameRate(), chooseFormat.maxFrameRate(), pixelFormat );

        m_Camera->setCameraFormat( chooseFormat );
        return true;
    }
    return false;
#endif // defined(ENABLE_JAVA_CAM)
}
#else
bool CamLogic::selectVideoFormat( const QCameraDevice& cameraDevice )
{
    if( VxIsAppShuttingDown() )
    {
        return false;
    }

#if defined(ENABLE_JAVA_CAM)
    return false;
#else
  
    QSize targetSize = GuiParams::getSnapshotDesiredSize();
    auto formats = cameraDevice.videoFormats();
    if( !formats.isEmpty() ) 
    {
        auto chooseFormat = formats.first();
        bool chooseFormatInvalid = true;
        //LogModule( eLogWebCam, LOG_VERBOSE, "Default camera resolution w %d h %d min fps %3.1f max fps %3.1f", chooseFormat.resolution().width(),
        //    chooseFormat.resolution().height(), chooseFormat.minFrameRate(), chooseFormat.maxFrameRate() );
        if( chooseFormat.resolution().width() >= targetSize.width() && chooseFormat.resolution().height() >= targetSize.height() )
        {
            chooseFormatInvalid = false;
        }

        int formatNum = 0;

        for( const auto& format : formats ) 
        {
            formatNum++;
            if( formatNum == 1 )
            {
                // no need to check the first format
                continue;
            }

            if( format.resolution().width() >= targetSize.width() && format.resolution().height() >= targetSize.height() )
            {
                QVideoFrameFormat::PixelFormat pixelFormat = chooseFormat.pixelFormat();
                if(LogEnabled( eLogWebCam ) ) LogModule( eLogWebCam, LOG_VERBOSE, "%s format resolution w %d h %d min fps %3.1f max fps %3.1f pix format %d", __func__, format.resolution().width(),
                       format.resolution().height(), format.minFrameRate(), format.maxFrameRate(), pixelFormat );
                if( isBetterVideoFormat( targetSize, format, chooseFormat ) )
                {
                    chooseFormat = format;
                    chooseFormatInvalid = false;
                }
            }
        }

        QVideoFrameFormat::PixelFormat pixelFormat = chooseFormat.pixelFormat();
        if(LogEnabled( eLogWebCam ) ) LogModule( eLogWebCam, LOG_VERBOSE, "%s Setting Format resolution w %d h %d min fps %3.1f max fps %3.1f pix format %d", __func__, chooseFormat.resolution().width(),
            chooseFormat.resolution().height(), chooseFormat.minFrameRate(), chooseFormat.maxFrameRate(), pixelFormat );

        m_Camera->setCameraFormat( chooseFormat );
    }
#endif // defined(ENABLE_JAVA_CAM)
    return true;
}

#endif

//============================================================================
bool CamLogic::isBetterVideoFormat( QSize& targetSize, const QCameraFormat& newFormat, const QCameraFormat& oldFormat )
{
#if defined(ENABLE_JAVA_CAM)
    return false;
#else
    if( newFormat.resolution() == targetSize )
    {
        if(LogEnabled( eLogWebCam ) ) LogModule( eLogWebCam, LOG_VERBOSE, "%s new resolution w %d h %d min fps %3.1f max fps %3.1f pix format %d", __func__,
                newFormat.resolution().width(), newFormat.resolution().height(), newFormat.minFrameRate(), newFormat.maxFrameRate(), newFormat.pixelFormat() );
        if(LogEnabled( eLogWebCam ) ) LogModule( eLogWebCam, LOG_VERBOSE, "%s old resolution w %d h %d min fps %3.1f max fps %3.1f pix format %d", __func__,
                oldFormat.resolution().width(), oldFormat.resolution().height(), oldFormat.minFrameRate(), oldFormat.maxFrameRate(), oldFormat.pixelFormat() );
    }

    QSize sizeDifNew = newFormat.resolution() - targetSize;
    QSize sizeDifOld = oldFormat.resolution() - targetSize;
    if( (std::abs( sizeDifNew.width() ) + std::abs( sizeDifNew.height() ) ) < (std::abs( sizeDifOld.width() ) + std::abs( sizeDifOld.height() )) )
    {
        // better resolution match
        if(LogEnabled( eLogWebCam ) ) LogModule( eLogWebCam, LOG_VERBOSE, "%s new is better resolution match", __func__ );
        return true;
    }
    else if( oldFormat.resolution() == targetSize && newFormat.resolution() != targetSize )
    {
        // resolution is most important.. if we have a matching resolution then do not consider others
        return false;
    }

    // 15 fps is a decent rate without overtaxing low resource devices
    static float targetRate{ 15.0f }; 
    if( newFormat.maxFrameRate() >= targetRate &&
        std::abs( targetRate - newFormat.maxFrameRate() ) < std::abs( targetRate - oldFormat.maxFrameRate() ) )
    {
        if(LogEnabled( eLogWebCam ) ) LogModule( eLogWebCam, LOG_VERBOSE, "%s new has better frame rate %3.1f", __func__, newFormat.maxFrameRate() );
        return true;
    }

    if( newFormat.maxFrameRate() == targetRate )
    {
        if( isBetterConversionSpeed( newFormat, oldFormat ) )
        {
            // some cameras have a faster rate with this format + used to be standard for android
            if(LogEnabled( eLogWebCam ) ) LogModule( eLogWebCam, LOG_VERBOSE, "%s new is better format match %d", __func__, newFormat.pixelFormat() );
            return true;
        }
    }

    return false;

#endif // defined(ENABLE_JAVA_CAM)
}

//============================================================================
bool CamLogic::isBetterConversionSpeed( const QCameraFormat& newFormat, const QCameraFormat& oldFormat )
{
    // consider conversion speed for formats that are not a direct match to what we want to process
    // faster formats will reduce cpu load and increase frame rate when we have to convert
    // this is only used when resolution and frame rate are not a clear winner
    auto getFormatPriority = [](QVideoFrameFormat::PixelFormat pixelFormat) -> int {
        switch (pixelFormat) {
            // Fast: Bi-planar (NV12/NV21) is typically most cache-friendly for YUV -> RGB
            case QVideoFrameFormat::Format_NV12:
            case QVideoFrameFormat::Format_NV21:
                return 1; 

            // Good: Standard 3-plane planar formats
            case QVideoFrameFormat::Format_YUV420P:
            case QVideoFrameFormat::Format_YV12:
                return 2;

            // Slow: Packed format requiring more bit-shifting
            case QVideoFrameFormat::Format_YUYV:
                return 3;

            // Slowest: Requires full CPU decompression (JPEG decoding)
            case QVideoFrameFormat::Format_Jpeg:
                return 4;

            default:
                return 99; // Unknown/Unsupported
        }
    };

    return getFormatPriority(newFormat.pixelFormat()) < getFormatPriority(oldFormat.pixelFormat());
}
#endif

//============================================================================
bool CamLogic::nextCamera( void )
{
    updateCameraDevices();
    if( getCameraCount() < 2 )
    {
        return false;
    }

#if defined(ENABLE_JAVA_CAM)
    std::string camId = getCamId();
    bool foundCurrent{false};
    for( auto device : m_CamIdList )
    {
        if(foundCurrent)
        {
            return startCamCapture( device.second );
        }
        else if(device.second == camId)
        {
            foundCurrent = true;
        }
    }

    return startCamCapture( m_CamIdList.front().second );
#elif defined(ENABLE_V4L2_CAM)
    std::string camId = getCamId();
    bool foundCurrent{false};
    for( const auto& device : m_V4L2DeviceMap )
    {
        if( foundCurrent )
        {
            return startCamCapture( device.first );
        }
        else if( device.first == camId )
        {
            foundCurrent = true;
        }
    }

    return startCamCapture( m_V4L2DeviceMap.begin()->first );
#else
    QString camDesc = getCamId().c_str();
    bool foundCurrent{false};
    for( auto device : m_AvailableCameras )
    {
        if(foundCurrent)
        {
            return startCamCapture( device.description().toUtf8().constData() );
        }
        else if(device.description() == camDesc)
        {
            foundCurrent = true;
        }
    }

    return startCamCapture( m_AvailableCameras.front().description().toUtf8().constData() );
#endif // defined(ENABLE_JAVA_CAM)
}

//============================================================================
int CamLogic::getCameraCount( void )
{
#if defined(ENABLE_JAVA_CAM)
    return m_CamIdList.size();
#elif defined(ENABLE_V4L2_CAM)
    return (int)m_V4L2DeviceMap.size();
#else
    return m_AvailableCameras.size();
#endif // defined(ENABLE_JAVA_CAM)
}

//============================================================================
bool CamLogic::isCamAvailable( void )
{
    return getCameraCount();
}

//============================================================================
std::string CamLogic::getCamId( void )
{
#if defined(ENABLE_JAVA_CAM)
    return m_CamId;
#elif defined(ENABLE_V4L2_CAM)
    return m_CamId;
#else
    return m_Camera ? m_Camera->cameraDevice().description().toUtf8().constData() : "";
#endif // defined(ENABLE_JAVA_CAM)
}

//============================================================================
int CamLogic::getCamCaptureRotation( void )
{
    return m_MyApp.getCamCaptureRotation();
}

//============================================================================
bool CamLogic::enableCamCapture( bool enable )
{
    if( !enable )
    {
    #if defined(ENABLE_JAVA_CAM)
        m_CamJavaClient.stopCamCapture();
    #elif defined(ENABLE_V4L2_CAM)
        m_CamV4L2.closeDevice();
    #else
        if( m_Camera )
        {
            m_Camera->stop();
        }
    #endif // defined(ENABLE_JAVA_CAM)
        if( m_CaptureRunning != enable )
        {
            updateCaptureRunning( enable );
        }

        return false;
    }

    if( m_CamId.empty() )
    {
        LogMsg( LOG_ERROR, "%s m_CamId.empty()", __func__ );
        return false;
    }

    if( !m_CameraEnabled )
    {
        LogMsg( LOG_ERROR, "%s !m_CameraEnabled", __func__ );
        return false;
    }

    if( !isCamCaptureRequested() )
    {
        LogMsg( LOG_ERROR, "%s !isCamCaptureRequested()", __func__ );
        return false;
    }

    if( m_CaptureRunning && m_CamId == m_LastCamUsed)
    {
        return true;
    }

#if defined(ENABLE_JAVA_CAM)


    bool capRunning = m_CamJavaClient.startCamCapture( m_CamId );
    if( m_CaptureRunning != capRunning )
    {
        updateCaptureRunning( capRunning );
    }

    return capRunning;
#elif defined(ENABLE_V4L2_CAM)
    auto devIter = m_V4L2DeviceMap.find( m_CamId );
    if( devIter == m_V4L2DeviceMap.end() )
    {
        LogMsg( LOG_DEBUG, "%s camera %s NOT available", __func__, m_CamId.c_str() );
        return false;
    }

    bool capRunning = setV4L2Camera( devIter->second );
    if( !capRunning )
    {
        // Some webcams expose multiple /dev/video* nodes; try other nodes as fallback.
        for( const auto& entry : m_V4L2DeviceMap )
        {
            if( entry.first == m_CamId )
            {
                continue;
            }

            if( setV4L2Camera( entry.second ) )
            {
                m_CamId = entry.first;
                m_MyApp.getAppSettings().setCamSourceId( m_CamId );
                capRunning = true;
                LogMsg( LOG_WARN, "%s fallback camera selected %s", __func__, m_CamId.c_str() );
                break;
            }
        }
    }

    if( m_CaptureRunning != capRunning )
    {
        updateCaptureRunning( capRunning );
    }

    return capRunning;
#else
    QString camDescription = m_CamId.c_str();
    if( m_Camera && m_Camera->cameraDevice().description() == camDescription )
    {
        if( m_CameraEnabled )
        {
            m_Camera->start();
        }
        else
        {
            m_Camera->stop();
        }
        
        if( !m_CaptureRunning )
        {
            updateCaptureRunning( true );
        }

        return true;
    }

    if( !m_CameraEnabled )
    {
        return false;
    }

    for( auto device : m_AvailableCameras )
    {
        if( device.description() == camDescription )
        {
            bool result = setCamera( device );
            if( result && !m_CaptureRunning )
            {
                updateCaptureRunning( true );
            }

            return result;
        }
    }

#endif // defined(ENABLE_JAVA_CAM)
    LogMsg( LOG_DEBUG, "%s camera %s NOT available", __func__, m_CamId.c_str() );
    return false;
}

//============================================================================
void CamLogic::updateCaptureRunning( bool capIsRunning )
{
    if( m_CaptureRunning != capIsRunning )
    {
        m_CaptureRunning = capIsRunning;
        m_LastCamUsed = capIsRunning ? m_CamId : "";
        m_MyApp.fromGuiCaptureRunning( capIsRunning );
    }
}
