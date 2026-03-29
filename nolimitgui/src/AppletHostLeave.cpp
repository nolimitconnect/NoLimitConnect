//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletHostLeave.h"

#include "AppCommon.h"
#include "AppletClientBase.h"
#include "AppletMgr.h"
#include "AppSettings.h"

#include "GuiHelpers.h"
#include "GuiParams.h"
#include "GuiUserJoinMgr.h"
#include "GuiUserListWidget.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletHostLeave.h"

//============================================================================
AppletHostLeave::AppletHostLeave( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_HOST_LEAVE, app, parent )
, ui(*(new Ui::AppletHostLeaveUi))
{
	setAppletType( eAppletHostLeave );
	ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	setPluginType( ePluginTypeClientChatRoom );

	ui.m_HostTypeButton->setFixedSize( eButtonSizeMedium );
	ui.m_LeaveButton->setFixedSize( eButtonSizeMedium );
	ui.m_BootButton->setFixedSize( eButtonSizeMedium );
	ui.m_BlockButton->setFixedSize( eButtonSizeMedium );
	ui.m_CancelButton->setFixedSize( eButtonSizeMedium );

	ui.m_LeaveButton->setIcon( eMyIconUserLeave );
	ui.m_BootButton->setIcon( eMyIconBoot );
	ui.m_BlockButton->setIcon( eMyIconIgnored );
	ui.m_CancelButton->setIcon( eMyIconRejectRedX );
	ui.m_CancelButton->setIconOverrideColor( m_MyApp.getAppTheme().getCancelColor() );

	connect( ui.m_LeaveButton, SIGNAL(clicked()), this, SLOT(slotLeaveButtonClicked()) );
	connect( ui.m_LeaveLabel, SIGNAL(clicked()), this, SLOT(slotLeaveButtonClicked()) );
	connect( ui.m_BootButton, SIGNAL(clicked()), this, SLOT(slotBootButtonClicked()) );
	connect( ui.m_BootLabel, SIGNAL(clicked()), this, SLOT(slotBootButtonClicked()) );
	connect( ui.m_BlockButton, SIGNAL(clicked()), this, SLOT(slotBlockButtonClicked()) );
	connect( ui.m_BlockLabel, SIGNAL(clicked()), this, SLOT(slotBlockButtonClicked()) );
	connect( ui.m_CancelButton, SIGNAL(clicked()), this, SLOT(slotCancelButtonClicked()) );
	connect( ui.m_CancelLabel, SIGNAL(clicked()), this, SLOT(slotCancelButtonClicked()) );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletHostLeave::~AppletHostLeave()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletHostLeave::setHostGroupieId( GroupieId groupieId )
{
	bool isJoined{ false };
	m_GroupieId = groupieId;
	m_HostType = groupieId.getHostType();
	if( m_HostType == eHostTypeChatRoom )
	{
		setPluginType( ePluginTypeClientChatRoom );
		isJoined = m_MyApp.getUserJoinMgr().isUserJoinedToHost( m_HostType );
		setTitleBarText( QObject::tr( "Leave Chat Room" ) );
		ui.m_HostTypeButton->setIcon( eMyIconChatRoomClient );
	}
	else if( m_HostType == eHostTypeGroup )
	{
		setPluginType( ePluginTypeClientGroup );
		isJoined = m_MyApp.getUserJoinMgr().isUserJoinedToHost( m_HostType );
		setTitleBarText( QObject::tr( "Leave Group" ) );
		ui.m_HostTypeButton->setIcon( eMyIconGroupClient );
	}
	else if( m_HostType == eHostTypeRandomConnect )
	{
		setPluginType( ePluginTypeClientRandomConnect );
		setTitleBarText( QObject::tr( "Leave Random Connect" ) );
		isJoined = m_MyApp.getUserJoinMgr().isUserJoinedToHost( m_HostType );
		ui.m_HostTypeButton->setIcon( eMyIconRandomConnectClient );
	}
	else
	{
		setTitleBarText( QObject::tr( "Unknown Host Type" ) );
	}

	ui.m_HostIdentWidget->setupIdentLogic();
	GuiUser* hostUser = m_MyApp.getUserMgr().getUser( groupieId.getHostOnlineId() );
	if( hostUser )
	{
		ui.m_HostIdentWidget->updateIdentity( hostUser );
	}
}

//============================================================================
void AppletHostLeave::slotLeaveButtonClicked( void )
{
	emit signalLeftHost();
	m_MyApp.getUserJoinMgr().leaveHost( m_GroupieId.getHostedId() );
	closeApplet();
}

//============================================================================
void AppletHostLeave::slotBootButtonClicked( void )
{
	emit signalLeftHost();
	m_MyApp.getUserJoinMgr().unjoinHost( m_GroupieId.getHostedId() );
	m_MyApp.getUserJoinMgr().clearLastJoined( m_HostType );
	closeApplet();
}

//============================================================================
void AppletHostLeave::slotBlockButtonClicked( void )
{
	emit signalLeftHost();
	m_MyApp.getUserJoinMgr().unjoinHost( m_GroupieId.getHostedId() );
	m_MyApp.getUserJoinMgr().clearLastJoined( m_HostType );
	m_MyApp.getUserMgr().blockUser( m_GroupieId.getHostOnlineId() );
	closeApplet();
}

//============================================================================
void AppletHostLeave::slotCancelButtonClicked( void )
{
	closeApplet();
}
