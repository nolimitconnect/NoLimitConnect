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

#include "GuiUserJoin.h"

#include <UserJoinMgr/UserJoinCallbackInterface.h>
#include <UserJoinMgr/UserJoinInfo.h>

#include <CoreLib/GroupieId.h>
#include <CoreLib/VxPtopUrl.h>

#include <QObject>

// client side manager of user join to host service states

class AppCommon;
class GuiUserJoinCallback;
class VxPtopUrl;

class GuiUserJoinMgr : public QObject, public UserJoinCallbackInterface
{
    Q_OBJECT
public:
    GuiUserJoinMgr() = delete;
    GuiUserJoinMgr( AppCommon& app );
    GuiUserJoinMgr( const GuiUserJoinMgr& rhs ) = delete;
	virtual ~GuiUserJoinMgr() = default;
    void                        onAppCommonCreated( void );
    void                        onMessengerReady( bool ready ) { }
    bool                        isMessengerReady( void );
    virtual void                onSystemReady( bool ready ) { }

    void                        setLastJoinAttempted( std::string& joinUrl ) { m_LastJoinAttemptedHostInviteUrl = joinUrl; if( !m_ReconnectToHost.empty() ) m_ReconnectToHost = joinUrl; }

    bool                        isUserJoinedToHost( EHostType hostType ); ///< return true if local user currently is joined to given host type
    bool                        isUserJoinedToHost( HostedId& adminId );
    bool                        isMemberActive( GroupieId& groupieId );

    bool                        isUserJoinedToHost( GroupieId& groupieId ) { return getUserJoinState( groupieId ) == eJoinStateJoinIsGranted; } ///< any member 

    bool                        isUserJoinInSession( GroupieId& groupieId );
    EJoinState                  getUserJoinState( GroupieId& groupieId );

    VxPtopUrl                   getLastJoinedPtopUrl( EHostType hostType );

    void                        onUserJoinAdded( GuiUserJoin* guiUserJoin );
    void                        onUserJoinRemoved( GroupieId& groupieId );
    void                        onUserJoinUpdated( GuiUserJoin* guiUserJoin );
    void                        onUserUnJoin( GuiUserJoin* guiUserJoin );
    void                        onUserOnlineStatusChange( GuiUserJoin* guiUserJoin, bool isOnline );
    void                        onMyIdentUpdated( GuiUserJoin* guiUserJoin );

    GuiUserJoin*                getUserJoin( GroupieId& groupieId )         { return findUserJoin( groupieId ); }
    std::map<GroupieId, GuiUserJoin*>& getUserJoinList( void )              { return m_UserJoinList; }
    GuiUserJoin*                updateUserJoin( VxNetIdent* hisIdent, EHostType hostType = eHostTypeUnknown );

    void                        wantUserJoinCallbacks( GuiUserJoinCallback* client, bool enable );

    void                        reconnectToLastConnectedHost( std::string& lastConnectedHost );
    void                        stopReconnectToLastConnectedHost( void );

    GroupieId                   getJoinedAdminGroupieId( EHostType hostType );
    void                        leaveHost( EHostType hostType );
    void                        leaveHost( GroupieId adminGroupieId );

signals:
    void				        signalMyIdentUpdated( GuiUserJoin* guiUserJoin );

    void				        signalUserJoinRequested( GuiUserJoin* guiUserJoin );
    void                        signalUserJoinUpdated( GuiUserJoin* guiUserJoin );
    void				        signalUserJoinRemoved( GroupieId& groupieId );
    void                        signalUserJoinOfferStateChange( GroupieId& groupieId, EJoinState hostOfferState );
    void                        signalUserJoinOnlineStatus( GuiUserJoin* guiUserJoin, bool isOnline );

    void                        signalInternalUserJoinRequested( UserJoinInfo* userJoinInfo );
    void                        signalInternalUserJoinUpdated( UserJoinInfo* userJoinInfo );
    void                        signalInternalUserUnJoin( UserJoinInfo* userJoinInfo );
    void                        signalInternalUserJoinRemoved( GroupieId groupieId );
    void                        signalInternalUserJoinOfferState( GroupieId groupieId, EJoinState hostOfferState );
    void                        signalInternalUserJoinOnlineState( GroupieId groupieId, EOnlineState onlineState, VxGUID connectionId );

    void				        signalInternalUserJoinAHostStatus( EHostType hostType, VxGUID sessionId, EConnectStatus connectStatus );

private slots:
    void                        slotInternalUserJoinRequested( UserJoinInfo* userJoinInfo );
    void                        slotInternalUserJoinUpdated( UserJoinInfo* userJoinInfo );
    void                        slotInternalUserUnJoin( UserJoinInfo* userJoinInfo );
    void                        slotInternalUserJoinRemoved( GroupieId groupieId );
    void                        slotInternalUserJoinOfferState( GroupieId groupieId, EJoinState hostOfferState );
    void                        slotInternalUserJoinOnlineState( GroupieId groupieId, EOnlineState onlineState, VxGUID connectionId );

    void                        slotReconnectToLastConnectedHost( void );

    void				        slotInternalUserJoinAHostStatus( EHostType hostType, VxGUID sessionId, EConnectStatus connectStatus );

protected:
    void                        removeUserJoin( GroupieId& groupieId );
    GuiUserJoin*                findUserJoin( GroupieId& groupieId );
    GuiUserJoin*                updateUserJoin( UserJoinInfo* userJoinInfo, bool unJoin = false );
    
    void				        callbackUserJoinAdded( UserJoinInfo* userJoinInfo ) override;
    void				        callbackUserJoinUpdated( UserJoinInfo* userJoinInfo ) override;
    void				        callbackUserUnJoin( UserJoinInfo* userJoinInfo ) override;
    void				        callbackUserJoinRemoved( GroupieId& groupieId ) override;
    void				        callbackUserJoinOfferState( GroupieId& groupieId, EJoinState userOfferState ) override;
    void				        callbackUserJoinOnlineState( GroupieId& groupieId, EOnlineState onlineState, VxGUID& connectionId ) override;

    void				        callbackUserJoinAHostStatus( EHostType hostType, VxGUID& sessionId, EConnectStatus connectStatus ) override;

    void                        announceUserJoinState( EJoinState joinState, GuiUserJoin* guiUserJoin );

    virtual void				announceUserJoinRequested( GroupieId& groupieId, GuiUserJoin* guiUserJoin );
    virtual void				announceUserJoinWasGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin );
    virtual void				announceUserJoinIsGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin );
    virtual void				announceUserUnJoinGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin );
    virtual void				announceUserJoinDenied( GroupieId& groupieId, GuiUserJoin* guiUserJoin );
    virtual void				announceUserJoinLeaveHost( GroupieId& groupieId );
    virtual void				announceUserJoinRemoved( GroupieId& groupieId );

    virtual void				announceUserJoinedToHostState( EHostType hostType, bool isJoined );

    virtual void				announceUserJoinAHostStatus( EHostType hostType, VxGUID& sessionId, EConnectStatus connectStatus );

    void                        saveLastHostJoined( EHostType hostType, VxPtopUrl& ptopUrl );


    AppCommon&                  m_MyApp;
    // map of online id to GuiUserJoin
    std::map<GroupieId, GuiUserJoin*>  m_UserJoinList;

    std::vector<GuiUserJoinCallback*>  m_UserJoinClients;

    std::string                 m_LastJoinAttemptedHostInviteUrl;

    VxPtopUrl                   m_LastJoinChatRoomUrl;
    VxPtopUrl                   m_LastJoinGroupUrl;
    VxPtopUrl                   m_LastJoinRandomConnectUrl;

    std::string                 m_ReconnectToHost;
    QTimer*                     m_ReconnectToHostTimer{ nullptr };
    int                         m_ReconnectToHostAttempts{ 0 };
};
