//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletGroupJoin.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "GuiHostedListSession.h"
#include "GuiHostedListItem.h"

#include <UserJoinMgr/UserJoinMgr.h>

#include <CoreLib/VxDebug.h>

//============================================================================
AppletGroupJoin::AppletGroupJoin( AppCommon& app, QWidget* parent )
: AppletJoinBase( OBJNAME_APPLET_GROUP_JOIN, app, parent )
{
    setAppletType( eAppletGroupJoin );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

	setHostType( eHostTypeGroup );
}

//============================================================================
AppletGroupJoin::~AppletGroupJoin()
{
}
