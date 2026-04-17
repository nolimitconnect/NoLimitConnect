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
#include "GuiMemberActiveMgr.h"
#include "GuiUserMultiListWidget.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include <GuiInterface/IFromGui.h>

#include <QFrame>

#include "ui_AppletHostClient.h"

//============================================================================
AppletGroupHostAdmin::AppletGroupHostAdmin( AppCommon& app, QWidget* parent )
    : AppletHostAdminBase( OBJNAME_APPLET_GROUP_HOST_ADMIN, app, parent )
{
    setAppletType( eAppletGroupHostAdmin );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );
	setPluginType( ePluginTypeHostGroup );

	GroupieId hostAdminId( m_MyApp.getMyOnlineId(), m_MyApp.getMyOnlineId(), eHostTypeGroup );

	ui.m_SessionWidget->setPluginType( ePluginTypeClientGroup );
	ui.m_SessionWidget->setHostAdminId( hostAdminId );
	ui.m_SessionWidget->setInputClientCallback( this );
    ui.m_SessionWidget->showInviteFrame( true );

    ui.m_UserListWidget->setHostAdminId( hostAdminId );

    connect( this, SIGNAL(signalBackButtonClicked()), this, SLOT(closeApplet()) );
    connect( ui.m_UserListWidget, SIGNAL(signalSetMembersVisible(bool)), this, SLOT(slotSetMembersVisible(bool)) );
    connect( ui.m_UserListWidget, SIGNAL(signalSetSessionVisible(bool)), this, SLOT(slotSetSessionVisible(bool)) );

    // allow send to individual members by clicking on them in the user list
    connect( ui.m_UserListWidget, SIGNAL(signalUserSelected(GuiUser*)), this, SLOT(slotUserSelected(GuiUser*)) );

    ui.m_UserListWidget->setUserViewType( eUserViewTypeGroup );

    m_MyApp.activityStateChange( this, true );
    m_MyApp.getFromGuiInterface().fromGuiAdminViewHost( ePluginTypeHostGroup, true );

    // Restore eye button states
    bool eyeUsersVisible = m_MyApp.getAppSettings().getAppletEyeUsersVisible( m_EAppletType );
    ui.m_UserListWidget->setMembersVisible( eyeUsersVisible );
    bool eyeSessionVisible = m_MyApp.getAppSettings().getAppletEyeSessionVisible( m_EAppletType );
    ui.m_UserListWidget->setSessionsVisible( eyeSessionVisible );
    ui.m_SessionWidget->setVisible( eyeSessionVisible );
}

//============================================================================
AppletGroupHostAdmin::~AppletGroupHostAdmin()
{
    m_MyApp.getFromGuiInterface().fromGuiAdminViewHost( ePluginTypeHostGroup, false );
    m_MyApp.activityStateChange( this, false );
}
