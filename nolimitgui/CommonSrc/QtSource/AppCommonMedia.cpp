//============================================================================
// Copyright (C) 2009 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiPlayerMgr.h"

#include "ActivityScanWebCams.h"
#include "ToGuiActivityInterface.h"
#include "ToGuiHardwareControlInterface.h"

#include "AppInterface/INlc.h"

#include <VxVideoLib/VxVideoLib.h>
#include <CoreLib/VxGlobals.h>

#include <QTimer>
#include <QMessageBox>

//============================================================================
bool AppCommon::toGuiMediaAction( EAppModule appModule, EMediaPlayerAction playerAction, int actionVal, const char* fileName )
{
	if( VxIsAppShuttingDown() )
	{
        return false;
	}

	emit signalInternalMediaAction( appModule, playerAction, actionVal, fileName );
	return true;
}

//============================================================================
void AppCommon::toGuiMediaError( EAppModule appModule, EMediaError mediaError, const char* msg )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalMediaError( appModule, mediaError, msg );
}

//============================================================================
void AppCommon::toGuiSetIsAppModuleRunning( EAppModule appModule, bool isRunning )
{
	m_Nlc.toGuiSetIsAppModuleRunning( appModule, isRunning );
}

//============================================================================
bool AppCommon::toGuiGetIsAppModuleRunning( EAppModule appModule )
{
	return m_Nlc.toGuiGetIsAppModuleRunning( appModule );
}

//============================================================================
bool AppCommon::toGuiRunModule( EAppModule appModule )
{
	return m_Nlc.toGuiRunModule( appModule );
}

//============================================================================
bool AppCommon::toGuiStopModule( EAppModule appModule )
{
    return m_Nlc.toGuiStopModule( appModule );
}

//============================================================================
void AppCommon::toGuiCreateUserDirs( void )
{
	m_Nlc.createUserDirs();
}

//============================================================================
void AppCommon::toGuiPlayNlcMedia( AssetBaseInfo* assetInfo )
{
	LogMsg( LOG_INFO, "#### AppCommon::toGuiPlayNlcMedia %s", assetInfo->getAssetName().c_str() );
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalPlayNlcMedia( *assetInfo );
}

//============================================================================
void AppCommon::slotInternalPlayNlcMedia( AssetBaseInfo assetInfo )
{
	m_PlayerMgr.playMedia( assetInfo );
}

//============================================================================
void AppCommon::toGuiWantVideoCapture( EAppModule appModule, bool wantVidCapture )
{
	LogMsg( LOG_INFO, "#### AppCommon::toGuiWantVideoCapture %d", wantVidCapture );
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalWantVideoCapture( appModule, wantVidCapture );
}

//============================================================================
void AppCommon::slotInternalWantVideoCapture( EAppModule appModule, bool enableVidCapture )
{
	bool wasCamEnabled = m_CamLogic.isCamCaptureRunning();
	m_CamLogic.toGuiWantVideoCapture( appModule, enableVidCapture );
	bool isCamEnabled = m_CamLogic.isCamCaptureRunning();

    if( wasCamEnabled != isCamEnabled )
    {
        if( isCamEnabled )
        {
            static bool bFirstTimeVideoCaptureStarted = true;
            if( bFirstTimeVideoCaptureStarted )
            {
                if( !m_CamLogic.isCamCaptureRunning() )
                {
                    QMessageBox::warning( this, QObject::tr( "Web Cam Video" ), QObject::tr( "No Video Capture Devices Found" ) );
                    return;
                }

                m_CamSourceId = m_CamLogic.getCamSourceId();

                setCamCaptureRotation( m_AppSettings.getCamRotation( m_CamSourceId ) );

                bFirstTimeVideoCaptureStarted = false;
            }
        }
        else
        {
            LogMsg( LOG_INFO, "AppCommon::slotEnableVideoCapture stopping capture" );
        }

		m_ToGuiHardwareCtrlBusy = true;
		for( auto hardwareIter = m_ToGuiHardwareCtrlList.begin(); hardwareIter != m_ToGuiHardwareCtrlList.end(); ++hardwareIter )
		{
			ToGuiHardwareControlInterface* toGuiClient = *hardwareIter;
			toGuiClient->callbackToGuiWantVideoCapture( enableVidCapture );
		}

		m_ToGuiHardwareCtrlBusy = false;
    }
}

//============================================================================
void AppCommon::toGuiPlayVideoFrame( VxGUID& feedOnlineId, uint8_t* pu8Jpg, uint32_t u32JpgDataLen, int motion0To100000 )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	m_PlayerMgr.toGuiPlayVideoFrame( feedOnlineId, pu8Jpg, u32JpgDataLen, motion0To100000 );
}

//============================================================================
int AppCommon::toGuiPlayVideoFrame( VxGUID& feedOnlineId, uint8_t* picBuf, uint32_t picBufLen, int picWidth, int picHeight )
{
	if( VxIsAppShuttingDown() )
	{
		return 0;
	}

	return m_PlayerMgr.toGuiPlayVideoFrame( feedOnlineId, picBuf, picBufLen, picWidth, picHeight );
}
