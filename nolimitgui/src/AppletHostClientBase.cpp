//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletHostClientBase.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiMemberActiveMgr.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

#include <QFrame>

#include "ui_AppletHostClient.h"

//============================================================================
AppletHostClientBase::AppletHostClientBase( const char* objName, AppCommon& app, EApplet applet, EHostType hostType, EPluginType pluginType, QWidget* parent )
: AppletClientBase( objName, app, parent )
, ui( *( new Ui::AppletHostClientUi ) )
{
	setAppletType( applet );
	setHostType( hostType );
	ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	setPluginType( pluginType );

	ui.m_SessionWidget->setPluginType( getPluginType() );
	ui.m_SessionWidget->setInputClientCallback( this );

	connect( this, SIGNAL(signalBackButtonClicked()), this, SLOT(closeApplet()) );
	connect( ui.m_UserListWidget, SIGNAL(signalSetSessionVisible(bool)), this, SLOT(slotSetSessionVisible(bool)) );
	connect( ui.m_UserListWidget, SIGNAL(signalViewChanged(EUserViewType)), this, SLOT(slotViewChanged(EUserViewType)) );
	connect( ui.m_UserListWidget, SIGNAL(signalLeftHost()), this, SLOT(closeApplet()) );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletHostClientBase::~AppletHostClientBase()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletHostClientBase::userJoinedHost( GuiHosted* guiHosted )
{
	if( guiHosted )
	{
		GuiUser* adminUser = guiHosted->getUser();
		if( adminUser )
		{
			HostedId adminId( adminUser->getMyOnlineId(), guiHosted->getHostType() );
			GroupieId adminGroupieId( m_MyApp.getMyOnlineId(), adminId );
			if( adminId.isValid() )
			{
				ui.m_UserListWidget->setHostAdminId( adminGroupieId );
				ui.m_SessionWidget->setHostAdminId( adminGroupieId );
				AppletClientBase::userJoinedHost( guiHosted );
			}
		}
	}
}

//============================================================================
void AppletHostClientBase::setAdminGroupieId( GroupieId& adminGroupieId )
{
	m_AdminGroupieId = adminGroupieId;
	m_HostType = adminGroupieId.getHostType();
	LogMsg( LOG_VERBOSE, "AppletChatRoomClient::%s %s", __func__, m_MyApp.describeGroupieId( adminGroupieId ).c_str() );
	ui.m_UserListWidget->setHostAdminId( adminGroupieId );
	ui.m_SessionWidget->setHostAdminId( adminGroupieId );

	m_MyApp.getConnectIdListMgr().dumpOnlineUsers();
	m_MyApp.getConnectIdListMgr().dumpHostedUsers( m_AdminGroupieId.getHostedId() );

	GuiUser* adminUser = m_MyApp.getUserMgr().getUser( adminGroupieId.getHostOnlineId() );
	if( adminUser )
	{
		if( adminUser->isOnline() )
		{
			ui.m_UserListWidget->setHostAdminId( adminGroupieId );
			ui.m_SessionWidget->setHostAdminId( adminGroupieId );
		}
	}
	else
	{
		LogMsg( LOG_ERROR, "AppletChatRoomClient::%s failed to find admin", __func__ );
	}
}

//============================================================================
void AppletHostClientBase::showEvent( QShowEvent* ev )
{
	ActivityBase::showEvent( ev );
}

//============================================================================
void AppletHostClientBase::slotSetSessionVisible( bool visible )
{
	ui.m_SessionWidget->setVisible( visible );
}

//============================================================================
void AppletHostClientBase::slotViewChanged( EUserViewType viewType )
{
	//setSelectedUser( nullptr );
}

//============================================================================
bool AppletHostClientBase::checkIfCanSend( void )
{
	return AppletBase::checkIfCanSend( ui.m_UserListWidget->getHostAdminId().getHostedId() );
}

//============================================================================
bool AppletHostClientBase::handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo )
{
	return handleGroupieAssetAction( ui.m_UserListWidget->getHostAdminId(), assetAction, assetInfo );
}
