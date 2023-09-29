//============================================================================
// Copyright (C) 2019 Brett R. Jones - Issued to proprietary license in 2022
//
// You may NOT use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// without written consent of copyright owner and this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://www.nolimitconnect.com
//============================================================================

#include "AppletSettingsHostGroupListing.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletSettingsHostGroupListing::AppletSettingsHostGroupListing( AppCommon& app, QWidget * parent )
: AppletServiceBaseSettings( OBJNAME_APPLET_SETTINGS_HOST_GROUP_LISTING, app, parent )
{
    setAppletType( eAppletSettingsHostGroupListing );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

	m_MyApp.activityStateChange( this, true );
}
