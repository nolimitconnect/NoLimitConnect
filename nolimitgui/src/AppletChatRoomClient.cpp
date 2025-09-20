//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletChatRoomClient.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletHostClient.h"

//============================================================================
AppletChatRoomClient::AppletChatRoomClient( AppCommon& app, QWidget* parent )
: AppletHostClientBase( OBJNAME_APPLET_CHAT_ROOM_CLIENT, app, eAppletChatRoomClient, eHostTypeChatRoom, ePluginTypeClientChatRoom, parent )
{
	manageUsers( ui.m_UserListWidget );

	ui.m_SessionWidget->setMediaModule( eMediaModuleChatRoomClient );

	ui.m_SessionWidget->setLimitToTextAndPhotos( true );

	ui.m_UserListWidget->setUserViewType( eUserViewTypeChatRoom );
}
