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
#include "AppletMgr.h"
#include "AppSettings.h"

#include "GuiParams.h"

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

	getInfoEdit()->setMaximumBlockCount( MAX_LOG_EDIT_BLOCK_CNT );
	getInfoEdit()->setReadOnly( true );

	m_HostUrl = url;
	m_HostPtopUrl.setUrl( url );
	m_HostOnlineId = m_HostPtopUrl.getOnlineId();
	m_HostType = m_HostPtopUrl.getHostType();

	if( !m_HostPtopUrl.isValid() || !m_HostPtopUrl.isHostTypeValid() )
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

	m_MyApp.activityStateChange( this, true );
	m_MyApp.getUserJoinMgr().wantUserJoinCallbacks( this, true );
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


////============================================================================
//void AppletHostJoinConnect::setHostType( EHostType hostType )
//{ 
//	bool isJoined{ false };
//	m_HostType = hostType;
//	if( m_HostType == eHostTypeChatRoom )
//	{
//		setPluginType( ePluginTypeClientChatRoom );
//		isJoined = m_MyApp.getUserJoinMgr().isUserJoinedToHost( m_HostType );
//		setTitleBarText( QObject::tr("Choose Chat Room Host") );
//		ui.m_ViewCurrentButton->setIcon( eMyIconEyeChatRoom );
//		ui.m_SearchButton->setIcon( eMyIconChatRoomClient );
//	}
//	else if( m_HostType == eHostTypeGroup )
//	{
//		setPluginType( ePluginTypeClientGroup );
//		isJoined = m_MyApp.getUserJoinMgr().isUserJoinedToHost( m_HostType );
//		setTitleBarText( QObject::tr( "Choose Group Host" ) );
//		ui.m_ViewCurrentButton->setIcon( eMyIconEyeAnnouncedGroups );
//		ui.m_SearchButton->setIcon( eMyIconGroupClient );
//	}
//	else if( m_HostType == eHostTypeRandomConnect )
//	{
//		setPluginType( ePluginTypeClientRandomConnect );
//		setTitleBarText( QObject::tr( "Choose Random Connect Host" ) );
//		isJoined = m_MyApp.getUserJoinMgr().isUserJoinedToHost( m_HostType );
//		ui.m_ViewCurrentButton->setIcon( eMyIconEyeAnnouncedRandomConnect );
//		ui.m_SearchButton->setIcon( eMyIconRandomConnectClient );
//	}
//	else
//	{
//		setTitleBarText( QObject::tr( "Unknown Host Type" ) );
//	}
//
//	ui.m_ViewCurrentFrame->setVisible( isJoined );
//	bool showRejoin{ false };
//	if( !isJoined )
//	{
//		VxPtopUrl lastJoined = m_MyApp.getUserJoinMgr().getLastJoinedPtopUrl( hostType );
//		ui.m_RejoinFrame->setVisible( lastJoined.isValid() );
//	}
//	else
//	{
//		ui.m_RejoinFrame->setVisible( false );
//	}
//	
//}
//
////============================================================================
//void AppletHostJoinConnect::slotViewCurrentButtonClicked( void )
//{
//	GroupieId adminGroupieId = m_MyApp.getUserJoinMgr().getJoinedAdminGroupieId( m_HostType );
//	if( adminGroupieId.isValid() )
//	{
//		EApplet joinedApplet = GuiParams::hostTypeToClientApplet( m_HostType );
//		ActivityBase* activity = m_MyApp.getAppletMgr().launchApplet( joinedApplet, getParentPageFrame() );
//		if( activity )
//		{
//			AppletClientBase* clientBase = dynamic_cast<AppletClientBase*>( activity );
//			if( clientBase )
//			{
//				clientBase->setAdminGroupieId( adminGroupieId );
//			}
//			else
//			{
//				LogMsg( LOG_ERROR, "AppletHostJoinConnect::%s dynamic cast failed", __func__ );
//			}
//		}
//		else
//		{
//			LogMsg( LOG_ERROR, "AppletHostJoinConnect::%s failed to launch", __func__ );
//		}
//	}
//	else
//	{
//		LogMsg( LOG_ERROR, "AppletHostJoinConnect::%s invalid admin id", __func__ );
//	}
//
//	closeApplet();
//}
//
////============================================================================
//void AppletHostJoinConnect::slotRejoinButtonClicked( void )
//{
//	VxPtopUrl lastJoined = m_MyApp.getUserJoinMgr().getLastJoinedPtopUrl( m_HostType );
//
//}
//
////============================================================================
//void AppletHostJoinConnect::slotSearchButtonClicked( void )
//{
//	ActivityBase * activity = m_MyApp.getAppletMgr().launchApplet( eAppletHostJoinRequestList, getParentPageFrame() );
//}
