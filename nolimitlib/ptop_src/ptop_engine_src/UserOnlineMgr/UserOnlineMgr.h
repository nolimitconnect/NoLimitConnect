#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <ptop_src/ptop_engine_src/ConnectIdListMgr/ConnectIdListCallback.h>

#include <CoreLib/VxMutex.h>
#include <PktLib/GroupieId.h>

#include <memory>

class BaseSessionInfo;
class P2PEngine;
class User;
class UserOnlineCallbackInterface;
class VxSktBase;
class VxNetIdent;

class UserOnlineMgr : public ConnectIdListCallback
{
public:
	UserOnlineMgr( P2PEngine& engine, const char* dbName, const char* dbStateName );
	virtual ~UserOnlineMgr() = default;

    void                        fromGuiUserLoggedOn( void );

    void                        addUserOnlineMgrClient( UserOnlineCallbackInterface * client, bool enable );

    VxMutex&					getResourceMutex( void )					{ return m_ResourceMutex; }
    void						lockResources( void )						{ m_ResourceMutex.lock(); }
    void						unlockResources( void )						{ m_ResourceMutex.unlock(); }

    bool                        isUserOnline( VxGUID& onlineId );
    bool                        isUserExcluded( VxGUID& onlineId );

    void                        onUserOnline( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo );
    bool                        onUserOnline( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent ); // return true if user was added
    void                        onHostJoinRequestedByUser( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo );
    void                        onHostJoinedByUser( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo );
    void                        onHostLeftByUser( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo );
    void                        onUserJoinedHost( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo );
    void                        onUserJoinedHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo );
    void                        onUserLeftHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo );
    void                        onUserUnJoinedHost( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo );
    virtual void                onConnectionLost( std::shared_ptr<VxSktBase>& sktBase, VxGUID& connectionId, VxGUID& peerOnlineId );
    virtual void                onUserOffline( VxGUID& onlineId );

    User*                       findUser( VxGUID& onlineId );

    bool                        updateUserJoinedFriendships( GroupieId& groupieId, VxNetIdent* netIdent );

protected:
    void				        callbackOnlineStatusChange( VxGUID& onlineId, bool isOnline ) override;
    void				        callbackConnectionStatusChange( ConnectId& connectId, bool isConnected ) override;
    void                        callbackRelayStatusChange( ConnectId& connectId, bool isRelayed ) override;

    void                        updateUserSession( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo, bool leftHost = false );
    void                        updateUserSession( GroupieId& groupieId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, BaseSessionInfo& sessionInfo, bool leftHost );

    virtual void				announceUserOnlineAdded( User* userJoinInfo, BaseSessionInfo& sessionInfo );
    virtual void				announceUserOnlineUpdated( User* userJoinInfo, BaseSessionInfo& sessionInfo );
    virtual void				announceUserOnlineRemoved( VxGUID& hostOnlineId, EHostType hostType );

    void				        announceUserOnlineState( User* userJoinInfo, bool isOnline );
    void                        announceUserSessionState( User* user, bool isInSession );

    void						lockClientList( void )						{ m_UserOnlineClientMutex.lock(); }
    void						unlockClientList( void )					{ m_UserOnlineClientMutex.unlock(); }

    P2PEngine&					m_Engine;
    VxMutex						m_ResourceMutex;
    bool						m_Initialized{ false };
 
    std::vector<User*>	        m_UserOnlineList;
    VxMutex						m_UserOnlineMutex;
    bool                        m_UserOnlineListInitialized{ false };

    std::vector<UserOnlineCallbackInterface *> m_UserOnlineClients;
    VxMutex						m_UserOnlineClientMutex;
};

