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

#include <P2PEngine/P2PEngine.h>
#include <User/User.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>
#include <NetLib/VxSktBase.h>

#include <time.h>

//============================================================================
UserOnlineMgr::UserOnlineMgr( P2PEngine& engine )
: m_Engine( engine )
{
    LogMsg( LOG_VERBOSE, "UserOnlineMgr::UserOnlineMgr" );
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
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockUserOnlineList", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    lockUserOnlineList();
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockUserOnlineList done", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    std::shared_ptr<User> user = findUser( onlineId );
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s unlockUserOnlineList", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    unlockUserOnlineList();

    if( user && user->getNetIdent() )
    {
        announceUserOnlineState( user, isOnline );
    }
}

//============================================================================
void UserOnlineMgr::callbackConnectionStatusChange( ConnectId& connectId, bool isConnected )
{
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockUserOnlineList", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    lockUserOnlineList();
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockUserOnlineList done", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    std::shared_ptr<User> user = findUser( connectId.getUserOnlineId() );
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s unlockUserOnlineList", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    unlockUserOnlineList();

    if( user && user->getNetIdent() )
    {
        announceUserOnlineState( user, m_Engine.getConnectIdListMgr().isUserOnline( connectId.getUserOnlineId() ) );
    }
	else
	{
   	    announceUserOnlineState( user, m_Engine.getConnectIdListMgr().isUserOnline( connectId.getUserOnlineId() ) );
	}
}

//============================================================================
void UserOnlineMgr::callbackRelayStatusChange( ConnectId& connectId, bool isRelayed )
{
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockUserOnlineList", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    lockUserOnlineList();
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockUserOnlineList done", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    std::shared_ptr<User> user = findUser( connectId.getUserOnlineId() );
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s unlockUserOnlineList", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    unlockUserOnlineList();

    if( user && user->getNetIdent() )
    {
        announceUserOnlineState( user, m_Engine.getConnectIdListMgr().isUserOnline( connectId.getUserOnlineId() ) );
    }
}

//============================================================================
void UserOnlineMgr::addUserOnlineMgrClient( UserOnlineCallbackInterface * client, bool enable )
{
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockClientList", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    lockClientList();
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockClientList done", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    if( enable )
    {
        m_UserOnlineClients.emplace_back( client );
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

    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s unlockClientList", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    unlockClientList();
}

//============================================================================
void UserOnlineMgr::announceUserOnlineAdded( std::shared_ptr<User> userInfo, BaseSessionInfo& sessionInfo )
{
    if( userInfo.get() )
    {
	    LogModule( eLogUsers, LOG_INFO, "UserOnlineMgr::%s start", __func__ );
        #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
            LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockClientList", __func__ );
        #endif // defined(DEBUG_SKT_MGR_LOCK)
        lockClientList();
        #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
            LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockClientList done", __func__ );
        #endif // defined(DEBUG_SKT_MGR_LOCK)
	    for( auto& client : m_UserOnlineClients )
	    {
		    client->callbackUserOnlineAdded( userInfo );
            if( sessionInfo.isValid() )
            {
                client->callbackUserSessionAdded( userInfo, sessionInfo );
            }
	    }

        #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
            LogMsg( LOG_DEBUG, "UserOnlineMgr::%s unlockClientList", __func__ );
        #endif // defined(DEBUG_SKT_MGR_LOCK)
	    unlockClientList();
	    LogModule( eLogUsers, LOG_INFO, "UserOnlineMgr::%s done user %s", __func__, userInfo->getNetIdent()->getOnlineName() );
    }
    else
    {
        LogMsg( LOG_ERROR, "UserOnlineMgr::%s null usr" );
    }
}

//============================================================================
void UserOnlineMgr::announceUserOnlineUpdated( std::shared_ptr<User> userInfo, BaseSessionInfo& sessionInfo )
{
    if( userInfo.get() )
    {
        #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
            LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockClientList", __func__ );
        #endif // defined(DEBUG_SKT_MGR_LOCK)
        lockClientList();
        #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
            LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockClientList done", __func__ );
        #endif // defined(DEBUG_SKT_MGR_LOCK)
        for( auto& client : m_UserOnlineClients )
        {
            client->callbackUserOnlineUpdated( userInfo );
            if( sessionInfo.isValid() )
            {
                client->callbackUserSessionUpdated( userInfo, sessionInfo );
            }
        }

        #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
            LogMsg( LOG_DEBUG, "UserOnlineMgr::%s unlockClientList", __func__ );
        #endif // defined(DEBUG_SKT_MGR_LOCK)
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
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockClientList", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    lockClientList();
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockClientList done", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    for( auto& client : m_UserOnlineClients )
    {
		client->callbackUserOnlineRemoved( hostOnlineId );
	}

    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s unlockClientList", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
	unlockClientList();
}

//============================================================================
void UserOnlineMgr::announceUserOnlineState( std::shared_ptr<User> userInfo, bool isOnline )
{
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockClientList", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    lockClientList();
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockClientList done", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    for( auto& client : m_UserOnlineClients )
    {
        client->callbackUserOnlineState( userInfo, isOnline );
    }

    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s unlockClientList", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    unlockClientList();
}

//============================================================================
void UserOnlineMgr::announceUserSessionState( std::shared_ptr<User> userInfo, bool isInSession )
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
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockUserOnlineList", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    lockUserOnlineList();
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockUserOnlineList done", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    std::shared_ptr<User> user = findUser( netIdent->getMyOnlineId() );
    if( !user.get() )
    {
        wasAdded = true;
        user = std::make_shared<User>( User( m_Engine, netIdent ) );
        m_UserOnlineList.emplace_back( user );
    }

    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s unlockUserOnlineList", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    unlockUserOnlineList();
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
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockUserOnlineList", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    lockUserOnlineList();
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockUserOnlineList done", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    std::shared_ptr<User> user = findUser( groupieId.getUserOnlineId() );
    if( !user.get() )
    {
        user = std::make_shared<User>( User( m_Engine, netIdent, sessionInfo ) );
        m_UserOnlineList.emplace_back( user );
        #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
            LogMsg( LOG_DEBUG, "UserOnlineMgr::%s unlockUserOnlineList", __func__ );
        #endif // defined(DEBUG_SKT_MGR_LOCK)
        unlockUserOnlineList();
        wasAdded = true;
    }
    else
    {
        #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
            LogMsg( LOG_DEBUG, "UserOnlineMgr::%s unlockUserOnlineList", __func__ );
        #endif // defined(DEBUG_SKT_MGR_LOCK)
        unlockUserOnlineList();
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
std::shared_ptr<User> UserOnlineMgr::findUser( VxGUID& onlineId )
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
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockUserOnlineList", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    lockUserOnlineList();
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockUserOnlineList done", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    std::shared_ptr<User> user = findUser( onlineId );
    #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
        LogMsg( LOG_DEBUG, "UserOnlineMgr::%s unlockUserOnlineList", __func__ );
    #endif // defined(DEBUG_SKT_MGR_LOCK)
    unlockUserOnlineList();

    if( !user )
    {
        #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
            LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockClientList", __func__ );
        #endif // defined(DEBUG_SKT_MGR_LOCK)
        lockClientList();
        #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
            LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockClientList done", __func__ );
        #endif // defined(DEBUG_SKT_MGR_LOCK)
        for( auto& client : m_UserOnlineClients )
        {
            client->callbackUserOffline( onlineId );
        }

        #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
            LogMsg( LOG_DEBUG, "UserOnlineMgr::%s unlockClientList", __func__ );
        #endif // defined(DEBUG_SKT_MGR_LOCK)
        unlockClientList();
    }
    else if( user->isInSession() )
    {
        #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
            LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockClientList", __func__ );
        #endif // defined(DEBUG_SKT_MGR_LOCK)
        lockClientList();
        #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
            LogMsg( LOG_DEBUG, "UserOnlineMgr::%s lockClientList done", __func__ );
        #endif // defined(DEBUG_SKT_MGR_LOCK)
        for( auto& client : m_UserOnlineClients )
        {
            client->callbackUserOnlineState( user, false );
        }

        #if defined(DEBUG_USER_ONLINE_MGR_LOCK)
            LogMsg( LOG_DEBUG, "UserOnlineMgr::%s unlockClientList", __func__ );
        #endif // defined(DEBUG_SKT_MGR_LOCK)
        unlockClientList();
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

        m_Engine.toGuiContactAnythingChange( netIdent );
    }

    return friendshipOk;
}
