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

#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiMemberActiveMgr.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

#include <QFrame>

#include "ui_AppletRandomConnectClient.h"

//============================================================================
AppletRandomConnectClient::AppletRandomConnectClient( AppCommon& app, QWidget* parent )
: AppletClientBase( OBJNAME_APPLET_RANDOM_CONNECT_CLIENT, app, parent )
, ui(*(new Ui::AppletRandomConnectClientUi))
{
    setAppletType( eAppletRandomConnectClient );
    setHostType( eHostTypeRandomConnect );
    ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
    setPluginType( ePluginTypeClientRandomConnect );

    ui.m_SessionWidget->setMediaModule( eMediaModuleRandomConnectClient );
    ui.m_SessionWidget->setPluginType( ePluginTypeClientRandomConnect );
	ui.m_SessionWidget->setInputClientCallback( this );

	ui.m_SessionWidget->setLimitToTextAndPhotos( true );

    ui.m_UserListWidget->setUserViewType( eUserViewTypeRandomConnect );

    connect( this,                  SIGNAL(signalBackButtonClicked()),          this, SLOT(closeApplet()) );
    connect( ui.m_UserListWidget,   SIGNAL(signalSetSessionVisible(bool)),      this, SLOT(slotSetSessionVisible(bool)) );
    connect( ui.m_UserListWidget,	SIGNAL(signalViewChanged(EUserViewType)),   this, SLOT(slotViewChanged(EUserViewType)));

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletRandomConnectClient::~AppletRandomConnectClient()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletRandomConnectClient::userJoinedHost( GuiHosted* guiHosted )
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
void AppletRandomConnectClient::showEvent( QShowEvent* ev )
{
    ActivityBase::showEvent( ev );
    ui.m_UserListWidget->setUserViewType( eUserViewTypeRandomConnect );
}

//============================================================================
void AppletRandomConnectClient::slotSetSessionVisible( bool visible )
{
    ui.m_SessionWidget->setVisible( visible );
}

//============================================================================
void AppletRandomConnectClient::slotViewChanged( EUserViewType viewType )
{
	//setSelectedUser( nullptr );
}

//============================================================================
bool AppletRandomConnectClient::checkIfCanSend( void )
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
bool AppletRandomConnectClient::handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo )
{
	return handleGroupieAssetAction( ui.m_UserListWidget->getHostAdminId(), assetAction, assetInfo );
}
