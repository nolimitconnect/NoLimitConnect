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

#include "AppletRandomConnectClient.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"

#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

//============================================================================
AppletRandomConnectClient::AppletRandomConnectClient( AppCommon& app, QWidget* parent )
: AppletClientBase( OBJNAME_APPLET_RANDOM_CONNECT_CLIENT, app, parent )
{
    setAppletType( eAppletRandomConnectClient );
    setHostType( eHostTypeRandomConnect );
    ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
    setPluginType( ePluginTypeClientRandomConnect );

    ui.m_SessionWidget->setAppModule( eAppModuleChatRoomClient );
    ui.m_SessionWidget->setPluginType( ePluginTypeClientRandomConnect );

    connect( this,                  SIGNAL(signalBackButtonClicked()),          this, SLOT(closeApplet()) );
    connect( ui.m_UserListWidget,   SIGNAL(signalSetSessionVisible(bool)),      this, SLOT(slotSetSessionVisible(bool)) );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletRandomConnectClient::~AppletRandomConnectClient()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletRandomConnectClient::showEvent( QShowEvent* ev )
{
    ActivityBase::showEvent( ev );
    ui.m_UserListWidget->setUserViewType( eUserViewTypeRandomConnect );
}

//============================================================================
void AppletRandomConnectClient::slotSetSessionVisible( bool visible )
{
    ui.m_SessionWidget->setVisible( visible );
}
