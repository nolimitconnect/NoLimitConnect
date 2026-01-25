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

#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiMemberActiveMgr.h"
#include "GuiMemberActiveMgr.h"
#include "GuiUserJoinCallback.h"

#include <P2PEngine/P2PEngine.h>
#include <UserJoinMgr/UserJoinInfo.h>
#include <UserJoinMgr/UserJoinMgr.h>

#include <CoreLib/VxDebug.h>
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

    m_LastJoinChatRoomUrl.setUrl( m_MyApp.getAppSettings().getLastHostJoined( eHostTypeChatRoom ) );
    m_LastJoinGroupUrl.setUrl( m_MyApp.getAppSettings().getLastHostJoined( eHostTypeGroup ) );
    m_LastJoinRandomConnectUrl.setUrl( m_MyApp.getAppSettings().getLastHostJoined( eHostTypeRandomConnect ) );
}

//============================================================================
void GuiUserJoinMgr::onAppCommonCreated( void )
{
    connect( this, SIGNAL(signalInternalUserJoinRequested(UserJoinInfo*)), this, SLOT(slotInternalUserJoinRequested(UserJoinInfo*)), Qt::QueuedConnection );
    connect( this, SIGNAL(signalInternalUserJoinUpdated(UserJoinInfo*)), this, SLOT(slotInternalUserJoinUpdated(UserJoinInfo*)), Qt::QueuedConnection );
    connect( this, SIGNAL(signalInternalUserUnJoin(UserJoinInfo*)), this, SLOT(slotInternalUserUnJoin(UserJoinInfo*)), Qt::QueuedConnection );
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
        LogMsg( LOG_ERROR, "GuiUserJoinMgr::%s null userJoinInfo", __func__ );
        return;
    }

    if( !userJoinInfo->getNetIdent().isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "GuiUserJoinMgr::%s invalid userJoinInfo->getNetIdent()->isValidNetIdent()", __func__ );
        return;
    }

    emit signalInternalUserJoinRequested( new UserJoinInfo( *userJoinInfo ) );
}

//============================================================================
void GuiUserJoinMgr::slotInternalUserJoinRequested( UserJoinInfo* userJoinInfo )
{
    stopReconnectToLastConnectedHost();
    updateUserJoin( userJoinInfo );
    delete userJoinInfo;
}

//============================================================================
void GuiUserJoinMgr::callbackUserJoinUpdated( UserJoinInfo* userJoinInfo )
{
    if( !userJoinInfo )
    {
        LogMsg( LOG_ERROR, "GuiUserJoinMgr::%s null userJoinInfo", __func__ );
        return;
    }

    emit signalInternalUserJoinUpdated( new UserJoinInfo(*userJoinInfo) );
}

//============================================================================
void GuiUserJoinMgr::slotInternalUserJoinUpdated( UserJoinInfo* userJoinInfo )
{
    stopReconnectToLastConnectedHost();
    updateUserJoin( userJoinInfo );
    delete userJoinInfo;
}

//============================================================================
void GuiUserJoinMgr::callbackUserUnJoin( UserJoinInfo* userJoinInfo )
{
    if( !userJoinInfo )
    {
        LogMsg( LOG_ERROR, "GuiUserJoinMgr::%s null userJoinInfo", __func__ );
        return;
    }

    emit signalInternalUserUnJoin( new UserJoinInfo( *userJoinInfo ) );
}

//============================================================================
void GuiUserJoinMgr::slotInternalUserUnJoin( UserJoinInfo* userJoinInfo )
{
    stopReconnectToLastConnectedHost();
    updateUserJoin( userJoinInfo, true );
    delete userJoinInfo;
}

//============================================================================
void GuiUserJoinMgr::callbackUserJoinRemoved( GroupieId& groupieId )
{
    emit signalInternalUserJoinRemoved( groupieId );
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
void GuiUserJoinMgr::callbackUserJoinOfferState( GroupieId& groupieId, EJoinState userOfferState )
{
    emit signalInternalUserJoinOfferState( groupieId, userOfferState );
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
void GuiUserJoinMgr::callbackUserJoinOnlineState( GroupieId& groupieId, EOnlineState onlineState, VxGUID& connectionId )
{
    emit signalInternalUserJoinOnlineState( groupieId, onlineState, connectionId );
}

//============================================================================
void GuiUserJoinMgr::slotInternalUserJoinOnlineState( GroupieId groupieId, EOnlineState onlineState, VxGUID connectionId )
{
    stopReconnectToLastConnectedHost();
    GuiUserJoin* guiUserJoin = findUserJoin( groupieId );
    bool isOnline = onlineState == eOnlineStateOnline ? true : false;
    if( guiUserJoin && isOnline != guiUserJoin->isOnline() )
    {
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
    GroupieId groupieId = userJoinInfo->getGroupieId();
    GuiUserJoin* guiUserJoin = findUserJoin( groupieId );
    GuiUser* guiUser = m_MyApp.getUserMgr().updateUser( &userJoinInfo->getNetIdent() );
    if( guiUser )
    {
        EJoinState joinState = userJoinInfo->getJoinState();
        if( joinState == eJoinStateJoinIsGranted )
        {
            if( groupieId.getUserOnlineId() == m_MyApp.getMyOnlineId() && groupieId.getHostOnlineId() != m_MyApp.getMyOnlineId() )
            {
                // we joined a host.. update last joined
                m_LastJoinAttemptedHostInviteUrl.clear();

                std::string hostUrl = userJoinInfo->getHostUrl();
                VxPtopUrl ptopUrl( hostUrl );
                ptopUrl.setUrlHostType( groupieId.getHostType() );
                if( ptopUrl.isValid() && ptopUrl.getOnlineId() == groupieId.getHostOnlineId() )
                {
                    m_MyApp.getAppSettings().setLastHostJoined( groupieId.getHostType(), ptopUrl.getHostUrl() );
                }
                else
                {
                    LogMsg( LOG_SEVERE, "GuiUserJoinMgr::%s invalid host url", __func__ );
                    vx_assert( false );
                    return nullptr;
                }
            }
        }

        if( LogEnabled( eLogHostJoin ) )LogModule( eLogHostJoin, LOG_VERBOSE, "GuiUserJoinMgr::%s user %s state %s %s", __func__,
                    guiUser->getOnlineName().c_str(),
                    DescribeJoinState( joinState ),
                    m_MyApp.describeGroupieId( groupieId ).c_str() );
            

        if( guiUserJoin )
        {
            guiUserJoin->setJoinState( joinState );
            if( unJoin )
            {
                if( LogEnabled( eLogHostJoin ) )LogModule( eLogHostJoin, LOG_VERBOSE, "GuiUserJoinMgr::%s unjoin state %s %s", __func__, DescribeJoinState( joinState ),
                        m_MyApp.describeGroupieId( groupieId ).c_str() );
                onUserUnJoin( guiUserJoin );
            }
            else
            {
                onUserJoinUpdated( guiUserJoin );
            }
        }
        else
        {
            guiUserJoin = new GuiUserJoin( m_MyApp, guiUser, userJoinInfo );
            guiUserJoin->setJoinState( joinState );
            m_UserJoinList[ groupieId ] = guiUserJoin;
            onUserJoinAdded( guiUserJoin );
            if( LogEnabled( eLogHostJoin ) )LogModule( eLogHostJoin, LOG_VERBOSE, "GuiUserJoinMgr::%s add state %s %s", __func__, DescribeJoinState( joinState ),
                    m_MyApp.describeGroupieId( groupieId ).c_str() );
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiUserJoinMgr::%s guiUser is null", __func__ );
        vx_assert( false );
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
    bool saveLastJoined{ false };
    GroupieId adminHostId = guiUserJoin->getGroupieId();
    VxGUID hostOnlineId = adminHostId.getHostOnlineId();
    bool hostOnline = m_MyApp.getConnectIdListMgr().isOnline( hostOnlineId  );
    if( !hostOnline )
    {
        LogMsg( LOG_ERROR, "GuiUserJoinMgr::%s host %s is offline", __func__, m_MyApp.describeUser( hostOnlineId ).c_str() );
    }

    switch( joinState )
    {
    case eJoinStateJoinRequested:
        announceUserJoinRequested( adminHostId, guiUserJoin );
        break;
    case eJoinStateJoinWasGranted:
        announceUserJoinWasGranted( adminHostId, guiUserJoin );
        break;
    case eJoinStateJoinIsGranted:
        announceUserJoinIsGranted( adminHostId, guiUserJoin );
        saveLastJoined = true;
        break;
    case eJoinStateJoinDenied:
        announceUserJoinDenied( adminHostId, guiUserJoin );
        break;
    case eJoinStateJoinLeaveHost:
        announceUserJoinLeaveHost( adminHostId );
        break;

    case eJoinStateSending:
    case eJoinStateSendFail:
    case eJoinStateSendAcked:
    case eJoinStateNone:
    default:
        break;
    }

    if( LogEnabled( eLogHostJoin ) ) LogMsg( LOG_VERBOSE, "GuiUserJoinMgr::%s %s state %s", __func__,
        m_MyApp.describeGroupieId( adminHostId ).c_str(), DescribeJoinState( joinState ) );

    if( hostOnline && saveLastJoined )
    {
        GuiUser* hostAdmin = m_MyApp.getUserMgr().getUser( hostOnlineId );
        if( hostAdmin )
        {
            std::string hostUrl = hostAdmin->getMyOnlineUrl();
            VxPtopUrl ptopUrl( hostUrl );
            ptopUrl.setUrlHostType( adminHostId.getHostType(), true );
            if( ptopUrl.isValid() )
            {
                saveLastHostJoined( adminHostId.getHostType(), ptopUrl );
            }
        }
    }
}

//============================================================================
void GuiUserJoinMgr::onUserUnJoin( GuiUserJoin* guiUserJoin )
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
        m_UserJoinClients.emplace_back( client );
    }
}

//============================================================================
void GuiUserJoinMgr::announceUserJoinRequested( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
    for( auto& client : m_UserJoinClients )
    {
        client->callbackGuiUserJoinRequested( groupieId, guiUserJoin );
    }
}

//============================================================================
void GuiUserJoinMgr::announceUserJoinWasGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
    for( auto& client : m_UserJoinClients )
    {
        client->callbackGuiUserJoinWasGranted( groupieId, guiUserJoin );
    }
}

//============================================================================
void GuiUserJoinMgr::announceUserJoinIsGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
    for( auto& client : m_UserJoinClients )
    {
        client->callbackGuiUserJoinIsGranted( groupieId, guiUserJoin );
    }
}

//============================================================================
void GuiUserJoinMgr::announceUserUnJoinGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
    for( auto& client : m_UserJoinClients )
    {
        client->callbackGuiUserUnJoinGranted( groupieId, guiUserJoin );
    }
}

//============================================================================
void GuiUserJoinMgr::announceUserJoinDenied( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
    for( auto& client : m_UserJoinClients )
    {
        client->callbackGuiUserJoinDenied( groupieId, guiUserJoin );
    }
}

//============================================================================
void GuiUserJoinMgr::announceUserJoinLeaveHost( GroupieId& groupieId )
{
    for( auto& client : m_UserJoinClients )
    {
        client->callbackGuiUserJoinLeaveHost( groupieId );
    }
}

//============================================================================
void GuiUserJoinMgr::announceUserJoinRemoved( GroupieId& groupieId )
{
    for( auto& client : m_UserJoinClients )
    {
        client->callbackGuiUserJoinRemoved( groupieId );
    }
}

//============================================================================
void GuiUserJoinMgr::announceUserJoinedToHostState( EHostType hostType, bool isJoined )
{
    for( auto& client : m_UserJoinClients )
    {
        client->callbackGuiUserJoinToHostState( hostType, isJoined );
    }
}

//============================================================================
void GuiUserJoinMgr::announceUserJoinAHostStatus( EHostType hostType, VxGUID& sessionId, EConnectStatus connectStatus )
{
    for( auto& client : m_UserJoinClients )
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

                LogModule( eLogUsers, LOG_VERBOSE, "checkReadyToConnectToLastConnectedHost attempting rejoin hot url %s", m_ReconnectToHost.c_str() );

                HostedId adminId( ptopUrl.getOnlineId(), hostType );
                m_MyApp.getFromGuiInterface().fromGuiJoinHost( adminId, sessionId, hostUrl );
            }
            else
            {
                LogModule( eLogUsers, LOG_ERROR, "checkReadyToConnectToLastConnectedHost cannot connect to ourself" );
                m_MyApp.getAppSettings().setLastHostJoined("");
                stopReconnectToLastConnectedHost();
            }
        }
        else
        {
            LogModule( eLogUsers, LOG_VERBOSE, "checkReadyToConnectToLastConnectedHost invalid ptop url %s", m_ReconnectToHost.c_str() );
            stopReconnectToLastConnectedHost();
        }
    }
    else
    {
        LogModule( eLogUsers, LOG_VERBOSE, "checkReadyToConnectToLastConnectedHost invalid host type for url %s", m_ReconnectToHost.c_str() );
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

//============================================================================
GroupieId GuiUserJoinMgr::getJoinedAdminGroupieId( EHostType hostType )
{
    return m_MyApp.getMemberActiveMgr().getJoinedAdminGroupieId( hostType, m_MyApp.getMyOnlineId() );
}

//============================================================================
void GuiUserJoinMgr::leaveHost( EHostType hostType )
{
    GroupieId adminGroupieId = getJoinedAdminGroupieId( hostType );
    if( adminGroupieId.isValid() )
    {
        leaveHost( adminGroupieId.getHostedId() );
    }
}

//============================================================================
void GuiUserJoinMgr::leaveHost( HostedId hostedId )
{
    m_MyApp.getFromGuiInterface().fromGuiLeaveHost( hostedId );
}

//============================================================================
void GuiUserJoinMgr::unjoinHost( HostedId hostedId )
{
    m_MyApp.getFromGuiInterface().fromGuiUnJoinHost( hostedId );
}

//============================================================================
void GuiUserJoinMgr::saveLastHostJoined( EHostType hostType, VxPtopUrl& ptopUrl )
{
    std::string url = ptopUrl.getUrl();
    switch( hostType )
    {
    case eHostTypeChatRoom:
        m_LastJoinChatRoomUrl = ptopUrl;
        break;
    case eHostTypeGroup:
        m_LastJoinGroupUrl = ptopUrl;
        break;
    case eHostTypeRandomConnect:
        m_LastJoinRandomConnectUrl = ptopUrl;
        break;
    default:
        LogMsg( LOG_ERROR, "GuiUserJoinMgr::%s invalid host type", __func__ );
        return;
    }

    m_MyApp.getAppSettings().setLastHostJoined( hostType, url );
}

//============================================================================
VxPtopUrl GuiUserJoinMgr::getLastJoinedPtopUrl( EHostType hostType )
{
    switch( hostType )
    {
    case eHostTypeChatRoom:
        return m_LastJoinChatRoomUrl;
        break;
    case eHostTypeGroup:
        return m_LastJoinGroupUrl;
        break;
    case eHostTypeRandomConnect:
        return m_LastJoinRandomConnectUrl;
        break;
    default:
        LogMsg( LOG_ERROR, "GuiUserJoinMgr::%s invalid host type", __func__ );
    }

    VxPtopUrl emptyUrl;
    return emptyUrl;
}

//============================================================================
void GuiUserJoinMgr::clearLastJoined( EHostType hostType )
{
    switch( hostType )
    {
    case eHostTypeChatRoom:
        m_LastJoinChatRoomUrl.clear();
        m_MyApp.getAppSettings().setLastHostJoined( hostType, "" );
        break;
    case eHostTypeGroup:
        m_LastJoinGroupUrl.clear();
        m_MyApp.getAppSettings().setLastHostJoined( hostType, "" );
        break;
    case eHostTypeRandomConnect:
        m_LastJoinRandomConnectUrl.clear();
        m_MyApp.getAppSettings().setLastHostJoined( hostType, "" );
        break;
    default:
        LogMsg( LOG_ERROR, "GuiUserJoinMgr::%s invalid host type", __func__ );
    }
}
