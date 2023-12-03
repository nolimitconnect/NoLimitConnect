//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletAboutMeServerViewMine.h"

#include "AppCommon.h"

//============================================================================
AppletAboutMeServerViewMine::AppletAboutMeServerViewMine( AppCommon& app, QWidget* parent )
: AppletAboutMeClient( app, parent )
{
}

//============================================================================
AppletAboutMeServerViewMine::~AppletAboutMeServerViewMine()
{
	m_MyApp.activityStateChange( this, false );
	m_MyApp.wantToGuiActivityCallbacks( this, false );
}

//============================================================================
void AppletAboutMeServerViewMine::setAppletType( EApplet applet )
{
	AppletAboutMeClient::setAppletType( eAppletAboutMeServerViewMine );
}