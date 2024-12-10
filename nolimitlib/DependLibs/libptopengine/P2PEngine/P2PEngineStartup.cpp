//============================================================================
// Copyright (C) 2023 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "P2PEngine.h"

#include <BigListLib/BigListInfo.h>

#include <HostServerJoinMgr/HostServerJoinMgr.h>

#include <OfferBase/OfferMgr.h>
#include <Plugins/PluginNetServices.h>
#include <Plugins/PluginMgr.h>
#include <SendQueue/SendQueueMgr.h>

#include <UserJoinMgr/UserJoinMgr.h>
#include <UserOnlineMgr/UserOnlineMgr.h>

#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxFileShredder.h>
#include <CoreLib/VxParse.h>

//============================================================================
void P2PEngine::fromGuiAppStartup( std::string assetsDir, std::string rootDataDir, bool fromThread )
{
	VxSetAppIsShuttingDown( false );

    if( fromThread )
    {
        LogMsg( LOG_VERBOSE, "P2PEngine::fromGuiAppStartup run" );
        enableTimerThread( true );

        LogMsg( LOG_INFO, "P2PEngine::fromGuiAppStartup %s %s", assetsDir.c_str(), rootDataDir.c_str() );
    }
    else
    {
        LogMsg( LOG_VERBOSE, "P2PEngine::fromGuiAppStartup queued" );
        m_FromGuiMgr.fromGuiAppStartup( assetsDir, rootDataDir );
    }
}

//============================================================================
void P2PEngine::fromGuiSetUserXferDir( std::string userXferDir, bool fromThread )
{
    if( fromThread )
    {
	    LogMsg( LOG_INFO, "P2PEngine::fromGuiSetUserXferDir %s", userXferDir.c_str() );
	    VxSetUserXferDirectory( userXferDir );
	    std::string incompleteDir = VxGetIncompleteDirectory();
	    // delete all incomplete files from previous usage
	    std::vector<std::string>	fileList;
	    VxFileUtil::listFilesInDirectory( incompleteDir.c_str(), fileList );
	    std::vector<std::string>::iterator iter;
	    for( iter = fileList.begin(); iter != fileList.end(); ++iter )
	    {
		    GetVxFileShredder().shredFile( *iter );
	    }
    }
    else
    {
        LogMsg( LOG_VERBOSE, "P2PEngine::fromGuiSetUserXferDir queued" );
        m_FromGuiMgr.fromGuiSetUserXferDir( userXferDir );
    }
}

//============================================================================
void P2PEngine::fromGuiSetUserSpecificDir( std::string userSpecificDir, bool fromThread )
{
    if( fromThread )
    {
        static std::string userDir;
        if( userDir == userSpecificDir )
        {
            LogMsg( LOG_INFO, "P2PEngine::fromGuiSetUserSpecificDir %s called twice", userSpecificDir.c_str() );
            return;
        }

        userDir = userSpecificDir;
	    LogMsg( LOG_INFO, "P2PEngine::fromGuiSetUserSpecificDir %s", userSpecificDir.c_str() );
	    VxSetUserSpecificDataDirectory( userSpecificDir.c_str() );

	    std::string strDbFileName = VxGetSettingsDirectory();
	    strDbFileName += "biglist.db3";
	    RCODE rc = m_BigListMgr.bigListMgrStartup( strDbFileName.c_str() );
	    if( rc )
	    {
		    LogMsg( LOG_ERROR, "P2PEngine::startupEngine error %d bigListMgrStartup", rc );
	    }

	    strDbFileName = VxGetSettingsDirectory();
	    strDbFileName += "HostUrlList.db3";
	    getHostUrlListMgr().hostUrlListMgrStartup( strDbFileName );

	    strDbFileName = VxGetSettingsDirectory();
	    strDbFileName += "EngineParams.db3";
	    getEngineParams().engineParamsStartup( strDbFileName );

        strDbFileName = VxGetSettingsDirectory();
        strDbFileName += "IgnoredHostsList.db3";
        getIgnoreListMgr().ignoredHostsListMgrStartup( strDbFileName );

	    strDbFileName = VxGetSettingsDirectory();
	    strDbFileName += "HostedList.db3";
	    getHostedListMgr().hostedListMgrStartup( strDbFileName );

        // if application was aborted it may have left the listen socket in a state
        // that the port cannot be listened to for incomming connections
        // ANDROID crashes if attempt to close an unowned socket so is excluded
        // instead must restart the android device
        #if !defined( TARGET_OS_ANDROID )
            int64_t listenSocket = getEngineParams().getLastListenSocket();
            if( listenSocket > 0 )
            {
                VxCloseSktNow( listenSocket );
                getEngineParams().setLastListenSocket(0);
            }

        #endif //!defined( TARGET_OS_ANDROID )

	    m_IsUserSpecificDirSet = true;
	    m_AppStartupCalled = true;
    }
    else
    {
        LogMsg( LOG_VERBOSE, "P2PEngine::fromGuiSetUserSpecificDir queued" );
        m_FromGuiMgr.fromGuiSetUserSpecificDir( userSpecificDir );
    }
}

//============================================================================
void P2PEngine::fromGuiUserLoggedOn( VxNetIdent* netIdent, bool fromThread )
{
    if( fromThread )
    {
        //assureUserSpecificDirIsSet( "P2PEngine::fromGuiUserLoggedOn" );
        LogMsg( LOG_INFO, "P2PEngine fromGuiUserLoggedOn" );
        memcpy( ( VxNetIdent* )&m_PktAnn, netIdent, sizeof( VxNetIdent ) );
        m_PktAnn.setSrcOnlineId( netIdent->getMyOnlineId() );
        m_MyOnlineId = netIdent->getMyOnlineId();

        m_HostJoinMgr.fromGuiUserLoggedOn();
        m_UserJoinMgr.fromGuiUserLoggedOn();
        m_UserOnlineMgr.fromGuiUserLoggedOn();

        // set network settings from saved settings
        startupEngine();
        //updateFromEngineSettings( getEngineSettings() );
        m_PluginMgr.fromGuiUserLoggedOn();

        m_AssetMgr.onPluginsInitialized();
        m_OfferMgr.fromGuiUserLoggedOn();
        m_OfferMgr.onPluginsInitialized();
        // m_BlobMgr.fromGuiUserLoggedOn();
        m_ThumbMgr.onPluginsInitialized();

        m_SendQueueMgr.fromGuiUserLoggedOn();
        LogMsg( LOG_INFO, "P2PEngine fromGuiUserLoggedOn done" );
        m_IsEngineReady = true;

        m_PluginMgr.onAfterUserLogOnThreaded();
        LogMsg( LOG_INFO, "P2PEngine PluginMgr onAfterUserLogOnThreaded done" );
    }
    else
    {
        LogMsg( LOG_VERBOSE, "P2PEngine::fromGuiUserLoggedOn queued" );
        m_FromGuiMgr.fromGuiUserLoggedOn( netIdent );
    }
}
