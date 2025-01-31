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

#include "GuiHostJoin.h"

#include <HostServerJoinMgr/HostJoinCallbackInterface.h>
#include <HostServerJoinMgr/HostJoinInfo.h>

#include <set>

#include <QObject>

// hosted side callbacks with client user states

class AppCommon;
class GuiHostJoinCallback;
class GroupieId;

class GuiHostedByMeJoinMgr : public QObject, public HostJoinCallbackInterface
{
    Q_OBJECT
public:
    GuiHostedByMeJoinMgr() = delete;
    GuiHostedByMeJoinMgr( AppCommon& app );
    GuiHostedByMeJoinMgr( const GuiHostedByMeJoinMgr& rhs ) = delete;
	virtual ~GuiHostedByMeJoinMgr() = default;
    void                        onAppCommonCreated( void );
    void                        onMessengerReady( bool ready );
    bool                        isMessengerReady( void );
    virtual void                onSystemReady( bool ready );

    GuiHostJoin *               getMyIdent( void )                          { return m_MyIdent; }  
    VxGUID                      getMyOnlineId( void )                       { return m_MyOnlineId; } 

    int                         getJoinRequestCount( void )                 { return m_JoinRequestCount; }

    bool                        isHostJoinInSession( GroupieId& groupieId );
    void                        setHostJoinOffline( GroupieId& groupieId );

    void                        onHostJoinAdded( GuiHostJoin* guiHostJoin );
    void                        onHostJoinUpdated( GuiHostJoin* guiHostJoin, EJoinState prevState );
    void                        onUserOnlineStatusChange( GuiHostJoin* guiHostJoin, bool isOnline );

    GuiHostJoin*                getHostJoin( GroupieId& groupieId );
    std::map<GroupieId, GuiHostJoin*>& getHostJoinList( void )             { return m_HostJoinList; }
    EJoinState                  getHostJoinState( GroupieId& groupieId );

    void                        joinAccepted( GuiHostJoin* guiHostJoin );
    void                        joinRejected( GuiHostJoin* guiHostJoin );

    void                        wantHostJoinCallbacks( GuiHostJoinCallback* callback, bool wantCallback );

    void                        getHostedMembers( EHostType hostType, std::set<VxGUID>& memberList );

signals:
    void                        signalHostJoinOfferStateChange( GroupieId groupieId, EJoinState hostOfferState );
    void                        signalHostJoinOnlineStatus( GuiHostJoin* guiHostJoin, bool isOnline );

    void                        signalInternalHostJoinRequested( HostJoinInfo* hostJoinInfo );
    void                        signalInternalHostJoinUpdated( HostJoinInfo* hostJoinInfo );
    void                        signalInternalHostUnJoin( GroupieId groupieId );
    void                        signalInternalHostJoinRemoved( GroupieId groupieId );
    void                        signalInternalHostJoinOfferState( GroupieId groupieId, EJoinState joinOfferState );
    void                        signalInternalHostJoinOnlineState( GroupieId groupieId, EOnlineState onlineState, VxGUID connectionId );

private slots:
    void                        slotInternalHostJoinRequested( HostJoinInfo* hostJoinInfo );
    void                        slotInternalHostJoinUpdated( HostJoinInfo* hostJoinInfo );
    void                        slotInternalHostUnJoin( GroupieId groupieId );
    void                        slotInternalHostJoinRemoved( GroupieId groupieId );
    void                        slotInternalHostJoinOfferState( GroupieId groupieId, EJoinState joinOfferState );
    void                        slotInternalHostJoinOnlineState( GroupieId groupieId, EOnlineState onlineState, VxGUID connectionId );

protected:
    GuiHostJoin*                findHostJoin( GroupieId groupieId );
    GuiHostJoin*                updateHostJoin( HostJoinInfo* hostJoinInfo );
    void                        updateJoinRequestCount( bool forceEmit = false );

    virtual void				callbackHostJoinRequested( HostJoinInfo* userHostInfo ) override;
    virtual void				callbackHostJoinUpdated( HostJoinInfo* userHostInfo ) override;
    virtual void				callbackHostUnJoin( GroupieId& groupieId ) override;
    virtual void				callbackHostJoinRemoved( GroupieId& groupieId ) override;
    virtual void				callbackHostJoinOfferState( GroupieId& groupieId, EJoinState userHostOfferState ) override;
    virtual void				callbackHostJoinOnlineState( GroupieId& groupieId, EOnlineState onlineState, VxGUID& connectionId ) override;

    void                        announceJoinState( GuiHostJoin* guiHostJoin, EJoinState joinState );

    virtual void				announceHostJoinRequested( GroupieId& groupieId, GuiHostJoin* guiHostJoin );
    virtual void				announceHostJoinWasGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin );
    virtual void				announceHostJoinIsGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin );
    virtual void				announceHostJoinDenied( GroupieId& groupieId, GuiHostJoin* guiHostJoin );
    virtual void				announceHostJoinLeaveHost( GroupieId& groupieId );
    virtual void				announceHostUnJoin( GroupieId& groupieId );
    virtual void				announceHostJoinRemoved( GroupieId& groupieId );
    
    AppCommon&                  m_MyApp;
    // map of online id to GuiHostJoin
    std::map<GroupieId, GuiHostJoin*>  m_HostJoinList;
    GuiHostJoin*                m_MyIdent{ nullptr };
    VxGUID                      m_MyOnlineId;
    int                         m_JoinRequestCount{ 0 };

    std::vector<GuiHostJoinCallback*>  m_HostJoinClients;
};
