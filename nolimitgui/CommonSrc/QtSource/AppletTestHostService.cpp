//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletTestHostService.h"

#include "AppCommon.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletTestHostService.h"

//============================================================================
AppletTestHostService::AppletTestHostService( AppCommon& app, QWidget* parent )
: AppletTestBase( OBJNAME_APPLET_TEST_HOST_SERVICE, app, parent )
, ui(*(new Ui::AppletTestHostServiceUi))
{
	ui.setupUi( getContentItemsFrame() );
    setAppletType( eAppletTestHostService );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	connect( this, SIGNAL( signalBackButtonClicked() ), this, SLOT( closeApplet() ) );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletTestHostService::~AppletTestHostService()
{
    m_MyApp.activityStateChange( this, false );
}