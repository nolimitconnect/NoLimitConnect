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
#include "GuiMemberActiveMgr.h"
#include "MyIconsDefs.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include <QFrame>

#include "ui_AppletHostClient.h"

//============================================================================
AppletChatRoomHostAdmin::AppletChatRoomHostAdmin( AppCommon& app, QWidget* parent )
: AppletHostAdminBase( OBJNAME_APPLET_HOST_CHAT_ROOM_ADMIN, app, parent )
{
    setAppletType( eAppletChatRoomHostAdmin );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );
	setPluginType( ePluginTypeHostChatRoom );

	GroupieId hostAdminId( m_MyApp.getMyOnlineId(), m_MyApp.getMyOnlineId(), eHostTypeChatRoom );

	ui.m_SessionWidget->setHostAdminId( hostAdminId );
	ui.m_SessionWidget->setPluginType( ePluginTypeClientChatRoom );
    ui.m_SessionWidget->setInputClientCallback( this );
	ui.m_SessionWidget->showInviteFrame( true );

    ui.m_UserListWidget->setHostAdminId( hostAdminId );
    ui.m_UserListWidget->setUserViewType( eUserViewTypeChatRoom );

    connect( this, SIGNAL(signalBackButtonClicked()), this, SLOT(closeApplet()) );
	connect( ui.m_UserListWidget, SIGNAL(signalSetMembersVisible(bool)), this, SLOT(slotSetMembersVisible(bool)) );
	connect( ui.m_UserListWidget, SIGNAL(signalSetSessionVisible(bool)), this, SLOT(slotSetSessionVisible(bool)) );

    m_MyApp.activityStateChange( this, true );
		m_MyApp.getFromGuiInterface().fromGuiAdminViewHost( ePluginTypeHostChatRoom, true );

		// Restore eye button states
		bool eyeUsersVisible = m_MyApp.getAppSettings().getAppletEyeUsersVisible( m_EAppletType );
		ui.m_UserListWidget->setMembersVisible( eyeUsersVisible );
		bool eyeSessionVisible = m_MyApp.getAppSettings().getAppletEyeSessionVisible( m_EAppletType );
		ui.m_UserListWidget->setSessionsVisible( eyeSessionVisible );
		ui.m_SessionWidget->setVisible( eyeSessionVisible );
}

//============================================================================
AppletChatRoomHostAdmin::~AppletChatRoomHostAdmin()
{
    m_MyApp.activityStateChange( this, false );
    m_MyApp.getFromGuiInterface().fromGuiAdminViewHost( ePluginTypeHostChatRoom, false );
}
