//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletHostJoinChoose.h"

#include "AppCommon.h"
#include "AppletClientBase.h"
#include "AppletMgr.h"

#include "GuiHelpers.h"
#include "GuiParams.h"
#include "GuiUserJoinMgr.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletHostJoinChoose.h"

//============================================================================
AppletHostJoinChoose::AppletHostJoinChoose( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_HOST_JOIN_CHOOSE, app, parent )
, ui(*(new Ui::AppletHostJoinChooseUi))
{
	setAppletType( eAppletHostJoinChoose );
	ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	setPluginType( ePluginTypeClientChatRoom );

    ui.m_LeaveFrame->setVisible( false );

	ui.m_ViewCurrentButton->setFixedSize( eButtonSizeMedium );
    ui.m_LeaveButton->setFixedSize( eButtonSizeMedium );
	ui.m_RejoinButton->setFixedSize( eButtonSizeMedium );
	ui.m_SearchButton->setFixedSize( eButtonSizeMedium );

    ui.m_LeaveButton->setIcon( eMyIconUserLeave );
	ui.m_RejoinButton->setIcon( eMyIconConnect );

	connect( ui.m_ViewCurrentButton, SIGNAL(clicked()), this, SLOT(slotViewCurrentButtonClicked()) );
	connect( ui.m_ViewCurrentLabel, SIGNAL(clicked()), this, SLOT(slotViewCurrentButtonClicked()) );
	connect( ui.m_LeaveButton, SIGNAL(clicked()), this, SLOT(slotLeaveButtonClicked()) );
	connect( ui.m_LeaveLabel, SIGNAL(clicked()), this, SLOT(slotLeaveButtonClicked()) );
	connect( ui.m_RejoinButton, SIGNAL(clicked()), this, SLOT(slotRejoinButtonClicked()) );
	connect( ui.m_RejoinLabel, SIGNAL(clicked()), this, SLOT(slotRejoinButtonClicked()) );
	connect( ui.m_SearchButton, SIGNAL(clicked()), this, SLOT(slotSearchButtonClicked()) );
	connect( ui.m_SearchLabel, SIGNAL(clicked()), this, SLOT(slotSearchButtonClicked()) );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletHostJoinChoose::~AppletHostJoinChoose()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletHostJoinChoose::setHostType( EHostType hostType )
{ 
	bool isJoined{ false };
	m_HostType = hostType;
	if( m_HostType == eHostTypeChatRoom )
	{
		setPluginType( ePluginTypeClientChatRoom );
		isJoined = m_MyApp.getUserJoinMgr().isUserJoinedToHost( m_HostType );
		setTitleBarText( QObject::tr("Choose Chat Room Host") );
		ui.m_ViewCurrentButton->setIcon( eMyIconEyeChatRoom );
		ui.m_SearchButton->setIcon( eMyIconChatRoomClient );
	}
	else if( m_HostType == eHostTypeGroup )
	{
		setPluginType( ePluginTypeClientGroup );
		isJoined = m_MyApp.getUserJoinMgr().isUserJoinedToHost( m_HostType );
		setTitleBarText( QObject::tr( "Choose Group Host" ) );
		ui.m_ViewCurrentButton->setIcon( eMyIconEyeGroup );
		ui.m_SearchButton->setIcon( eMyIconGroupClient );
	}
	else if( m_HostType == eHostTypeRandomConnect )
	{
		setPluginType( ePluginTypeClientRandomConnect );
		setTitleBarText( QObject::tr( "Choose Random Connect Host" ) );
		isJoined = m_MyApp.getUserJoinMgr().isUserJoinedToHost( m_HostType );
		ui.m_ViewCurrentButton->setIcon( eMyIconEyeRandomConnect );
		ui.m_SearchButton->setIcon( eMyIconRandomConnectClient );
	}
	else
	{
		setTitleBarText( QObject::tr( "Unknown Host Type" ) );
	}

	ui.m_ViewCurrentFrame->setVisible( isJoined );
    ui.m_LeaveFrame->setVisible( isJoined );
	VxPtopUrl lastJoined = m_MyApp.getUserJoinMgr().getLastJoinedPtopUrl( hostType );
	bool rejoinValid = lastJoined.isValid();
	if( !isJoined )
	{	
		ui.m_RejoinFrame->setVisible( rejoinValid );
	}
	else
	{
		ui.m_RejoinFrame->setVisible( false );
	}

	if( !isJoined && !rejoinValid )
	{
		// the only option left is search so launch now
		slotSearchButtonClicked();
	}
}

//============================================================================
void AppletHostJoinChoose::slotViewCurrentButtonClicked( void )
{
	GroupieId adminGroupieId = m_MyApp.getUserJoinMgr().getJoinedAdminGroupieId( m_HostType );
	if( adminGroupieId.isValid() )
	{
		EApplet joinedApplet = GuiParams::hostTypeToClientApplet( m_HostType );
		ActivityBase* activity = m_MyApp.getAppletMgr().launchApplet( joinedApplet, getParentPageFrame() );
		if( activity )
		{
			AppletClientBase* clientBase = dynamic_cast<AppletClientBase*>( activity );
			if( clientBase )
			{
				clientBase->setAdminGroupieId( adminGroupieId );
			}
			else
			{
				LogMsg( LOG_ERROR, "AppletHostJoinChoose::%s dynamic cast failed", __func__ );
			}
		}
		else
		{
			LogMsg( LOG_ERROR, "AppletHostJoinChoose::%s failed to launch", __func__ );
		}
	}
	else
	{
		LogMsg( LOG_ERROR, "AppletHostJoinChoose::%s invalid admin id", __func__ );
	}

	closeApplet();
}

//============================================================================
void AppletHostJoinChoose::slotRejoinButtonClicked( void )
{
	m_MyApp.getUserJoinMgr().leaveHost( m_HostType );
	VxPtopUrl lastJoined = m_MyApp.getUserJoinMgr().getLastJoinedPtopUrl( m_HostType );
	m_MyApp.getAppletMgr().launchApplet( eAppletHostJoinConnect, getParentPageFrame(), lastJoined.getHostUrl().c_str() );
	closeApplet();
}

//============================================================================
void AppletHostJoinChoose::slotSearchButtonClicked( void )
{
	m_MyApp.getUserJoinMgr().leaveHost( m_HostType );
	EApplet applet{ eAppletUnknown };
	switch( m_HostType )
	{
	case eHostTypeChatRoom:
		applet = eAppletChatRoomJoin;
		break;

	case eHostTypeGroup:
		applet = eAppletGroupJoin;
		break;

	case eHostTypeRandomConnect:
		applet = eAppletRandomConnectJoin;
		break;

	default:
		return;
	}

	m_MyApp.getAppletMgr().launchApplet( applet, getParentPageFrame() );
	closeApplet();
}

//============================================================================
void AppletHostJoinChoose::slotLeaveButtonClicked( void )
{
    m_MyApp.getUserJoinMgr().leaveHost( m_HostType );   
    closeApplet();
}
