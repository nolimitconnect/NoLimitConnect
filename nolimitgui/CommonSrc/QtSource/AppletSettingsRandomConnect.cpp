//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletSettingsRandomConnect.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletSettingsRandomConnect::AppletSettingsRandomConnect( AppCommon& app, QWidget* parent )
: AppletSettingsBase( OBJNAME_APPLET_SETTINGS_RANDOM_CONNECT, app, parent )
{
	ui.setupUi( getContentItemsFrame() );
	setAppletType( eAppletSettingsRandomConnect );
	setPluginType( ePluginTypeClientRandomConnect );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	connect( this, SIGNAL(signalBackButtonClicked()), this, SLOT( closeApplet()) );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletSettingsRandomConnect::~AppletSettingsRandomConnect()
{
	m_MyApp.activityStateChange( this, false );
}
