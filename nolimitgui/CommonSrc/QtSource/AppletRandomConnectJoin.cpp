//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletRandomConnectJoin.h"

//============================================================================
AppletRandomConnectJoin::AppletRandomConnectJoin( AppCommon& app, QWidget* parent )
: AppletJoinBase( OBJNAME_APPLET_RANDOM_CONNECT_JOIN, app, parent )
{
    setAppletType( eAppletRandomConnectJoin );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

    setHostType( eHostTypeRandomConnect );
}

//============================================================================
AppletRandomConnectJoin::~AppletRandomConnectJoin()
{
}
