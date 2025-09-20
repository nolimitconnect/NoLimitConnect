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

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletUserConnections.h"

//============================================================================
AppletUserConnections::AppletUserConnections( AppCommon& app, QWidget* parent )
: AppletClientBase( OBJNAME_APPLET_USER_CONNECTIONS, app, parent )
, ui(*(new Ui::AppletUserConnectionsUi))
{
	setAppletType( eAppletUserConnections );
    setHostType( eHostTypeChatRoom );
	ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	setPluginType( ePluginTypeClientChatRoom );

	// only update users using this applet
	ui.m_UserListWidget->disconnectUserUpdates();

	for( int i = 0; i < eMaxUserViewType; i++ )
	{
        ui.m_UserViewTypeComboBox->addItem( GuiParams::describeUserViewType( (EUserViewType)i ) );
	}

	connect( this,							SIGNAL(signalBackButtonClicked()),	this, SLOT(closeApplet()) );
    connect( ui.m_UserViewTypeComboBox,		SIGNAL(currentIndexChanged(int)),	this, SLOT(slotUserViewTypeSelectionChange(int)) );

	int hostComboIdx = m_MyApp.getAppSettings().getLastUserConnectionsUserViewType();
	if( hostComboIdx )
	{
        ui.m_UserViewTypeComboBox->setCurrentIndex( hostComboIdx );
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
EUserViewType AppletUserConnections::getSelectedUserViewType( void )
{
	return (EUserViewType)ui.m_UserViewTypeComboBox->currentIndex();
}

//============================================================================
void AppletUserConnections::slotUserViewTypeSelectionChange( int comboIdx )
{
    m_MyApp.getAppSettings().setLastUserConnectionsUserViewType( comboIdx );
	ui.m_UserListWidget->setUserViewType( getSelectedUserViewType() );
}

//============================================================================
void AppletUserConnections::showEvent( QShowEvent* ev )
{
	AppletClientBase::showEvent( ev );
}

//============================================================================
//! GuiUserUpdateCallback
//============================================================================
void AppletUserConnections::callbackIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp )
{
	if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "AppletUserConnections callbackIndentListUpdate %s %s %lld", 
			GuiParams::describeUserViewType(listType).toUtf8().constData(), onlineId.toOnlineIdString().c_str(), timestamp );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackIndentListRemove( EUserViewType listType, VxGUID& onlineId )
{
	if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "AppletUserConnections callbackIndentListRemove %s %s", 
			GuiParams::describeUserViewType(listType).toUtf8().constData(), onlineId.toOnlineIdString().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline )
{
	if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "AppletUserConnections callbackOnlineStatusChange %s isOnline %d id %s", 
			guiUser->describeUser(true).toUtf8().constData(), isOnline, guiUser->getMyOnlineId().toOnlineIdString().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackUserAdded( GuiUser* guiUser )
{
	if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "AppletUserConnections callbackUserAdded %s isOnline %d id %s", 
			guiUser->describeUser(true).toUtf8().constData(), guiUser->isOnline(), guiUser->getMyOnlineId().toOnlineIdString().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackUserUpdated( GuiUser* guiUser )
{
	if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "AppletUserConnections callbackUserUpdated %s", 
			guiUser->describeUser(true).toUtf8().constData() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackUserRemoved( VxGUID& onlineId )
{
	if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "AppletUserConnections callbackUserRemoved id %s", 
			onlineId.toOnlineIdString().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackMyIdentUpdated( GuiUser* guiUser ) 
{
    if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "AppletUserConnections callbackMyIdentUpdated %s", 
			guiUser->describeUser(true).toUtf8().constData() );
	refreshList();
}

//============================================================================
//! GuiUserJoinCallback
//============================================================================
void AppletUserConnections::callbackGuiUserJoinRequested( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) 
{
	if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "AppletUserConnections callbackGuiUserJoinRequested %s %s",
			guiUserJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiUserJoinWasGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
    if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "AppletUserConnections callbackGuiUserJoinWasGranted %s %s",
			guiUserJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiUserJoinIsGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
    if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "AppletUserConnections callbackGuiUserJoinIsGranted %s %s", 
			guiUserJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiUserUnJoinGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
    if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "AppletUserConnections callbackGuiUserUnJoinGranted %s %s",
			guiUserJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiUserJoinDenied( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
    if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "AppletUserConnections callbackGuiUserJoinDenied %s %s",
			guiUserJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiUserJoinLeaveHost( GroupieId& groupieId )
{
    if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "AppletUserConnections callbackGuiUserJoinLeaveHost %s",
			groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiUserJoinRemoved( GroupieId& groupieId )
{
    if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "AppletUserConnections callbackGuiUserJoinRemoved %s",
			groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
//! GuiHostJoinCallback
//============================================================================
//============================================================================
void AppletUserConnections::callbackJoinRequestCount( int requestCnt )
{
    if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "GuiHostJoinCallback callbackJoinRequestCount %d",
			requestCnt );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiHostJoinRequested( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
    if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostJoinRequested %s %s",
			guiHostJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiHostJoinWasGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
    if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostJoinWasGranted %s %s",
			guiHostJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiHostJoinIsGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
    if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostJoinIsGranted %s %s",
			guiHostJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiHostUnJoinGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
    if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostUnJoinGranted %s %s",
			guiHostJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiHostJoinDenied( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{
    if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostJoinDenied %s %s",
			guiHostJoin->getUser()->describeUser(true).toUtf8().constData(), groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiHostJoinLeaveHost( GroupieId& groupieId )
{
    if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostJoinLeaveHost %s",
			groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiHostUnJoin( GroupieId& groupieId )
{
    if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostUnJoin %s",
			groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::callbackGuiHostJoinRemoved( GroupieId& groupieId )
{
    if(LogEnabled(eLogUsers))LogModule( eLogUsers, LOG_DEBUG, "GuiHostJoinCallback callbackGuiHostJoinRemoved %s",
			groupieId.describeGroupieId().c_str() );
	refreshList();
}

//============================================================================
void AppletUserConnections::refreshList( void )
{
	ui.m_UserListWidget->setUserViewType( getSelectedUserViewType() );
}
