//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletGroupClient.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletHostClient.h"

//============================================================================
AppletGroupClient::AppletGroupClient( AppCommon& app, QWidget* parent )
: AppletHostClientBase( OBJNAME_APPLET_GROUP_CLIENT, app, eAppletGroupClient, eHostTypeGroup, ePluginTypeClientGroup, parent )
{
	ui.m_SessionWidget->setMediaModule( eMediaModuleChatRoomClient );
	ui.m_UserListWidget->setUserViewType( eUserViewTypeGroup );
}
