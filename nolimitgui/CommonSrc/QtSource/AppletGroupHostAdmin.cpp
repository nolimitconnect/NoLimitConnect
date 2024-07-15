//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletGroupHostAdmin.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiMemberActiveMgr.h"
#include "GuiUserMultiListWidget.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletChatRoomClient.h"

//============================================================================
AppletGroupHostAdmin::AppletGroupHostAdmin( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_GROUP_HOST_ADMIN, app, parent )
, ui(*(new Ui::AppletChatRoomClientUi))
{
    setAppletType( eAppletGroupHostAdmin );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );
	setPluginType( ePluginTypeClientGroup );

	GroupieId hostAdminId( m_MyApp.getMyOnlineId(), m_MyApp.getMyOnlineId(), eHostTypeGroup );

	ui.m_ChatRoomWidget->setPluginType( ePluginTypeClientGroup );
	ui.m_ChatRoomWidget->setHostAdminId( hostAdminId );
	ui.m_ChatRoomWidget->setInputClientCallback( this );
    ui.m_UserListWidget->setHostAdminId( hostAdminId );

    connect( this, SIGNAL( signalBackButtonClicked() ), this, SLOT( closeApplet() ) );

    ui.m_UserListWidget->setUserViewType( eUserViewTypeGroup );

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletGroupHostAdmin::~AppletGroupHostAdmin()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
bool AppletGroupHostAdmin::checkIfCanSend( void )
{
	HostedId hostId =  ui.m_UserListWidget->getHostAdminId().getHostedId();

	if( !hostId.isValid() )
	{
		okMessageBox( QObject::tr( "Invalid Host Id" ),
						QObject::tr( "Host Id has not been set" ) );
		return false;
	}

	std::set<VxGUID> memberList;
	getMyApp().getMemberActiveMgr().getActiveMembers( hostId, memberList );
	if( memberList.empty() )
	{
		okMessageBox( QObject::tr( "No Members Online" ),
						QObject::tr( "There are no members online to send to" ) );
		return false;
	}

	return true;
}

//============================================================================
bool AppletGroupHostAdmin::handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo )
{
	return handleGroupieAssetAction( ui.m_UserListWidget->getHostAdminId(), assetAction, assetInfo );
}
