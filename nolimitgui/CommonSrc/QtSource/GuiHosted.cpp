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

#include <HostListMgr/HostedInfo.h>

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
    , m_SessionId( sessionId )
{
}

//============================================================================
GuiHosted::GuiHosted( AppCommon& app, GuiUser* guiUser, VxGUID& sessionId, HostedInfo& hostedInfo )
    : QWidget( &app )
    , m_MyApp( app )
    , m_HostedListMgr( app.getHostedListMgr() )
    , m_AdminId( hostedInfo.getAdminId() )
    , m_SessionId( sessionId )
    , m_IsFavorite( hostedInfo.getIsFavorite() )
    , m_ConnectedTimestampMs( hostedInfo.getConnectedTimestamp() )
    , m_JoinedTimestampMs( hostedInfo.getJoinedTimestamp() )
    , m_HostInfoTimestampMs( hostedInfo.getHostInfoTimestamp() )
    , m_HostInviteUrl( hostedInfo.getHostInviteUrl() )
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
    , m_AdminId( rhs.m_AdminId )
    , m_SessionId( rhs.m_SessionId )
    , m_IsFavorite( rhs.m_IsFavorite )
    , m_ConnectedTimestampMs( rhs.m_ConnectedTimestampMs )
    , m_JoinedTimestampMs( rhs.m_JoinedTimestampMs )
    , m_HostInfoTimestampMs( rhs.m_HostInfoTimestampMs )
    , m_HostInviteUrl( rhs.m_HostInviteUrl )
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

    if( m_MyApp.getConnectIdListMgr().isDirectConnect( groupieId.getHostOnlineId() ) || m_MyApp.getConnectIdListMgr().isConnected( groupieId ) )
    {
        if( eJoinStateJoinIsGranted == joinState )
        {
            isReady = true;
        }
        else
        {
            LogMsg( LOG_VERBOSE, "GuiHosted::readyForClientLaunch false because join state %s", DescribeJoinState( joinState ) );
        }
    }
    else
    {
        LogMsg( LOG_VERBOSE, "GuiHosted::readyForClientLaunch false because user %s is offline", m_MyApp.describeUser( groupieId.getHostOnlineId() ).c_str() );
    }

    return isReady;
}

//============================================================================
GroupieId GuiHosted::getMyGroupieId( void )
{
    return GroupieId( m_MyApp.getMyOnlineId(), m_AdminId );
}

//============================================================================
GuiUser* GuiHosted::getUser( void )
{
    GuiUser* adminUser = m_MyApp.getUserMgr().getUser( m_AdminId.getHostOnlineId() );
    if( !adminUser )
    {
        LogMsg( LOG_ERROR, "GuiHosted::getUser null admin user %s", m_MyApp.describeUser( m_AdminId.getHostOnlineId() ).c_str() );
        vx_assert( false );
    }
    
    return adminUser;
}
