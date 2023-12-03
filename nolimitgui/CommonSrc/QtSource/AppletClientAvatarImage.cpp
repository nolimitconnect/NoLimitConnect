//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletClientAvatarImage.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletClientAvatarImage::AppletClientAvatarImage( AppCommon& app, QWidget* parent )
: AppletClientBase( OBJNAME_APPLET_AVATAR_IMAGE_CLIENT, app, parent )
{
	setAppletType( eAppletAvatarImageClient );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	connect( this, SIGNAL(signalBackButtonClicked()), this, SLOT( closeApplet()) );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletClientAvatarImage::~AppletClientAvatarImage()
{
    m_MyApp.activityStateChange( this, false );
}
