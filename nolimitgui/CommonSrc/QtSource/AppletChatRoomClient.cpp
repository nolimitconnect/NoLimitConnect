//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletChatRoomClient.h"
#include "AppCommon.h"
#include "MyIcons.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>

//============================================================================
AppletChatRoomClient::AppletChatRoomClient( AppCommon& app, QWidget* parent )
: AppletClientBase( OBJNAME_APPLET_CHAT_ROOM_CLIENT, app, parent )
{
	setAppletType( eAppletChatRoomClient );
    setHostType( eHostTypeChatRoom );
	ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	setPluginType( ePluginTypeClientChatRoom );

	manageUsers( ui.m_UserListWidget );

	ui.m_ChatRoomWidget->setAppModule( eAppModuleChatRoomClient );
	ui.m_ChatRoomWidget->setPluginType( ePluginTypeClientChatRoom );
	ui.m_ChatRoomWidget->setIdents( m_MyApp.getUserMgr().getMyIdent(), m_MyApp.getUserMgr().getMyIdent() );
	ui.m_ChatRoomWidget->setInputClientCallback( this );

	//connect( this,					SIGNAL(signalBackButtonClicked()),			this, SLOT(closeApplet()) );
	connect( ui.m_UserListWidget,		SIGNAL(signalSetSessionVisible(bool)),		this, SLOT(slotSetSessionVisible(bool)) );
	connect( ui.m_UserListWidget,		SIGNAL(signalViewChanged(EUserViewType)),	this,	SLOT(slotViewChanged(EUserViewType)));

	m_MyApp.activityStateChange( this, true );

	ui.m_UserListWidget->setUserViewType( eUserViewTypeChatRoom );
}

//============================================================================
AppletChatRoomClient::~AppletChatRoomClient()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletChatRoomClient::slotSetSessionVisible( bool makeVisible )
{
	ui.m_ChatRoomWidget->setVisible( makeVisible );
}

//============================================================================
void AppletChatRoomClient::showEvent( QShowEvent* ev )
{
	AppletClientBase::showEvent( ev );
}

//============================================================================
void AppletChatRoomClient::slotViewChanged( EUserViewType viewType )
{
	//setSelectedUser( nullptr );
}

//============================================================================
bool AppletChatRoomClient::checkIfCanSend( void )
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
bool AppletChatRoomClient::handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo )
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

	assetInfo.setHostId( hostId );
	for( auto memberId : memberList )
	{
		assetInfo.setDestUserId( memberId );
		getMyApp().getEngine().fromGuiAssetAction( assetAction, assetInfo );
	}

	return true;
}
