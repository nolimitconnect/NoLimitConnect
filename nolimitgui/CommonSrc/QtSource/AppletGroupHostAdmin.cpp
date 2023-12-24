//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletGroupHostAdmin.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiUserMultiListWidget.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletGroupHostAdmin::AppletGroupHostAdmin( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_GROUP_HOST_ADMIN, app, parent )
{
    setAppletType( eAppletGroupHostAdmin );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    connect( this, SIGNAL( signalBackButtonClicked() ), this, SLOT( closeApplet() ) );

    GroupieId hostAdminId( m_MyApp.getMyOnlineId(), m_MyApp.getMyOnlineId(), eHostTypeGroup );
    ui.m_UserListWidget->setHostAdminId( hostAdminId );

    ui.m_UserListWidget->setUserViewType( eUserViewTypeGroup );

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletGroupHostAdmin::~AppletGroupHostAdmin()
{
    m_MyApp.activityStateChange( this, false );
}
