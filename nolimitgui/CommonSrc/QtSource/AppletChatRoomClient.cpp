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
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletChatRoomClient::AppletChatRoomClient( AppCommon& app, QWidget* parent )
: AppletClientBase( OBJNAME_APPLET_CHAT_ROOM_CLIENT, app, parent )
{
	setAppletType( eAppletChatRoomClient );
    setHostType( eHostTypeChatRoom );
	ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	setPluginType( ePluginTypeClientChatRoom );

	manageUsers( ui.m_UserListWidget );

	ui.m_ChatRoomWidget->setAppModule( eAppModuleChatRoomClient );
	ui.m_ChatRoomWidget->setPluginType( ePluginTypeClientChatRoom );
	ui.m_ChatRoomWidget->setIdents( m_MyApp.getUserMgr().getMyIdent(), m_MyApp.getUserMgr().getMyIdent() );

	//connect( this,					SIGNAL(signalBackButtonClicked()),		this, SLOT(closeApplet()) );
	connect( ui.m_UserListWidget,	SIGNAL(signalSetSessionVisible(bool)),	this, SLOT(slotSetSessionVisible(bool)) );
	connect( ui.m_UserListWidget,		SIGNAL(signalViewChanged(EUserViewType)),  this,	SLOT(slotViewChanged(EUserViewType)));

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletChatRoomClient::~AppletChatRoomClient()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletChatRoomClient::slotSetSessionVisible( bool makeVisible )
{
	ui.m_ChatRoomWidget->setVisible( makeVisible );
}

//============================================================================
void AppletChatRoomClient::showEvent( QShowEvent* ev )
{
	AppletClientBase::showEvent( ev );
	ui.m_UserListWidget->setUserViewType( eUserViewTypeChatRoom );
}

//============================================================================
void AppletChatRoomClient::slotViewChanged( EUserViewType viewType )
{
	//setSelectedUser( nullptr );
}