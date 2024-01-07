//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiHostedListMgr.h"

#include "AppCommon.h"
#include "AppletMgr.h"
#include "AppSettings.h"
#include "GuiHostedListCallback.h"

#include <HostListMgr/HostedInfo.h>
#include <HostListMgr/HostedListMgr.h>
#include <HostServerJoinMgr/HostServerJoinMgr.h>
#include <P2PEngine/P2PEngine.h>

#include <PktLib/VxCommon.h>
#include <CoreLib/VxPtopUrl.h>
#include <NetLib/VxSktUtil.h>

//============================================================================
GuiHostedListMgr::GuiHostedListMgr( AppCommon& app )
    : QObject( &app )
    , m_MyApp( app )
{
}

//============================================================================
void GuiHostedListMgr::onAppCommonCreated( void )
{
    m_MyApp.getAppSettings().getFavoriteHostGroupUrl( m_FavoriteHostGroup );

    connect( this, SIGNAL( signalInternalHostedUpdated( HostedInfo* ) ), this, SLOT( slotInternalHostedUpdated( HostedInfo* ) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalHostedRemoved( VxGUID, EHostType ) ), this, SLOT( slotInternalHostedRemoved( VxGUID, EHostType ) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalHostSearchResult( HostedInfo*, VxGUID ) ), this, SLOT( slotInternalHostSearchResult( HostedInfo*, VxGUID ) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalHostSearchComplete( EHostType, VxGUID) ), this, SLOT( slotInternalHostSearchComplete( EHostType, VxGUID ) ), Qt::QueuedConnection );


    m_MyApp.getEngine().getHostedListMgr().wantHostedListCallback( dynamic_cast< HostedListCallbackInterface*>(this), true );
}

//============================================================================
bool GuiHostedListMgr::isMessengerReady( void )
{
    return m_MyApp.isMessengerReady();
}

//============================================================================
void GuiHostedListMgr::callbackHostedInfoListUpdated( HostedInfo* hostedInfo )
{
    if( !hostedInfo )
    {
        LogMsg( LOG_ERROR, "GuiHostedListMgr::callbackHostedAdded null hostedInfo" );
        return;
    }

    emit signalInternalHostedUpdated( new HostedInfo( *hostedInfo ) );
}

//============================================================================
void GuiHostedListMgr::slotInternalHostedUpdated( HostedInfo* hostedInfo )
{
    if( hostedInfo )
    {
        updateHostedInfo( *hostedInfo );
        delete hostedInfo;
    }
}

//============================================================================
void GuiHostedListMgr::callbackHostedInfoListRemoved( VxGUID& hostOnlineId, EHostType hostType )
{
    emit signalInternalHostedRemoved( hostOnlineId, hostType );
}

//============================================================================
void GuiHostedListMgr::slotInternalHostedRemoved( VxGUID hostOnlineId, EHostType hostType )
{
    HostedId hostedId( hostOnlineId, hostType );
    auto iter = m_HostedList.find( hostedId );
    if( iter != m_HostedList.end() && hostType != eHostTypeUnknown )
    {
        announceHostedListRemoved( hostedId );
    }
}

//============================================================================
void GuiHostedListMgr::callbackHostedInfoListSearchResult( HostedInfo* hostedInfo, VxGUID& sessionId )
{
    if( hostedInfo && hostedInfo->getAdminOnlineId().isVxGUIDValid() && hostedInfo->isHostInviteValid() )
    {
        emit signalInternalHostSearchResult( new HostedInfo( *hostedInfo ), sessionId );
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiHostedListMgr::callbackHostedInfoListSearchResult invalid invite" );
    }
}

//============================================================================
void GuiHostedListMgr::slotInternalHostSearchResult( HostedInfo* hostedInfo, VxGUID sessionId )
{
    updateHostSearchResult( *hostedInfo, sessionId );
    delete hostedInfo;
}

//============================================================================
void GuiHostedListMgr::callbackHostedInfoListSearchComplete( EHostType hostType, VxGUID& sessionId )
{
    if( hostType != eHostTypeUnknown )
    {
        emit signalInternalHostSearchComplete( hostType, sessionId );
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiHostedListMgr::callbackHostedInfoListSearchResult invalid host type" );
    }
}

//============================================================================
void GuiHostedListMgr::slotInternalHostSearchComplete( EHostType hostType, VxGUID sessionId )
{
    announceHostedListSearchComplete( hostType, sessionId );
}

//============================================================================
void GuiHostedListMgr::toGuiHostSearchResult( EHostType hostType, VxGUID& sessionId, HostedInfo& hostedInfo )
{
    if( m_MyApp.getEngine().getIgnoreListMgr().isHostIgnored( hostedInfo.getAdminOnlineId() ) )
    {
        LogMsg( LOG_VERBOSE, "GuiHostedListMgr::toGuiHostSearchResult ignored host %s", hostedInfo.getHostTitle().c_str() );
        return;
    }

    if( hostedInfo.isHostInviteValid() )
    {
        HostedInfo* newHostedInfo = new HostedInfo( hostedInfo );
        emit signalInternalHostSearchResult( newHostedInfo, sessionId );
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiHostedListMgr::toGuiHostSearchResult invalid invite" );
    }
}

//============================================================================
void GuiHostedListMgr::toGuiHostSearchComplete( EHostType hostType, VxGUID& sessionId )
{
    emit signalInternalHostSearchComplete( hostType, sessionId );
}

//============================================================================
GuiHosted* GuiHostedListMgr::findHosted( VxGUID& onlineId, EHostType hostType )
{
    HostedId hostTypeId( onlineId, hostType );
    return findHosted( hostTypeId );
}

//============================================================================
GuiHosted* GuiHostedListMgr::findHosted( HostedId& adminId, bool calledFromUpdateHosted )
{
    GuiHosted* guiHosted = nullptr;
    auto iter = m_HostedList.find( adminId );
    if( iter != m_HostedList.end() )
    {
        guiHosted = iter->second;
    }

    if( !guiHosted && !calledFromUpdateHosted && adminId.getHostOnlineId() != m_MyApp.getMyOnlineId() )
    {
        GroupieId groupieId( m_MyApp.getMyOnlineId(), adminId );
        if( m_MyApp.getMemberActiveMgr().isMemberActive( groupieId ) )
        {
            // this can happen if user has never actually done a host listing but instead was auto joined on startup
            GuiUser* guiUser = m_MyApp.getUserMgr().getUser( adminId.getHostOnlineId() );
            if( guiUser )
            {
                guiHosted = updateHosted( guiUser, adminId.getHostType() );
            }
        }
    }

    return guiHosted;
}

//============================================================================
void GuiHostedListMgr::removeHosted( VxGUID& onlineId, EHostType hostType )
{
    HostedId hostTypeId( onlineId, hostType );
    auto iter = m_HostedList.find( hostTypeId );
    if( iter != m_HostedList.end() )
    {
        iter->second->deleteLater();
        m_HostedList.erase( iter );
    }
}

//============================================================================
bool GuiHostedListMgr::isHostedInSession( VxGUID& onlineId )
{
    // TODO ?
    return false;
}

//============================================================================
GuiHosted* GuiHostedListMgr::updateHosted( VxNetIdent* hisIdent, EHostType hostType )
{
    if( !hisIdent )
    {
        LogMsg( LOG_ERROR, "GuiHostedListMgr::updateHosted invalid param" );
        return nullptr;
    }

    GuiUser* guiUser = m_MyApp.getUserMgr().updateUser( hisIdent );
    if( guiUser )
    {
        return updateHosted( guiUser, hostType );
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiHostedListMgr::updateHosted failed to update user" );
        return nullptr;
    }
}

//============================================================================
GuiHosted* GuiHostedListMgr::updateHosted( GuiUser* guiUser, EHostType hostType )
{
    if( !guiUser )
    {
        LogMsg( LOG_ERROR, "GuiHostedListMgr::updateHosted invalid param" );
        return nullptr;
    }

    HostedId adminId( guiUser->getMyOnlineId(), hostType );
    GuiHosted* guiHosted = findHosted( adminId, true );
    
    if( !guiHosted )
    {
        guiHosted = new GuiHosted( m_MyApp );
        guiHosted->setAdminId( adminId );
    }

    if( guiHosted->getUser()->getMyOnlineId() == guiUser->getMyOnlineId() )
    {
        if( isMessengerReady() )
        {
            announceHostedListUpdated( adminId, guiHosted );
        }
    }
    else
    {
        m_HostedList[adminId] = guiHosted;
        if( isMessengerReady() )
        {
            announceHostedListAdded( adminId, guiHosted );
        }
    }

    return guiHosted;
}

//============================================================================
GuiHosted* GuiHostedListMgr::updateHostedInfo( HostedInfo& hostedInfo )
{
    EHostType hostType = hostedInfo.getHostType();
    HostedId adminId( hostedInfo.getAdminId() );

    GuiHosted* guiHosted = findHosted( hostedInfo.getAdminOnlineId(), hostType );
    if( !guiHosted )
    {
        GuiUser* guiUser = m_MyApp.getUserMgr().getUser( hostedInfo.getAdminOnlineId() );
        // make a new one
        guiHosted = new GuiHosted( m_MyApp, guiUser, hostedInfo.getAdminOnlineId(), hostedInfo );

        m_HostedList[adminId] = guiHosted;
        if( isMessengerReady() )
        {
            announceHostedListAdded( adminId, guiHosted );
        }
    }
    else
    {
        // make sure is up to date. search results should be the latest info
        guiHosted->setHostType( hostedInfo.getHostType() );
        // skip setIsFavorite.. is probably not set correctly in search result
        bool updated = guiHosted->getJoinedTimestamp() != hostedInfo.getJoinedTimestamp() || guiHosted->getHostInfoTimestamp() != hostedInfo.getHostInfoTimestamp();

        guiHosted->setConnectedTimestamp( hostedInfo.getConnectedTimestamp() );
        guiHosted->setJoinedTimestamp( hostedInfo.getJoinedTimestamp() );
        guiHosted->setHostInfoTimestamp( hostedInfo.getHostInfoTimestamp() );
        guiHosted->setHostInviteUrl( false, hostedInfo.getHostInviteUrl( false ) );
        guiHosted->setHostInviteUrl( true, hostedInfo.getHostInviteUrl( true ) );
        guiHosted->setHostTitle( hostedInfo.getHostTitle() );
        guiHosted->setHostDescription( hostedInfo.getHostDescription() );
        if( updated )
        {
            if( isMessengerReady() )
            {
                announceHostedListUpdated( adminId, guiHosted );
            }
        }
    }

    return guiHosted;
}

//============================================================================
void GuiHostedListMgr::setHostedOffline( VxGUID& onlineId )
{
    /*
    GuiHosted* guiHosted = findHosted( onlineId );
    if( guiHosted )
    {
        guiHosted->setOnlineStatus( false );
    }*/
}

//============================================================================
void GuiHostedListMgr::onMyIdentUpdated( GuiHosted* guiHosted )
{
    if( isMessengerReady() )
    {
        emit signalMyIdentUpdated( guiHosted );
    }
}

//============================================================================
void GuiHostedListMgr::updateHostSearchResult( HostedInfo& hostedInfo, VxGUID& sessionId )
{
    // hosted info is temporary and will soon be deleted so make copy if required
    if( hostedInfo.isHostInviteValid() )
    {
        HostedId hostedId( hostedInfo.getAdminId() );
        GuiHosted* guiHosted = updateHostedInfo( hostedInfo );
        if( guiHosted )
        {
            announceHostedListSearchResult( hostedId, guiHosted, sessionId );
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiHostedListMgr::updateHostSearchResult invalid invite" );
    }
}

//============================================================================
void GuiHostedListMgr::wantHostedListCallbacks( GuiHostedListCallback* client, bool enable )
{
    for( auto iter = m_HostedListClients.begin(); iter != m_HostedListClients.end(); ++iter )
    {
        if( *iter == client )
        {
            m_HostedListClients.erase( iter );
            break;
        }
    }

    if( enable )
    {
        m_HostedListClients.push_back( client );
    }
}

//============================================================================
void GuiHostedListMgr::announceHostedListAdded( HostedId& hostedId, GuiHosted* guiHosted )
{
    for( auto client : m_HostedListClients )
    {
        client->callbackGuiHostedListAdded( hostedId, guiHosted );
    }
}

//============================================================================
void GuiHostedListMgr::announceHostedListUpdated( HostedId& hostedId, GuiHosted* guiHosted )
{
    for( auto client : m_HostedListClients )
    {
        client->callbackGuiHostedListUpdated( hostedId, guiHosted );
    }
}

//============================================================================
void GuiHostedListMgr::announceHostedListRemoved( HostedId& hostedId )
{
    for( auto client : m_HostedListClients )
    {
        client->callbackGuiHostedListRemoved( hostedId );
    }
}

//============================================================================
void GuiHostedListMgr::announceHostedListSearchResult( HostedId& hostedId, GuiHosted* guiHosted, VxGUID& sessionId )
{
    for( auto client : m_HostedListClients )
    {
        client->callbackGuiHostedListSearchResult( hostedId, guiHosted, sessionId );
    }
}

//============================================================================
void GuiHostedListMgr::announceHostedListSearchComplete( EHostType hostType, VxGUID& sessionId )
{
    for( auto client : m_HostedListClients )
    {
        client->callbackGuiHostedListSearchComplete( hostType, sessionId );
    }
}

//============================================================================
void GuiHostedListMgr::setJoinOnStartup( std::string& hostUrlIpv4, std::string& hostUrlIpv6, bool enable )
{
    if( !enable && isJoinOnStartup( hostUrlIpv4, hostUrlIpv6 ) )
    {
        m_FavoriteHostGroup.clear();
        m_MyApp.getAppSettings().setFavoriteHostGroupUrl( m_FavoriteHostGroup );
        return;
    }

    VxPtopUrl urlIpv4( hostUrlIpv4 );
    if( enable && urlIpv4.isValid() )
    {
        m_FavoriteHostGroup = hostUrlIpv4;
        m_MyApp.getAppSettings().setFavoriteHostGroupUrl( m_FavoriteHostGroup );
        return;
    }

    VxPtopUrl urlIpv6( hostUrlIpv6 );
    if( enable && urlIpv6.isValid() )
    {
        m_FavoriteHostGroup = hostUrlIpv6;
        m_MyApp.getAppSettings().setFavoriteHostGroupUrl( m_FavoriteHostGroup );
    }
}

//============================================================================
bool GuiHostedListMgr::isJoinOnStartup( std::string& hostUrlIpv4, std::string& hostUrlIpv6 )
{
    if( m_FavoriteHostGroup.empty() )
    {
        return false;
    }

    if( m_FavoriteHostGroup == hostUrlIpv4 )
    {
        VxPtopUrl nowUrl( m_FavoriteHostGroup );
        VxPtopUrl checkUrl( hostUrlIpv4 );
        return nowUrl.isValid() && checkUrl.isValid() && nowUrl.getOnlineId() == checkUrl.getOnlineId() && nowUrl.getHostType() == checkUrl.getHostType();
    }
    else if( m_FavoriteHostGroup == hostUrlIpv6 )
    {
        VxPtopUrl nowUrl( m_FavoriteHostGroup );
        VxPtopUrl checkUrl( hostUrlIpv6 );
        return nowUrl.isValid() && checkUrl.isValid() && nowUrl.getOnlineId() == checkUrl.getOnlineId() && nowUrl.getHostType() == checkUrl.getHostType();
    }

    return false;
}

//============================================================================
void GuiHostedListMgr::slotNetAvailableStatus( ENetAvailStatus eNetAvailStatus )
{
    if( eNetAvailStatus >= eNetAvailOnlineButNoRelay )
    {
        checkAutoJoinGroupHost();
    }
}

//============================================================================
void GuiHostedListMgr::checkAutoJoinGroupHost( void )
{
    if( !m_AttemptedJoinHostGroup && !m_FavoriteHostGroup.empty() )
    {
        LogMsg( LOG_VERBOSE, "checkAutoJoinGroupHost attempt join %s", m_FavoriteHostGroup.c_str() );

        m_AttemptedJoinHostGroup = true;
        VxGUID sessionId;
        VxPtopUrl checkUrl( m_FavoriteHostGroup );
        if( checkUrl.isValid() )
        {
            std::string host = checkUrl.getHost();
            std::string empyUrl;
            bool ipv6 = VxIsIpv6Address( host );
            HostedId hostId( checkUrl.getOnlineId(), eHostTypeGroup );
            m_MyApp.getFromGuiInterface().fromGuiJoinHost( hostId, sessionId, ipv6 ? empyUrl : m_FavoriteHostGroup, ipv6 ? m_FavoriteHostGroup : empyUrl );
        }
    }
}

//============================================================================
EJoinState GuiHostedListMgr::getHostJoinState( GroupieId& groupieId )
{
    return m_MyApp.getEngine().getHostJoinMgr().getHostJoinState( groupieId );
}

//============================================================================
bool GuiHostedListMgr::launchClientAppletOfAlreadyConnectedHost( HostedId& adminId, QWidget* parentPageFrame )
{
    bool wasLaunched{ false };
    if( !adminId.isValid() )
    {
        LogModule( eLogHostedUser, LOG_ERROR, "GuiHostedListMgr::launchClientAppletOfAlreadyConnectedHost invalid host id for %s", DescribeHostType( adminId.getHostType() ) );
        return false;
    }

    if( !IsHostARelayForUsers( adminId.getHostType() ) )
    {
        LogModule( eLogHostedUser, LOG_ERROR, "GuiHostedListMgr::launchClientAppletOfAlreadyConnectedHost host type %s", DescribeHostType( adminId.getHostType() ) );
        return false;
    }

    // find the host id of the host we are connected to
    GuiHosted* guiHosted = findHosted( adminId );
    if( guiHosted )
    {
        LogModule( eLogHostedUser, LOG_VERBOSE, "GuiHostedListMgr::launchClientAppletOfAlreadyConnectedHost found host %s", DescribeHostType( adminId.getHostType() ) );
        wasLaunched = m_MyApp.getAppletMgr().launchClientApplet( guiHosted, parentPageFrame );
    }
    else
    {
        LogModule( eLogHostedUser, LOG_WARN, "GuiHostedListMgr::launchClientAppletOfAlreadyConnectedHost no host %s", DescribeHostType( adminId.getHostType() ) );
    }

    return wasLaunched;
}
