//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletHostJoinConnect.h"

#include "AppCommon.h"
#include "AppletHostLeave.h"
#include "AppletMgr.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <GuiInterface/IFromGui.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include <QPlainTextEdit>
#include <QScrollBar>

#include "ui_AppletHostJoinConnect.h"

namespace
{
	const int MAX_LOG_EDIT_BLOCK_CNT = 1000;
	const int MAX_INFO_MSG_SIZE = 2048;
}

//============================================================================
AppletHostJoinConnect::AppletHostJoinConnect( AppCommon& app, QWidget* parent, std::string url )
	: AppletBase( OBJNAME_APPLET_HOST_JOIN_CONNECT, app, parent )
	, ui( *( new Ui::AppletHostJoinConnectUi ) )
{
	setAppletType( eAppletHostJoinConnect );
	ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	ui.m_AdminIdentWidget->setupIdentLogic();

	getInfoEdit()->setMaximumBlockCount( MAX_LOG_EDIT_BLOCK_CNT );
	getInfoEdit()->setReadOnly( true );

	ui.m_ViewCurrentButton->setVisible( false );
	ui.m_ViewCurrentLabel->setVisible( false );
	ui.m_RejoinButton->setVisible( false );
	ui.m_RejoinLabel->setVisible( false );
	ui.m_SearchButton->setVisible( false );
	ui.m_SearchLabel->setVisible( false );

	m_HostUrl = url;
	m_HostPtopUrl.setUrl( url );
	m_HostOnlineId = m_HostPtopUrl.getOnlineId();
	m_HostType = m_HostPtopUrl.getHostType();

	if( !m_HostPtopUrl.isValid() || !m_HostPtopUrl.isHostTypeValid() || !m_HostOnlineId.isValid() )
	{
		QString title = QObject::tr( "Host URL is not valid" );
		QString msg = m_HostUrl.c_str();
		msg += "\n";
		if( m_HostPtopUrl.isHostTypeValid() )
		{
			msg += QObject::tr( "Failed to resolve into valid ptop url" );
		}
		else
		{
			msg += QObject::tr( "Host Type Invalid" );
		}

		m_IsClosing = true;
		errMessageBox( title, msg );
		closeApplet();
		return;
	}

	m_AdminGroupieId.setHostType( m_HostType );
	m_AdminGroupieId.setHostOnlineId( m_HostOnlineId );
	m_AdminGroupieId.setUserOnlineId( m_MyApp.getMyOnlineId() );

	logMsg( "%s url %s admin %s", __func__, m_HostUrl.c_str(), m_MyApp.describeUser( m_HostOnlineId ).c_str() );

    VxPtopUrl lastJoinedUrl = m_MyApp.getUserJoinMgr().getLastJoinedPtopUrl( m_HostType );
    GroupieId lastHostGroupieId = lastJoinedUrl.getHostGroupieId();
    if( lastHostGroupieId.isValid() && m_MyApp.getConnectIdListMgr().isConnected( lastHostGroupieId ) )
    {
		AppletHostLeave* leaveApplet = dynamic_cast<AppletHostLeave*>( m_MyApp.getAppletMgr().launchApplet( eAppletHostLeave, getParentPageFrame() ) );
		if( leaveApplet )
		{
			leaveApplet->setHostGroupieId( m_AdminGroupieId );
		}
       
		closeApplet();
		return;
    }

	m_MyApp.activityStateChange( this, true );
	m_MyApp.getUserJoinMgr().wantUserJoinCallbacks( this, true );

	GuiUser* netHostUser = m_MyApp.getUserMgr().getOrQueryUser( m_HostOnlineId );
	if( netHostUser )
	{
		ui.m_AdminIdentWidget->updateIdentity( netHostUser );
	}

	joinHost();
}

//============================================================================
AppletHostJoinConnect::~AppletHostJoinConnect()
{
	m_MyApp.getUserJoinMgr().wantUserJoinCallbacks( this, false );
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
QPlainTextEdit* AppletHostJoinConnect::getInfoEdit( void )
{
	return ui.m_InfoPlainTextEdit;
}

//============================================================================
void AppletHostJoinConnect::callbackGuiUserJoinRequested( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
	logMsg( __func__ );
}

//============================================================================
void AppletHostJoinConnect::callbackGuiUserJoinWasGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
	logMsg( __func__ );
}

//============================================================================
void AppletHostJoinConnect::callbackGuiUserJoinIsGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
	logMsg( __func__ );
	if( groupieId.getHostedId() == m_AdminGroupieId.getHostedId() && 
		guiUserJoin->getGroupieId().getUserOnlineId() == m_MyApp.getMyOnlineId() && 
		groupieId.getHostOnlineId() != m_MyApp.getMyOnlineId() )
	{
		emit signalJoinedHost( groupieId.getHostedId(), true );
		m_MyApp.getAppletMgr().launchApplet( GuiHelpers::hostTypeToHostClientApplet( groupieId.getHostType() ), getParentPageFrame() );
		closeApplet();
	}
}

//============================================================================
void AppletHostJoinConnect::callbackGuiUserUnJoinGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
	logMsg( __func__ );
}

//============================================================================
void AppletHostJoinConnect::callbackGuiUserJoinDenied( GroupieId& groupieId, GuiUserJoin* guiUserJoin )
{
	logMsg( __func__ );
}

//============================================================================
void AppletHostJoinConnect::callbackGuiUserJoinLeaveHost( GroupieId& groupieId )
{
	logMsg( __func__ );
}

//============================================================================
void AppletHostJoinConnect::callbackGuiUserJoinRemoved( GroupieId& groupieId )
{
	logMsg( __func__ );
}

//============================================================================
void AppletHostJoinConnect::callbackGuiUserJoinToHostState( EHostType hostType, bool isJoined )
{
	logMsg( __func__ );
}

//============================================================================
void AppletHostJoinConnect::callbackGuiUserJoinAHostStatus( EHostType hostType, VxGUID& sessionId, EConnectStatus connectStatus )
{
	logMsg( __func__ );
}


//============================================================================
void AppletHostJoinConnect::logMsg( const char* msg, ... )
{
	std::array<char, MAX_INFO_MSG_SIZE> szBuffer;
	va_list arg_ptr;
	va_start( arg_ptr, msg );
	vsnprintf( szBuffer.data(), MAX_INFO_MSG_SIZE, msg, arg_ptr );
	szBuffer.data()[MAX_INFO_MSG_SIZE - 1] = 0;
	va_end( arg_ptr );

	getInfoEdit()->appendPlainText( QString( szBuffer.data() ) ); // Adds the message to the widget
	getInfoEdit()->verticalScrollBar()->setValue( getInfoEdit()->verticalScrollBar()->maximum() ); // Scrolls to the bottom

	if(LogEnabled( eLogHostJoin ))LogMsg( LOG_INFO, szBuffer.data() );
}


//============================================================================
void AppletHostJoinConnect::joinHost( void )
{
	GuiUserJoin* userJoin = m_MyApp.getUserJoinMgr().getUserJoin( m_AdminGroupieId );
	EJoinState joinState{ eJoinStateNone };
	if( userJoin )
	{
		joinState = userJoin->getJoinState();
	}


	m_JoinHostSessionId.initializeWithNewVxGUID();
	m_MyApp.getFromGuiInterface().fromGuiJoinHost( m_AdminGroupieId.getHostedId(), m_JoinHostSessionId, m_HostUrl );
}
