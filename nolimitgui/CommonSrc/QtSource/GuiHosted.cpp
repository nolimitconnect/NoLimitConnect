//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppCommon.h"
#include "GuiHosted.h"
#include "GuiHostedListMgr.h"
#include "GuiParams.h"

#include <ptop_src/ptop_engine_src/HostListMgr/HostedInfo.h>

//============================================================================
GuiHosted::GuiHosted( AppCommon& app )
    : QWidget( &app )
    , m_MyApp( app )
    , m_HostedListMgr( app.getHostedListMgr() )
{
}

//============================================================================
GuiHosted::GuiHosted( AppCommon& app, GuiUser* guiUser, VxGUID& sessionId )
    : QWidget( &app )
    , m_MyApp( app )
    , m_HostedListMgr( app.getHostedListMgr() )
    , m_GuiUser( guiUser )
    , m_SessionId( sessionId )
{
}

//============================================================================
GuiHosted::GuiHosted( AppCommon& app, GuiUser* guiUser, VxGUID& sessionId, HostedInfo& hostedInfo )
    : QWidget( &app )
    , m_MyApp( app )
    , m_HostedListMgr( app.getHostedListMgr() )
    , m_GuiUser( guiUser )
    , m_HostedId( hostedInfo.getHostedId() )
    , m_SessionId( sessionId )
    , m_IsFavorite( hostedInfo.getIsFavorite() )
    , m_ConnectedTimestampMs( hostedInfo.getConnectedTimestamp() )
    , m_JoinedTimestampMs( hostedInfo.getJoinedTimestamp() )
    , m_HostInfoTimestampMs( hostedInfo.getHostInfoTimestamp() )
    , m_HostInviteUrlIpv4( hostedInfo.getHostInviteUrl( false ) )
    , m_HostInviteUrlIpv6( hostedInfo.getHostInviteUrl( true ) )
    , m_HostTitle( hostedInfo.getHostTitle() )
    , m_HostDesc( hostedInfo.getHostDescription() )
    , m_ThumbId( hostedInfo.getThumbId() )
{
}

//============================================================================
GuiHosted::GuiHosted( const GuiHosted& rhs )
    : QWidget( &rhs.m_MyApp )
    , m_MyApp( rhs.m_MyApp )
    , m_HostedListMgr( rhs.m_HostedListMgr )
    , m_GuiUser( rhs.m_GuiUser )
    , m_HostedId( rhs.m_HostedId )
    , m_SessionId( rhs.m_SessionId )
    , m_IsFavorite( rhs.m_IsFavorite )
    , m_ConnectedTimestampMs( rhs.m_ConnectedTimestampMs )
    , m_JoinedTimestampMs( rhs.m_JoinedTimestampMs )
    , m_HostInfoTimestampMs( rhs.m_HostInfoTimestampMs )
    , m_HostInviteUrlIpv4( rhs.m_HostInviteUrlIpv4 )
    , m_HostInviteUrlIpv6( rhs.m_HostInviteUrlIpv6 )
    , m_HostTitle( rhs.m_HostTitle )
    , m_HostDesc( rhs.m_HostDesc )
    , m_ThumbId( rhs.m_ThumbId )
{
}

//============================================================================
bool GuiHosted::readyForClientLaunch( void )
{
    bool isReady{ false };
    GroupieId groupieId( getMyGroupieId() );
    EJoinState joinState = m_MyApp.getUserJoinMgr().getUserJoinState( groupieId );
    if( (m_MyApp.getConnectIdListMgr().isDirectConnect( m_GuiUser->getMyOnlineId() ) || m_MyApp.getConnectIdListMgr().isConnected( groupieId )) &&
        eJoinStateJoinIsGranted == joinState )
    {
        isReady = true;
    }

    return isReady;
}

//============================================================================
GroupieId GuiHosted::getMyGroupieId( void )
{
    return GroupieId( m_MyApp.getMyOnlineId(), m_HostedId );
}