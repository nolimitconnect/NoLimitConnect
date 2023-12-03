//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletServiceConnectionTest.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletServiceConnectionTest::AppletServiceConnectionTest( AppCommon& app, QWidget* parent )
: AppletServiceBase( OBJNAME_APPLET_SERVICE_CONNECT_TEST, app, parent )
{
    setupServiceBaseApplet( eAppletServiceConnectionTest, ePluginTypeHostConnectTest );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletServiceConnectionTest::~AppletServiceConnectionTest()
{
    m_MyApp.activityStateChange( this, false );
}
