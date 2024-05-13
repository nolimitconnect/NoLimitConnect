//============================================================================
// Copyright (C) 2009 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "P2PEngine.h"
#include "GuiInterface/IToGui.h"

#include "P2PConnectList.h"
#include "../NetServices/NetServicesMgr.h"
#include <Network/NetworkMgr.h>
#include <Network/NetworkStateMachine.h>

#include <AssetMgr/AssetInfo.h>
#include <AssetMgr/AssetMgr.h>
#include <BigListLib/BigListInfo.h>
#include <BlobXferMgr/BlobMgr.h>

#include <NetworkTest/IsPortOpenTest.h>
#include <NetworkTest/RunUrlAction.h>
#include <MediaProcessor/MediaProcessor.h>
#include <MediaToolsLib/MediaTools.h>

#include <OfferBase/OfferMgr.h>

#include <Plugins/FileToXfer.h>
#include <Plugins/PluginBase.h>
#include <Plugins/PluginFileShareServer.h>
#include <Plugins/PluginNetServices.h>
#include <Plugins/PluginMgr.h>

#include <HostServerJoinMgr/HostServerJoinMgr.h>
#include <SendQueue/SendQueueMgr.h>
#include <UserJoinMgr/UserJoinMgr.h>
#include <UserOnlineMgr/UserOnlineMgr.h>

#include <UrlMgr/UrlMgr.h>

#include <CoreLib/Invite.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxFileShredder.h>
#include <CoreLib/VxParse.h>
#include <CoreLib/VxPtopUrl.h>

#include <NetLib/VxGetRandomPort.h>
#include <NetLib/VxPeerMgr.h>

#include <PktLib/PktsRandConnect.h>

#include <string.h>
#include <stdio.h>

//============================================================================
void P2PEngine::assureUserSpecificDirIsSet( const char* checkReason )
{
	if( false == m_IsUserSpecificDirSet )
	{
		LogMsg( LOG_ERROR, "P2PEngine::assureUserSpecificDirIsSet %s", checkReason );
		vx_assert( false );
	}
}

//============================================================================
uint64_t P2PEngine::fromGuiGetDiskFreeSpace( const char* dir  ) 
{
	if( dir )
	{
		std::string storageFile = dir;
		std::string storageDir;
		std::string fileName;
		VxFileUtil::seperatePathAndFile( storageFile, storageDir, fileName );
		if( !storageDir.empty() )
		{
			return VxFileUtil::getDiskFreeSpace( storageDir.c_str() );
		}
		else
		{
			return 0;
		}
	}
	else
	{
		std::string incompleteDir = VxGetIncompleteDirectory();
		return VxFileUtil::getDiskFreeSpace( incompleteDir.c_str() );
	}
}

//============================================================================
uint64_t P2PEngine::fromGuiClearCache( ECacheType cacheType )
{
	if( eCacheTypeThumbnail == cacheType )
	{
		return m_ThumbMgr.fromGuiClearCache( cacheType );
	}

	return 0;
}

//============================================================================
bool P2PEngine::fromGuiDeleteUser( VxGUID& onlineId )
{
	return getBigListMgr().fromGuiDeleteUser( onlineId );
}

//============================================================================
void P2PEngine::fromGuiAppShutdown( void )
{
	VxSetAppIsShuttingDown( true );
    enableTimerThread( false );
	shutdownEngine();
}

//============================================================================
void P2PEngine::fromGuiDebugSettings( uint32_t u32LogFlags, const char* pLogFileName )
{
	VxSetLogFlags( u32LogFlags );
	if( pLogFileName && strlen( pLogFileName ) )
	{
		VxSetLogToFile( pLogFileName );
	}
}

//============================================================================
void P2PEngine::updateFromEngineSettings( EngineSettings& engineSettings )
{
	getPeerMgr().setUpnpEnable( engineSettings.getUseUpnpPortForward() );

    std::string netHostUrl;
    engineSettings.getNetworkHostUrl( netHostUrl );

    // we need to update the globals so accessable everywhere
    std::string webHostName;
    std::string webFileName;
    uint16_t port = 0;
    VxSplitHostAndFile( netHostUrl.c_str(), webHostName, webFileName, port );
    if( !webHostName.empty() )
    {
        VxSetNetworkHostName( webHostName.c_str() );
        VxSetNetworkHostPort( port );
        VxSetNetworkHostUrl( netHostUrl.c_str() );
    }
    else
    {
        LogMsg( LOG_ERROR, "Empty Network Host Name" );
    }

    getConnectionMgr().applyDefaultHostUrl( eHostTypeNetwork, netHostUrl );

    std::string hostUrl;
	engineSettings.getConnectTestUrl( hostUrl );
	getConnectionMgr().applyDefaultHostUrl( eHostTypeConnectTest, hostUrl );

    // engineSettings.getChatRoomHostUrl( hostUrl );
    // getConnectionMgr().applyDefaultHostUrl( eHostTypeChatRoom, hostUrl );
    //engineSettings.getGroupHostUrl( hostUrl );
    //getConnectionMgr().applyDefaultHostUrl( eHostTypeGroup, hostUrl );
    //engineSettings.getRandomConnectUrl( hostUrl );
    //getConnectionMgr().applyDefaultHostUrl( eHostTypeRandomConnect, hostUrl );

    m_NetworkStateMachine.updateFromEngineSettings( engineSettings );
}

//============================================================================
void P2PEngine::fromGuiOnlineNameChanged( const char* newOnlineName )
{
	m_PktAnn.setOnlineName( newOnlineName );
	doPktAnnHasChanged( false );
}

//============================================================================
void P2PEngine::fromGuiMoodMessageChanged( const char* newMoodMessage )
{
	m_PktAnn.setOnlineDescription( newMoodMessage );
	doPktAnnHasChanged( false );
}

//============================================================================
void P2PEngine::fromGuiIdentPersonalInfoChanged( int age, int gender, int language, int preferredContent )
{
    m_PktAnn.setAgeType( (EAgeType)age );
    m_PktAnn.setGender( gender );
    m_PktAnn.setPrimaryLanguage( language );
    m_PktAnn.setPreferredContent( preferredContent );
    doPktAnnHasChanged( false );
}

//============================================================================
void P2PEngine::fromGuiSetUserHasProfilePicture( bool haveProfilePick )
{
	if( m_PktAnn.hasProfilePicture() != haveProfilePick )
	{
		m_PktAnn.setHasProfilePicture( haveProfilePick );
		doPktAnnHasChanged( false );
	}
}

//============================================================================
void P2PEngine::fromGuiUpdateMyIdent( VxNetIdent* netIdent, bool permissionAndStatsOnly )
{
	if( permissionAndStatsOnly )
	{
		memcpy( m_PktAnn.getPluginPermissions(), netIdent->getPluginPermissions(), PERMISSION_ARRAY_SIZE );
		m_PktAnn.setDareCount( netIdent->getDareCount() );
		m_PktAnn.setTruthCount( netIdent->getTruthCount() );
		m_PktAnn.setRejectedTruthCount( netIdent->getRejectedTruthCount() );
		m_PktAnn.setRejectedDareCount( netIdent->getRejectedDareCount() );
	}
	else
	{
		memcpy( (VxNetIdent*)&m_PktAnn, netIdent, sizeof( VxNetIdent ));
	}

	doPktAnnHasChanged( false );
}

//============================================================================
void P2PEngine::fromGuiSetIdentHasTextOffers( VxGUID& onlineId, bool hasTextOffers )
{
	BigListInfo * bigListInfo = m_BigListMgr.findBigListInfo( onlineId );
	if( bigListInfo )
	{
		if( bigListInfo->getHasTextOffers() != hasTextOffers )
		{
			bigListInfo->setHasTextOffers( hasTextOffers );
			m_BigListMgr.updateBigListDatabase( bigListInfo, getNetworkMgr().getNetworkKey() );
		}
	}
}

//============================================================================
void P2PEngine::fromGuiQueryMyIdent( VxNetIdent* poRetIdent )
{
	memcpy( poRetIdent, (VxNetIdent*)&m_PktAnn, sizeof( VxNetIdent ) );
}

//============================================================================
void P2PEngine::fromGuiAppPause( void )
{
	m_AppIsPaused = true;
	m_PluginMgr.fromGuiAppPause();
}

//============================================================================
void P2PEngine::fromGuiAppResume( void )
{
	m_AppIsPaused = false;
	m_PluginMgr.fromGuiAppResume();
}

//============================================================================
void P2PEngine::fromGuiNetworkAvailable( const char* lclIp, bool isCellularNetwork )
{
	m_NetworkStateMachine.fromGuiNetworkAvailable( lclIp, isCellularNetwork );
}

//============================================================================
void P2PEngine::fromGuiNetworkLost( void )
{
	m_NetworkStateMachine.fromGuiNetworkLost();
}

//============================================================================
ENetLayerState P2PEngine::fromGuiGetNetLayerState( ENetLayerType netLayer )
{
    return m_NetworkStateMachine.fromGuiGetNetLayerState( netLayer );
}

//============================================================================
void P2PEngine::fromGuiNetworkSettingsChanged( void )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiNetworkSettingsChanged" );
	m_NetworkStateMachine.fromGuiNetworkSettingsChanged();
}

//============================================================================
//=== user gui input actions ===//
//============================================================================
//! called after processed HandleOrientationEvent for derived classes to override
bool P2PEngine::fromGuiOrientationEvent( float f32RotX, float f32RotY, float f32RotZ  )
{
	return false;
}

//============================================================================
//! called after processed HandleMouseEvent for derived classes to override
bool P2PEngine::fromGuiMouseEvent( EMouseButtonType eMouseButType, EMouseEventType eMouseEventType, int iMouseXPos, int iMouseYPos  )
{
	return false;
}

//============================================================================
//! called after processed HandleMouseWheel for derived classes to override
bool P2PEngine::fromGuiMouseWheel( float f32MouseWheelDist )
{
	return false;
}

//============================================================================
//! called after processed HandleKeyEvent for derived classes to override
bool P2PEngine::fromGuiKeyEvent( EKeyEventType eKeyEventType, EKeyCode eKey, int iFlags )
{
	return false;
}

//============================================================================
void P2PEngine::fromGuiNativeGlInit( void )
{
}

//============================================================================
//! resize window
void P2PEngine::fromGuiNativeGlResize( int w, int h )
{
}

//============================================================================
//! called to render the next frame
int  P2PEngine::fromGuiNativeGlRender( void )
{
	return 0;
}

//============================================================================
//! called to pause the render loop
void P2PEngine::fromGuiNativeGlPauseRender( void )
{
}

//============================================================================
//! called to resume the render loop
void P2PEngine::fromGuiNativeGlResumeRender( void )
{
}

//============================================================================
//! called when game window is being destroyed
void P2PEngine::fromGuiNativeGlDestroy( void )
{
}

//============================================================================
void P2PEngine::fromGuiVideoData( uint32_t u32FourCc, uint8_t * pu8VidDataIn, int iWidth, int iHeight, uint32_t u32VidDataLen, int iRotation )
{
	m_MediaProcessor.fromGuiVideoData( u32FourCc, pu8VidDataIn, iWidth, iHeight, u32VidDataLen, iRotation );
}

//============================================================================
bool P2PEngine::fromGuiSndRecord( ESndRecordState eRecState, VxGUID& feedId, const char* fileName )
{
	return m_MediaProcessor.getMediaTools().fromGuiSndRecord( eRecState, feedId, fileName );
}

//============================================================================
bool P2PEngine::fromGuiAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo, int pos0to100000 )
{
	AssetBaseInfo* createdAssetInfo = nullptr;
	if( eAssetActionAddAssetAndSend == assetAction && assetInfo.getPluginType() == ePluginTypePersonalRecorder )
	{
		// never send personal recordings
		assetAction = eAssetActionAddToAssetMgr;
	}

	if( eAssetActionAddToAssetMgr == assetAction )
	{
		return m_AssetMgr.addAsset( assetInfo, createdAssetInfo );
	}
	else if( eAssetActionAddAssetAndSend == assetAction )
	{
		assetInfo.setAssetSendState( eAssetSendStateTxProgress );
		bool result = m_AssetMgr.addAsset( assetInfo, createdAssetInfo );
        if( false == result )
        {
            LogMsg( LOG_ERROR, "PEngine::fromGuiAssetAction failed to add asset" );
			return false;
        }

		if( createdAssetInfo )
		{
			return fromGuiSendAsset( *createdAssetInfo );
		}	
	}
	else if( eAssetActionAssetSend == assetAction )
	{
		assetInfo.setAssetSendState( eAssetSendStateTxProgress );
		IToGui::getToGui().toGuiAssetAction( eAssetActionTxBegin, assetInfo.getAssetUniqueId(), 0 );
		return fromGuiSendAsset( assetInfo );
	}
	else if( eAssetActionAssetResend == assetAction )
	{
		assetInfo.setAssetSendState( eAssetSendStateTxProgress );
		IToGui::getToGui().toGuiAssetAction( eAssetActionTxBegin, assetInfo.getAssetUniqueId(), 0 );
		return fromGuiSendAsset( assetInfo );
	}
	else if( eAssetActionShreadFile == assetAction )
	{
		bool isFileAsset = assetInfo.isAudioAsset() || assetInfo.isPhotoAsset() || assetInfo.isVideoAsset();
		std::string fileName = assetInfo.getAssetName();
		if( isFileAsset )
		{
            FileInfo fileInfo = assetInfo.getFileInfo();
            fromGuiRemoveSharedFile( fileInfo );
			fromGuiRemoveFromLibrary( fileName );	
		}

		m_AssetMgr.removeAsset( assetInfo.getAssetUniqueId(), false );
		if( isFileAsset )
		{
			GetVxFileShredder().shredFile( fileName );
		}

		return true;
	}
	else if( eAssetActionRemoveFromAssetMgr == assetAction )
	{
		m_AssetMgr.removeAsset( assetInfo.getAssetUniqueId() );
		return true;
	}
	else if( eAssetActionPlayOneFrame == assetAction )
	{
		// takes too much time doing all the file io so queue the command instead
		getFromGuiMgr().fromGuiPlayOneFrame( assetInfo );
		return true;
	}

	return m_MediaProcessor.getMediaTools().fromGuiAssetAction( assetInfo, assetAction, pos0to100000 );
}

//============================================================================
bool P2PEngine::fromGuiQueueAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo, int pos0to100000 )
{
	assetInfo.setAssetSendState( eAssetSendStateQueued );
	AssetBaseInfo* createdAssetInfo = nullptr;
	bool result = getSendQueueMgr().updateSendQueue( assetInfo.getSendToId(), assetInfo.getAssetUniqueId(), eSendQueStateWaiting );
	if( result )
	{		
		result = m_AssetMgr.addAsset( assetInfo, createdAssetInfo );
	}

	return result;
}

//============================================================================
bool P2PEngine::fromGuiAssetAction( EPluginType pluginType, EAssetAction assetAction, VxGUID& assetId, int pos0to100000 )
{
	if( eAssetActionAddToAssetMgr == assetAction )
	{
		LogMsg( LOG_ERROR, "fromGuiAssetAction Insufficient asset info to add asset" );
		return false;
	}

	//TODO figure out lock protection here... should we make copy? would be slow
	AssetInfo* assetInfo =  dynamic_cast<AssetInfo*>(m_AssetMgr.findAsset( assetId ));
	if( assetInfo )
	{
		return fromGuiAssetAction( assetAction, *assetInfo, pos0to100000 );
	}

	return false;
}

//============================================================================
bool P2PEngine::fromGuiVideoRecord( EVideoRecordState eRecState, VxGUID& feedId, const char* fileName   )
{
	return m_MediaProcessor.getMediaTools().fromGuiVideoRecord( eRecState, feedId, fileName );
}

//! play video or audio file
//============================================================================
bool P2PEngine::fromGuiPlayLocalMedia( const char* fileName, uint64_t fileLen, uint8_t fileType, int pos0to100000 )
{
	return fromGuiPlayLocalMedia( fileName, fileLen, fileType, VxGUID::nullVxGUID(), pos0to100000 );
}

//============================================================================
bool P2PEngine::fromGuiPlayLocalMedia( const char*  fileName, uint64_t fileLen, uint8_t fileType, VxGUID assetId, int pos0to100000  )
{
    std::string fileNameStr = fileName;
    bool result = true;
    EAssetType assetType = VxFileTypeToAssetType( fileType );
    if( !fileNameStr.empty() &&  fileLen && eAssetTypeUnknown != assetType )
    {
        AssetInfo * assetInfo =  dynamic_cast<AssetInfo*>(getAssetMgr().findAsset( fileNameStr ));
        if( 0 == assetInfo )
        {
			if( assetId.isVxGUIDValid() )
			{
				assetInfo = dynamic_cast<AssetInfo*>(getAssetMgr().addAssetFile( assetType, fileName, fileLen, assetId ));
			}
			else
			{
				assetInfo = dynamic_cast<AssetInfo*>(getAssetMgr().addAssetFile( assetType, fileName, fileLen ));
			}

            assetInfo->setPlayPosition( pos0to100000 );
        }

        if( 0 == assetInfo )
        {
            LogMsg( LOG_ERROR, "P2PEngine::fromGuiPlayLocalMedia INVALID PARAM" );
            result = false;
        }
        else
        {
            if( eAssetTypeVideo == assetInfo->getAssetType() )
            {
                if( fromGuiIsNoLimitVideoFile( fileName ) )
                {
                    fromGuiAssetAction( eAssetActionPlayBegin, *assetInfo, assetInfo->getPlayPosition() );
                }
                else
                {
                    IToGui::getToGui().toGuiPlayNlcMedia( assetInfo );
                }
            }
            else if( eAssetTypeAudio == assetInfo->getAssetType() )
            {
                if( fromGuiIsNoLimitAudioFile( fileName ) ) 
                {
                    fromGuiAssetAction( eAssetActionPlayBegin, *assetInfo, assetInfo->getPlayPosition() );
                }
                else
                {
                    IToGui::getToGui().toGuiPlayNlcMedia( assetInfo );
                }
            }
			else if( eAssetTypePhoto == assetInfo->getAssetType() )
			{
				IToGui::getToGui().toGuiPlayNlcMedia( assetInfo );
			}
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "P2PEngine::fromGuiPlayLocalMedia INVALID PARAM" );
        result = false;
    }

    return result;
}

//============================================================================
//! update web page profile
void P2PEngine::fromGuiUpdateWebPageProfile(	const char*	pProfileDir,	// directory containing user profile	
												const char*	pGreeting,		// greeting text
												const char*	pAboutMe,		// about me text
												const char*	url1,			// favorite url 1
												const char*	url2,			// favorite url 2
												const char*	url3, 			// favorite url 3
                                                const char*	donation )	    // donation information
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiUpdateWebPageProfile" );
	std::string strWebPageHdr = "";
	StdStringFormat( strWebPageHdr, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"><html>\
<head><meta http-equiv=\"Content-Type\" content=\"text/html;charset=iso-8859-1\" name=\"description\" content=\"%s :: About Me Web Page - Share introduction information about yourself in a personal web page served on your own device with access allowed or denied by friendship permission level\">\
<FONT COLOR=\"#0000FF\"><title>%s :: About Me</title></FONT></head><body bgcolor=\"#E3EFFF\"><h2><p align=\"center\"><font color=\"#007F00\">%s - %s - About Me</font></p></h2>",
		VxGetApplicationTitle(), VxGetApplicationTitle(), VxGetApplicationNameNoSpaces(), m_PktAnn.getOnlineName() );

	std::string strGreeting = "";
	if( strlen( pGreeting ))
	{
		StdStringFormat( strGreeting, "<h4><p align=\"center\">%s</p></h4>", pGreeting );
	}

	std::string strAbout		= "";
	std::string strPicLabel		= "<h3><p align=\"center\">My Picture</p></h3>";
	std::string strPicture		= "<p align=\"center\"><IMG SRC = \"me.png\" width=\"320\" height=\"240\"><br></p>";
	std::string strFavWebsites	= "</h2><p align=\"center\"><font color=\"#007F00\">My Favorite Web Sites</font><br></p></h2>";
	std::string strUrl1			= "";
	std::string strUrl2			= "";
	std::string strUrl3			= "";
    std::string strDonationInfo = "</h2><p align=\"center\"><font color=\"#007F00\">Donation Information</font><br></p></h2>";
    std::string strDonation = "";

	if( 0 !=  strlen( pGreeting ) )
	{
		StdStringFormat( strGreeting, "<h4><p align=\"center\">%s</p></h4>", pGreeting );    		
	}

	if( 0 !=  strlen( pAboutMe ) )
	{
		StdStringFormat( strAbout,  "<h4><p align=\"center\">%s</p></h4>", pAboutMe );
	}

	if( 0 !=  strlen( url1 ) )
	{
		StdStringFormat( strUrl1, "<h5><p align=\"center\"><font color=\"#0000FF\"><a href=\"%s\">%s</a></font><br></p><h5>", url1, url1 );
	}

	if( 0 !=  strlen( url2 ) )
	{
		StdStringFormat( strUrl2, "<h5><p align=\"center\"><font color=\"#0000FF\"><a href=\"%s\">%s</a></font><br></p><h5>", url2, url2 );
	}

	if( 0 !=  strlen( url3 ) )
	{
		StdStringFormat( strUrl3, "<h5><p align=\"center\"><font color=\"#0000FF\"><a href=\"%s\">%s</a></font><br></p><h5>", url3, url3 );
	}

    if( 0 != strlen( donation ) )
    {
        StdStringFormat( strDonation, "<h4><p align=\"center\">%s</p></h4>", donation );
    }


	std::string strTrailer = "</body></html>\r\r\r";

	std::string strWebPage;
    strWebPage = strWebPageHdr + strGreeting + strAbout + strPicLabel + strPicture + strFavWebsites + strUrl1 + strUrl2 + strUrl3 + strDonation + strTrailer;

	std::string strWebFile = pProfileDir;
	strWebFile += "index.htm";

	VxFileUtil::writeWholeFile( strWebFile.c_str(), (void *)strWebPage.c_str(), (uint32_t)(strWebPage.length() + 1) );
}

//============================================================================
void P2PEngine::fromGuiStartPluginSession( EPluginType pluginType, VxGUID oOnlineId, int pvUserData, VxGUID lclSessionId )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiStartPluginSession" );
	m_PluginMgr.fromGuiStartPluginSession( pluginType, oOnlineId, pvUserData, lclSessionId );
}

//============================================================================
void P2PEngine::fromGuiStopPluginSession( EPluginType pluginType, VxGUID oOnlineId, int pvUserData, VxGUID lclSessionId )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiStopPluginSession" );
	m_PluginMgr.fromGuiStopPluginSession( pluginType, oOnlineId, pvUserData, lclSessionId );
}

//============================================================================
bool P2PEngine::fromGuiIsPluginInSession( EPluginType pluginType,VxGUID& onlineId, int pvUserData, VxGUID lclSessionId )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiIsPluginInSession" );
	if( ( false == m_IsUserSpecificDirSet ) || VxIsAppShuttingDown() )
	{
		// wait until some things are started
		return false;	
	}

	return m_PluginMgr.fromGuiIsPluginInSession( pluginType, onlineId, pvUserData, lclSessionId );
}

//============================================================================
void P2PEngine::fromGuiSetPluginPermission( EPluginType pluginType, EFriendState eFriendState )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiSetPluginPermission" );
	EFriendState eCurFriendState = m_PktAnn.getPluginPermission( pluginType );
	if( eCurFriendState != eFriendState )
	{
		m_PluginMgr.setPluginPermission( pluginType, eFriendState );
		m_PktAnn.setPluginPermission( pluginType, eFriendState );
		doPktAnnHasChanged( false );
	}
}

//============================================================================
int P2PEngine::fromGuiGetPluginPermission( EPluginType pluginType )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiGetPluginPermission" );
	return m_PktAnn.getPluginPermission( pluginType );
}

//============================================================================
EPluginServerState P2PEngine::fromGuiGetPluginServerState( EPluginType pluginType )
{
	if( eFriendStateIgnore == m_PktAnn.getPluginPermission( pluginType ) )
	{
		return ePluginServerStateDisabled;
	}

	return m_PluginMgr.fromGuiIsPluginInSession( pluginType, getMyOnlineId() ) ? ePluginServerStateStarted : ePluginServerStateStopped;
}

//============================================================================
//! called with offer to create session.. return false if cannot connect
bool P2PEngine::fromGuiMakePluginOffer( VxGUID& onlineId, OfferBaseInfo& offerInfo )
{
	offerInfo.setDestUserId( onlineId );
	VxNetIdent* netIdent = m_BigListMgr.findNetIdent( onlineId );
	PluginBase* poPlugin = m_PluginMgr.getPlugin( offerInfo.getPluginType() );
	if( netIdent && poPlugin )
	{
		return poPlugin->fromGuiMakePluginOffer( onlineId, offerInfo );
	}
	else
	{
		LogMsg(LOG_ERROR, "P2PEngine::fromGuiMakePluginOffer: poInfo not found VxGUID %s", onlineId.toHexString().c_str());
	}

	return false;
}

//============================================================================
//! handle reply to offer
bool P2PEngine::fromGuiToPluginOfferReply( VxGUID& onlineId, OfferBaseInfo& offerInfo )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiToPluginOfferReply" );
	if( VxIsAppShuttingDown() )
	{
		return false;
	}

	VxNetIdent* netIdent = m_BigListMgr.findNetIdent( onlineId );
	PluginBase* poPlugin = m_PluginMgr.getPlugin( offerInfo.getPluginType() );
	if( netIdent && poPlugin )
	{
		return poPlugin->fromGuiOfferReply( onlineId, offerInfo );
	}
	else
	{
		LogMsg( LOG_ERROR, "ERROR P2PEngine::fromGuiToPluginOfferReply invalid plugin or info" );
		return false;
	}
}

//============================================================================
EXferError P2PEngine::fromGuiFileXferControl( EPluginType pluginType, EXferAction xferAction, FileInfo& fileInfo )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiFileXferControl" );
	PluginBase* poPlugin = m_PluginMgr.getPlugin( pluginType );
	if( poPlugin )
	{
		return poPlugin->fromGuiFileXferControl( fileInfo.getOnlineId(), xferAction, fileInfo );
	}
	else
	{
		LogMsg( LOG_ERROR, "ERROR P2PEngine::fromGuiFileXferControl invalid plugin" );
		return eXferErrorBadParam;
	}
}

//============================================================================
bool P2PEngine::fromGuiInstMsg(	EPluginType	pluginType, VxGUID&	onlineId, const char* pMsg )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiInstMsg" );

	PluginBase* poPlugin = m_PluginMgr.getPlugin( pluginType );
	if( poPlugin )
	{
		return poPlugin->fromGuiInstMsg( onlineId, pMsg );
	}
	else
	{
		LogMsg( LOG_ERROR, "ERROR P2PEngine::fromGuiInstMsg invalid plugin" );
		return false;
	}
}

//============================================================================
bool P2PEngine::fromGuiPushToTalk( VxGUID& onlineId, bool enableTalk )
{
	PluginBase* poPlugin = m_PluginMgr.getPlugin( ePluginTypePushToTalk );
	if( poPlugin )
	{
		return poPlugin->fromGuiPushToTalk( onlineId, enableTalk );
	}
	else
	{
		LogMsg( LOG_ERROR, "ERROR P2PEngine::fromGuiPushToTalk invalid plugin or user" );
		return false;
	}
}

//============================================================================
bool P2PEngine::isSystemPlugin( EPluginType	pluginType )
{
	//assureUserSpecificDirIsSet( "P2PEngine::isSystemPlugin" );
	if( ( ePluginTypeInvalid < pluginType ) && ( eMaxUserPluginType > pluginType ) )
	{
		return false;
	}

	return true;
}

//============================================================================
bool P2PEngine::isUserConnected( VxGUID& onlineId )
{
	return m_ConnectIdListMgr.isUserOnline( onlineId );
}

//============================================================================
bool P2PEngine::isNearbyAvailable( void )
{
	return m_NetStatusAccum.getNearbyAvailable();
}

//============================================================================
bool P2PEngine::isInternetAvailable( void )
{
	return m_NetStatusAccum.isInternetAvailable();
}

//============================================================================
bool P2PEngine::isP2POnline( void )
{ 
	//assureUserSpecificDirIsSet( "P2PEngine::isP2POnline" );
	return m_NetworkStateMachine.isP2POnline(); 
}

//============================================================================
bool P2PEngine::isDirectConnectTested( void )
{
	return m_NetStatusAccum.isDirectConnectTested();
}

//============================================================================
bool P2PEngine::getIsMyHostServiceEnabled( EHostServiceType hostService )
{
    switch( hostService )
    {
    case eHostServiceNetworkHost: return m_PktAnn.getPluginPermission( ePluginTypeHostNetwork ) != eFriendStateIgnore;
    case eHostServiceGroupListing: return m_PktAnn.getPluginPermission( ePluginTypeNetworkSearchList ) != eFriendStateIgnore;
    case eHostServiceGroup: return m_PktAnn.getPluginPermission( ePluginTypeHostGroup ) != eFriendStateIgnore;
    case eHostServiceRelay: return m_PktAnn.getPluginPermission( ePluginTypeHostGroup ) != eFriendStateIgnore;
	case eHostServiceConnectTest: return m_PktAnn.getPluginPermission( ePluginTypeHostConnectTest ) != eFriendStateIgnore;
	case eHostServiceChatRoom: return m_PktAnn.getPluginPermission( ePluginTypeHostChatRoom ) != eFriendStateIgnore;
    case eHostServiceRandomConnect: return m_PktAnn.getPluginPermission( ePluginTypeHostRandomConnect ) != eFriendStateIgnore;
    case eHostServiceRandomConnectRelay: return m_PktAnn.getPluginPermission( ePluginTypeHostRandomConnect ) != eFriendStateIgnore;
    default:
        break;
    }

    return false;
}

//============================================================================
bool P2PEngine::getIsMyHostServiceEnabled( EHostType hostService )
{
	switch( hostService )
	{
	case eHostTypeNetwork: return m_PktAnn.getPluginPermission( ePluginTypeHostNetwork ) != eFriendStateIgnore;
	case eHostTypeGroup: return m_PktAnn.getPluginPermission( ePluginTypeHostGroup ) != eFriendStateIgnore;
	case eHostTypeConnectTest: return m_PktAnn.getPluginPermission( ePluginTypeHostConnectTest ) != eFriendStateIgnore;
	case eHostTypeChatRoom: return m_PktAnn.getPluginPermission( ePluginTypeHostChatRoom ) != eFriendStateIgnore;
	case eHostTypeRandomConnect: return m_PktAnn.getPluginPermission( ePluginTypeHostRandomConnect ) != eFriendStateIgnore;
	default:
		break;
	}

	return false;
}

//============================================================================
bool P2PEngine::getHasAnyAnnonymousHostService( void )
{
	bool hasAnnonService{ false };

	hasAnnonService |= m_PktAnn.getPluginPermission( ePluginTypeHostGroup ) == eFriendStateAnonymous;
	hasAnnonService |= m_PktAnn.getPluginPermission( ePluginTypeHostChatRoom ) == eFriendStateAnonymous;
	hasAnnonService |= m_PktAnn.getPluginPermission( ePluginTypeHostRandomConnect ) == eFriendStateAnonymous;
	hasAnnonService |= m_PktAnn.getPluginPermission( ePluginTypeHostNetwork ) == eFriendStateAnonymous;
	hasAnnonService |= m_PktAnn.getPluginPermission( ePluginTypeHostConnectTest ) == eFriendStateAnonymous;

	return hasAnnonService;
}

//============================================================================
bool P2PEngine::getHasFixedIpAddress( void )
{
    if( eFirewallTestAssumeNoFirewall == getEngineSettings().getFirewallTestSetting() )
    {
        std::string externIp;
        getEngineSettings().getUserSpecifiedExternIpAddr( externIp );
        if( externIp.empty() )
        {
            LogMsg( LOG_WARN, "Firewall assume no Firewall set but external ip is empty" );
        }

        return true;
    }

    return false;
}

//============================================================================
void P2PEngine::fromGuiMuteMicrophone(	bool muteMic )
{
	m_MediaProcessor.muteMicrophone( muteMic );
}

//============================================================================
bool P2PEngine::fromGuiIsMicrophoneMuted( void )
{
	return m_MediaProcessor.isMicrophoneMuted();
}

//============================================================================
void P2PEngine::fromGuiMuteSpeaker(	bool muteSpeaker )
{
	m_MediaProcessor.muteSpeaker( muteSpeaker );
}

//============================================================================
bool P2PEngine::fromGuiIsSpeakerMuted( void )
{
	return m_MediaProcessor.isSpeakerMuted();
}

//============================================================================
void P2PEngine::fromGuiWantMediaInput( EMediaInputType mediaType, MediaCallbackInterface * callback, EAppModule appModule, bool wantInput )
{
	if( false == VxIsAppShuttingDown() )
	{
		m_MediaProcessor.wantMediaInput( mediaType, callback, appModule, wantInput );
	}
}

//============================================================================
void P2PEngine::fromGuiWantMediaInput( VxGUID& onlineId, EMediaInputType mediaType, EAppModule appModule, bool wantInput )
{
	if( false == VxIsAppShuttingDown() )
	{
		BigListInfo * poInfo = m_BigListMgr.findBigListInfo( onlineId );
		if( poInfo )
		{
			if( ( eMediaInputVideoJpgSmall !=  mediaType ) // no need to activate cam if requesting other person's video feed
				&& ( eMediaInputVideoJpgBig !=  mediaType ) )
			{			
				m_MediaProcessor.wantMediaInput( mediaType, this, appModule, wantInput );
			}
		}
		else
		{
			if( wantInput )
			{
				m_MediaProcessor.setMyIdInVidPktListCount( m_MediaProcessor.getMyIdInVidPktListCount() + 1 );
			}
			else
			{
				if( m_MediaProcessor.getMyIdInVidPktListCount() )
				{
					m_MediaProcessor.setMyIdInVidPktListCount( m_MediaProcessor.getMyIdInVidPktListCount() - 1 );
				}
			}

			m_MediaProcessor.wantMediaInput( mediaType, this, appModule, wantInput );
		}
	}
}

//============================================================================
bool P2PEngine::fromGuiChangeMyFriendshipToHim(	VxGUID&			onlineId, 
											    EFriendState	myFriendshipToHim,
											    EFriendState	hisFriendshipToMe )
{
	if( false == VxIsAppShuttingDown() )
	{
		if( !onlineId.isVxGUIDValid() )
		{
			LogMsg( LOG_ERROR, "IgnoreListMgr::updateIgnoreIdent invalid id" );
			return false;
		}

		//assureUserSpecificDirIsSet( "P2PEngine::fromGuiChangeMyFriendshipToHim" );
		BigListInfo * poInfo = m_BigListMgr.findBigListInfo( onlineId );
		if( poInfo )
		{
			if( onlineId != getMyOnlineId() )
			{
				updateIdentLists( poInfo );
			}
			
			EFriendState eOldFriendship = poInfo->getMyFriendshipToHim();
			poInfo->setMyFriendshipToHim( myFriendshipToHim );
			m_BigListMgr.updateVectorList( eOldFriendship, poInfo );

			BigListInfo* dummyBigListInfo = nullptr;
			EHostType hostType{ eHostTypeUnknown };
			m_BigListMgr.updatePktAnn( poInfo->getPktAnnounce(), &dummyBigListInfo, hostType, true );

			LogMsg( LOG_VERBOSE, "P2PEngine::fromGuiChangeMyFriendshipToHim: SUCCESS changed %s friendship to %s", 
				poInfo->getOnlineName(),
				poInfo->describeMyFriendshipToHim());

			getConnectIdListMgr().updateOnlineExclusion( onlineId, myFriendshipToHim == eFriendStateIgnore );

			m_ConnectionList.fromGuiChangeMyFriendshipToHim( onlineId,
																myFriendshipToHim,
																hisFriendshipToMe );
			return true;
		}
		else
		{
			LogMsg( LOG_ERROR, "P2PEngine::fromGuiChangeMyFriendshipToHim: FAILED find friend %s", onlineId.toOnlineIdString().c_str() );
		}
	}

	return false;
}

//============================================================================
//! network test failed.. requires proxy service from friend
void P2PEngine::fromGuiRequireRelay( bool bRequireRelay )
{
	if( false == VxIsAppShuttingDown() )
	{
		//assureUserSpecificDirIsSet( "P2PEngine::fromGuiRequireRelay" );
		if( m_PktAnn.requiresRelay() != bRequireRelay )
		{
			 m_PktAnn.setRequiresRelay( bRequireRelay );
			 doPktAnnHasChanged( false );
		}
	}
}

//============================================================================
void P2PEngine::fromGuiRelayPermissionCount( int userPermittedCount, int anonymousCount )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiRelayPermissionCount" );
	m_PluginMgr.fromGuiRelayPermissionCount( userPermittedCount, anonymousCount );
}

//============================================================================
InetAddress P2PEngine::fromGuiGetMyIPv4Address( void )
{
	return VxGetDefaultIPv4Address();
}

//============================================================================
InetAddress P2PEngine::fromGuiGetMyIPv6Address( void )
{
	return VxGetMyGlobalIPv6Address();
}

//============================================================================
void P2PEngine::fromGuiUserModifiedStoryboard( void )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiUserModifiedStoryboard" );
	m_PktAnn.setHasModifiedStoryboard( true );
	doPktAnnHasChanged( false );
}

//============================================================================
void P2PEngine::fromGuiCancelDownload( VxGUID& fileInstance )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiCancelDownload" );
	m_PluginMgr.getPlugin(ePluginTypePersonFileXfer)->fromGuiCancelDownload( fileInstance );
	m_PluginMgr.getPlugin(ePluginTypeFileShareServer)->fromGuiCancelDownload( fileInstance );
}

//============================================================================
void P2PEngine::fromGuiCancelUpload( VxGUID& fileInstance )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiCancelUpload" );
	m_PluginMgr.getPlugin(ePluginTypePersonFileXfer)->fromGuiCancelUpload( fileInstance );
	m_PluginMgr.getPlugin(ePluginTypeFileShareServer)->fromGuiCancelUpload( fileInstance );
}

//============================================================================
void P2PEngine::fromGuiGetNetSettings( NetSettings& netSettings )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiGetNetSettings" );
	m_EngineSettings.getNetSettings( netSettings );
}

//============================================================================
void P2PEngine::fromGuiApplyNetHostSettings( NetHostSetting& netHostSetting )
{
	getPeerMgr().setUpnpEnable( netHostSetting.getUseUpnpPortForward() );

    NetHostSetting origSettings;
    m_EngineSettings.getNetHostSettings( origSettings );

    if( origSettings != netHostSetting )
    {
		bool haveFixedIp{ false };
        m_EngineSettings.setNetHostSettings( netHostSetting );
        if( origSettings.getUserSpecifiedExternIpAddr() != netHostSetting.getUserSpecifiedExternIpAddr() )
        {
            if( eFirewallTestAssumeNoFirewall == netHostSetting.getFirewallTestType() && !netHostSetting.getUserSpecifiedExternIpAddr().empty() )
            {
				getMyPktAnnounce().setOnlineIpAddress( false, netHostSetting.getUserSpecifiedExternIpAddr().c_str() );
                setPktAnnLastModTime( GetTimeStampMs() );
				haveFixedIp = true;
            }
        }

        if( origSettings.getTcpPort() != netHostSetting.getTcpPort() )
        {
            getPeerMgr().stopListening( false );
            getMyPktAnnounce().setMyOnlinePort( netHostSetting.getTcpPort() );
            setPktAnnLastModTime( GetTimeStampMs() );
            getNetStatusAccum().setIpPort( netHostSetting.getTcpPort() );
            getPeerMgr().startListening( false, netHostSetting.getTcpPort() );   
        }

		if( haveFixedIp )
		{
            std::string myOnlineUrl = getMyPktAnnounce().getMyOnlineUrl( false );
            getUrlMgr().setMyOnlineNodeUrl( false, myOnlineUrl );
		}

        if( origSettings.getNetworkHostUrl() != netHostSetting.getNetworkHostUrl() )
        {
            getConnectionMgr().applyDefaultHostUrl( eHostTypeNetwork, netHostSetting.getNetworkHostUrl() );
        }

        if( origSettings.getConnectTestUrl() != netHostSetting.getConnectTestUrl() )
        {
            getConnectionMgr().applyDefaultHostUrl( eHostTypeConnectTest, netHostSetting.getConnectTestUrl() );
        }

        if( origSettings.getRandomConnectUrl() != netHostSetting.getRandomConnectUrl() )
        {
            getConnectionMgr().applyDefaultHostUrl( eHostTypeRandomConnect, netHostSetting.getRandomConnectUrl() );
        }

        if( origSettings.getGroupHostUrl() != netHostSetting.getGroupHostUrl() )
        {
            getConnectionMgr().applyDefaultHostUrl( eHostTypeGroup, netHostSetting.getGroupHostUrl() );
        }

        if( origSettings.getChatRoomHostUrl() != netHostSetting.getChatRoomHostUrl() )
        {
            getConnectionMgr().applyDefaultHostUrl( eHostTypeChatRoom, netHostSetting.getChatRoomHostUrl() );
        }

        fromGuiNetworkSettingsChanged();
    }
}

//============================================================================
void P2PEngine::fromGuiSetNetSettings( NetSettings& netSettings )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiSetNetSettings" );
	m_EngineSettings.setNetSettings( netSettings );
}

//============================================================================
void P2PEngine::fromGuiSetRelaySettings( int userRelayMaxCnt, int systemRelayMaxCnt )
{
	m_EngineSettings.setMaxRelaysInUse( userRelayMaxCnt, systemRelayMaxCnt );
}

//============================================================================
void P2PEngine::fromGuiGetFileShareSettings( FileShareSettings& fileShareSettings )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiGetFileShareSettings" );
	m_PluginMgr.getPlugin(ePluginTypeFileShareServer)->fromGuiGetFileShareSettings( fileShareSettings );
}

//============================================================================
void P2PEngine::fromGuiSetFileShareSettings( FileShareSettings& fileShareSettings )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiSetFileShareSettings" );
	m_PluginMgr.getPlugin(ePluginTypeFileShareServer)->fromGuiSetFileShareSettings( fileShareSettings );
}

//============================================================================
bool P2PEngine::fromGuiSetGameValueVar(	EPluginType		pluginType, 
										VxGUID&			onlineId, 
										int32_t			varId, 
										int32_t			varValue )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiSetGameValueVar" );
	PluginBase* poPlugin = m_PluginMgr.getPlugin( pluginType );
	if( poPlugin )
	{
		return poPlugin->fromGuiSetGameValueVar( onlineId, varId, varValue );
	}
	else
	{
		LogMsg( LOG_ERROR, "P2PEngine::fromGuiSetGameValueVar: could not locate idenitiy");
	}

	return false;
}

//============================================================================
bool P2PEngine::fromGuiSetGameActionVar(	EPluginType		pluginType, 
											VxGUID&			onlineId, 
											int32_t			varId, 
											int32_t			varValue )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiSetGameActionVar" );
	PluginBase* poPlugin = m_PluginMgr.getPlugin( pluginType );
	if( poPlugin )
	{
		return poPlugin->fromGuiSetGameActionVar( onlineId, varId, varValue );
	}
	else
	{
		LogMsg( LOG_ERROR, "P2PEngine::fromGuiSetGameActionVar: could not locate idenitiy");
	}

	return false;
}

//============================================================================
bool P2PEngine::fromGuiTestCmd(	ETestParam1		eTestParam1, 
								int				testParam2, 
								const char*		testParam3 )
{
	//assureUserSpecificDirIsSet( "P2PEngine::fromGuiTestCmd" );
	bool result = false;
	switch( eTestParam1 )
	{
	case eTestParam1FullNetTest1:
		{
			//NetSettings netSettings;
			//m_EngineSettings.getNetSettings( netSettings );
			//fromGuiTestNetwork( netSettings );
		}
		break;

	case eTestParam1WhatsMyIp:
		{
			//void						queryWhatsMyIp( void );
		}
		break;

	case eTestParam1IsMyPortOpen:
		{
			//m_PluginNetServices.testIsMyPortOpen();
		}
		break;

	//case eTestParam1AnnounceNow:
	//	{
	//		m_NetServicesMgr.announceToHost( m_NetworkStateMachine.getHostIp(), m_NetworkStateMachine.getHostPort() );
	//	}
	//	break;

	case eTestParamSoundDelayTest:
		{
			// m_MediaProcessor.fromGuiSoundDelayTest();
		}
		break;

	default:
		LogMsg( LOG_INFO, "Unknown eTestParam1 %d", eTestParam1 );
	}

	return result;
}

//============================================================================
uint16_t P2PEngine::fromGuiGetRandomTcpPort( void )
{
	return VxGetRandomTcpPort();
}

/// Get url for this node
//============================================================================
void P2PEngine::fromGuiGetNodeUrl( bool ipv6, std::string& nodeUrl )
{
    nodeUrl = getMyOnlineUrl( ipv6 );
}

//============================================================================
/// Get internet status
EInternetStatus P2PEngine::fromGuiGetInternetStatus( void )
{
    return getNetStatusAccum().getInternetStatus();
}

//============================================================================
/// Get network status
ENetAvailStatus P2PEngine::fromGuiGetNetAvailStatus( void )
{
    return getNetStatusAccum().getNetAvailStatus();
}

//============================================================================
bool P2PEngine::fromGuiNearbyBroadcastEnable( bool enable )
{
#if ENABLE_COMPONENT_NEARBY
	return m_NetworkMgr.getNearbyMgr().fromGuiNearbyBroadcastEnable( enable );
#else
	return false;
#endif // ENABLE_COMPONENT_NEARBY
}

//============================================================================
void P2PEngine::fromGuiAnnounceHost( HostedId& adminId, VxGUID& sessionId, std::string& hostUrlIpv4, std::string& hostUrlIpv6, bool fromThread )
{
	if( fromThread )
	{
		PluginBase* plugin = m_PluginMgr.findHostClientPlugin( adminId.getHostType() );
		if( plugin )
		{
			plugin->fromGuiAnnounceHost( adminId, sessionId, hostUrlIpv4, hostUrlIpv6 );
		}
		else
		{
			LogMsg( LOG_ERROR, "Plugin not found for host %d", adminId.getHostType() );
			vx_assert( false );
		}
	}
	else
	{
		m_FromGuiMgr.fromGuiAnnounceHost( adminId, sessionId, hostUrlIpv4, hostUrlIpv6 );
	}
}

//============================================================================
void P2PEngine::fromGuiJoinHost( HostedId& adminId, VxGUID& sessionId, std::string& hostUrlIpv4, std::string& hostUrlIpv6, bool fromThread )
{
	if( fromThread )
	{
		PluginBase* plugin = m_PluginMgr.findHostClientPlugin( adminId.getHostType() );
		if( plugin )
		{
			plugin->fromGuiJoinHost( adminId, sessionId, hostUrlIpv4, hostUrlIpv6 );
		}
		else
		{
			LogMsg( LOG_ERROR, "Plugin not found for host %d", adminId.getHostType() );
			vx_assert( false );
		}
	}
	else
	{
		m_FromGuiMgr.fromGuiJoinHost( adminId, sessionId, hostUrlIpv4, hostUrlIpv6 );
	}
}

//============================================================================
void P2PEngine::fromGuiLeaveHost( HostedId& adminId, VxGUID& sessionId, std::string& hostUrlIpv4, std::string& hostUrlIpv6, bool fromThread )
{
	if( fromThread )
	{
		if( getUserJoinMgr().fromGuiLeaveHost( adminId, sessionId ) )
		{
			PluginBase* plugin = m_PluginMgr.findHostClientPlugin( adminId.getHostType() );
			if( plugin )
			{
				plugin->fromGuiLeaveHost( adminId, sessionId, hostUrlIpv4, hostUrlIpv6 );
			}
			else
			{
				LogMsg( LOG_ERROR, "Plugin not found for host %d", adminId.getHostType() );
				vx_assert( false );
			}
		}

	}
	else
	{
		m_FromGuiMgr.fromGuiLeaveHost( adminId, sessionId, hostUrlIpv4, hostUrlIpv6 );
	}
}

//============================================================================
void P2PEngine::fromGuiUnJoinHost( HostedId& adminId, VxGUID& sessionId, std::string& hostUrlIpv4, std::string& hostUrlIpv6, bool fromThread )
{
	if( fromThread )
	{
		if( getUserJoinMgr().fromGuiUnJoinHost( adminId, sessionId ) )
		{
			PluginBase* plugin = m_PluginMgr.findHostClientPlugin( adminId.getHostType() );
			if( plugin )
			{
				plugin->fromGuiUnJoinHost( adminId, sessionId, hostUrlIpv4, hostUrlIpv6 );
			}
			else
			{
				LogMsg( LOG_ERROR, "Plugin not found for host %d", adminId.getHostType() );
				vx_assert( false );
			}
		}
	}
	else
	{
		m_FromGuiMgr.fromGuiUnJoinHost( adminId, sessionId, hostUrlIpv4, hostUrlIpv6 );
	}
}

//============================================================================
void P2PEngine::fromGuiSearchHost( EHostType hostType, SearchParams& searchParams, bool enable, bool fromThread )
{
	if( fromThread )
	{
		PluginBase* plugin = m_PluginMgr.findHostClientPlugin( hostType );
		if( plugin )
		{
			plugin->fromGuiSearchHost( hostType, searchParams, enable );
		}
		else
		{
			LogMsg( LOG_ERROR, "Plugin not found for host %d", hostType );
			vx_assert( false );
		}
	}
	else
	{
		m_FromGuiMgr.fromGuiSearchHost( hostType, searchParams, enable );
	}
}

//============================================================================
void P2PEngine::fromGuiSendAnnouncedList( EHostType hostType, VxGUID& sessionId )
{
	PluginBase* plugin = m_PluginMgr.findPlugin( ePluginTypeHostNetwork );
	if( plugin )
	{
		plugin->fromGuiSendAnnouncedList( hostType, sessionId );
	}
	else
	{
		LogMsg( LOG_ERROR, "Plugin not found for host %d", hostType );
		vx_assert( false );
	}
}

//============================================================================
void P2PEngine::fromGuiDisconnectFromUser( VxGUID& onlineId )
{
	getConnectIdListMgr().fromGuiDisconnectFromUser( onlineId );
}

//============================================================================
void P2PEngine::fromGuiRunIsPortOpenTest( uint16_t port )
{
    m_IsPortOpenTest.fromGuiRunIsPortOpenTest( port );
}

//============================================================================
void P2PEngine::fromGuiRunUrlAction( VxGUID& sessionId, const char* myUrl, const char* ptopUrl, ENetCmdType testType )
{
    getRunUrlAction().runUrlAction( sessionId, testType, ptopUrl, myUrl );
}

//============================================================================
bool P2PEngine::fromGuiBrowseFiles( std::string& dir, uint8_t fileFilterMask )
{
	return getPluginFileShareServer().fromGuiBrowseFiles( dir, fileFilterMask );
}

//============================================================================
bool P2PEngine::fromGuiGetSharedFiles( uint8_t fileTypeFilter )
{
	return getPluginFileShareServer().fromGuiGetSharedFiles( fileTypeFilter );
}

//============================================================================
bool P2PEngine::fromGuiSetFileIsShared( FileInfo& fileInfo, bool isShared )
{
	return getPluginFileShareServer().fromGuiSetFileIsShared( fileInfo, isShared );
}

//============================================================================
bool P2PEngine::fromGuiGetIsFileShared( FileInfo& fileInfo )
{
	return getPluginFileShareServer().fromGuiGetFileIsShared( fileInfo );
}

//============================================================================
bool P2PEngine::fromGuiRemoveSharedFile( FileInfo& fileInfo )
{
	return getPluginFileShareServer().fromGuiRemoveSharedFile( fileInfo );
}

//============================================================================
// returns -1 if unknown else percent downloaded
int P2PEngine::fromGuiGetFileDownloadState( uint8_t* fileHashId )
{
	return getPluginFileShareServer().fromGuiGetFileDownloadState( fileHashId );
}

//============================================================================
bool P2PEngine::fromGuiGetFileIsInLibrary( FileInfo& fileInfo )
{
	return getPluginLibraryServer().fromGuiGetFileIsInLibrary( fileInfo );
}
//============================================================================
bool P2PEngine::fromGuiSetFileIsInLibrary( FileInfo& fileInfo, bool isInLibrary )
{
	return getPluginLibraryServer().fromGuiSetFileIsInLibrary( fileInfo, isInLibrary );
}

//============================================================================
bool P2PEngine::fromGuiSetFileIsInLibrary( std::string& fileName, bool isInLibrary )
{
	return getPluginLibraryServer().fromGuiSetFileIsInLibrary( fileName, isInLibrary );
}

//============================================================================
void P2PEngine::fromGuiGetFileLibraryList( uint8_t fileTypeFilter )
{
	getPluginLibraryServer().fromGuiGetFileLibraryList(	fileTypeFilter );
}

//============================================================================
bool P2PEngine::fromGuiGetIsFileInLibrary( std::string& fileName )
{
	return getPluginLibraryServer().fromGuiGetIsFileInLibrary( fileName );
}

//============================================================================
bool P2PEngine::fromGuiRemoveFromLibrary( std::string& fileName )
{
	return getPluginLibraryServer().fromGuiRemoveFromLibrary( fileName );
}

//============================================================================
bool P2PEngine::fromGuiIsNoLimitVideoFile( const char* fileName )
{
	return m_MediaProcessor.getMediaTools().fromGuiIsNoLimitVideoFile( fileName );
}

//============================================================================
bool P2PEngine::fromGuiIsNoLimitAudioFile( const char* fileName )
{
	return m_MediaProcessor.getMediaTools().fromGuiIsNoLimitAudioFile( fileName );
}

//============================================================================
int P2PEngine::fromGuiDeleteFile( std::string fileName, bool shredFile )
{
	int result = -1;
	if( !fileName.empty() )
	{
		FILE* fileHandle = fopen( fileName.c_str(), "rb" );
		if( fileHandle )
		{
			fclose( fileHandle );
			result = 0;

			// tell plugins we are removing file
			m_PluginMgr.fromGuiDeleteFile( fileName, shredFile );

			// if exists as asset then announce asset removal
			AssetBaseInfo* assetInfo = getAssetMgr().findAsset( fileName );
			if( assetInfo )
			{
				getAssetMgr().removeAsset( assetInfo->getAssetUniqueId() );
			}

			// remove from library and shared files then delete the file
			getPluginFileShareServer().deleteFile( fileName.c_str(), shredFile );

			getToGui().toGuiFileDeleted( fileName );
		}
		else
		{
			result = VxGetLastError();
			if( 0 == result )
			{
				result = -1;
			}

			LogMsg( LOG_WARNING, "P2PEngine::fromGuiDeleteFile  file cannot be opened err %d file name %s", result, fileName.c_str() );
		}
	}
	else
	{
		LogMsg( LOG_ERROR, "P2PEngine::fromGuiDeleteFile bad fileName param" );
	}


	return result;
}

//============================================================================
void P2PEngine::fromGuiQuerySessionHistory( GroupieId& groupieId )
{
	m_AssetMgr.fromGuiQuerySessionHistory( groupieId );
}

//============================================================================
bool P2PEngine::fromGuiSendAsset( AssetBaseInfo& assetInfo )
{
	return m_PluginMgr.fromGuiSendAsset( assetInfo );
}

//============================================================================
bool P2PEngine::fromGuiMultiSessionAction( EMSessionAction mSessionAction, VxGUID& onlineId, int pos0to100000, VxGUID lclSessionId )
{
	return m_PluginMgr.fromGuiMultiSessionAction( mSessionAction, onlineId, pos0to100000, lclSessionId );
}

//============================================================================
int P2PEngine::fromGuiGetJoinedListCount( EPluginType pluginType )
{
	return getHostJoinMgr().fromGuiGetJoinedListCount( pluginType );
}

//============================================================================
EJoinState P2PEngine::fromGuiQueryJoinState( EHostType hostType, VxNetIdent& netIdent )
{
	return getHostJoinMgr().fromGuiQueryJoinState( hostType, netIdent );
}

//============================================================================
void P2PEngine::fromGuiListAction( EListAction listAction )
{
	m_PluginMgr.fromGuiListAction( listAction );
}

//============================================================================
std::string P2PEngine::fromGuiQueryDefaultUrl( EHostType hostType )
{
    if( eHostTypeNetwork == hostType )
    {
        if( ( m_PktAnn.getPluginPermission( ePluginTypeHostNetwork ) != eFriendStateIgnore ) &&
            isDirectConnectTested() )
        {
            // I am the network host
            return getMyOnlineUrl( hostType );
        }
	}

	if( eHostTypeChatRoom == hostType )
	{
		if( ( m_PktAnn.getPluginPermission( ePluginTypeHostChatRoom ) != eFriendStateIgnore) &&
			isDirectConnectTested() )
		{
			// I am the chat room host
			return getMyOnlineUrl( hostType );
		}
	}

	if( eHostTypeGroup == hostType )
	{
		if( (m_PktAnn.getPluginPermission( ePluginTypeHostGroup ) != eFriendStateIgnore) &&
			isDirectConnectTested() )
		{
			// I am the group host
			return getMyOnlineUrl( hostType );
		}
	}

	if( eHostTypeRandomConnect == hostType )
	{
		if( (m_PktAnn.getPluginPermission( ePluginTypeHostRandomConnect ) != eFriendStateIgnore) &&
			isDirectConnectTested() )
		{
			// I am the random connect host
			return getMyOnlineUrl( hostType );
		}
	}

	std::string defaultUrl = getEngineSettings().fromGuiQueryDefaultUrl( hostType );
    std::string resolvedUrl = getUrlMgr().resolveUrl( false, defaultUrl );
	Invite::appendHostTypeSuffix( hostType, resolvedUrl );
	return resolvedUrl;
}

//============================================================================
bool P2PEngine::fromGuiQueryIdentity( std::string& url, VxNetIdent& retNetIdent, bool requestIdentityIfUnknown )
{
	bool result{ false };
	VxPtopUrl ptopUrl( url );
	if( ptopUrl.isValid() )
	{
		if( getMyOnlineId() == ptopUrl.getOnlineId() )
		{
			retNetIdent = *getMyPktAnnounce().getVxNetIdent();
			return true;
		}

		BigListInfo* bigListInfo = m_BigListMgr.findBigListInfo( ptopUrl.getOnlineId() );
		if( bigListInfo )
		{
			retNetIdent = *bigListInfo->getVxNetIdent();
			result = true;
		}

		if( !result && requestIdentityIfUnknown )
		{
			// connect to url just to get identity
			getHostUrlListMgr().requestIdentity( ptopUrl.getUrl() );
		}
	}

	return result;
}

//============================================================================
bool P2PEngine::fromGuiQueryIdentity( VxGUID onlineId, VxNetIdent& retNetIdent )
{
	if( !onlineId.isVxGUIDValid() )
	{
		LogMsg( LOG_ERROR, "P2PEngine::fromGuiQueryIdentity invalid id" );
		return false;
	}

	if( getMyOnlineId() == onlineId )
	{
		retNetIdent = *getMyPktAnnounce().getVxNetIdent();
		return true;
	}

	BigListInfo* bigListInfo = m_BigListMgr.findBigListInfo( onlineId );
	if( bigListInfo )
	{
		retNetIdent = *bigListInfo->getVxNetIdent();
		return true;
	}

	return false;
}

//============================================================================
bool P2PEngine::fromGuiSetDefaultUrl( EHostType hostType, std::string& hostUrl )
{
	return getEngineSettings().fromGuiSetDefaultUrl( hostType, hostUrl );
}

//============================================================================
bool P2PEngine::fromGuiQueryHosts( std::string& netHostUrlIn, EHostType hostType, std::vector<HostedInfo>& hostedInfoList, VxGUID& hostIdIfNullThenAll )
{
	bool result{ false };
	VxPtopUrl netHostUrl( netHostUrlIn );
	if( netHostUrl.isValid() )
	{
		VxGUID onlineId = netHostUrl.getOnlineId();
		if( getMyOnlineId() == netHostUrl.getOnlineId() )
		{
			return fromGuiQueryMyHostedInfo( hostType, hostedInfoList );
		}

		getHostedListMgr().fromGuiQueryHostedInfoList( hostType, hostedInfoList, hostIdIfNullThenAll );
		return getHostedListMgr().fromGuiQueryHostListFromNetworkHost( netHostUrl, hostType, hostIdIfNullThenAll );
	}

	return result;
}

//============================================================================
bool P2PEngine::fromGuiQueryMyHostedInfo( EHostType hostType, std::vector<HostedInfo>& hostedInfoList )
{
	return getHostedListMgr().fromGuiQueryMyHostedInfo( hostType, hostedInfoList );
}

//============================================================================
bool P2PEngine::fromGuiQueryHostListFromNetworkHost( VxPtopUrl& netHostUrl, EHostType hostType, VxGUID& hostIdIfNullThenAll )
{
	return getHostedListMgr().fromGuiQueryHostListFromNetworkHost( netHostUrl, hostType, hostIdIfNullThenAll );
}

//============================================================================
bool P2PEngine::fromGuiQueryGroupiesFromHosted( VxPtopUrl& hostedUrl, EHostType hostType, VxGUID& onlineIdIfNullThenAll )
{
	return getHostedListMgr().fromGuiQueryGroupiesFromHosted( hostedUrl, hostType, onlineIdIfNullThenAll );
}

//============================================================================
bool P2PEngine::fromGuiDownloadWebPage( EWebPageType webPageType, VxGUID& onlineId )
{
	bool result{ false };
	if( eWebPageTypeAboutMe == webPageType )
	{
		PluginBase* plugin = m_PluginMgr.findPlugin( ePluginTypeAboutMePageClient );
		if( plugin )
		{
			result = plugin->fromGuiDownloadWebPage( eWebPageTypeAboutMe, onlineId );
		}
		else
		{
			LogMsg( LOG_ERROR, "Plugin not found for web page %s", DescribeWebPageType( webPageType ) );
		}
	}
	else if( eWebPageTypeStoryboard == webPageType )
	{
		PluginBase* plugin = m_PluginMgr.findPlugin( ePluginTypeStoryboardClient );
		if( plugin )
		{
			result = plugin->fromGuiDownloadWebPage( eWebPageTypeStoryboard, onlineId );
		}
		else
		{
			LogMsg( LOG_ERROR, "Plugin not found for web page %s", DescribeWebPageType( webPageType ) );
		}
	}
	else
	{
		LogMsg( LOG_ERROR, "Plugin unknown web page type %d", webPageType );
	}

	return result;
}

//============================================================================
bool P2PEngine::fromGuiCancelWebPage( EWebPageType webPageType, VxGUID& onlineId )
{
	bool result{ false };
	if( eWebPageTypeAboutMe == webPageType )
	{
		PluginBase* plugin = m_PluginMgr.findPlugin( ePluginTypeAboutMePageClient );
		if( plugin )
		{
			result = plugin->fromGuiCancelWebPage( eWebPageTypeAboutMe, onlineId );
		}
		else
		{
			LogMsg( LOG_ERROR, "Plugin not found for web page %s", DescribeWebPageType( webPageType ) );
		}
	}
	else if( eWebPageTypeStoryboard == webPageType )
	{
		PluginBase* plugin = m_PluginMgr.findPlugin( ePluginTypeStoryboardClient );
		if( plugin )
		{
			result = plugin->fromGuiCancelWebPage( eWebPageTypeStoryboard, onlineId );
		}
		else
		{
			LogMsg( LOG_ERROR, "Plugin not found for web page %s", DescribeWebPageType( webPageType ) );
		}
	}
	else
	{
		LogMsg( LOG_ERROR, "Plugin unknown web page type %d", webPageType );
	}

	return result;
}

//============================================================================
void P2PEngine::fromGuiUpdatePluginPermission( EPluginType pluginType, EFriendState pluginPermission )
{
	m_AnnouncePktMutex.lock(); 
	m_PktAnn.setPluginPermission( pluginType, pluginPermission );
	m_AnnouncePktMutex.unlock();
	getPluginMgr().fromGuiUpdatePluginPermission( pluginType, pluginPermission );
	doPktAnnHasChanged( false );
}

//============================================================================
bool P2PEngine::fromGuiDownloadFileList( EPluginType pluginType, VxGUID& onlineId, VxGUID& sessionId, uint8_t fileTypes )
{
	bool result{ false };
	PluginBase* plugin = m_PluginMgr.findPlugin( pluginType );
	if( plugin )
	{
		result = plugin->fromGuiDownloadFileList( onlineId, sessionId, fileTypes );
	}
	else
	{
		LogMsg( LOG_ERROR, "Plugin %s fromGuiDownloadFileList failed", DescribePluginType( pluginType ) );
	}

	return result;
}

//============================================================================
bool P2PEngine::fromGuiDownloadFileListCancel( EPluginType pluginType, VxGUID& onlineId, VxGUID& sessionId )
{
	bool result{ false };

	PluginBase* plugin = m_PluginMgr.findPlugin( ePluginTypeAboutMePageClient );
	if( plugin )
	{
		result = plugin->fromGuiDownloadFileListCancel( onlineId, sessionId );
	}
	else
	{
		LogMsg( LOG_ERROR, "Plugin %s fromGuiDownloadFileListCancel failed", DescribePluginType( pluginType ) );
	}

	return result;
}

//============================================================================
bool P2PEngine::fromGuiQueryFileHash( FileInfo& fileInfo )
{
	if( fileInfo.getFileLength() && !fileInfo.getFullFileName().empty() )
	{
		bool result = getPluginLibraryServer().fromGuiQueryFileHash( fileInfo );
		if( !result )
		{
			result = getPluginFileShareServer().fromGuiQueryFileHash( fileInfo );
		}

		if( !result )
		{
			result = getAssetMgr().fromGuiQueryFileHash( fileInfo );
		}

		return result;
	}

	return false;
}

//============================================================================
void P2PEngine::fromGuiFileHashGenerated( std::string& fileName, int64_t fileLen, VxSha1Hash& fileHash )
{
	if( fileLen && !fileName.empty() )
	{
		getPluginLibraryServer().fromGuiFileHashGenerated( fileName, fileLen, fileHash );
		getPluginFileShareServer().fromGuiFileHashGenerated( fileName, fileLen, fileHash );
		getAssetMgr().fromGuiFileHashGenerated( fileName, fileLen, fileHash );
	}
}

//============================================================================
bool P2PEngine::fromGuiDeleteDatabase( EDatabaseType databaseType )
{
	bool result{ false };
    switch( databaseType )
    {

    case eDatabaseTypeAssets:
        return getAssetMgr().getAssetInfoDb().deleteDatabase();
    case eDatabaseTypeBlobAssets:
        return getBlobMgr().getAssetInfoDb().deleteDatabase();
    case eDatabaseTypeConnectMgr:
        return getConnectMgr().getConnectInfoDb().deleteDatabase();
    case eDatabaseTypeEngineParams:
        return getEngineParams().deleteDatabase();
    case eDatabaseTypeEngineSettings:
        return getEngineSettings().deleteDatabase();
    case eDatabaseTypeHostServerJoin:
        return getHostJoinMgr().deleteDatabase();
    case eDatabaseTypeOffers:
        return getOfferMgr().deleteDatabase();
    case eDatabaseTypeThumbs:
        return getThumbMgr().getAssetInfoDb().deleteDatabase();
    case eDatabaseTypeUserJoin:
        return getUserJoinMgr().deleteDatabase();

    case eDatabaseTypeAllUsers:
        return getBigListMgr().deleteDatabase();
    default:
         LogMsg( LOG_ERROR, "P2PEngine::%s Unkonwn Database Type", __func__ );
		 break;
    }

	return result;
}

//============================================================================
void P2PEngine::fromGuiSetIsAutomatedHost( bool automatedHost )
{
	getMyPktAnnounce().setIsAutomatedHost( automatedHost );
}

//============================================================================
bool P2PEngine::fromGuiSendRandConnectSelected( VxGUID& onlineId, bool isSelected )
{
	bool result{ false };

	std::shared_ptr<VxSktBase> sktBase = getConnectIdListMgr().findAnyHostConnection( eHostTypeRandomConnect );
	if( sktBase && sktBase->isConnected() && sktBase->getIsPeerPktAnnSet() )
	{
		PktRandConnectReq pktReq;
		pktReq.setPluginNum( ePluginTypeHostRandomConnect );
		GroupieId groupieId( getMyOnlineId(), sktBase->getPeerOnlineId(), eHostTypeRandomConnect);
		pktReq.setGroupieId( groupieId );
		pktReq.setToUserOnlineId( onlineId );
		pktReq.setRandAction( isSelected ? eRandActionSelectUser : eRandActionDeselectUser );
		pktReq.setDestOnlineId( sktBase->getPeerOnlineId() );
		return 0 == sktBase->txPacketWithDestId( &pktReq );
	}
	else
	{
		LogMsg( LOG_ERROR, "P2PEngine::%s No Connection", __func__ );
	}

	return result;
}
