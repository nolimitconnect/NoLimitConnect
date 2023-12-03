//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletChatRoomJoin.h"

//============================================================================
AppletChatRoomJoin::AppletChatRoomJoin( AppCommon& app, QWidget* parent )
: AppletJoinBase( OBJNAME_APPLET_CHAT_ROOM_JOIN, app, parent )
{
    setAppletType( eAppletChatRoomJoin );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

    setHostType( eHostTypeChatRoom );
}

//============================================================================
AppletChatRoomJoin::~AppletChatRoomJoin()
{
}
