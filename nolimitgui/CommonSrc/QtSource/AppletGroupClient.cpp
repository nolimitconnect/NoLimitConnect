//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletGroupClient.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

//============================================================================
AppletGroupClient::AppletGroupClient( AppCommon& app, QWidget* parent )
: AppletClientBase( OBJNAME_APPLET_GROUP_CLIENT, app, parent )
{
    setAppletType( eAppletGroupClient );
    setHostType( eHostTypeGroup );
    ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
    setPluginType( ePluginTypeClientGroup );

    ui.m_SessionWidget->setAppModule( eAppModuleChatRoomClient );
	ui.m_SessionWidget->setPluginType( getPluginType() );
    ui.m_SessionWidget->setInputClientCallback( this );

    ui.m_UserListWidget->setUserViewType( eUserViewTypeGroup );

    connect( this,                  SIGNAL(signalBackButtonClicked()),          this, SLOT(closeApplet()) );
    connect( ui.m_UserListWidget,   SIGNAL(signalSetSessionVisible(bool)),      this, SLOT(slotSetSessionVisible(bool)) );
    connect( ui.m_UserListWidget,	SIGNAL(signalViewChanged(EUserViewType)),   this, SLOT(slotViewChanged(EUserViewType)));

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletGroupClient::~AppletGroupClient()
{
    m_MyApp.activityStateChange( this, false );
}


//============================================================================
void AppletGroupClient::userJoinedHost( GuiHosted* guiHosted )
{
	if( guiHosted )
	{
		GuiUser* adminUser = guiHosted->getUser();
		if( adminUser )
		{
			HostedId adminId( adminUser->getMyOnlineId(), guiHosted->getHostType() );
			GroupieId groupieId( m_MyApp.getMyOnlineId(), adminId );
			if( adminId.isValid() )
			{
				ui.m_SessionWidget->setHostAdminId( groupieId );
				AppletClientBase::userJoinedHost( guiHosted );
			}
		}
	}
}

//============================================================================
void AppletGroupClient::showEvent( QShowEvent* ev )
{
    ActivityBase::showEvent( ev );
    ui.m_UserListWidget->setUserViewType( eUserViewTypeGroup );
}

//============================================================================
void AppletGroupClient::slotSetSessionVisible( bool visible )
{
    ui.m_SessionWidget->setVisible( visible );
}

//============================================================================
void AppletGroupClient::slotViewChanged( EUserViewType viewType )
{
	//setSelectedUser( nullptr );
}

//============================================================================
bool AppletGroupClient::checkIfCanSend( void )
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
bool AppletGroupClient::handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo )
{
	return handleGroupieAssetAction( ui.m_UserListWidget->getHostAdminId(), assetAction, assetInfo );
}