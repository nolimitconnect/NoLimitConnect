//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletGetStarted.h"

#include "AppCommon.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

//============================================================================
AppletGetStarted::AppletGetStarted( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_GET_STARTED, app, parent )
{
	ui.setupUi( getContentItemsFrame() );
    setAppletType( eAppletGetStarted );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	connect( this, SIGNAL( signalBackButtonClicked() ), this, SLOT( closeApplet() ) );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletGetStarted::~AppletGetStarted()
{
    m_MyApp.activityStateChange( this, false );
}
