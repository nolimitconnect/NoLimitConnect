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

#include "AppletUserConnections.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletUserConnections::AppletUserConnections( AppCommon& app, QWidget* parent )
: AppletClientBase( OBJNAME_APPLET_USER_CONNECTIONS, app, parent )
{
	setAppletType( eAppletUserConnections );
    setHostType( eHostTypeChatRoom );
	ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	setPluginType( ePluginTypeClientChatRoom );

	manageUsers( ui.m_UserListWidget );

	connect( this,						SIGNAL(signalBackButtonClicked()),	this, SLOT(closeApplet()) );
	connect( ui.m_ConnectTypeComboBox,	SIGNAL(currentIndexChanged(int)),	this, SLOT(slotConnectTypeSelectionChange(int)) );
	connect( ui.m_HostTypeComboBox,		SIGNAL(currentIndexChanged(int)),	this, SLOT(slotHostTypeSelectionChange(int)) );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletUserConnections::~AppletUserConnections()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletUserConnections::slotConnectTypeSelectionChange( int comboIdx )
{

}

//============================================================================
void AppletUserConnections::slotHostTypeSelectionChange( int comboIdx )
{

}

//============================================================================
void AppletUserConnections::showEvent( QShowEvent* ev )
{
	AppletClientBase::showEvent( ev );
	ui.m_UserListWidget->setUserViewType( eUserViewTypeChatRoom );
}