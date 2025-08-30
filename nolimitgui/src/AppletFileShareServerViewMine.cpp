//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletFileShareServerViewMine.h"

#include "AppCommon.h"

//============================================================================
AppletFileShareServerViewMine::AppletFileShareServerViewMine( AppCommon& app, QWidget* parent )
    : AppletFileShareClientView( app, parent )
{
}

//============================================================================
AppletFileShareServerViewMine::~AppletFileShareServerViewMine()
{
	wantFileXferCallbacks( false );
	wantActivityCallbacks( false );
	m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletFileShareServerViewMine::setAppletType( EApplet applet )
{
    AppletFileShareClientView::setAppletType( applet );
}
