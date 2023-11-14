//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletUserConnections.h"
#include "AppCommon.h"

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

	manageUsers( ui.m_UserListWidget );

	connect( this,						SIGNAL(signalBackButtonClicked()),	this, SLOT(closeApplet()) );
	connect( ui.m_ConnectTypeComboBox,	SIGNAL(currentIndexChanged(int)),	this, SLOT(slotConnectTypeSelectionChange(int)) );
	connect( ui.m_HostTypeComboBox,		SIGNAL(currentIndexChanged(int)),	this, SLOT(slotHostTypeSelectionChange(int)) );

	m_MyApp.activityStateChange( this, true );
	m_MyApp.getUserMgr().wantGuiUserUpdateCallbacks( this, true );
	m_MyApp.getUserJoinMgr().wantUserJoinCallbacks( this, true );
	m_MyApp.getHostJoinMgr().wantHostJoinCallbacks( this, true );
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
void AppletUserConnections::slotConnectTypeSelectionChange( int comboIdx )
{

}

//============================================================================
void AppletUserConnections::slotHostTypeSelectionChange( int comboIdx )
{

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
	LogMsg( LOG_DEBUG, "GuiUserUpdateCallback callbackIndentListUpdate %s %s %lld", 
			GuiParams::describeUserViewType(listType).toUtf8().constData(), onlineId.toOnlineIdString().c_str(), timestamp );
}

//============================================================================
void AppletUserConnections::callbackIndentListRemove( EUserViewType listType, VxGUID& onlineId )
{
	LogMsg( LOG_DEBUG, "GuiUserUpdateCallback callbackIndentListRemove %s %s", 
			GuiParams::describeUserViewType(listType).toUtf8().constData(), onlineId.toOnlineIdString().c_str() );
}

//============================================================================
void AppletUserConnections::callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline )
{
	LogMsg( LOG_DEBUG, "GuiUserUpdateCallback callbackIndentListRemove %s isOnline %d", 
			guiUser->describeUser(true).toUtf8().constData(), isOnline );
}

//============================================================================
void AppletUserConnections::callbackUserAdded( GuiUser* guiUser )
{
	LogMsg( LOG_DEBUG, "GuiUserUpdateCallback callbackUserAdded %s", 
			guiUser->describeUser(true).toUtf8().constData() );
}

//============================================================================
void AppletUserConnections::callbackUserUpdated( GuiUser* guiUser )
{
	LogMsg( LOG_DEBUG, "GuiUserUpdateCallback callbackUserUpdated %s", 
			guiUser->describeUser(true).toUtf8().constData() );
}

//============================================================================
void AppletUserConnections::callbackUserRemoved( VxGUID& onlineId )
{
	LogMsg( LOG_DEBUG, "GuiUserUpdateCallback callbackUserRemoved id %s", 
			onlineId.toOnlineIdString().c_str() );
}

//============================================================================
void AppletUserConnections::callbackPushToTalkStatus( VxGUID& onlineId, EPushToTalkStatus pushToTalkStatus )
{
	LogMsg( LOG_DEBUG, "GuiUserUpdateCallback callbackUserRemoved id %s talk %s", 
			onlineId.toOnlineIdString().c_str(), GuiParams::describePushToTalkStatus(pushToTalkStatus).toUtf8().constData() );
}

//============================================================================
void AppletUserConnections::callbackMyIdentUpdated( GuiUser* guiUser ) 
{
	LogMsg( LOG_DEBUG, "GuiUserUpdateCallback callbackMyIdentUpdated %s", 
			guiUser->describeUser(true).toUtf8().constData() );
}

//============================================================================
//! GuiUserJoinCallback
//============================================================================
void AppletUserConnections::callbackGuiUserJoinRequested( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) 
{
	LogMsg( LOG_DEBUG, "GuiUserJoinCallback callbackGuiUserJoinRequested %s %s",
			guiUserJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
}

//============================================================================
void AppletUserConnections::callbackGuiUserJoinWasGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
	LogMsg( LOG_DEBUG, "GuiUserJoinCallback callbackGuiUserJoinWasGranted %s %s",
			guiUserJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
}

//============================================================================
void AppletUserConnections::callbackGuiUserJoinIsGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
	LogMsg( LOG_DEBUG, "GuiUserJoinCallback callbackGuiUserJoinIsGranted %s %s", 
			guiUserJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
}

//============================================================================
void AppletUserConnections::callbackGuiUserUnJoinGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
	LogMsg( LOG_DEBUG, "GuiUserJoinCallback callbackGuiUserUnJoinGranted %s %s",
			guiUserJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
}

//============================================================================
void AppletUserConnections::callbackGuiUserJoinDenied( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
	LogMsg( LOG_DEBUG, "GuiUserJoinCallback callbackGuiUserJoinDenied %s %s",
			guiUserJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
}

//============================================================================
void AppletUserConnections::callbackGuiUserJoinLeaveHost( GroupieId& groupieId )
{
	LogMsg( LOG_DEBUG, "GuiUserJoinCallback callbackGuiUserJoinLeaveHost %s",
			groupieId.describeGroupieId().c_str() );
}

//============================================================================
void AppletUserConnections::callbackGuiUserJoinRemoved( GroupieId& groupieId )
{
	LogMsg( LOG_DEBUG, "GuiUserJoinCallback callbackGuiUserJoinRemoved %s",
			groupieId.describeGroupieId().c_str() );
}

//============================================================================
//! GuiHostJoinCallback
//============================================================================
//============================================================================
void AppletUserConnections::callbackJoinRequestCount( int requestCnt )
{
	LogMsg( LOG_DEBUG, "GuiHostJoinCallback callbackJoinRequestCount %d",
			requestCnt );
}

//============================================================================
void AppletUserConnections::callbackGuiHostJoinRequested( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
	LogMsg( LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostJoinRequested %s %s",
			guiHostJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
}

//============================================================================
void AppletUserConnections::callbackGuiHostJoinWasGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
	LogMsg( LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostJoinWasGranted %s %s",
			guiHostJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
}

//============================================================================
void AppletUserConnections::callbackGuiHostJoinIsGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
	LogMsg( LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostJoinIsGranted %s %s",
			guiHostJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
}

//============================================================================
void AppletUserConnections::callbackGuiHostUnJoinGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
	LogMsg( LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostUnJoinGranted %s %s",
			guiHostJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
}

//============================================================================
void AppletUserConnections::callbackGuiHostJoinDenied( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
	LogMsg( LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostJoinDenied %s %s",
			guiHostJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
}

//============================================================================
void AppletUserConnections::callbackGuiHostJoinLeaveHost( GroupieId& groupieId )
{
	LogMsg( LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostJoinLeaveHost %s",
			groupieId.describeGroupieId().c_str() );
}

//============================================================================
void AppletUserConnections::callbackGuiHostUnJoin( GroupieId& groupieId )
{
	LogMsg( LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostUnJoin %s",
			groupieId.describeGroupieId().c_str() );
}

//============================================================================
void AppletUserConnections::callbackGuiHostJoinRemoved( GroupieId& groupieId )
{
	LogMsg( LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostJoinRemoved %s",
			groupieId.describeGroupieId().c_str() );
}

