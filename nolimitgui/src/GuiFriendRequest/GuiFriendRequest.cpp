//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiFriendRequest.h"
#include "AppCommon.h"
#include "GuiFriendRequestMgr.h"
#include "GuiParams.h"
#include <FriendRequestMgr/FriendRequestInfo.h>

#include <CoreLib/VxGlobals.h>

//============================================================================
GuiFriendRequest::GuiFriendRequest( AppCommon& app )
    : QWidget( &app )
    , m_MyApp( app )
    , m_FriendRequestMgr( app.getFriendRequestMgr() )
{
}

//============================================================================
GuiFriendRequest::GuiFriendRequest( AppCommon& app, std::shared_ptr<FriendRequestInfo>& requestInfo )
    : QWidget( &app )
    , m_MyApp( app )
    , m_FriendRequestMgr( app.getFriendRequestMgr() )
    , m_RequestInfo( requestInfo )
{
}

//============================================================================
GuiFriendRequest::GuiFriendRequest( const GuiFriendRequest& rhs )
    : QWidget( &rhs.m_MyApp )
    , m_MyApp( rhs.m_MyApp )
    , m_FriendRequestMgr( rhs.m_FriendRequestMgr )
    , m_RequestInfo( rhs.m_RequestInfo )
{
}
