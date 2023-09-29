//============================================================================
// Copyright (C) 2018 Brett R. Jones - Issued to proprietary license in 2022
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

#include "AppletClientStoryboard.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletClientStoryboard::AppletClientStoryboard( AppCommon& app, QWidget * parent )
: AppletBase( OBJNAME_APPLET_CLIENT_STORYBOARD, app, parent )
{
	setAppletType( eAppletClientStoryboard );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	connect( this, SIGNAL(signalBackButtonClicked()), this, SLOT( closeApplet()) );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletClientStoryboard::~AppletClientStoryboard()
{
    m_MyApp.activityStateChange( this, false );
}
