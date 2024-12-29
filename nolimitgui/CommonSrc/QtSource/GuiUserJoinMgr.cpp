//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiUserJoinMgr.h"
#include "GuiUserJoinCallback.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiMemberActiveMgr.h"

#include <P2PEngine/P2PEngine.h>
#include <UserJoinMgr/UserJoinInfo.h>
#include <UserJoinMgr/UserJoinMgr.h>

#include <CoreLib/VxPtopUrl.h>
#include <PktLib/VxCommon.h>

#include <QTimer>

//============================================================================
GuiUserJoinMgr::GuiUserJoinMgr( AppCommon& app )
    : QObject( &app )
    , m_MyApp( app )
    , m_ReconnectToHostTimer( new QTimer( this ) )
{
    m_ReconnectToHostTimer->setInterval( 3 * 60 * 1000 );
    connect( m_ReconnectToHostTimer, SIGNAL(timeout()), this, SLOT(slotReconnectToLastConnectedHost()) );
}

//============================================================================
void GuiUserJoinMgr::onAppCommonCreated( void )
{
    connect( this, SIGNAL(signalInternalUserJoinRequested(UserJoinInfo*)), this, SLOT(slotInternalUserJoinRequested(UserJoinInfo*)), Qt::QueuedConnection );
    connect( this, SIGNAL(signalInternalUserJoinUpdated(UserJoinInfo*)), this, SLOT(slotInternalUserJoinUpdated(UserJoinInfo*)), Qt::QueuedConnection );
    connect( this, SIGNAL(signalInternalUserUnJoinUpdated(UserJoinInfo*)), this, SLOT(slotInternalUserUnJoinUpdated(UserJoinInfo*)), Qt::QueuedConnection );
    connect( this, SIGNAL(signalInternalUserJoinRemoved(GroupieId)), this, SLOT(slotInternalUserJoinRemoved(GroupieId)), Qt::QueuedConnection );
    connect( this, SIGNAL(signalInternalUserJoinOfferState(GroupieId,EJoinState)), this, SLOT(slotInternalUserJoinOfferState(GroupieId,EJoinState)), Qt::QueuedConnection );
    connect( this, SIGNAL(signalInternalUserJoinOnlineState(GroupieId,EOnlineState,VxGUID)), this, SLOT(slotInternalUserJoinOnlineState(GroupieId,EOnlineState,VxGUID)), Qt::QueuedConnection );

    connect( this, SIGNAL(signalInternalUserJoinAHostStatus(EHostType,VxGUID,EConnectStatus)), this, SLOT(slotInternalUserJoinAHostStatus(EHostType,VxGUID,EConnectStatus)), Qt::QueuedConnection );

    m_MyApp.getEngine().getUserJoinMgr().addUserJoinMgrClient( this, true );
}

//============================================================================
bool GuiUserJoinMgr::isMessengerReady( void )
{
    return m_MyApp.isMessengerReady();
}

//============================================================================
void GuiUserJoinMgr::callbackUserJoinAdded( UserJoinInfo* userJoinInfo )
{
    if( !userJoinInfo )
    {
        LogMsg( LOG_ERROR, "GuiUserJoinMgr::callbackUserJoinAdded null userJoinInfo" );
        return;
    }

    emit signalInternalUserJoinRequested( new UserJoinInfo( *userJoinInfo ) );
}

//============================================================================
void GuiUserJoinMgr::callbackUserJoinUpdated( UserJoinInfo* userJoinInfo )
{
    if( !userJoinInfo )
    {
        LogMsg( LOG_ERROR, "GuiUserJoinMgr::callbackUserJoinAdded null userJoinInfo" );
        return;
    }

    emit signalInternalUserJoinUpdated( new UserJoinInfo(*userJoinInfo) );
}

//============================================================================
void GuiUserJoinMgr::callbackUserUnJoinUpdated( UserJoinInfo* userJoinInfo )
{
    if( !userJoinInfo )
    {
        LogMsg( LOG_ERROR, "GuiUserJoinMgr::callbackUserJoinAdded null userJoinInfo" );
        return;
    }

    emit signalInternalUserUnJoinUpdated( new UserJoinInfo( *userJoinInfo ) );
}

//============================================================================
void GuiUserJoinMgr::callbackUserJoinRemoved( GroupieId& groupieId )
{
    emit signalInternalUserJoinRemoved( groupieId );
}

//============================================================================
void GuiUserJoinMgr::callbackUserJoinOfferState( GroupieId& groupieId, EJoinState userOfferState )
{
    emit signalInternalUserJoinOfferState( groupieId, userOfferState );
}

//============================================================================
void GuiUserJoinMgr::callbackUserJoinOnlineState( GroupieId& groupieId, EOnlineState onlineState, VxGUID& connectionId )
{
    emit signalInternalUserJoinOnlineState( groupieId, onlineState, connectionId );
}

//============================================================================
void GuiUserJoinMgr::slotInternalUserJoinRequested( UserJoinInfo* userJoinInfo )
{
    stopReconnectToLastConnectedHost();
    updateUserJoin( userJoinInfo );
    delete userJoinInfo;
}

//============================================================================
void GuiUserJoinMgr::slotInternalUserJoinUpdated( UserJoinInfo* userJoinInfo )
{
    stopReconnectToLastConnectedHost();
    updateUserJoin( userJoinInfo );
    delete userJoinInfo;
}

//============================================================================
void GuiUserJoinMgr::slotInternalUserUnJoinUpdated( UserJoinInfo* userJoinInfo )
{
    stopReconnectToLastConnectedHost();
    updateUserJoin( userJoinInfo, true  );
    delete userJoinInfo;
}

//============================================================================
void GuiUserJoinMgr::slotInternalUserJoinRemoved( GroupieId groupieId )
{
    stopReconnectToLastConnectedHost();
    announceUserJoinRemoved( groupieId );
    auto iter = m_UserJoinList.find( groupieId );
    GuiUserJoin* guiUserJoin = nullptr;
    if( iter != m_UserJoinList.end() )
    {
        emit signalUserJoinRemoved( groupieId );
        guiUserJoin = iter->second;
        m_UserJoinList.erase( iter );
        guiUserJoin->deleteLater();
    }
}

//============================================================================
void GuiUserJoinMgr::slotInternalUserJoinOfferState( GroupieId groupieId, EJoinState joinOfferState )
{
    stopReconnectToLastConnectedHost();
    GuiUserJoin* guiUserJoin = findUserJoin( groupieId );
    if( guiUserJoin && joinOfferState != eJoinStateNone )
    {
        if( guiUserJoin->setJoinState( joinOfferState ) )
        {
            emit signalUserJoinOfferStateChange( groupieId, joinOfferState );
        }
    }
}

//============================================================================
void GuiUserJoinMgr::slotInternalUserJoinOnlineState( GroupieId groupieId, EOnlineState onlineState, VxGUID connectionId )
{
    stopReconnectToLastConnectedHost();
    GuiUserJoin* guiUserJoin = findUserJoin( groupieId );
    bool isOnline = onlineState == eOnlineStateOnline ? true : false;
    if( guiUserJoin && isOnline != guiUserJoin->isOnline() )
    {
        // guiUserJoin->setOnlineStatus( isOnline );
        emit signalUserJoinOnlineStatus( guiUserJoin, isOnline );
    }
}

//============================================================================
GuiUserJoin* GuiUserJoinMgr::findUserJoin( GroupieId& groupieId )
{
    GuiUserJoin* user = nullptr;
    auto iter = m_UserJoinList.find( groupieId );
    if( iter != m_UserJoinList.end() )
    {
        user = iter->second;
    }

    return user;
}

//============================================================================
void GuiUserJoinMgr::removeUserJoin( GroupieId& groupieId )
{
    auto iter = m_UserJoinList.find( groupieId );
    if( iter != m_UserJoinList.end() )
    {
        iter->second->deleteLater();
        m_UserJoinList.erase( iter );
    }
}

//============================================================================
bool GuiUserJoinMgr::isUserJoinInSession( GroupieId& groupieId )
{
    GuiUserJoin* guiUserJoin = findUserJoin( groupieId );
    return guiUserJoin && guiUserJoin->isInSession();
}

//============================================================================
GuiUserJoin* GuiUserJoinMgr::updateUserJoin( VxNetIdent* hisIdent, EHostType hostType )
{
    if( !hisIdent )
    {
        LogMsg( LOG_ERROR, "GuiUserJoinMgr::updateUserJoinOnline invalid param" );
        return nullptr;
    }

    VxGUID myOnlineId = m_MyApp.getMyOnlineId();
    GroupieId groupieId( myOnlineId, hisIdent->getMyOnlineId(), hostType );
    GuiUserJoin* guiUserJoin = findUserJoin( groupieId );
    GuiUser* guiUser = m_MyApp.getUserMgr().updateUser( hisIdent );
    if( guiUser )
    {
        if( guiUserJoin )
        {
            onUserJoinUpdated( guiUserJoin );
        }
        else
        {
            guiUserJoin = new GuiUserJoin( m_MyApp );
            guiUserJoin->setGroupieId( groupieId );
            guiUserJoin->setUser( guiUser );
            m_UserJoinList[guiUserJoin->getGroupieId()] = guiUserJoin;
            onUserJoinAdded( guiUserJoin );
        }
    }

    return guiUserJoin;
}

//============================================================================
GuiUserJoin* GuiUserJoinMgr::updateUserJoin( UserJoinInfo* userJoinInfo, bool unJoin )
{
    GuiUserJoin* guiUserJoin = findUserJoin( userJoinInfo->getGroupieId() );
    GuiUser* guiUser = m_MyApp.getUserMgr().updateUser( userJoinInfo->getNetIdent() );
    if( guiUser )
    {
        if( guiUserJoin )
        {
            guiUserJoin->setJoinState( userJoinInfo->getJoinState() );
            if( unJoin )
            {
                LogModule( eLogHostJoin, LOG_VERBOSE, "GuiUserJoinMgr::updateUserJoin unjoin state %s %s", DescribeJoinState( guiUserJoin->getJoinState() ),
                        m_MyApp.describeGroupieId( guiUserJoin->getGroupieId() ).c_str() );
                onUserUnJoinUpdated( guiUserJoin );
            }
            else
            {
                LogModule( eLogHostJoin, LOG_VERBOSE, "GuiUserJoinMgr::updateUserJoin update state %s %s", DescribeJoinState( guiUserJoin->getJoinState() ),         m_MyApp.describeGroupieId( guiUserJoin->getGroupieId() ).c_str() );
                onUserJoinUpdated( guiUserJoin );
            }
        }
        else
        {
            guiUserJoin = new GuiUserJoin( m_MyApp, guiUser, userJoinInfo );
            guiUserJoin->setJoinState( userJoinInfo->getJoinState() );
            m_UserJoinList[guiUserJoin->getGroupieId()] = guiUserJoin;
            onUserJoinAdded( guiUserJoin );
            LogModule( eLogHostJoin, LOG_VERBOSE, "GuiUserJoinMgr::updateUserJoin add state %s %s", DescribeJoinState( guiUserJoin->getJoinState() ),
                    m_MyApp.describeGroupieId( guiUserJoin->getGroupieId() ).c_str() );
        }
    }

    return guiUserJoin;
}

//============================================================================
void GuiUserJoinMgr::onUserJoinAdded( GuiUserJoin* guiUserJoin )
{
    announceUserJoinState( guiUserJoin->getJoinState(), guiUserJoin );
}

//============================================================================
void GuiUserJoinMgr::onUserJoinRemoved( GroupieId& groupieId )
{
    announceUserJoinRemoved( groupieId );
}

//============================================================================
void GuiUserJoinMgr::onUserOnlineStatusChange( GuiUserJoin* guiUserJoin, bool isOnline )
{
    if( isMessengerReady() )
    {
        emit signalUserJoinOnlineStatus( guiUserJoin, isOnline );
    }
}

//============================================================================
void GuiUserJoinMgr::onUserJoinUpdated( GuiUserJoin* guiUserJoin )
{
    announceUserJoinState( guiUserJoin->getJoinState(), guiUserJoin );
}

//============================================================================
void GuiUserJoinMgr::announceUserJoinState( EJoinState joinState, GuiUserJoin* guiUserJoin )
{
    switch( joinState )
    {
    case eJoinStateJoinRequested:
        announceUserJoinRequested( guiUserJoin->getGroupieId(), guiUserJoin );
        break;
    case eJoinStateJoinWasGranted:
        announceUserJoinWasGranted( guiUserJoin->getGroupieId(), guiUserJoin );
        break;
    case eJoinStateJoinIsGranted:
        announceUserJoinIsGranted( guiUserJoin->getGroupieId(), guiUserJoin );
        break;
    case eJoinStateJoinDenied:
        announceUserJoinDenied( guiUserJoin->getGroupieId(), guiUserJoin );
        break;
    case eJoinStateJoinLeaveHost:
        announceUserJoinLeaveHost( guiUserJoin->getGroupieId() );
        break;

    case eJoinStateSending:
    case eJoinStateSendFail:
    case eJoinStateSendAcked:
    case eJoinStateNone:
    default:
        break;
    }

    if( guiUserJoin->getGroupieId().isUserOnlineId( m_MyApp.getMyOnlineId() ) )
    {
        if( guiUserJoin->getGroupieId().getHostType() == eHostTypeGroup &&
            joinState != m_LastGroupJoinState )
        {
            m_LastGroupJoinState = joinState;
            if( joinState == eJoinStateJoinIsGranted )
            {
                m_LastGroupJoinHostOnlineId = guiUserJoin->getGroupieId().getHostOnlineId();
            }

            announceUserJoinedToHostState( eHostTypeGroup, joinState == eJoinStateJoinIsGranted );
        }
        else if( guiUserJoin->getGroupieId().getHostType() == eHostTypeChatRoom &&
            joinState != m_LastChatRoomJoinState )
        {
            m_LastChatRoomJoinState = joinState;
            if( joinState == eJoinStateJoinIsGranted )
            {
                m_LastChatRoomJoinHostOnlineId = guiUserJoin->getGroupieId().getHostOnlineId();
            }

            announceUserJoinedToHostState( eHostTypeChatRoom, joinState == eJoinStateJoinIsGranted );
        }
        else if( guiUserJoin->getGroupieId().getHostType() == eHostTypeRandomConnect &&
            joinState != m_LastRandomConnectJoinState )
        {
            m_LastRandomConnectJoinState = joinState;
            if( joinState == eJoinStateJoinIsGranted )
            {
                m_LastRandomeConnectJoinHostOnlineId = guiUserJoin->getGroupieId().getHostOnlineId();
            }

            announceUserJoinedToHostState( eHostTypeRandomConnect, joinState == eJoinStateJoinIsGranted );
        }
    }
}

//============================================================================
void GuiUserJoinMgr::onUserUnJoinUpdated( GuiUserJoin* guiUserJoin )
{
    announceUserJoinState( guiUserJoin->getJoinState(), guiUserJoin );
}

//============================================================================
void GuiUserJoinMgr::onMyIdentUpdated( GuiUserJoin* guiUserJoin )
{
    if( isMessengerReady() )
    {
        emit signalMyIdentUpdated( guiUserJoin );
    }
}

//============================================================================
void GuiUserJoinMgr::wantUserJoinCallbacks( GuiUserJoinCallback* client, bool enable )
{
    for( auto iter = m_UserJoinClients.begin(); iter != m_UserJoinClients.end(); ++iter )
    {
        if( *iter == client )
        {
            m_UserJoinClients.erase( iter );
            break;
        }
    }

    if( enable )
    {
        m_UserJoinClients.push_back( client );
    }
}

//============================================================================
void GuiUserJoinMgr::announceUserJoinRequested( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
    for( auto client : m_UserJoinClients )
    {
        client->callbackGuiUserJoinRequested( groupieId, guiUserJoin );
    }
}

//============================================================================
void GuiUserJoinMgr::announceUserJoinWasGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
    for( auto client : m_UserJoinClients )
    {
        client->callbackGuiUserJoinWasGranted( groupieId, guiUserJoin );
    }
}

//============================================================================
void GuiUserJoinMgr::announceUserJoinIsGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
    for( auto client : m_UserJoinClients )
    {
        client->callbackGuiUserJoinIsGranted( groupieId, guiUserJoin );
    }

    VxPtopUrl ptopUrl( m_LastJoinAttemptedHostInviteUrl );
    if( ptopUrl.isValid() && ptopUrl.getOnlineId() == groupieId.getHostOnlineId() )
    {
        m_MyApp.getAppSettings().setLastHostJoined( m_LastJoinAttemptedHostInviteUrl );
        m_LastJoinAttemptedHostInviteUrl.clear();
    }
}

//============================================================================
void GuiUserJoinMgr::announceUserUnJoinGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
    for( auto client : m_UserJoinClients )
    {
        client->callbackGuiUserUnJoinGranted( groupieId, guiUserJoin );
    }
}

//============================================================================
void GuiUserJoinMgr::announceUserJoinDenied( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
    for( auto client : m_UserJoinClients )
    {
        client->callbackGuiUserJoinDenied( groupieId, guiUserJoin );
    }
}

//============================================================================
void GuiUserJoinMgr::announceUserJoinLeaveHost( GroupieId& groupieId )
{
    for( auto client : m_UserJoinClients )
    {
        client->callbackGuiUserJoinLeaveHost( groupieId );
    }
}

//============================================================================
void GuiUserJoinMgr::announceUserJoinRemoved( GroupieId& groupieId )
{
    for( auto client : m_UserJoinClients )
    {
        client->callbackGuiUserJoinRemoved( groupieId );
    }
}

//============================================================================
void GuiUserJoinMgr::announceUserJoinedToHostState( EHostType hostType, bool isJoined )
{
    for( auto client : m_UserJoinClients )
    {
        client->callbackGuiUserJoinToHostState( hostType, isJoined );
    }
}

//============================================================================
void GuiUserJoinMgr::announceUserJoinAHostStatus( EHostType hostType, VxGUID& sessionId, EConnectStatus connectStatus )
{
    for( auto client : m_UserJoinClients )
    {
        client->callbackGuiUserJoinAHostStatus( hostType, sessionId, connectStatus );
    }
}

//============================================================================
EJoinState GuiUserJoinMgr::getUserJoinState( GroupieId& groupieId )
{
    EJoinState joinState = m_MyApp.getEngine().getUserJoinMgr().getUserJoinState( groupieId );
    bool isActive = m_MyApp.getMemberActiveMgr().isMemberActive( groupieId );

    if( ( eJoinStateJoinWasGranted == joinState || eJoinStateJoinIsGranted == joinState ) &&
        groupieId.getHostOnlineId() != groupieId.getUserOnlineId() )
    {    
        if( !isActive )
        {
            return eJoinStateJoinLeaveHost;
        }
    }

    if( isActive )
    {
        joinState = eJoinStateJoinIsGranted;
    }

    return joinState;
}

//============================================================================
bool GuiUserJoinMgr::isUserJoinedToHost( EHostType hostType )
{
    return m_MyApp.getMemberActiveMgr().isMemberOfHostType( hostType, m_MyApp.getMyOnlineId() );
}

//============================================================================
bool GuiUserJoinMgr::isUserJoinedToHost( HostedId& adminId )
{
    GroupieId groupieId( m_MyApp.getMyOnlineId(), adminId );
    return isMemberActive( groupieId );
}

//============================================================================
bool GuiUserJoinMgr::isMemberActive( GroupieId& groupieId )
{
    return m_MyApp.getMemberActiveMgr().isMemberActive( groupieId );
}

//============================================================================
VxGUID& GuiUserJoinMgr::getUserJoinedHostOnlineId( EHostType hostType )
{
	switch( hostType )
	{
	case eHostTypeGroup:
        return m_LastGroupJoinHostOnlineId;

	case eHostTypeChatRoom:
		return m_LastChatRoomJoinHostOnlineId;

	case eHostTypeRandomConnect:
        return m_LastRandomeConnectJoinHostOnlineId;

	default:
		return VxGUID::nullVxGUID();
	}
}

//============================================================================
void GuiUserJoinMgr::reconnectToLastConnectedHost( std::string& lastConnectedHost )
{
    m_ReconnectToHost = lastConnectedHost;
    slotReconnectToLastConnectedHost();
    m_ReconnectToHostTimer->start();
}

//============================================================================
void GuiUserJoinMgr::stopReconnectToLastConnectedHost( void )
{
    m_ReconnectToHostTimer->stop();
    m_ReconnectToHost.clear();
}

//============================================================================
void GuiUserJoinMgr::slotReconnectToLastConnectedHost( void )
{
    m_ReconnectToHostAttempts++;
    VxPtopUrl ptopUrl( m_ReconnectToHost );
    EHostType hostType = ptopUrl.getHostType();
    if( IsHostARelayForUsers( hostType ) )
    {
        if( ptopUrl.isValid() )
        {
            if( m_MyApp.getMemberActiveMgr().isMemberOfHostType( hostType, m_MyApp.getMyOnlineId() ) )
            {
                // already connected and joined a host
                stopReconnectToLastConnectedHost();
                return;
            }

            if( ptopUrl.getOnlineId() != m_MyApp.getMyOnlineId() )
            {
                std::string hostUrl = ptopUrl.getHostUrl();

                VxGUID sessionId;
                sessionId.initializeWithNewVxGUID();

                LogModule( eLogUserEvent, LOG_VERBOSE, "checkReadyToConnectToLastConnectedHost attempting rejoin hot url %s", m_ReconnectToHost.c_str() );

                HostedId adminId( ptopUrl.getOnlineId(), hostType );
                m_MyApp.getFromGuiInterface().fromGuiJoinHost( adminId, sessionId, hostUrl );
            }
            else
            {
                LogModule( eLogUserEvent, LOG_ERROR, "checkReadyToConnectToLastConnectedHost cannot connect to ourself" );
                m_MyApp.getAppSettings().setLastHostJoined("");
                stopReconnectToLastConnectedHost();
            }
        }
        else
        {
            LogModule( eLogUserEvent, LOG_VERBOSE, "checkReadyToConnectToLastConnectedHost invalid ptop url %s", m_ReconnectToHost.c_str() );
            stopReconnectToLastConnectedHost();
        }
    }
    else
    {
        LogModule( eLogUserEvent, LOG_VERBOSE, "checkReadyToConnectToLastConnectedHost invalid host type for url %s", m_ReconnectToHost.c_str() );
        stopReconnectToLastConnectedHost();
    }

    if( m_ReconnectToHostAttempts >= 3 )
    {
        stopReconnectToLastConnectedHost();
    }
}


//============================================================================
void GuiUserJoinMgr::callbackUserJoinAHostStatus( EHostType hostType, VxGUID& sessionId, EConnectStatus connectStatus )
{
    emit signalInternalUserJoinAHostStatus( hostType, sessionId, connectStatus );
}

//============================================================================
void GuiUserJoinMgr::slotInternalUserJoinAHostStatus( EHostType hostType, VxGUID sessionId, EConnectStatus connectStatus )
{
    announceUserJoinAHostStatus( hostType, sessionId, connectStatus );
}
