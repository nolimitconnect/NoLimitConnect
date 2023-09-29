//============================================================================
// Copyright (C) 2020 Brett R. Jones
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
