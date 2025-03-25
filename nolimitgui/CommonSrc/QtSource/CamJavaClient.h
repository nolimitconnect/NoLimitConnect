#pragma once
//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#if defined(ENABLE_JAVA_CAM)

#include <QObject>

#include <QtCore/qjniobject.h>
#include <QtCore/private/qandroidextras_p.h>

class AppCommon;
class CamLogic;

class CamJavaClient : public QObject
{
    Q_OBJECT
public:
    explicit CamJavaClient( AppCommon& myApp, CamLogic& camLogic, QObject *parent = 0 );

    void                        startupCamLogic( void );
    void                        shutdownCamLogic( void );

    void                        getCameraDevices( std::vector<std::pair<bool,std::string>>& camIdList );

    void                        onCamServiceStarted( void );
    bool                        canProcessCamCapture( void );
    void                        processCamCapture( int width, int height, std::shared_ptr<uint8_t>& rgbData, int dataLen );

    bool                        startCamCapture( std::string camId );
    void                        stopCamCapture( void );

protected:
    void                        updateCameraList( void );
    bool                        isBackFacing( std::string& camId );

    AppCommon&                  m_MyApp;
    CamLogic&                   m_CamLogic;

    std::vector<std::string>    m_CamIdList;

};

#endif // defined(ENABLE_JAVA_CAM)
