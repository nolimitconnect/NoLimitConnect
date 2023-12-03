//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletUserConnections.h"

#include "AppCommon.h"
#include "AppSettings.h"

#include "GuiParams.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletUserConnections::AppletUserConnections( AppCommon& app, QWidget* parent )
: AppletClientBase( OBJNAME_APPLET_USER_CONNECTIONS, app, parent )
{
	setAppletType( eAppletUserConnections );
    setHostType( eHostTypeChatRoom );
	ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	setPluginType( ePluginTypeClientChatRoom );

	// only update users using this applet
	ui.m_UserListWidget->disconnectUserUpdates();

	for( int i = 0; i < eMaxHostType; i++ )
	{
		ui.m_HostTypeComboBox->addItem( GuiParams::describeHostType( (EHostType)i ) );
	}

	for( int i = 0; i < eMaxConnectType; i++ )
	{
		ui.m_ConnectTypeComboBox->addItem( GuiParams::describeConnectType( (EConnectType)i ) );
	}

	connect( this,						SIGNAL(signalBackButtonClicked()),	this, SLOT(closeApplet()) );
	connect( ui.m_ConnectTypeComboBox,	SIGNAL(currentIndexChanged(int)),	this, SLOT(slotConnectTypeSelectionChange(int)) );
	connect( ui.m_HostTypeComboBox,		SIGNAL(currentIndexChanged(int)),	this, SLOT(slotHostTypeSelectionChange(int)) );

	int connectTypeComboIdx = m_MyApp.getAppSettings().getLastUserConnectionsConnectType();
	if( connectTypeComboIdx )
	{
		ui.m_ConnectTypeComboBox->setCurrentIndex( connectTypeComboIdx );
	}

	int hostComboIdx = m_MyApp.getAppSettings().getLastUserConnectionsHostType();
	if( hostComboIdx )
	{
		ui.m_HostTypeComboBox->setCurrentIndex( hostComboIdx );
	}

	m_MyApp.activityStateChange( this, true );
	m_MyApp.getUserMgr().wantGuiUserUpdateCallbacks( this, true );
	m_MyApp.getUserJoinMgr().wantUserJoinCallbacks( this, true );
	m_MyApp.getHostJoinMgr().wantHostJoinCallbacks( this, true );

	refreshList();
}

//============================================================================
AppletUserConnections::~AppletUserConnections()
{
	m_MyApp.getHostJoinMgr().wantHostJoinCallbacks( this, false );
	m_MyApp.getUserJoinMgr().wantUserJoinCallbacks( this, false );
	m_MyApp.getUserMgr().wantGuiUserUpdateCallbacks( this, false );
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
EHostType AppletUserConnections::getSelectedHostType( void )
{
	return (EHostType)ui.m_HostTypeComboBox->currentIndex();
}

//============================================================================
EConnectType AppletUserConnections::getSelectedConnectType( void )
{
	return (EConnectType)ui.m_ConnectTypeComboBox->currentIndex();
}

//============================================================================
void AppletUserConnections::slotConnectTypeSelectionChange( int comboIdx )
{
	m_MyApp.getAppSettings().setLastUserConnectionsConnectType( comboIdx );
	refreshList();
}

//============================================================================
void AppletUserConnections::slotHostTypeSelectionChange( int comboIdx )
{
	m_MyApp.getAppSettings().setLastUserConnectionsHostType( comboIdx );
	refreshList();
}

//============================================================================
void AppletUserConnections::showEvent( QShowEvent* ev )
{
	AppletClientBase::showEvent( ev );
	ui.m_UserListWidget->setUserViewType( eUserViewTypeChatRoom );
}

//============================================================================
//! GuiUserUpdateCallback
//============================================================================
void AppletUserConnections::callbackIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "AppletUserConnections callbackIndentListUpdate %s %s %lld", 
			GuiParams::describeUserViewType(listType).toUtf8().constData(), onlineId.toOnlineIdString().c_str(), timestamp );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackIndentListRemove( EUserViewType listType, VxGUID& onlineId )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "AppletUserConnections callbackIndentListRemove %s %s", 
			GuiParams::describeUserViewType(listType).toUtf8().constData(), onlineId.toOnlineIdString().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "AppletUserConnections callbackOnlineStatusChange %s isOnline %d id %s", 
			guiUser->describeUser(true).toUtf8().constData(), isOnline, guiUser->getMyOnlineId().toOnlineIdString().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackUserAdded( GuiUser* guiUser )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "AppletUserConnections callbackUserAdded %s isOnline %d id %s", 
			guiUser->describeUser(true).toUtf8().constData(), guiUser->isOnline(), guiUser->getMyOnlineId().toOnlineIdString().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackUserUpdated( GuiUser* guiUser )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "AppletUserConnections callbackUserUpdated %s", 
			guiUser->describeUser(true).toUtf8().constData() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackUserRemoved( VxGUID& onlineId )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "AppletUserConnections callbackUserRemoved id %s", 
			onlineId.toOnlineIdString().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackPushToTalkStatus( VxGUID& onlineId, EPushToTalkStatus pushToTalkStatus )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "AppletUserConnections callbackUserRemoved id %s talk %s", 
			onlineId.toOnlineIdString().c_str(), GuiParams::describePushToTalkStatus(pushToTalkStatus).toUtf8().constData() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackMyIdentUpdated( GuiUser* guiUser ) 
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "AppletUserConnections callbackMyIdentUpdated %s", 
			guiUser->describeUser(true).toUtf8().constData() );
	refreshList();
}

//============================================================================
//! GuiUserJoinCallback
//============================================================================
void AppletUserConnections::callbackGuiUserJoinRequested( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) 
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "AppletUserConnections callbackGuiUserJoinRequested %s %s",
			guiUserJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiUserJoinWasGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "AppletUserConnections callbackGuiUserJoinWasGranted %s %s",
			guiUserJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiUserJoinIsGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "AppletUserConnections callbackGuiUserJoinIsGranted %s %s", 
			guiUserJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiUserUnJoinGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "AppletUserConnections callbackGuiUserUnJoinGranted %s %s",
			guiUserJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiUserJoinDenied( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "AppletUserConnections callbackGuiUserJoinDenied %s %s",
			guiUserJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiUserJoinLeaveHost( GroupieId& groupieId )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "AppletUserConnections callbackGuiUserJoinLeaveHost %s",
			groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiUserJoinRemoved( GroupieId& groupieId )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "AppletUserConnections callbackGuiUserJoinRemoved %s",
			groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
//! GuiHostJoinCallback
//============================================================================
//============================================================================
void AppletUserConnections::callbackJoinRequestCount( int requestCnt )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "GuiHostJoinCallback callbackJoinRequestCount %d",
			requestCnt );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiHostJoinRequested( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostJoinRequested %s %s",
			guiHostJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiHostJoinWasGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostJoinWasGranted %s %s",
			guiHostJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiHostJoinIsGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostJoinIsGranted %s %s",
			guiHostJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiHostUnJoinGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostUnJoinGranted %s %s",
			guiHostJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiHostJoinDenied( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostJoinDenied %s %s",
			guiHostJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiHostJoinLeaveHost( GroupieId& groupieId )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostJoinLeaveHost %s",
			groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiHostUnJoin( GroupieId& groupieId )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostUnJoin %s",
			groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiHostJoinRemoved( GroupieId& groupieId )
{
	LogModule( eLogUserGuiEvent, LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostJoinRemoved %s",
			groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::refreshList( void )
{
	ui.m_UserListWidget->clearUserList();
	EHostType hostType = getSelectedHostType();
	EConnectType connectType = getSelectedConnectType();
	
	if( hostType == eHostTypeUnknown )
	{
		refreshUserList( false );
		return;
	}
	else if( hostType == eHostTypeConnectTest || hostType == eHostTypeNetwork )
	{
		LogModule( eLogUserGuiEvent, LOG_DEBUG, "AppletUserConnections::refreshList cannot list connect test or network host" );
		return;
	}
	else if( hostType == eHostTypePeerUserDirect )
	{
		refreshUserList( true );
		return;
	}

	if( eConnectTypeHost == connectType )
	{
		std::map<GroupieId, GuiHostJoin*> & hostJoinList = m_MyApp.getHostJoinMgr().getHostJoinList();
		for( auto hostJoinPair : hostJoinList )
		{
			GuiUser* user = hostJoinPair.second->getUser();
			ui.m_UserListWidget->updateUser( user );
		}
	}
	else if( eConnectTypeClient == connectType )
	{
		std::map<GroupieId, GuiUserJoin*> & hostJoinList = m_MyApp.getUserJoinMgr().getUserJoinList();
		for( auto userJoinPair : hostJoinList )
		{
			GuiUser* user = userJoinPair.second->getUser();
			ui.m_UserListWidget->updateUser( user );
		}
	}
	else
	{
		refreshUserList( false );
	}
}

//============================================================================
void AppletUserConnections::refreshUserList( bool directConnectOnly )
{
	std::map<VxGUID, GuiUser*>& userList = m_MyApp.getUserMgr().getUserList();
	for( auto userPair : userList )
	{
		GuiUser* user = userPair.second;
		if( user && m_MyApp.getUserMgr().isUserOnline( user->getMyOnlineId() ) )
		{
			if( !directConnectOnly || directConnectOnly && user->isDirectConnect() )
			{
				ui.m_UserListWidget->updateUser( user );
			}
		}
	}
}