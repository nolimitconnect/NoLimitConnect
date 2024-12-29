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

#include "AppModuleState.h"
#include "AppSettings.h"
#include "GuiPlayerMgr.h"
#include "HomeWindow.h"

#include "ActivityScanWebCams.h"
#include "ToGuiActivityInterface.h"
#include "ToGuiHardwareControlInterface.h"

#include <AssetBase/AssetPlaySession.h>
#include <CoreLib/VxGlobals.h>
#include <VxVideoLib/VxVideoLib.h>

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
	m_AppModuleState.toGuiSetIsAppModuleRunning( appModule, isRunning );
}

//============================================================================
bool AppCommon::toGuiGetIsAppModuleRunning( EAppModule appModule )
{
	return m_AppModuleState.toGuiGetIsAppModuleRunning( appModule );
}

//============================================================================
bool AppCommon::toGuiRunModule( EAppModule appModule )
{
	return m_AppModuleState.toGuiRunModule( appModule );
}

//============================================================================
bool AppCommon::toGuiStopModule( EAppModule appModule )
{
    return m_AppModuleState.toGuiStopModule( appModule );
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
	AssetPlaySession playSession( assetInfo );
	m_PlayerMgr.playMedia( playSession, false );
}

//============================================================================
void AppCommon::toGuiWantVideoCapture( EAppModule appModule, bool wantVidCapture )
{
	LogModule( eLogWebCam, LOG_INFO, "#### AppCommon::toGuiWantVideoCapture %s wantCapture %d", DescribeAppModule( appModule ), wantVidCapture );
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

                m_CamSourceId = m_CamLogic.getCamId();

                setCamCaptureRotation( m_AppSettings.getCamRotation( m_CamSourceId ) );

                bFirstTimeVideoCaptureStarted = false;
            }
        }
        else
        {
           LogModule( eLogWebCam, LOG_INFO, "AppCommon::slotEnableVideoCapture stopping capture" );
        }

		m_ToGuiHardwareCtrlBusy = true;
		for( auto toGuiClient : m_ToGuiHardwareCtrlList )
		{
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

//============================================================================
void AppCommon::slotInternalMediaAction( EAppModule appModule, EMediaPlayerAction playerAction, int actionVal, QString fileName )
{
	LogMsg( LOG_VERBOSE, "Media Action %d val %d fileName %s", playerAction, actionVal, fileName.toUtf8().constData() );
}

//============================================================================
void AppCommon::slotInternalMediaError( EAppModule appModule, EMediaError mediaError, QString msg )
{
    static bool isBusy{false};
	LogMsg( LOG_ERROR, "Media Error %d %s", mediaError, msg.toUtf8().constData() );

	if( isBusy )
	{
		// just log instead of show message box if user has not acked the previous message	
		return;
	}

	//isBusy = true;
	//QMessageBox::warning(&getHomeWindow(), QObject::tr("Media Error"), msg);
	//isBusy = false;
}
