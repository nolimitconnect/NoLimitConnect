//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletSettingsFriendRequest.h"

#include "AppCommon.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletSettingsNoUserSettings.h"

//============================================================================
AppletSettingsFriendRequest::AppletSettingsFriendRequest( AppCommon& app, QWidget* parent )
: AppletServiceBaseSettings( OBJNAME_APPLET_SETTINGS_FRIEND_REQUEST, app, parent )
, ui(*(new Ui::AppletSettingsNoUserSettingsUi))
{
	ui.setupUi( getContentItemsFrame() );
	setAppletType( eAppletSettingsFriendRequest );
	setPluginType( ePluginTypeFriendRequest );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	connect( this, SIGNAL(signalBackButtonClicked()), this, SLOT( closeApplet()) );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletSettingsFriendRequest::~AppletSettingsFriendRequest()
{
	m_MyApp.activityStateChange( this, false );
}
