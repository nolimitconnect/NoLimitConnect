//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityInformation.h"
#include "AppletSettingsHostChatRoom.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"
#include "GuiHelpers.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>

//============================================================================
AppletSettingsHostChatRoom::AppletSettingsHostChatRoom( AppCommon& app, QWidget* parent )
    : AppletSettingsHostBase( OBJNAME_APPLET_SETTINGS_HOST_CHAT_ROOM, app, parent )
{
    setAppletType( eAppletSettingsHostChatRoom );
    setPluginType( ePluginTypeHostChatRoom );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    getPluginSettingsWidget()->setupSettingsWidget( eAppletSettingsHostChatRoom, ePluginTypeHostChatRoom );

    loadPluginSetting();

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletSettingsHostChatRoom::~AppletSettingsHostChatRoom()
{
    m_MyApp.activityStateChange( this, false );
}
