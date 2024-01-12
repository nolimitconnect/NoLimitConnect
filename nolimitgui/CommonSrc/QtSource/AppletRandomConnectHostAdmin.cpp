//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletRandomConnectHostAdmin.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiMemberActiveMgr.h"
#include "GuiUserMultiListWidget.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

//============================================================================
AppletRandomConnectHostAdmin::AppletRandomConnectHostAdmin( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_RANDOM_CONNECT_HOST_ADMIN, app, parent )
{
    setAppletType( eAppletRandomConnectHostAdmin );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );
	setPluginType( ePluginTypeHostRandomConnect );

    connect( this, SIGNAL( signalBackButtonClicked() ), this, SLOT( closeApplet() ) );

    GroupieId hostAdminId( m_MyApp.getMyOnlineId(), m_MyApp.getMyOnlineId(), eHostTypeRandomConnect );
	ui.m_ChatRoomWidget->setPluginType( ePluginTypeClientRandomConnect );
	ui.m_ChatRoomWidget->setHostAdminId( hostAdminId );
	ui.m_ChatRoomWidget->setInputClientCallback( this );

	ui.m_UserListWidget->setHostAdminId( hostAdminId );

    ui.m_UserListWidget->setUserViewType( eUserViewTypeRandomConnect );

    HostedId hostId( m_MyApp.getMyOnlineId(), eHostTypeRandomConnect );
    std::set<VxGUID> memberList;
    m_MyApp.getMemberActiveMgr().getActiveMembers( hostId, memberList );
    for( auto onlineId : memberList )
    {
        ui.m_UserListWidget->getUserListWidget()->updateUser( onlineId );
    }

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletRandomConnectHostAdmin::~AppletRandomConnectHostAdmin()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
bool AppletRandomConnectHostAdmin::checkIfCanSend( void )
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
bool AppletRandomConnectHostAdmin::handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo )
{
	return handleGroupieAssetAction( ui.m_UserListWidget->getHostAdminId(), assetAction, assetInfo );
}