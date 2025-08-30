//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiGroupie.h"
#include "AppCommon.h"
#include "GuiGroupieListMgr.h"
#include "GuiParams.h"
#include <GroupieListMgr/GroupieInfo.h>

#include <CoreLib/VxGlobals.h>

//============================================================================
GuiGroupie::GuiGroupie( AppCommon& app )
    : QWidget( &app )
    , m_MyApp( app )
    , m_GroupieListMgr( app.getGroupieListMgr() )
{
}

//============================================================================
GuiGroupie::GuiGroupie( AppCommon& app, GuiUser* guiUser, VxGUID& sessionId )
    : QWidget( &app )
    , m_MyApp( app )
    , m_GroupieListMgr( app.getGroupieListMgr() )
    , m_GuiUser( guiUser )
    , m_SessionId( sessionId )
{
}
//============================================================================
GuiGroupie::GuiGroupie( AppCommon& app, GuiUser* guiUser, GroupieId& groupieId, VxGUID& sessionId )
    : QWidget( &app )
    , m_MyApp( app )
    , m_GroupieListMgr( app.getGroupieListMgr() )
    , m_GuiUser( guiUser )
    , m_GroupieId( groupieId )
    , m_SessionId( sessionId )
{
    setGroupieInfoTimestamp( GetTimeStampMs() );
    setGroupieUrl( guiUser->getNetIdent().getMyOnlineUrl() );
    setGroupieTitle( guiUser->getOnlineName() );
    setGroupieDescription( guiUser->getOnlineDescription() );
}

//============================================================================
GuiGroupie::GuiGroupie( AppCommon& app, GuiUser* guiUser, VxGUID& sessionId, GroupieInfo& groupieInfo )
    : QWidget( &app )
    , m_MyApp( app )
    , m_GroupieListMgr( app.getGroupieListMgr() )
    , m_GuiUser( guiUser )
    , m_GroupieId( groupieInfo.getGroupieId() )
    , m_SessionId( sessionId )
    , m_IsFavorite( groupieInfo.getIsFavorite() )
    , m_GroupieInfoTimestampMs( groupieInfo.getGroupieInfoTimestamp() )
    , m_GroupieUrl( groupieInfo.getGroupieUrl() )
    , m_GroupieTitle( groupieInfo.getGroupieTitle() )
    , m_GroupieDesc( groupieInfo.getGroupieDescription() )
{
}

//============================================================================
GuiGroupie::GuiGroupie( const GuiGroupie& rhs )
    : QWidget( &rhs.m_MyApp )
    , m_MyApp( rhs.m_MyApp )
    , m_GroupieListMgr( rhs.m_GroupieListMgr )
    , m_GuiUser( rhs.m_GuiUser )
    , m_GroupieId( rhs.m_GroupieId )
    , m_SessionId( rhs.m_SessionId )
    , m_IsFavorite( rhs.m_IsFavorite )
    , m_GroupieInfoTimestampMs( rhs.m_GroupieInfoTimestampMs )
    , m_GroupieUrl( rhs.m_GroupieUrl )
    , m_GroupieTitle( rhs.m_GroupieTitle )
    , m_GroupieDesc( rhs.m_GroupieDesc )
{
}
