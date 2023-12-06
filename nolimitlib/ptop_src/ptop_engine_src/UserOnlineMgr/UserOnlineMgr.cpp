//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "UserOnlineMgr.h"
#include "UserOnlineCallbackInterface.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>
#include <ptop_src/ptop_engine_src/User/User.h>

#include <CoreLib/VxTime.h>
#include <NetLib/VxSktBase.h>

#include <time.h>

//============================================================================
UserOnlineMgr::UserOnlineMgr( P2PEngine& engine, const char* dbName, const char* dbStateName )
: m_Engine( engine )
{
    m_Engine.getConnectIdListMgr().wantConnectIdListCallback( this, true );
}

//============================================================================
void UserOnlineMgr::fromGuiUserLoggedOn( void )
{
    // dont call HostBaseMgr::fromGuiUserLoggedOn because we never generate sha hash for thumbnails
    if( !m_Initialized )
    {
        m_Initialized = true;
    }
}

//============================================================================
void UserOnlineMgr::callbackOnlineStatusChange( VxGUID& onlineId, bool isOnline )
{
    lockResources();
    User* user = findUser( onlineId );
    if( user && user->getNetIdent() )
    {
        announceUserOnlineState( user, isOnline );
    }

    unlockResources();
}

//============================================================================
void UserOnlineMgr::callbackConnectionStatusChange( ConnectId& connectId, bool isConnected )
{
    lockResources();
    User* user = findUser( connectId.getUserOnlineId() );
    if( user && user->getNetIdent() )
    {
        announceUserOnlineState( user, m_Engine.getConnectIdListMgr().isUserOnline( connectId.getUserOnlineId() ) );
    }

    unlockResources();
}

//============================================================================
void UserOnlineMgr::callbackRelayStatusChange( ConnectId& connectId, bool isRelayed )
{
    lockResources();
    User* user = findUser( connectId.getUserOnlineId() );
    if( user && user->getNetIdent() )
    {
        announceUserOnlineState( user, m_Engine.getConnectIdListMgr().isUserOnline( connectId.getUserOnlineId() ) );
    }

    unlockResources();
}

//============================================================================
void UserOnlineMgr::addUserOnlineMgrClient( UserOnlineCallbackInterface * client, bool enable )
{
    lockClientList();
    if( enable )
    {
        m_UserOnlineClients.push_back( client );
    }
    else
    {
        std::vector<UserOnlineCallbackInterface *>::iterator iter;
        for( iter = m_UserOnlineClients.begin(); iter != m_UserOnlineClients.end(); ++iter )
        {
            if( *iter == client )
            {
                m_UserOnlineClients.erase( iter );
                break;
            }
        }
    }

    unlockClientList();
}

//============================================================================
void UserOnlineMgr::announceUserOnlineAdded( User * userInfo, BaseSessionInfo& sessionInfo )
{
    User * userHostInfo = userInfo;
    if( userHostInfo )
    {
	    LogMsg( LOG_INFO, "UserOnlineMgr::announceUserOnlineAdded start" );
	
	    lockClientList();
	    std::vector<UserOnlineCallbackInterface *>::iterator iter;
	    for( iter = m_UserOnlineClients.begin();	iter != m_UserOnlineClients.end(); ++iter )
	    {
		    UserOnlineCallbackInterface * client = *iter;
		    client->callbackUserOnlineAdded( userHostInfo );
            if( sessionInfo.isValid() )
            {
                client->callbackUserSessionAdded( userHostInfo, sessionInfo );
            }
	    }

	    unlockClientList();
	    LogMsg( LOG_INFO, "UserOnlineMgr::announceUserOnlineAdded done" );
    }
    else
    {
        LogMsg( LOG_ERROR, "UserOnlineMgr::announceUserOnlineAdded dynamic_cast failed" );
    }
}

//============================================================================
void UserOnlineMgr::announceUserOnlineUpdated( User * userInfo, BaseSessionInfo& sessionInfo )
{
    User * userHostInfo = userInfo;
    if( userHostInfo )
    {
        lockClientList();
        std::vector<UserOnlineCallbackInterface *>::iterator iter;
        for( iter = m_UserOnlineClients.begin();	iter != m_UserOnlineClients.end(); ++iter )
        {
            UserOnlineCallbackInterface * client = *iter;
            client->callbackUserOnlineUpdated( userHostInfo );
            if( sessionInfo.isValid() )
            {
                client->callbackUserSessionUpdated( userHostInfo, sessionInfo );
            }
        }

        unlockClientList();
    }
    else
    {
        LogMsg( LOG_ERROR, "UserOnlineMgr::announceUserOnlineRemoved dynamic_cast failed" );
    }
}

//============================================================================
void UserOnlineMgr::announceUserOnlineRemoved( VxGUID& hostOnlineId, EHostType hostType )
{
	lockClientList();
	std::vector<UserOnlineCallbackInterface *>::iterator iter;
	for( iter = m_UserOnlineClients.begin();	iter != m_UserOnlineClients.end(); ++iter )
	{
		UserOnlineCallbackInterface * client = *iter;
		client->callbackUserOnlineRemoved( hostOnlineId );
	}

	unlockClientList();
}

//============================================================================
void UserOnlineMgr::announceUserOnlineState( User* user, bool isOnline )
{
    lockClientList();
    std::vector<UserOnlineCallbackInterface *>::iterator iter;
    for( iter = m_UserOnlineClients.begin(); iter != m_UserOnlineClients.end(); ++iter )
    {
        UserOnlineCallbackInterface * client = *iter;
        client->callbackUserOnlineState( user, isOnline );
    }

    unlockClientList();
}

//============================================================================
void UserOnlineMgr::announceUserSessionState( User* user, bool isInSession )
{
    /*
    lockClientList();
    std::vector<UserOnlineCallbackInterface*>::iterator iter;
    for( iter = m_UserOnlineClients.begin(); iter != m_UserOnlineClients.end(); ++iter )
    {
        UserOnlineCallbackInterface* client = *iter;
        client->callbackUserSessionState( user, isInSession );
    }

    unlockClientList();
    */
}

//============================================================================
void UserOnlineMgr::onUserOnline( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo )
{
    updateUserSession( sktBase, netIdent, sessionInfo );
}

//============================================================================
bool UserOnlineMgr::onUserOnline( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent )
{
    bool wasAdded = false;
    lockResources();
    User* user = findUser( netIdent->getMyOnlineId() );
    if( !user )
    {
        wasAdded = true;
        user = new User( m_Engine, netIdent );
        m_UserOnlineList.push_back( user );
    }

    unlockResources();
    if( wasAdded && !sktBase->isTempConnection() )
    {
        m_Engine.getToGui().toGuiContactAdded( netIdent );
    }

    callbackOnlineStatusChange( groupieId.getUserOnlineId(), true );
    return wasAdded;
}

//============================================================================
void UserOnlineMgr::onHostJoinRequestedByUser( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo )
{
    updateUserSession( sktBase, netIdent, sessionInfo ); 
}

//============================================================================
void UserOnlineMgr::onHostJoinedByUser( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo )
{
    updateUserSession( sktBase, netIdent, sessionInfo );
}

//============================================================================
void UserOnlineMgr::onHostLeftByUser( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo )
{
    updateUserSession( sktBase, netIdent, sessionInfo, true );
}

//============================================================================
void UserOnlineMgr::onUserJoinedHost( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo )
{
    updateUserSession( sktBase, netIdent, sessionInfo );
}

//============================================================================
void UserOnlineMgr::onUserJoinedHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo )
{
    updateUserSession( groupieId, sktBase, netIdent, sessionInfo, false );
}

//============================================================================
void UserOnlineMgr::onUserLeftHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo )
{
    updateUserSession( groupieId, sktBase, netIdent, sessionInfo, true );
}

//============================================================================
void UserOnlineMgr::onUserUnJoinedHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo )
{
    updateUserSession( groupieId, sktBase, netIdent, sessionInfo, true );
}

//============================================================================
void UserOnlineMgr::updateUserSession( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo, bool leftHost )
{
    if( IsHostPluginType( sessionInfo.getHostPluginType() ) )
    {
        updateUserSession( sessionInfo.getGroupieId(), sktBase, netIdent, sessionInfo, leftHost );
    }
    else
    {
        updateUserSession( sessionInfo.getGroupieId(), sktBase, netIdent, sessionInfo, leftHost );
    }
}

//============================================================================
void UserOnlineMgr::updateUserSession( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo, bool leftHost )
{
    bool wasAdded = false;
    bool wasInSession = false;
    bool isinSession = false;
    lockResources();
    User* user = findUser( groupieId.getUserOnlineId() );
    if( !user )
    {
        user = new User( m_Engine, netIdent, sessionInfo );
        wasAdded = true;
    }
    else
    {
        wasInSession = user->isInSession();
        if( leftHost )
        {
            user->removeSession( sessionInfo );
        }
        else
        {
            user->addSession( sessionInfo );
        }
        
        isinSession = user->isInSession();
    }

    unlockResources();

    if( !leftHost )
    {
        m_Engine.getThumbMgr().queryThumbIfNeeded( sktBase, netIdent, sessionInfo.getHostPluginType() );
    }

    if( wasAdded )
    {
        announceUserOnlineAdded( user, sessionInfo );
    }
    else
    {
        announceUserOnlineUpdated( user, sessionInfo );
    }

    if( wasInSession != isinSession )
    {
        announceUserSessionState( user, isinSession );
    }
}

//============================================================================
User* UserOnlineMgr::findUser( VxGUID& onlineId )
{
    for( auto user : m_UserOnlineList )
    {
        if( user->getMyOnlineId() == onlineId )
        {
            return user;
        }
    }

    return nullptr;
}

//============================================================================
bool UserOnlineMgr::isUserOnline( VxGUID& onlineId )
{
    return m_Engine.getConnectIdListMgr().isUserOnline( onlineId );
}

//============================================================================
bool UserOnlineMgr::isUserExcluded( VxGUID& onlineId )
{
    return m_Engine.getConnectIdListMgr().isUserExcluded( onlineId );
}

//============================================================================
void UserOnlineMgr::onConnectionLost( std::shared_ptr<VxSktBase>& sktBase, VxGUID& connectionId, VxGUID& peerOnlineId )
{
    // TODO BRJ handle disconnect
}

//============================================================================
void UserOnlineMgr::onUserOffline( VxGUID& onlineId )
{
    lockResources();
    User* user = findUser( onlineId );
    if( !user )
    {
        unlockResources();

        lockClientList();
        std::vector<UserOnlineCallbackInterface *>::iterator iter;
        for( iter = m_UserOnlineClients.begin(); iter != m_UserOnlineClients.end(); ++iter )
        {
            UserOnlineCallbackInterface * client = *iter;
            client->callbackUserOffline( onlineId );
        }

        unlockClientList();
    }
    else if( user->isInSession() )
    {
        lockClientList();
        std::vector<UserOnlineCallbackInterface *>::iterator iter;
        for( iter = m_UserOnlineClients.begin(); iter != m_UserOnlineClients.end(); ++iter )
        {
            UserOnlineCallbackInterface * client = *iter;
            client->callbackUserOnlineState( user, false );
        }

        unlockClientList();
        unlockResources();
    }
}


//============================================================================
bool UserOnlineMgr::updateUserJoinedFriendships( GroupieId& groupieId, VxNetIdent* netIdent )
{
    bool friendshipOk{ true };
    EFriendState prevMyFriendship = netIdent->getMyFriendshipToHim();
    EFriendState prevHisFriendship = netIdent->getHisFriendshipToMe();
    EFriendState curMyFriendship = prevMyFriendship;
    EFriendState curHisFriendship = prevHisFriendship;
    if( eFriendStateIgnore == curHisFriendship )
    {
        if( curMyFriendship == eFriendStateIgnore )
        {
            LogMsg( LOG_ERROR, "HostServerMgr::updateUserToJoinedToMyHostGuest got ignored user %s %s ",
                    netIdent->getOnlineName(), netIdent->getMyOnlineId().toOnlineIdString().c_str() );
            friendshipOk = false;
        }
        else
        {
            // not sure how this can happen but I guess is allowed so just downgrade to anonymous
            curMyFriendship = eFriendStateAnonymous;
        }
    }
    else if( eFriendStateIgnore == curMyFriendship )
    {
        curHisFriendship = eFriendStateIgnore; // so will appear in the blocked list
        friendshipOk = false;
    }
    else
    {
        if( eFriendStateAnonymous == curMyFriendship )
        {
            curMyFriendship = eFriendStateGuest;
        }

        if( eFriendStateAnonymous == curHisFriendship )
        {
            curHisFriendship = eFriendStateGuest;
        }
    }

    if( curMyFriendship != prevMyFriendship || curHisFriendship != prevHisFriendship )
    {
        netIdent->setMyFriendshipToHim( curMyFriendship );
        netIdent->setHisFriendshipToMe( curHisFriendship );

        if( curMyFriendship != prevMyFriendship )
        {
            m_Engine.getBigListMgr().onMyFriendshipChanged( prevMyFriendship, netIdent );
        }

        m_Engine.toGuiContactHisFriendshipChange( netIdent );
    }

    return friendshipOk;
}