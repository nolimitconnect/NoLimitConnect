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

#include "VideoFrameProcessor.h"

#include <GuiInterface/IDefs.h>

#include <QCoreApplication>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QVideoSink>
#include <QImage>

#include <string>

class AppCommon;

class CamLogic : public QObject {
    Q_OBJECT

public:
    CamLogic( AppCommon& myApp, QObject* parent = nullptr );
    ~CamLogic();

    void                        startupCamLogic( void );
    void                        shutdownCamLogic( void );

    int                         getCameraCount( void ) { return m_AvailableCameras.size(); }
    bool                        isCamAvailable( void ) { return m_AvailableCameras.size(); }
    bool                        isCamCaptureRequested( void );
    bool                        isCamCaptureRunning( void );

    void                        toGuiWantVideoCapture( EAppModule appModule, bool wantVidCapture );

    void                        updateCameraDevices( void );
    void                        getAvailableCameras( std::vector<QString>& retCamList );
    bool                        selectDefaultCamera( void );
    bool                        selectCamera( QString camDescription );

    QString                     getCamDescription( void ) { return m_Camera ? m_Camera->cameraDevice().description() : ""; }
    std::string                 getCamId( void ) { return m_Camera ? m_Camera->cameraDevice().description().toUtf8().constData() : ""; }

    bool                        cameraExists( QString camId );

    void                        setCameraEnable( bool camEnable );
    bool                        getCameraEnable( void ) { return m_CameraEnabled; }

    QString                     getCameraBackgroundFile( void );

    bool                        nextCamera( void );

protected:
    bool                        setCamera( const QCameraDevice& cameraDevice );
    void                        selectVideoFormat( const QCameraDevice& cameraDevice );
    bool                        isBetterVideoFormat( QSize& targetSize, const QCameraFormat& newFormat, const QCameraFormat& oldFormat );

    AppCommon&                  m_MyApp;
    bool                        m_WantCamInput[ eMaxAppModule ];
    QCamera*                    m_Camera{ nullptr };
    QMediaCaptureSession *      m_CaptureSession{ nullptr };
    QVideoSink *                m_CamFrameSink{ nullptr };

    QList<QCameraDevice>        m_AvailableCameras;
    bool                        m_CameraEnabled{ false };

    VideoFrameProcessor         m_VideoFrameProcessor;
};
