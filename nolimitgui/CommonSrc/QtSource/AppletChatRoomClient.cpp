//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
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
: AppletClientBase( OBJNAME_APPLET_CLIENT_CHAT_ROOM, app, parent )
{
	setAppletType( eAppletClientChatRoom );
    setHostType( eHostTypeChatRoom );
	ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	setPluginType( ePluginTypeClientChatRoom );

	manageUsers( ui.m_UserListWidget );

	ui.m_ChatRoomWidget->setAppModule( eAppModuleChatRoomClient );
	ui.m_ChatRoomWidget->setPluginType( ePluginTypeClientChatRoom );
	ui.m_ChatRoomWidget->setIdents( m_MyApp.getUserMgr().getMyIdent(), m_MyApp.getUserMgr().getMyIdent() );

	connect( this,					SIGNAL(signalBackButtonClicked()),		this, SLOT(closeApplet()) );
	connect( ui.m_UserListWidget,	SIGNAL(signalSetSessionVisible(bool)),	this, SLOT(slotSetSessionVisible(bool)) );

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