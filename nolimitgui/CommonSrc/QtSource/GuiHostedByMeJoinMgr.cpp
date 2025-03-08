//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiHostedByMeJoinMgr.h"
#include "GuiHostJoinCallback.h"
#include "GuiParams.h"
#include "AppCommon.h"

#include <HostServerJoinMgr/HostJoinInfo.h>
#include <HostServerJoinMgr/HostServerJoinMgr.h>
#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>

//============================================================================
GuiHostedByMeJoinMgr::GuiHostedByMeJoinMgr( AppCommon& app )
    : QObject( &app )
    , m_MyApp( app )
{
}

//============================================================================
void GuiHostedByMeJoinMgr::onAppCommonCreated( void )
{
    connect( this, SIGNAL( signalInternalHostJoinRequested(HostJoinInfo*) ),	                            this, SLOT( slotInternalHostJoinRequested( HostJoinInfo* ) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalHostJoinUpdated(HostJoinInfo*) ),                                  this, SLOT( slotInternalHostJoinUpdated( HostJoinInfo* ) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalHostUnJoin(GroupieId) ),                                           this, SLOT( slotInternalHostUnJoin( GroupieId ) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalHostJoinRemoved(GroupieId) ),	                                    this, SLOT( slotInternalHostJoinRemoved( GroupieId ) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalHostJoinOfferState(GroupieId,EJoinState) ),                        this, SLOT( slotInternalHostJoinOfferState( GroupieId, EJoinState ) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalHostJoinOnlineState(GroupieId,EOnlineState,VxGUID) ),              this, SLOT( slotInternalHostJoinOnlineState( GroupieId, EOnlineState, VxGUID ) ), Qt::QueuedConnection );

    m_MyApp.getEngine().getHostJoinMgr().wantHostJoinMgrCallbacks( this, true );
}

//============================================================================
bool GuiHostedByMeJoinMgr::isMessengerReady( void )
{
    return m_MyApp.isMessengerReady();
}

//============================================================================
void GuiHostedByMeJoinMgr::onMessengerReady( bool ready )
{
}

//============================================================================
void GuiHostedByMeJoinMgr::onSystemReady( bool ready )
{
    if( ready )
    {
        std::vector<HostJoinInfo*> hostJoinList;
        m_MyApp.getEngine().getHostJoinMgr().lockHostJoinInfoList();
        m_MyApp.getEngine().getHostJoinMgr().fromGuiGetJoinedStateList( ePluginTypeHostGroup, eJoinStateJoinRequested, hostJoinList );
        for( auto hostJoinInfo : hostJoinList )
        {
            updateHostJoin( hostJoinInfo );
        }

        m_MyApp.getEngine().getHostJoinMgr().unlockHostJoinInfoList();
        updateJoinRequestCount( true );
    }
}

//============================================================================
void GuiHostedByMeJoinMgr::callbackHostJoinRequested( HostJoinInfo* hostJoinInfo )
{
    if( !hostJoinInfo )
    {
        LogMsg( LOG_ERROR, "GuiHostedByMeJoinMgr::callbackHostJoinAdded null hostJoinInfo" );
        return;
    }

    HostJoinInfo* hostJoin = new HostJoinInfo( *hostJoinInfo );

    emit signalInternalHostJoinRequested( hostJoin );
}

//============================================================================
void GuiHostedByMeJoinMgr::callbackHostJoinUpdated( HostJoinInfo* hostJoinInfo )
{
    if( !hostJoinInfo )
    {
        LogMsg( LOG_ERROR, "GuiHostedByMeJoinMgr::callbackHostJoinAdded null hostJoinInfo" );
        return;
    }

    // there is a possiblility of the hostJoinInfo delete while in signal queue so make copy and delete in slot
    HostJoinInfo* hostJoin = new HostJoinInfo( *hostJoinInfo );

    emit signalInternalHostJoinUpdated( hostJoin );
}

//============================================================================
void GuiHostedByMeJoinMgr::callbackHostUnJoin( GroupieId& groupieId )
{
    emit signalInternalHostUnJoin( groupieId );
}

//============================================================================
void GuiHostedByMeJoinMgr::callbackHostJoinRemoved( GroupieId& groupieId )
{
    emit signalInternalHostJoinRemoved( groupieId );
}

//============================================================================
void GuiHostedByMeJoinMgr::callbackHostJoinOfferState( GroupieId& groupieId, EJoinState joinOfferState )
{
    emit signalInternalHostJoinOfferState( groupieId, joinOfferState );
}

//============================================================================
void GuiHostedByMeJoinMgr::callbackHostJoinOnlineState( GroupieId& groupieId, EOnlineState onlineState, VxGUID& connectionId )
{
    emit signalInternalHostJoinOnlineState( groupieId, onlineState, connectionId );
}

//============================================================================
void GuiHostedByMeJoinMgr::slotInternalHostJoinRequested( HostJoinInfo* hostJoinInfo )
{
    updateHostJoin( hostJoinInfo );
    updateJoinRequestCount();
    delete hostJoinInfo; // was created new on callback
}

//============================================================================
void GuiHostedByMeJoinMgr::slotInternalHostJoinUpdated( HostJoinInfo* hostJoinInfo )
{
    updateHostJoin( hostJoinInfo );
    updateJoinRequestCount();
    delete hostJoinInfo;
}

//============================================================================
void GuiHostedByMeJoinMgr::slotInternalHostUnJoin( GroupieId groupieId )
{
    announceHostUnJoin( groupieId );

    auto iter = m_HostJoinList.find( groupieId );
    GuiHostJoin* joinInfo = nullptr;
    if( iter != m_HostJoinList.end() && groupieId.getHostType() != eHostTypeUnknown )
    {
        //emit signalHostJoinRemoved( onlineId, hostType );
        joinInfo = iter->second;

        if( !joinInfo->getUser()->isHosted() )
        {
            m_HostJoinList.erase( iter );
            joinInfo->deleteLater();
        }
    }

    updateJoinRequestCount();
}

//============================================================================
void GuiHostedByMeJoinMgr::slotInternalHostJoinRemoved( GroupieId groupieId )
{
    auto iter = m_HostJoinList.find( groupieId );
    GuiHostJoin* joinInfo = nullptr;
    if( iter != m_HostJoinList.end() && groupieId.getHostType() != eHostTypeUnknown )
    {
        //emit signalHostJoinRemoved( onlineId, hostType );
        joinInfo = iter->second;

        if( !joinInfo->getUser()->isHosted() )
        {
            m_HostJoinList.erase( iter );
            joinInfo->deleteLater();
        }
    }

    updateJoinRequestCount();
}

//============================================================================
void GuiHostedByMeJoinMgr::slotInternalHostJoinOfferState( GroupieId groupieId, EJoinState joinOfferState )
{
    GuiHostJoin* guiHostJoin = findHostJoin( groupieId );
    if( guiHostJoin && joinOfferState != eJoinStateNone )
    {
        if( guiHostJoin->getJoinState() != joinOfferState )
        {
            guiHostJoin->setJoinState( joinOfferState );
            emit signalHostJoinOfferStateChange( groupieId, joinOfferState );
        }
    }

    updateJoinRequestCount();
}

//============================================================================
void GuiHostedByMeJoinMgr::slotInternalHostJoinOnlineState( GroupieId groupieId, EOnlineState onlineState, VxGUID connectionId )
{
    GuiHostJoin* guiHostJoin = findHostJoin( groupieId );
    bool isOnline = onlineState == eOnlineStateOnline ? true : false;
    if( guiHostJoin && isOnline != guiHostJoin->isOnline() )
    {
        guiHostJoin->setHostOnlineStatus( isOnline );
        emit signalHostJoinOnlineStatus( guiHostJoin, isOnline );
    }

    updateJoinRequestCount();
}

//============================================================================
GuiHostJoin* GuiHostedByMeJoinMgr::findHostJoin( GroupieId groupieId )
{
    GuiHostJoin* guiHostJoin = nullptr;
    auto iter = m_HostJoinList.find( groupieId );
    if( iter != m_HostJoinList.end() )
    {
        guiHostJoin = iter->second;
    }

    return guiHostJoin;
}

//============================================================================
bool GuiHostedByMeJoinMgr::isHostJoinInSession( GroupieId& groupieId )
{
    GuiHostJoin* guiHostJoin = findHostJoin( groupieId );
    return guiHostJoin && guiHostJoin->isInSession();
}

//============================================================================
GuiHostJoin* GuiHostedByMeJoinMgr::getHostJoin( GroupieId& groupieId )
{
    return findHostJoin( groupieId );
}

//============================================================================
GuiHostJoin* GuiHostedByMeJoinMgr::updateHostJoin( HostJoinInfo* hostJoinInfo )
{
    if( !hostJoinInfo )
    {
        LogMsg( LOG_ERROR, "GuiHostedByMeJoinMgr::updateHostJoin invalid hostJoinInfo param" );
        return nullptr;
    }

    if( !hostJoinInfo->getNetIdent() )
    {
        LogMsg( LOG_ERROR, "GuiHostedByMeJoinMgr::updateHostJoin hostJoinInfo does not contain a net ident" );
        return nullptr;
    }

    GuiHostJoin* guiHostJoin = findHostJoin( hostJoinInfo->getGroupieId() );
    GuiUser* guiUser = m_MyApp.getUserMgr().updateUser( hostJoinInfo->getNetIdent() );
    if( guiUser )
    {
        if( guiHostJoin )
        {
            EJoinState prevState = guiHostJoin->getJoinState();
            guiHostJoin->setJoinState( hostJoinInfo->getJoinState() );
            onHostJoinUpdated( guiHostJoin, prevState );
        }
        else
        {
            guiHostJoin = new GuiHostJoin( m_MyApp );
            guiHostJoin->setGroupieId( hostJoinInfo->getGroupieId() );
            guiHostJoin->setUser( guiUser );
            guiHostJoin->getUser()->setNetIdent( hostJoinInfo->getNetIdent() );
            guiHostJoin->setJoinState( hostJoinInfo->getJoinState() );
            m_HostJoinList[hostJoinInfo->getGroupieId()] = guiHostJoin;
            onHostJoinAdded( guiHostJoin );
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiHostedByMeJoinMgr::updateHostJoin no user found" );
    }

    return guiHostJoin;
}

//============================================================================
void GuiHostedByMeJoinMgr::setHostJoinOffline( GroupieId& groupieId )
{
    GuiHostJoin* guiHostJoin = findHostJoin( groupieId );
    if( guiHostJoin )
    {
        guiHostJoin->setHostOnlineStatus( false );
    }
}

//============================================================================
void GuiHostedByMeJoinMgr::onHostJoinAdded( GuiHostJoin* guiHostJoin )
{
    announceJoinState( guiHostJoin, guiHostJoin->getJoinState() );
}

//============================================================================
void GuiHostedByMeJoinMgr::onUserOnlineStatusChange( GuiHostJoin* guiHostJoin, bool isOnline )
{
    if( isMessengerReady() )
    {
        emit signalHostJoinOnlineStatus( guiHostJoin, isOnline );
    }
}

//============================================================================
void GuiHostedByMeJoinMgr::onHostJoinUpdated( GuiHostJoin* guiHostJoin, EJoinState prevState )
{
    // BRJ temp always update for testing
    // if( prevState != guiHostJoin->getJoinState() )
    {
        announceJoinState( guiHostJoin, guiHostJoin->getJoinState() );
    }
}

//============================================================================
void GuiHostedByMeJoinMgr::updateJoinRequestCount( bool forceEmit )
{
    int hostRequestCount = 0;
    for( auto& item : m_HostJoinList )
    {
        GuiHostJoin* joinInfo = item.second;
        hostRequestCount += joinInfo->getHostRequestCount();
    }

    if( hostRequestCount != m_JoinRequestCount || forceEmit )
    {
        m_JoinRequestCount = hostRequestCount;
        for( auto& client : m_HostJoinClients )
        {
            client->callbackJoinRequestCount( m_JoinRequestCount );
        }
    }
}

//============================================================================
void GuiHostedByMeJoinMgr::joinAccepted( GuiHostJoin* guiHostJoin )
{
    if( guiHostJoin && guiHostJoin->setJoinState( eJoinStateJoinIsGranted ) )
    {
        m_MyApp.getEngine().getHostJoinMgr().changeJoinState( guiHostJoin->getGroupieId(), eJoinStateJoinIsGranted );
        announceJoinState( guiHostJoin, eJoinStateJoinIsGranted );
    }
}

//============================================================================
void GuiHostedByMeJoinMgr::joinRejected( GuiHostJoin* guiHostJoin )
{
    if( guiHostJoin && guiHostJoin->setJoinState( eJoinStateJoinDenied ) )
    {
        m_MyApp.getEngine().getHostJoinMgr().changeJoinState( guiHostJoin->getGroupieId(), eJoinStateJoinDenied );
        announceJoinState( guiHostJoin, eJoinStateJoinDenied );
    }
}

//============================================================================
void GuiHostedByMeJoinMgr::wantHostJoinCallbacks( GuiHostJoinCallback* client, bool enable )
{
    for( auto iter = m_HostJoinClients.begin(); iter != m_HostJoinClients.end(); ++iter )
    {
        if( *iter == client )
        {
            m_HostJoinClients.erase( iter );
            break;
        }
    }

    if( enable )
    {
        m_HostJoinClients.push_back( client );
    }
}

//============================================================================
void GuiHostedByMeJoinMgr::announceJoinState( GuiHostJoin* guiHostJoin, EJoinState joinState )
{
    if( guiHostJoin->getJoinState() != joinState )
    {
        LogMsg( LOG_ERROR, "GuiHostedByMeJoinMgr::announceJoinState join state %s != param state %s",
                GuiParams::describeJoinState( guiHostJoin->getJoinState() ).toUtf8().constData(),
                GuiParams::describeJoinState( joinState ).toUtf8().constData() );
    }

    switch( joinState )
    {
    case eJoinStateSending:
    case eJoinStateSendFail:
    case eJoinStateSendAcked:
    case eJoinStateJoinRequested:
        announceHostJoinRequested( guiHostJoin->getGroupieId(), guiHostJoin );
        break;
    case eJoinStateJoinWasGranted:
        announceHostJoinWasGranted( guiHostJoin->getGroupieId(), guiHostJoin );
        break;
    case eJoinStateJoinIsGranted:
        announceHostJoinIsGranted( guiHostJoin->getGroupieId(), guiHostJoin );
        break;
    case eJoinStateJoinDenied:
        announceHostJoinDenied( guiHostJoin->getGroupieId(), guiHostJoin );
        break;
    case eJoinStateJoinLeaveHost:
        announceHostJoinLeaveHost( guiHostJoin->getGroupieId() );
        break;
    default:
        LogMsg( LOG_ERROR, "GuiHostedByMeJoinMgr::announceJoinState unknown join state %s",
                GuiParams::describeJoinState( joinState ).toUtf8().constData() );
        break;
    }
}

//============================================================================
void GuiHostedByMeJoinMgr::announceHostJoinRequested( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
    for( auto& client : m_HostJoinClients )
    {
        client->callbackGuiHostJoinRequested( groupieId, guiHostJoin );
    }
}

//============================================================================
void GuiHostedByMeJoinMgr::announceHostJoinWasGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
    for( auto& client : m_HostJoinClients )
    {
        client->callbackGuiHostJoinWasGranted( groupieId, guiHostJoin );
    }
}

//============================================================================
void GuiHostedByMeJoinMgr::announceHostJoinIsGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
    for( auto& client : m_HostJoinClients )
    {
        client->callbackGuiHostJoinIsGranted( groupieId, guiHostJoin );
    }
}

//============================================================================
void GuiHostedByMeJoinMgr::announceHostJoinDenied( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
    for( auto& client : m_HostJoinClients )
    {
        client->callbackGuiHostJoinDenied( groupieId, guiHostJoin );
    }
}

//============================================================================
void GuiHostedByMeJoinMgr::announceHostJoinLeaveHost( GroupieId& groupieId )
{
    for( auto& client : m_HostJoinClients )
    {
        client->callbackGuiHostJoinLeaveHost( groupieId );
    }
}

//============================================================================
void GuiHostedByMeJoinMgr::announceHostUnJoin( GroupieId& groupieId )
{
    for( auto& client : m_HostJoinClients )
    {
        client->callbackGuiHostUnJoin( groupieId );
    }
}

//============================================================================
void GuiHostedByMeJoinMgr::announceHostJoinRemoved( GroupieId& groupieId )
{
    for( auto& client : m_HostJoinClients )
    {
        client->callbackGuiHostJoinRemoved( groupieId );
    }
}

//============================================================================
EJoinState GuiHostedByMeJoinMgr::getHostJoinState( GroupieId& groupieId )
{
    return m_MyApp.getEngine().getHostJoinMgr().getHostJoinState( groupieId );
}

//============================================================================
void GuiHostedByMeJoinMgr::getHostedMembers( EHostType hostType, std::set<VxGUID>& memberList )
{
    memberList.clear();
    VxGUID myOnlineId = m_MyApp.getMyOnlineId();
    for( auto groupie : m_HostJoinList )
    {
        if( groupie.first.isHostType( hostType ) && groupie.first.isHostOnlineId( myOnlineId ) )
        {
            VxGUID userId;
            groupie.first.getUserOnlineId( userId );
            memberList.insert( userId );
        }
    }
}
