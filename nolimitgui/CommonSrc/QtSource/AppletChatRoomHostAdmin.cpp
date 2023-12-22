//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletChatRoomHostAdmin.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletChatRoomHostAdmin::AppletChatRoomHostAdmin( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_HOST_CHAT_ROOM_ADMIN, app, parent )
{
    setAppletType( eAppletChatRoomHostAdmin );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    GroupieId hostAdmin( m_MyApp.getMyOnlineId(), m_MyApp.getMyOnlineId(), eHostTypeChatRoom );
    ui.m_UserListWidget->setHostAdminId( hostAdmin );

    ui.m_UserListWidget->setUserViewType( eUserViewTypeChatRoom );

    connect( this, SIGNAL( signalBackButtonClicked() ), this, SLOT( closeApplet() ) );

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletChatRoomHostAdmin::~AppletChatRoomHostAdmin()
{
    m_MyApp.activityStateChange( this, false );
}
