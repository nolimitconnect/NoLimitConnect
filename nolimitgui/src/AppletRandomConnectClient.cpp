//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletRandomConnectClient.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletHostClient.h"

//============================================================================
AppletRandomConnectClient::AppletRandomConnectClient( AppCommon& app, QWidget* parent )
: AppletHostClientBase( OBJNAME_APPLET_RANDOM_CONNECT_CLIENT, app, eAppletRandomConnectClient, eHostTypeRandomConnect, ePluginTypeClientRandomConnect, parent )
{
    ui.m_SessionWidget->setMediaModule( eMediaModuleRandomConnectClient );

	ui.m_SessionWidget->setLimitToTextAndPhotos( true );

    ui.m_UserListWidget->setUserViewType( eUserViewTypeRandomConnect );
}
