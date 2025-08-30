//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or linux Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "AppCommon.h"
#include "GuiUserJoinMgr.h"
#include "GuiParams.h"

//============================================================================
GuiUserJoin::GuiUserJoin( AppCommon& app )
    : QWidget( &app )
    , m_MyApp( app )
    , m_UserJoinMgr( app.getUserJoinMgr() )
{
}

//============================================================================
GuiUserJoin::GuiUserJoin( AppCommon& app, GuiUser* guiUser, VxGUID& sessionId, bool online )
    : QWidget( &app )
    , m_MyApp( app )
    , m_UserJoinMgr( app.getUserJoinMgr() )
    , m_GuiUser( guiUser )
    , m_SessionId( sessionId )
{
}

//============================================================================
GuiUserJoin::GuiUserJoin( AppCommon& app, GuiUser* guiUser, UserJoinInfo* userJoinInfo )
    : QWidget( &app )
    , m_MyApp( app )
    , m_UserJoinMgr( app.getUserJoinMgr() )
    , m_GuiUser( guiUser )
{
    if( userJoinInfo )
    {
        m_GroupieId = userJoinInfo->getGroupieId();
        m_JoinState = userJoinInfo->getJoinState();
        m_SessionId = userJoinInfo->getSessionId();
    }
}

//============================================================================
GuiUserJoin::GuiUserJoin( const GuiUserJoin& rhs )
    : QWidget( &rhs.m_MyApp )
    , m_MyApp( rhs.m_MyApp )
    , m_UserJoinMgr( rhs.m_UserJoinMgr )
    , m_GuiUser( rhs.m_GuiUser )
    , m_GroupieId( rhs.m_GroupieId )
    , m_JoinState( rhs.m_JoinState )
    , m_SessionId( rhs.m_SessionId )
{
}

//============================================================================
bool GuiUserJoin::setJoinState( EJoinState joinState )
{
    if( joinState != m_JoinState )
    {
        m_JoinState = joinState;
        return true;
    }

    return false;
}

//============================================================================
bool GuiUserJoin::isOnline( void ) 
{ 
    return getUser() && getUser()->isOnline(); 
}

//============================================================================
std::string GuiUserJoin::getOnlineName( void )
{
    return m_GuiUser ? m_GuiUser->getOnlineName() : "Unknown User";
}

//============================================================================
VxGUID GuiUserJoin::getMyOnlineId( void )
{
    return m_GuiUser ? m_GuiUser->getMyOnlineId() : VxGUID();
}