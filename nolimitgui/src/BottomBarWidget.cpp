//============================================================================
// Copyright (C) 2015 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "BottomBarWidget.h"

#include "AppletMgr.h"
#include "AppCommon.h"
#include "AppletChooseHost.h"
#include "GuiHelpers.h"
#include "GuiHostedListMgr.h"
#include "GuiMemberActiveMgr.h"
#include "GuiParams.h"
#include "MyIconsDefs.h"

#include <P2PEngine/P2PEngine.h>
#include <NetworkMonitor/NetStatusAccum.h>

#include <CoreLib/VxDebug.h>

#include "ui_BottomBarWidget.h"

//============================================================================
BottomBarWidget::BottomBarWidget( QWidget* parent )
: QFrame( parent )
, ui(*(new Ui::BottomBarWidgetClass))
, m_MyApp( GetAppInstance() )
{
    ui.setupUi( this );
	ui.m_MenuBottomButton->setFixedSize( eButtonSizeSmall );
	ui.m_ExpandButton->setFixedSize( eButtonSizeSmall );

	ui.m_MessengerButton->setFixedSize( eButtonSizeSmall );

    ui.m_GroupHostButton->setFixedSize( eButtonSizeSmall );
	ui.m_ChatRoomHostButton->setFixedSize( eButtonSizeSmall );
    ui.m_RandomConnectHostButton->setFixedSize( eButtonSizeSmall );
	ui.m_GroupJoinButton->setFixedSize( eButtonSizeSmall );
	ui.m_ChatRoomJoinButton->setFixedSize( eButtonSizeSmall );
	ui.m_RandomConnectJoinButton->setFixedSize( eButtonSizeSmall );

	ui.m_SettingsButton->setFixedSize( eButtonSizeSmall );

	ui.m_MessengerButton->setIcon( eMyIconMessenger );

	ui.m_GroupHostButton->setIcon( eMyIconGroupClient );
	ui.m_GroupHostButton->setNotifyAdminEnabled( true );
	ui.m_ChatRoomHostButton->setIcon( eMyIconChatRoomClient );
	ui.m_ChatRoomHostButton->setNotifyAdminEnabled( true );
	ui.m_RandomConnectHostButton->setIcon( eMyIconRandomConnectClient );
	ui.m_RandomConnectHostButton->setNotifyAdminEnabled( true );

	ui.m_GroupJoinButton->setIcon( eMyIconGroupClient );
	ui.m_ChatRoomJoinButton->setIcon( eMyIconChatRoomClient );
	ui.m_RandomConnectJoinButton->setIcon( eMyIconRandomConnectClient );

	ui.m_SettingsButton->setIcon( eMyIconSettingsGear );

	setMenuBottomButtonIcon();
	setExpandWindowButtonIcon();

	setMenuBottomVisibility( false );
	setExpandWindowVisibility( true );

	connect( ui.m_MenuBottomButton,			SIGNAL(clicked()),	this, SLOT(slotMenuBottomButtonClicked()) );
	connect( ui.m_ExpandButton,				SIGNAL(clicked()),	this, SLOT(slotExpandWindowButtonClicked()) );

	connect( ui.m_MessengerButton,			SIGNAL(clicked()), this, SLOT(slotMessengerButtonClicked()) );
	connect( ui.m_GroupHostButton,			SIGNAL(clicked()), this, SLOT(slotGroupHostButtonClicked()) );
	connect( ui.m_ChatRoomHostButton,		SIGNAL(clicked()), this, SLOT(slotChatRoomHostButtonClicked()) );
	connect( ui.m_RandomConnectHostButton,	SIGNAL(clicked()), this, SLOT(slotRandomConnectHostButtonClicked()) );

	connect( ui.m_GroupJoinButton,			SIGNAL(clicked()), this, SLOT(slotGroupJoinButtonClicked()) );
	connect( ui.m_ChatRoomJoinButton,		SIGNAL(clicked()), this, SLOT(slotChatRoomJoinButtonClicked()) );
	connect( ui.m_RandomConnectJoinButton,	SIGNAL(clicked()), this, SLOT(slotRandomConnectJoinButtonClicked()) );

	connect( ui.m_SettingsButton,			SIGNAL(clicked()), this, SLOT(slotSettingsButtonClicked()) );

    setFixedHeight( GuiParams::getButtonSize(eButtonSizeSmall).height() + 6 );

	connect( &m_MyApp, SIGNAL(signalSystemReady(bool) ), this, SLOT(slotSystemReady(bool)) );

	refreshUserJoinedToHostStates();

	m_MyApp.getMemberActiveMgr().wantMemberActiveCallback( this, true );
	m_MyApp.getAppletMgr().wantAppletMgrCallback( this, true );
}

//============================================================================
BottomBarWidget::~BottomBarWidget() 
{
	m_MyApp.getAppletMgr().wantAppletMgrCallback( this, false );
	m_MyApp.getMemberActiveMgr().wantMemberActiveCallback( this, false );
};

//============================================================================
//=== bottom bar button visibility ===// 
//============================================================================
void BottomBarWidget::setMenuBottomVisibility( bool visible )		{ ui.m_MenuBottomButton->setVisible( visible ); }
void BottomBarWidget::setExpandWindowVisibility( bool visible )		{ ui.m_ExpandButton->setVisible( visible ); }

//============================================================================
MyIcons&  BottomBarWidget::getMyIcons( void )
{
	return m_MyApp.getMyIcons();
}

//============================================================================
VxMenuButton * BottomBarWidget::getMenuButton( void )
{
    return ui.m_MenuBottomButton;
}

//============================================================================
void BottomBarWidget::setMenuBottomButtonIcon( EMyIcons myIcon )
{
	ui.m_MenuBottomButton->setIcon( myIcon );
}

//============================================================================
void BottomBarWidget::setExpandWindowButtonIcon( EMyIcons myIcon )
{
	ui.m_ExpandButton->setIcon( myIcon );
}

//=== bottom bar button color ===// 
//============================================================================
void BottomBarWidget::setMenuBottomButtonColor( QColor iconColor )
{
	ui.m_MenuBottomButton->setIconOverrideColor( iconColor );
}

//============================================================================
void BottomBarWidget::setExpandWindowButtonColor( QColor iconColor )
{
	ui.m_ExpandButton->setIconOverrideColor( iconColor );
}

//=== bottom bar button clicked ===// 
//============================================================================
void BottomBarWidget::slotMenuBottomButtonClicked( void )
{
	emit signalMenuBottomButtonClicked();
}

//============================================================================
void BottomBarWidget::slotExpandWindowButtonClicked( void )
{
	emit signalExpandWindowButtonClicked();
}

//============================================================================
void BottomBarWidget::callbackJoinRequestCount( int requestCnt )
{

}

//============================================================================
void BottomBarWidget::callbackGuiHostJoinIsGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin )
{

}

//============================================================================
void BottomBarWidget::callbackGuiHostJoinLeaveHost( GroupieId& groupieId )
{

}

//============================================================================
// get home page activity ( Launch or Messenger Page )
QWidget* BottomBarWidget::getParentPageFrame( void )
{
    return GuiHelpers::getParentPageFrame( this );
}

//============================================================================
void BottomBarWidget::slotMessengerButtonClicked( void )
{
	m_MyApp.getAppletMgr().launchApplet( eAppletMultiMessenger, getParentPageFrame() );
}

//============================================================================
void BottomBarWidget::slotGroupJoinButtonClicked( void )
{
	launchJoinHostView( eHostTypeGroup );
}

//============================================================================
void BottomBarWidget::slotChatRoomJoinButtonClicked( void )
{
	launchJoinHostView( eHostTypeChatRoom );
}

//============================================================================
void BottomBarWidget::slotRandomConnectJoinButtonClicked( void )
{
	launchJoinHostView( eHostTypeRandomConnect );
}

//============================================================================
void BottomBarWidget::launchJoinHostView( EHostType hostType )
{
	if( !m_MyApp.isMessengerReady() )
	{
		return;
	}

	if( !m_MyApp.getEngine().getNetStatusAccum().isNetworkOnline() )
	{
		GuiHelpers::showApplicationNotReadyError( true ); 
        return;
	}

	EApplet appleHostJoin{ eAppletUnknown };

	switch( hostType )
	{
	case eHostTypeGroup:
		appleHostJoin = eAppletGroupJoin;
		break;

	case eHostTypeChatRoom:
		appleHostJoin = eAppletChatRoomJoin;
		break;

	case eHostTypeRandomConnect:
		appleHostJoin = eAppletRandomConnectJoin;
		break;

	default:
		return;
	}

	if( eAppletUnknown != appleHostJoin )
	{
		m_MyApp.getAppletMgr().launchApplet( appleHostJoin, getParentPageFrame() );
	}
	else
	{
		LogMsg( LOG_ERROR, "BottomBarWidget::%s invalid host type", __func__ );
	}
}

//============================================================================
void BottomBarWidget::slotGroupHostButtonClicked( void )
{
	launchAdminHostView( eHostTypeGroup );
}

//============================================================================
void BottomBarWidget::slotChatRoomHostButtonClicked( void )
{
	launchAdminHostView( eHostTypeChatRoom );
}

//============================================================================
void BottomBarWidget::slotRandomConnectHostButtonClicked( void )
{
	launchAdminHostView( eHostTypeRandomConnect );
}

//============================================================================
void BottomBarWidget::launchAdminHostView( EHostType hostType )
{
	if( !m_MyApp.isMessengerReady() )
	{
		return;
	}

	if( !m_MyApp.getEngine().getNetStatusAccum().isNetworkOnline() )
	{
		GuiHelpers::showApplicationNotReadyError( true );
		return;
	}

	EApplet appletHostAdmin{ eAppletUnknown };

	switch( hostType )
	{
	case eHostTypeGroup:
		appletHostAdmin = eAppletGroupHostAdmin;
		break;

	case eHostTypeChatRoom:
		appletHostAdmin = eAppletChatRoomHostAdmin;
		break;

	case eHostTypeRandomConnect:
		appletHostAdmin = eAppletRandomConnectHostAdmin;
		break;

	default:
		return;
	}

	if( eAppletUnknown != appletHostAdmin )
	{
		m_MyApp.getAppletMgr().launchApplet( appletHostAdmin, getParentPageFrame() );
	}
	else
	{
		LogMsg( LOG_ERROR, "BottomBarWidget::%s invalid host type", __func__ );
	}

	refreshAdminHostStates();
}

//============================================================================
void BottomBarWidget::slotSettingsButtonClicked( void )
{
	m_MyApp.getAppletMgr().launchApplet( eAppletSettingsPage, getParentPageFrame() );
}

//============================================================================
void BottomBarWidget::callbackGuiMemberIsJoinedToHost( VxGUID& onlineId, EHostType hostType, bool isJoined )
{
	if( onlineId != m_MyApp.getMyOnlineId() )
	{
		return;
	}

	switch( hostType )
	{
	case eHostTypeGroup:
		ui.m_GroupJoinButton->setNotifyOnline( isJoined );
		break;

	case eHostTypeChatRoom:
		ui.m_ChatRoomJoinButton->setNotifyOnline( isJoined );
		break;

	case eHostTypeRandomConnect:
		ui.m_RandomConnectJoinButton->setNotifyOnline( isJoined );
		break;

	default:
		break;
	}
}

//============================================================================
void BottomBarWidget::callbackAppletIsOpen( EApplet applet, bool isOpen )
{
	switch( applet )
	{
	case eAppletGroupHostAdmin:
		updateAdminHostState( eHostTypeGroup, ui.m_GroupHostButton );
		break;
	case eAppletChatRoomHostAdmin:
		updateAdminHostState( eHostTypeChatRoom, ui.m_ChatRoomHostButton );
		break;
	case eAppletRandomConnectHostAdmin:
		updateAdminHostState( eHostTypeRandomConnect, ui.m_RandomConnectHostButton );
		break;
	default:
		break;
	}
}

//============================================================================
void BottomBarWidget::refreshUserJoinedToHostStates( void )
{
	ui.m_GroupJoinButton->setNotifyOnline( m_MyApp.getUserJoinMgr().isUserJoinedToHost( eHostTypeGroup ) );
	ui.m_ChatRoomJoinButton->setNotifyOnline( m_MyApp.getUserJoinMgr().isUserJoinedToHost( eHostTypeChatRoom ) );
	ui.m_RandomConnectJoinButton->setNotifyOnline( m_MyApp.getUserJoinMgr().isUserJoinedToHost( eHostTypeRandomConnect ) );
}

//============================================================================
void BottomBarWidget::refreshAdminHostStates( void )
{
	updateAdminHostState( eHostTypeGroup, ui.m_GroupHostButton );
	updateAdminHostState( eHostTypeChatRoom, ui.m_ChatRoomHostButton );
	updateAdminHostState( eHostTypeRandomConnect, ui.m_RandomConnectHostButton );
}

//============================================================================
void BottomBarWidget::updateAdminHostState( EHostType hostType, VxPushButton* adminButton )
{
	VxNetIdent& myNetIdent = m_MyApp.getUserMgr().getMyIdent()->getNetIdent();
	bool isHosting = myNetIdent.userIsHosting( hostType );
	adminButton->setVisible( isHosting );
	if( isHosting )
	{
		if( myNetIdent.canDirectConnectToUser() )
		{
			EApplet appletHostAdmin{ eAppletUnknown };

			switch( hostType )
			{
			case eHostTypeGroup:
				appletHostAdmin = eAppletGroupHostAdmin;
				break;

			case eHostTypeChatRoom:
				appletHostAdmin = eAppletChatRoomHostAdmin;
				break;

			case eHostTypeRandomConnect:
				appletHostAdmin = eAppletRandomConnectHostAdmin;
				break;

			default:
				return;
			}

			if( m_MyApp.getAppletMgr().isAppletLaunched( appletHostAdmin ) )
			{
				QColor adminColor = m_MyApp.getAppTheme().getNotifyColor( eNotifyOnline );
				adminButton->setNotifyAdminColor( adminColor );
			}
			else
			{
				QColor adminColor = m_MyApp.getAppTheme().getNotifyColor( eNotifyRelayed );
				adminButton->setNotifyAdminColor( adminColor );
			}
		}
		else
		{
			QColor adminColor = m_MyApp.getAppTheme().getNotifyColor( eNotifyOffline );
			adminButton->setNotifyAdminColor( adminColor );
		}
	}
}

//============================================================================
void BottomBarWidget::slotSystemReady( bool isReady )
{
	if( isReady )
	{
		refreshUserJoinedToHostStates();
		refreshAdminHostStates();
	}
}

//============================================================================
void BottomBarWidget::showEvent( QShowEvent* showEvent )
{
	QFrame::showEvent( showEvent );
	if( m_MyApp.isSystemReady() )
	{
		refreshUserJoinedToHostStates();
		refreshAdminHostStates();
	}
}
