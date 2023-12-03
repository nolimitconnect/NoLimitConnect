//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletHostChatRoomAdmin.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletHostChatRoomAdmin::AppletHostChatRoomAdmin( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_HOST_CHAT_ROOM_ADMIN, app, parent )
{
    setAppletType( eAppletHostChatRoomAdmin );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    connect( this, SIGNAL( signalBackButtonClicked() ), this, SLOT( closeApplet() ) );

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletHostChatRoomAdmin::~AppletHostChatRoomAdmin()
{
    m_MyApp.activityStateChange( this, false );
}
