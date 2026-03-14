//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================
#pragma once

#include "CamProcessor.h"
#include "CamFrameProcessor.h"

#include <GuiInterface/IDefs.h>

#include <QCoreApplication>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QVideoSink>
#include <QImage>

#include <string>

#if defined(ENABLE_JAVA_CAM)
#include "CamJavaClient.h"
#endif // defined(ENABLE_JAVA_CAM)

class AppCommon;
class CamJpgVideo;
class MediaProcessor;

class CamLogic : public QObject 
{
    Q_OBJECT

public:
    static const int64_t CAM_SNAPSHOT_INTERVAL_MS = 60; // 60 ms = approx 15 frames per second 30 ms = approx 30 fps

    CamLogic( AppCommon& myApp, QObject* parent = nullptr );
    ~CamLogic();

    void                        startupCamLogic( void );
    void                        shutdownCamLogic( void );

    CamProcessor&               getCamProcessor( void ) { return m_CamProcessor; }

    void                        onCamCaptureReady( bool isReady );
    bool                        canProcessCamCapture( void );
    void                        processCamCapture( std::shared_ptr<CamJpgVideo>& jpgVideo );
    void                        camImageConsumed( void ) { if( m_CamImageInTransitCnt.load() ) m_CamImageInTransitCnt--; }

    int                         getCameraCount( void );
    bool                        isCamAvailable( void );
    bool                        isCamCaptureRequested( void );
    bool                        isCamCaptureRunning( void ) { return m_CaptureRunning; }

    void                        toGuiWantVideoCapture( EMediaModule mediaModule, bool wantVidCapture );

    void                        updateCameraDevices( void );
    void                        getAvailableCameras( std::vector<QString>& retCamList );
    bool                        cameraExists( std::string camId );

    // enable or disable all camera activity
    void                        setCameraEnable( bool camEnable );
    bool                        getCameraEnable( void ) { return m_CameraEnabled; }

    // get last used or in use camera id
    std::string                 getCamId( void );
    // start capture using the given cam id if camera is not disabled
    bool                        startCamCapture( std::string camId );
    // start/stop capture using the last used cam id if camera is not disabled
    bool                        enableCamCapture( bool enableCapture );
    // start capture using next cam id if camera is not disabled
    bool                        nextCamera( void );

    int                         getCamCaptureRotation( void );

    QString                     getCameraBackgroundFile( void );

signals:
    void                        signalCamCaptureReady( void );

protected:
    std::string                 selectLastUsedCamera( void ); // only called on startup

    bool                        setCamera( const QCameraDevice& cameraDevice );
    bool                        selectVideoFormat( const QCameraDevice& cameraDevice );
    bool                        isBetterVideoFormat( QSize& targetSize, const QCameraFormat& newFormat, const QCameraFormat& oldFormat );
    bool                        isBetterConversionSpeed( const QCameraFormat& newFormat, const QCameraFormat& oldFormat );

    void                        updateCaptureRunning( bool capIsRunning );

    AppCommon&                  m_MyApp;
    MediaProcessor&             m_MediaProcessor;

    CamProcessor                m_CamProcessor;

    bool                        m_WantCamInput[ eMaxMediaModule ];
    bool                        m_CameraEnabled{ false };
    bool                        m_CaptureRunning{ false };

    std::string                 m_CamId;
    std::string                 m_LastCamUsed;

#if defined(ENABLE_JAVA_CAM)
    CamJavaClient               m_CamJavaClient;
    std::vector<std::pair<bool,std::string>> m_CamIdList;
#else
    QCamera*                    m_Camera{ nullptr };
    QMediaCaptureSession *      m_CaptureSession{ nullptr };
    QVideoSink *                m_CamFrameSink{ nullptr };

    QList<QCameraDevice>        m_AvailableCameras;

    CamFrameProcessor         m_VideoFrameProcessor;
#endif // defined(ENABLE_JAVA_CAM)

    std::atomic<int>            m_CamImageInTransitCnt;
};
