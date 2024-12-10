#pragma once
//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include <GuiInterface/IDefs.h>

#include "VideoFrameProcessor.h"
#include "VideoSinkGrabber.h"

#include <QTimer>
#include <QKeyEvent>
#include <QMediaMetaData>
#include <QMessageBox>
#include <QPalette>
#include <QtWidgets>
#include <QMediaRecorder>

# include <QCamera>
# include <QCameraDevice>
# include <QCameraFormat>
# include <QImageCapture>
# include <QMediaCaptureSession>

class AppCommon;
class QVideoWidget;

class CamLogic : public QWidget
{
    Q_OBJECT

public:
    CamLogic( AppCommon& myApp );
    virtual ~CamLogic() = default;

    void                        camLogicStartup( void );
    void                        camLogicShutdown( void );

    void                        cameraEnable( bool wantVidCapture );

    bool                        isCamCaptureRequested( void );
    bool                        isCamCaptureRunning( void );
    bool                        isCamAvailable( void );
    bool                        updateCamAvailable( void ); // recheck devices for available cam

    /// TODO implement option to select cam hardware
    void						setCamSourceId( uint32_t camId )                        { m_CamId = camId; }
    uint32_t					getCamSourceId( void )                                  { return m_CamId; }

    void						setCamShowPreview( bool showPreview )                   { m_ShowPreview = showPreview; };
    bool						getCamShowPreview( void )                               { return m_ShowPreview; }

    void						setCamRotation( uint32_t camId, uint32_t camRotation )  { m_CamRotation = camRotation; };
    uint32_t					getCamRotation( uint32_t camId )                        { return m_CamRotation; }

    void						setVidFeedRotation( uint32_t feedRotation )             { m_FeedRotation = feedRotation; };
    uint32_t					getVidFeedRotation( void )                              { return m_FeedRotation; }

    void                        toGuiWantVideoCapture( EAppModule appModule, bool wantVidCapture );

    QString                     getCamDescription( void )                               { return m_CamDescription; }

    bool                        getCamStartupCompleted( void );

signals:
    void                        signalCameraDescription( QString camDescription );

public slots:
    void                        slotNetAvailStatus( ENetAvailStatus netAvailStatus );

    void                        setCamera( const QCameraDevice& cameraDevice );

    void                        startCamera();
    void                        stopCamera();

    void                        nextCamera( void );

    void                        record();
    void                        pause();
    void                        stop();
    void                        setMuted( bool );

    void                        displayRecorderError();
    void                        displayCameraError();

    void                        updateRecordTime();

    void                        updateCameraActive( bool active );
    void                        updateRecorderState( QMediaRecorder::RecorderState state );
    void                        displayCaptureError( int, QImageCapture::Error, const QString& errorString );

protected:
    bool                        initializeCam( void );

    void                        keyPressEvent( QKeyEvent *event ) override;
    void                        keyReleaseEvent( QKeyEvent *event ) override;

    void                        selectVideoFormat( const QCameraDevice& cameraDevice );

    AppCommon&                  m_MyApp;

    bool                        m_StartupWasCompleted{ false };
    uint32_t                    m_CamId{ 1 };
    bool                        m_CamsEnumerated{ false };
    bool                        m_CamInitiated{ false };
    bool                        m_ShowPreview{ false };
    uint32_t                    m_CamRotation{ 0 };
    uint32_t                    m_FeedRotation{ 0 };

    QScopedPointer<QCamera>     m_camera;

    bool                        m_CamIsStarted{ false };

    QMediaCaptureSession        m_captureSession;
    QScopedPointer<QMediaRecorder> m_mediaRecorder;

    QString                     m_CamDescription;
    int                         m_LastFrameNum{ 0 };
    QSize                       m_DesiredFrameSize;
    bool                        m_WantCamInput[ eMaxAppModule ];

    VideoSinkGrabber            m_VideoSinkGrabber;
    VideoFrameProcessor         m_VideoFrameProcessor;

};


