//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletChatRoomHostAdmin.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiMemberActiveMgr.h"
#include "MyIconsDefs.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include <QFrame>

#include "ui_AppletHostClient.h"

//============================================================================
AppletChatRoomHostAdmin::AppletChatRoomHostAdmin( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_HOST_CHAT_ROOM_ADMIN, app, parent )
, ui(*(new Ui::AppletHostClientUi ))
{
    setAppletType( eAppletChatRoomHostAdmin );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );
	setPluginType( ePluginTypeHostChatRoom );

	GroupieId hostAdminId( m_MyApp.getMyOnlineId(), m_MyApp.getMyOnlineId(), eHostTypeChatRoom );

	ui.m_SessionWidget->setHostAdminId( hostAdminId );
	ui.m_SessionWidget->setPluginType( ePluginTypeClientChatRoom );
    ui.m_SessionWidget->setInputClientCallback( this );
	ui.m_SessionWidget->showInviteFrame( true );

    ui.m_UserListWidget->setHostAdminId( hostAdminId );
    ui.m_UserListWidget->setUserViewType( eUserViewTypeChatRoom );

    connect( this, SIGNAL(signalBackButtonClicked()), this, SLOT(closeApplet()) );

    m_MyApp.activityStateChange( this, true );
	m_MyApp.getFromGuiInterface().fromGuiAdminViewHost( ePluginTypeHostChatRoom, true );
}

//============================================================================
AppletChatRoomHostAdmin::~AppletChatRoomHostAdmin()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
bool AppletChatRoomHostAdmin::checkIfCanSend( void )
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
bool AppletChatRoomHostAdmin::handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo )
{
	return handleGroupieAssetAction( ui.m_UserListWidget->getHostAdminId(), assetAction, assetInfo );
}
