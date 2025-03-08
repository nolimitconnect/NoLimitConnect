//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletPopupMenu.h"

#include "ActivityMessageBox.h"	
#include "AppCommon.h"	
#include "AppSettings.h"

#include "AppletAboutMeClient.h"
#include "AppletAboutUser.h"
#include "AppletCamClient.h"
#include "AppletFileShareClientView.h"
#include "AppletPeerChangeFriendship.h"
#include "AppletPeerTodGame.h"
#include "AppletPeerVideoPhone.h"
#include "AppletPeerVoicePhone.h"
#include "AppletStoryboardClient.h"

#include "FileShareItemWidget.h"
#include "MyIcons.h"
#include "AppGlobals.h"
#include "AppletMgr.h"
#include "FileItemInfo.h"
#include "FileActionMenu.h"
#include "GuiHelpers.h"
#include "GuiParams.h"
#include "GuiGroupieListSession.h"
#include "GuiHostSession.h"
#include "GuiHostedListSession.h"
#include "BottomBarWidget.h"
#include "TitleBarWidget.h"

#include <P2PEngine/P2PEngine.h>
#include <AssetMgr/AssetMgr.h>

#include <PktLib/VxSearchDefs.h>
#include <NetLib/VxFileXferInfo.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileInfo.h>
#include <CoreLib/VxGlobals.h>
#include <VxVideoLib/VxVideoLib.h>

#include <QResizeEvent>
#include <QMessageBox>
#include <QTimer>

#include "ui_AppletPopupMenu.h"

//============================================================================
AppletPopupMenu::AppletPopupMenu( AppCommon& app, QWidget* parent, VxGUID assetId )
: AppletBase( OBJNAME_APPLET_POPUP_MENU, app, parent )
, ui(*(new Ui::AppletPopupMenuUi))
{
    setAppletType( eAppletPopupMenu );
	initAppletPopupMenu();
    setTitleBarText( DescribeApplet( m_EAppletType ) );
	getTitleBarWidget()->setVisible( false );
	getBottomBarWidget()->setVisible( false );

    connect( ui.m_ExitPopupButton, SIGNAL(clicked()), this, SLOT(closeApplet()) );
    connect( ui.m_MenuItemList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)) );

    // NOT USED FOR POPUP
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
AppletPopupMenu::~AppletPopupMenu()
{
    // NOT USED FOR POPUP
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletPopupMenu::initAppletPopupMenu( void )
{
	ui.setupUi( getContentItemsFrame() );
	ui.m_ExitPopupButton->setIcons( eMyIconBack );
	ui.m_ExitPopupButton->setFixedSize( GuiParams::getButtonSize( eButtonSizeSmall ) );
}

//============================================================================
void AppletPopupMenu::setTitle( QString strTitle )
{
	ui.m_PopupTitleLabel->setText( strTitle );
}

//============================================================================
void AppletPopupMenu::setMenuType( EPopupMenuType menuType ) 
{ 
	m_MenuType = menuType; 
	clearMenulList();
	m_MyApp.getAppStyle().clearFocusFrameWidget();
};

//============================================================================
void AppletPopupMenu::clearMenulList( void )
{
	ui.m_MenuItemList->clear();
};

//============================================================================
void AppletPopupMenu::addMenuItem( int iItemId, QIcon& oIcon, QString strMenuItemText )
{
	QListWidgetItem* poMenuItem = new QListWidgetItem( strMenuItemText );
	poMenuItem->setIcon( oIcon );
	poMenuItem->setData( Qt::UserRole, iItemId );
	ui.m_MenuItemList->addItem( poMenuItem );
}

//============================================================================
//! clicked the upper right back button
void AppletPopupMenu::slotBackButtonClicked( void )
{
	emit signalBackButtonClicked();
	m_MyApp.getAppStyle().clearFocusFrameWidget();
}

//============================================================================
void AppletPopupMenu::itemClicked( QListWidgetItem* item )
{
	m_MyApp.playSound( eSndDefButtonClick );
	// resignal with id set by user
	int iItemId = item->data( Qt::UserRole ).toInt();

	switch( getMenuType() )
	{
	case EPopupMenuType::ePopupMenuTitleBarAppMenu:
		onTitleBarAppMenuSelected( iItemId );
		break;
	
	case EPopupMenuType::ePopupMenuTitleBarUserMenu:
		onTitleBarUserMenuSelected( iItemId );
		break;

	case EPopupMenuType::ePopupMenuFriend:
		onFriendActionSelected( iItemId );
		break;
	
	case EPopupMenuType::ePopupMenuOfferFriendship:
	case EPopupMenuType::ePopupMenuUserSession:
		onPersonActionSelected( iItemId );
		break;

	case EPopupMenuType::ePopupMenuDeleteDb:
		onDeleteDbSelected( iItemId );
		break;

	case EPopupMenuType::ePopupMenuGroupieListSession:
		onGroupieSessionActionSelected( iItemId );
		break;

	case EPopupMenuType::ePopupMenuHostedListSession:
		onHostListSessionMenu( iItemId );
		break;

	case EPopupMenuType::ePopupMenuHostSession:
		onHostSessionActionSelected( iItemId );
		break;

	default:
		LogMsg( LOG_ERROR, "AppletPopupMenu::%s Unknown menu type %d", __func__, getMenuType() );
		emit menuItemClicked( iItemId, this, m_ParentActivity );
		vx_assert( false );
	}

	closeApplet();
}

//============================================================================
void AppletPopupMenu::showFriendMenu( GuiUser* poSelectedFriend, bool inGroup )
{
	setMenuType( EPopupMenuType::ePopupMenuFriend );
	m_SelectedFriend = poSelectedFriend;
	m_InGroup = inGroup;
	bool isMyself = poSelectedFriend->isMyself();
	// populate title
	QString strTitle = poSelectedFriend->describeMyFriendshipToHim( inGroup );
	if( isMyself )
	{
		strTitle = GuiParams::describeFriendship( eFriendStateAdmin );
	}

	strTitle += ": ";
	strTitle += poSelectedFriend->getOnlineName().c_str();
	setTitle( strTitle );
	// populate menu
	EPluginAccess ePluginAccess;
	QString strAction;


	if( m_SelectedFriend->isIgnored() )
	{
		addMenuItem( (int)eMaxPluginType + 1, getMyIcons().getIcon( eMyIconFriend ), QObject::tr( "Unblock User" ) );
		return;
	}
	else if( !isMyself )
	{
		addMenuItem( (int)eMaxPluginType + 1, getMyIcons().getIcon( eMyIconIgnored ), QObject::tr( "Block User" ) );
	}

	if( m_SelectedFriend->isMyAccessAllowedFromHim( ePluginTypeAboutMePageServer, m_InGroup ) )
	{
		ePluginAccess = poSelectedFriend->getMyAccessPermissionFromHim( ePluginTypeAboutMePageServer, m_InGroup );
		strAction = GuiParams::describePluginAction( poSelectedFriend, ePluginTypeAboutMePageServer, ePluginAccess );
		addMenuItem( (int)ePluginTypeAboutMePageServer, getMyIcons().getIcon( getMyIcons().getPluginIcon( ePluginTypeAboutMePageServer, ePluginAccess ) ), strAction );
	}

	if( m_SelectedFriend->isMyAccessAllowedFromHim( ePluginTypeStoryboardServer, m_InGroup ) )
	{
		ePluginAccess = poSelectedFriend->getMyAccessPermissionFromHim( ePluginTypeStoryboardServer, m_InGroup );
		strAction = GuiParams::describePluginAction( poSelectedFriend, ePluginTypeStoryboardServer, ePluginAccess );
		addMenuItem( (int)ePluginTypeStoryboardServer, getMyIcons().getIcon( getMyIcons().getPluginIcon( ePluginTypeStoryboardServer, ePluginAccess ) ), strAction );
	}

	if( !isMyself && m_SelectedFriend->isMyAccessAllowedFromHim( ePluginTypeMessenger, m_InGroup ) )
	{
		ePluginAccess = poSelectedFriend->getMyAccessPermissionFromHim( ePluginTypeMessenger, m_InGroup );
		strAction = GuiParams::describePluginAction( poSelectedFriend, ePluginTypeMessenger, ePluginAccess );
		addMenuItem( ( int )ePluginTypeMessenger, getMyIcons().getIcon( getMyIcons().getPluginIcon( ePluginTypeMessenger, ePluginAccess ) ), strAction );
	}

	if( !isMyself && m_SelectedFriend->isMyAccessAllowedFromHim( ePluginTypeVideoPhone, m_InGroup ) )
	{
		ePluginAccess = poSelectedFriend->getMyAccessPermissionFromHim( ePluginTypeVideoPhone, m_InGroup );
		strAction = GuiParams::describePluginAction( poSelectedFriend, ePluginTypeVideoPhone, ePluginAccess );
		addMenuItem( (int)ePluginTypeVideoPhone, getMyIcons().getIcon( getMyIcons().getPluginIcon( ePluginTypeVideoPhone, ePluginAccess ) ), strAction );
	}

	if( !isMyself && m_SelectedFriend->isMyAccessAllowedFromHim( ePluginTypeVoicePhone, m_InGroup ) )
	{
		ePluginAccess = poSelectedFriend->getMyAccessPermissionFromHim( ePluginTypeVoicePhone, m_InGroup );
		strAction = GuiParams::describePluginAction( poSelectedFriend, ePluginTypeVoicePhone, ePluginAccess );
		addMenuItem( (int)ePluginTypeVoicePhone, getMyIcons().getIcon( getMyIcons().getPluginIcon( ePluginTypeVoicePhone, ePluginAccess ) ), strAction );
	}

	if( !isMyself && m_SelectedFriend->isMyAccessAllowedFromHim( ePluginTypeTruthOrDare, m_InGroup ) )
	{
		ePluginAccess = poSelectedFriend->getMyAccessPermissionFromHim( ePluginTypeTruthOrDare, m_InGroup );
		strAction = GuiParams::describePluginAction( poSelectedFriend, ePluginTypeTruthOrDare, ePluginAccess );
		addMenuItem( (int)ePluginTypeTruthOrDare, getMyIcons().getIcon( getMyIcons().getPluginIcon( ePluginTypeTruthOrDare, ePluginAccess ) ), strAction );
	}

	if( m_SelectedFriend->isMyAccessAllowedFromHim( ePluginTypeCamServer, m_InGroup ) )
	{
		ePluginAccess = poSelectedFriend->getMyAccessPermissionFromHim( ePluginTypeCamServer, m_InGroup );
		strAction = GuiParams::describePluginAction( poSelectedFriend, ePluginTypeCamServer, ePluginAccess );
		addMenuItem( (int)ePluginTypeCamServer, getMyIcons().getIcon( getMyIcons().getPluginIcon( ePluginTypeCamServer, ePluginAccess ) ), strAction );
	}

	if( !isMyself && m_SelectedFriend->isMyAccessAllowedFromHim( ePluginTypePersonFileXfer, m_InGroup ) )
	{
		ePluginAccess = poSelectedFriend->getMyAccessPermissionFromHim( ePluginTypePersonFileXfer, m_InGroup );
		strAction = GuiParams::describePluginAction( poSelectedFriend, ePluginTypePersonFileXfer, ePluginAccess );
		addMenuItem( (int)ePluginTypePersonFileXfer, getMyIcons().getIcon( getMyIcons().getPluginIcon( ePluginTypePersonFileXfer, ePluginAccess ) ), strAction );
	}

	if( !isMyself && m_SelectedFriend->isMyAccessAllowedFromHim( ePluginTypeFileShareServer, m_InGroup ) )
	{
		ePluginAccess = poSelectedFriend->getMyAccessPermissionFromHim( ePluginTypeFileShareServer, m_InGroup );
		strAction = GuiParams::describePluginAction( poSelectedFriend, ePluginTypeFileShareServer, ePluginAccess );
		addMenuItem( (int)ePluginTypeFileShareServer, getMyIcons().getIcon( getMyIcons().getPluginIcon( ePluginTypeFileShareServer, ePluginAccess ) ), strAction );
	}

	addSetUnsetPreferredMenuItem( (int)eMaxPluginType + 2, m_SelectedFriend->getMyOnlineId() );
	addChangeFriendshipMenuItem( (int)eMaxPluginType + 3 );
	addUserDetailsMenuItem( (int)eMaxPluginType + 4, m_SelectedFriend );

#if defined(DEBUG)
	addMenuItem( (int)eMaxPluginType + 5, getMyIcons().getIcon( eMyIconDebug ), QObject::tr( "Delete User From Database" ) );
#endif // defined(DEBUG)
}

//============================================================================
//! user selected friend action
void AppletPopupMenu::onFriendActionSelected( int iMenuId )
{
	switch( iMenuId )
	{
	case ePluginTypeAboutMePageServer: 
		if( isMyAccessAllowed( m_SelectedFriend, ePluginTypeAboutMePageServer, m_InGroup ) )
		{
			AppletAboutMeClient* applet = dynamic_cast< AppletAboutMeClient* >( m_MyApp.launchApplet( eAppletAboutMeClient, getParentPageFrame() ) );
			if( applet )
			{
				applet->setIdentity( m_SelectedFriend );
			}
		}

		break;

	case ePluginTypeStoryboardServer:
		if( isMyAccessAllowed( m_SelectedFriend, ePluginTypeStoryboardServer, m_InGroup ) )
		{
			AppletStoryboardClient* applet = dynamic_cast< AppletStoryboardClient* >( m_MyApp.launchApplet( eAppletStoryboardClient, getParentPageFrame() ) );
			if( applet )
			{
				applet->setIdentity( m_SelectedFriend );
			}
		}

		break;

	case ePluginTypeCamServer:
		if( isMyAccessAllowed( m_SelectedFriend, ePluginTypeCamServer, m_InGroup ) )
		{
			AppletCamClient* applet = dynamic_cast<AppletCamClient*>(m_MyApp.launchApplet( eAppletCamClient, getParentPageFrame() ));
			if( applet )
			{
				applet->setupCamFeed( m_SelectedFriend );
			}
		}

		break;

	case ePluginTypeFileShareServer:
		if( isMyAccessAllowed( m_SelectedFriend, ePluginTypeFileShareServer, m_InGroup ) )
		{
			AppletFileShareClientView* applet = dynamic_cast<AppletFileShareClientView*>(m_MyApp.launchApplet( eAppletFileShareClientView, getParentPageFrame() ));
			if( applet )
			{
				applet->setIdentity( m_SelectedFriend );
			}
		}

		break;

	case ePluginTypeVideoPhone:
		if( isMyAccessAllowed( m_SelectedFriend, (EPluginType)iMenuId, m_InGroup ) )
		{
			m_MyApp.offerToFriendPluginSession( m_SelectedFriend, (EPluginType)iMenuId, m_InGroup, getParentPageFrame() );
		}

		break;

	case ePluginTypeVoicePhone:
		if( isMyAccessAllowed( m_SelectedFriend, (EPluginType)iMenuId, m_InGroup ) )
		{
			m_MyApp.offerToFriendPluginSession( m_SelectedFriend, (EPluginType)iMenuId, m_InGroup, getParentPageFrame() );
		}

		break;

	case ePluginTypeTruthOrDare:
		if( isMyAccessAllowed( m_SelectedFriend, (EPluginType)iMenuId, m_InGroup ) )
		{
			m_MyApp.offerToFriendPluginSession( m_SelectedFriend, (EPluginType)iMenuId, m_InGroup, getParentPageFrame() );
		}

		break;

	case ePluginTypeMessenger:
		if( isMyAccessAllowed( m_SelectedFriend, (EPluginType)iMenuId, m_InGroup ) )
		{
			m_MyApp.offerToFriendPluginSession( m_SelectedFriend, (EPluginType)iMenuId, m_InGroup, getParentPageFrame() );
		}

		break;

	case ePluginTypePersonFileXfer:
		if( isMyAccessAllowed( m_SelectedFriend, ePluginTypePersonFileXfer, m_InGroup ) )
		{
			m_MyApp.offerToFriendSendFile( m_SelectedFriend, m_InGroup, getParentPageFrame() );
		}

		break;

	case eMaxPluginType + 2: // make preferred
		m_MyApp.getFavoriteMgr().toggleIsFavorite( m_SelectedFriend->getMyOnlineId() );
		break;

	case eMaxPluginType + 1: // block/unblock
	case eMaxPluginType + 3: // change friendship
		launchChangeFriendship(m_SelectedFriend);
		break;

	case eMaxPluginType + 4: // user details
		launchUserDetails();
		break;

	case eMaxPluginType + 5: // debug only .. delete user
		m_MyApp.getUserMgr().deleteUser( m_SelectedFriend, getParentPageFrame() );
		break;

	default:
		LogMsg( LOG_ERROR, "%s Unknown Menu id", __func__ );
	}

	closeApplet();
}

//============================================================================
void AppletPopupMenu::showHostSessionMenu( GuiHostSession* hostSession )
{
	if( !hostSession )
	{
		LogMsg( LOG_ERROR, "AppletPopupMenu::%s null session", __func__ );
		return;
	}

	setMenuType( EPopupMenuType::ePopupMenuHostSession );
	m_HostSession = hostSession;

	setTitle( GuiParams::describeHostType( hostSession->getHostType() ) );

	addSetUnsetPreferredMenuItem( 0, hostSession->getOnlineId() );
	
	if( m_HostSession->getGuiUser() )
	{
		addChangeFriendshipMenuItem( 1 );
		addUserDetailsMenuItem( 2, m_HostSession->getGuiUser() );
	}
}

//============================================================================
void AppletPopupMenu::onHostSessionActionSelected( int iMenuId )
{
	switch( iMenuId )
	{
	case 0: 
		m_MyApp.getFavoriteMgr().toggleIsFavorite( m_HostSession->getOnlineId() );
		break;

	case 1: // change friendship
		launchChangeFriendship( m_HostSession->getGuiUser() );
		break;

	case 2: // user details
		launchUserDetails();
		break;

	default:
		LogMsg( LOG_ERROR, "Unknown AppletPopupMenu::%s value %d", __func__, iMenuId );
	}
}

//============================================================================
void AppletPopupMenu::showHostedListSessionMenu( GuiHostedListSession* hostSession )
{
	if( !hostSession )
	{
		LogMsg( LOG_ERROR, "AppletPopupMenu::%s null session", __func__ );
		return;
	}

	setMenuType( EPopupMenuType::ePopupMenuHostedListSession );
	m_HostedListSession = hostSession;

	setTitle( GuiParams::describeHostType( hostSession->getHostType() ) );

	addSetUnsetPreferredMenuItem( 0, hostSession->getHostOnlineId() );

	if( m_HostedListSession->getGuiUser() )
	{
		addChangeFriendshipMenuItem( 1 );
		addUserDetailsMenuItem( 2, m_HostedListSession->getGuiUser() );
	}
}

//============================================================================
void AppletPopupMenu::onHostListSessionMenu( int iMenuId )
{
	switch( iMenuId )
	{

	case 0: // set favorite
		m_MyApp.getFavoriteMgr().toggleIsFavorite( m_HostedListSession->getHostOnlineId() );
		break;

	case 1: // change friendship
		launchChangeFriendship( m_HostedListSession->getGuiUser() );
		break;

	case 2: // user details
		launchUserDetails();
		break;

	default:
		LogMsg( LOG_ERROR, "AppletPopupMenu::%s Unknown menu id %d", __func__, iMenuId );
	}
}

//============================================================================
void AppletPopupMenu::showGroupieListSessionMenu( GuiGroupieListSession* groupieSession, bool inGroup )
{
	if( !groupieSession )
	{
		LogMsg( LOG_ERROR, "AppletPopupMenu::%s null session", __func__ );
		return;
	}

	bool menuShown{ false };
	GuiGroupie* guiGroupie = groupieSession->getGroupie();
	if( guiGroupie )
	{
		VxGUID userOnlineId = guiGroupie->getUserOnlineId();
		if( userOnlineId.isVxGUIDValid() )
		{
			GuiUser* guiUser = m_MyApp.getUserMgr().getUser( userOnlineId );
			if( guiUser )
			{
				menuShown = true;
				showFriendMenu( guiUser, inGroup );
			}
		}
	}

	if( !menuShown )
	{
		LogMsg( LOG_ERROR, "AppletPopupMenu::%s user not found", __func__ );
		return;
	}
}

//============================================================================
void AppletPopupMenu::onGroupieSessionActionSelected( int iMenuId )
{
	LogMsg( LOG_ERROR, "Unknown AppletPopupMenu::%s value %d", __func__, iMenuId );
}

//============================================================================
void AppletPopupMenu::showPersonOfferMenu( GuiUser* poSelectedFriend )
{
	m_SelectedFriend = poSelectedFriend;
	setMenuType( EPopupMenuType::ePopupMenuOfferFriendship );
	setTitle( QObject::tr( "Offer Friendship" ) );
	addMenuItem( 0, getMyIcons().getIcon( eMyIconFriend ), QObject::tr( "Offer Friendship" ) );
	addMenuItem( 1, getMyIcons().getIcon( eMyIconFriend ), QObject::tr( "Offer Join Group" ) );

	addSetUnsetPreferredMenuItem( 2, poSelectedFriend->getMyOnlineId() );
}

//============================================================================
void AppletPopupMenu::onPersonActionSelected( int iMenuId )
{
	switch( iMenuId )
	{
	case 0: // friends listing
		m_MyApp.getAppletMgr().launchApplet( eAppletFriendList, getParentPageFrame() );
		break;

	case 1: // group listing
		m_MyApp.getAppletMgr().launchApplet( eAppletGroupListClient, getParentPageFrame() );
		break;

	case 2: // favorite
		if( m_SelectedFriend )
		{
			m_MyApp.getFavoriteMgr().toggleIsFavorite( m_SelectedFriend->getMyOnlineId() );
		}
		else
		{
			LogMsg( LOG_ERROR, "Unknown AppletPopupMenu::%s value %d null selected friend", __func__, iMenuId );
		}
		
		break;

	default:
		LogMsg( LOG_ERROR, "Unknown AppletPopupMenu::%s value %d", __func__, iMenuId );
	}
}

//============================================================================
void AppletPopupMenu::showUserSessionMenu( EApplet appletType, GuiUserSessionBase* userSession )
{
	if( !userSession )
	{
		LogMsg( LOG_ERROR, "AppletPopupMenu::%s null session", __func__ );
		return;
	}

	m_AppletType = appletType;
	m_UserSession = userSession;
	m_SelectedFriend = userSession->getUserIdent();

	showFriendMenu( m_SelectedFriend );
}

//============================================================================
void AppletPopupMenu::onUserSessionActionSelected( int iMenuId )
{
	switch( iMenuId )
	{
	case 0: // friends listing
		m_MyApp.getAppletMgr().launchApplet( eAppletFriendList, getParentPageFrame() );
		break;

	case 1: // group listing
		m_MyApp.getAppletMgr().launchApplet( eAppletGroupListClient, getParentPageFrame() );
		break;

	case 2: // favorite
		m_MyApp.getFavoriteMgr().toggleIsFavorite( m_SelectedFriend->getMyOnlineId() );
		break;

	default:
		LogMsg( LOG_ERROR, "Unknown AppletPopupMenu::%s value %d", __func__, iMenuId );
	}
}

//============================================================================
void AppletPopupMenu::showTitleBarUserMenu( void )
{
	showTitleBarAppMenu();
}

//============================================================================
void AppletPopupMenu::onTitleBarUserMenuSelected( int iMenuId )
{
	// should not be called. onTitleBarAppMenuSelected will be called instead
	LogMsg( LOG_ERROR, "AppletPopupMenu::%s value %d should not have been called", __func__, iMenuId );
}

//============================================================================
void AppletPopupMenu::showTitleBarAppMenu( void )
{
	setMenuType( EPopupMenuType::ePopupMenuTitleBarAppMenu );
	setTitle( QObject::tr( "Applet Menu" ) );
	addMenuItem( 1, getMyIcons().getIcon( GetAppletIcon( eAppletLibrary ) ), DescribeApplet( eAppletLibrary ) );
	addMenuItem( 2, getMyIcons().getIcon( GetAppletIcon( eAppletPersonalRecorder ) ), DescribeApplet(eAppletPersonalRecorder));

	if( isPluginEnabled( ePluginTypeAboutMePageServer ) )
	{
		addMenuItem( 3, getMyIcons().getIcon(  GetAppletIcon( eAppletEditAboutMe ) ), DescribeApplet( eAppletEditAboutMe ) );
	}

	if( isPluginEnabled( ePluginTypeStoryboardServer ) )
	{
		addMenuItem( 4, getMyIcons().getIcon(  GetAppletIcon( eAppletEditStoryboard ) ), DescribeApplet( eAppletEditStoryboard ) );
	}

	if( isPluginEnabled( ePluginTypeCamServer ) )
	{
		addMenuItem( 5, getMyIcons().getIcon(  GetAppletIcon( eAppletCamServerViewMine ) ), DescribeApplet( eAppletCamServerViewMine ) );
	}

	if( isPluginEnabled( ePluginTypeMessenger ) )
	{
		addMenuItem( 6, getMyIcons().getIcon( GetAppletIcon( eAppletMultiMessenger ) ), DescribeApplet( eAppletMultiMessenger ));
	}

	addMenuItem( 7, getMyIcons().getIcon( GetAppletIcon( eAppletGroupJoin ) ), DescribeApplet( eAppletGroupJoin ) );
	addMenuItem( 8, getMyIcons().getIcon( GetAppletIcon( eAppletChatRoomJoin ) ), DescribeApplet( eAppletChatRoomJoin ) );
	addMenuItem( 9, getMyIcons().getIcon( GetAppletIcon( eAppletRandomConnectJoin ) ), DescribeApplet( eAppletRandomConnectJoin ) );
	addMenuItem( 10, getMyIcons().getIcon( GetAppletIcon( eAppletDownloads ) ), DescribeApplet( eAppletDownloads ) );
	if( isPluginEnabled( ePluginTypeFileShareServer ) )
	{
		addMenuItem( 11, getMyIcons().getIcon(  GetAppletIcon( eAppletUploads ) ), DescribeApplet( eAppletUploads ) );
	}

	if( isPluginEnabled( ePluginTypeHostGroup ) )
	{
		addMenuItem( 12, getMyIcons().getIcon(  GetAppletIcon( eAppletGroupHostAdmin ) ), DescribeApplet( eAppletGroupHostAdmin ) );
	}

	if( isPluginEnabled( ePluginTypeHostChatRoom ) )
	{
		addMenuItem( 13, getMyIcons().getIcon(  GetAppletIcon( eAppletChatRoomHostAdmin ) ), DescribeApplet( eAppletChatRoomHostAdmin ) );
	}

	if( isPluginEnabled( ePluginTypeHostRandomConnect ) )
	{
		addMenuItem( 13, getMyIcons().getIcon(  GetAppletIcon( eAppletRandomConnectHostAdmin ) ), DescribeApplet( eAppletRandomConnectHostAdmin ) );
	}
}

//============================================================================
void AppletPopupMenu::onTitleBarAppMenuSelected( int iMenuId )
{
	switch( iMenuId )
	{
	case 1: // library
		m_MyApp.getAppletMgr().launchApplet( eAppletLibrary, getParentPageFrame() );
		break;

	case 2: // personal recorder
		m_MyApp.getAppletMgr().launchApplet( eAppletPersonalRecorder, getParentPageFrame() );
		break;

	case 3: // about me server
		m_MyApp.getAppletMgr().launchApplet( eAppletEditAboutMe, getParentPageFrame() );
		break;

	case 4: // storyboard server
		m_MyApp.getAppletMgr().launchApplet( eAppletEditStoryboard, getParentPageFrame() );
		break;

	case 5: // cam server
		m_MyApp.getAppletMgr().launchApplet( eAppletCamServerViewMine, getParentPageFrame() );
		break;

	case 6: // messenger
		m_MyApp.getAppletMgr().launchApplet( eAppletMultiMessenger, getParentPageFrame() );
		break;

	case 7: // group host
		m_MyApp.getAppletMgr().launchApplet( eAppletGroupJoin, getParentPageFrame() );
		break;

	case 8: // chat room
		m_MyApp.getAppletMgr().launchApplet( eAppletChatRoomJoin,  getParentPageFrame() );
		break;

	case 9: // random connect
		m_MyApp.getAppletMgr().launchApplet( eAppletRandomConnectJoin, getParentPageFrame() );
		break;

	case 10: // downloads
		m_MyApp.getAppletMgr().launchApplet( eAppletDownloads, getParentPageFrame() );
		break;

	case 11: // uploads
		m_MyApp.getAppletMgr().launchApplet( eAppletUploads, getParentPageFrame() );
		break;

	case 12: // group admin
		m_MyApp.getAppletMgr().launchApplet( eAppletGroupHostAdmin, getParentPageFrame() );
		break;

	case 13: // chat room admin
		m_MyApp.getAppletMgr().launchApplet( eAppletChatRoomHostAdmin, getParentPageFrame() );
		break;

	case 14: // random connect admin
		m_MyApp.getAppletMgr().launchApplet( eAppletRandomConnectHostAdmin, getParentPageFrame() );
		break;

	default:
		LogMsg( LOG_ERROR, "Unknown AppletPopupMenu::%s value %d", __func__, iMenuId );
	}
}

//============================================================================
void AppletPopupMenu::showDeleteDbMenu( void )
{
	setMenuType( EPopupMenuType::ePopupMenuDeleteDb);
	setTitle( QObject::tr( "Delete A Database" ) );
	for( int dbType = eDatabaseTypeNone; dbType < eMaxDatabaseType; dbType++ )
	{
        addMenuItem( dbType, getMyIcons().getIcon( eMyIconShredderNormal ), GuiParams::describeDatabaseType( (EDatabaseType)dbType ) );
	}
}

//============================================================================
void AppletPopupMenu::onDeleteDbSelected( int menuId )
{
    EDatabaseType databaseType = (EDatabaseType)menuId;
	bool wasDeleted{ false };

	ActivityMessageBox errMsgBox( m_MyApp, this, LOG_INFO, "Are you sure you want to delete the database?" );
	errMsgBox.showCancelButton( true );
    errMsgBox.exec();

	if( !errMsgBox.wasOkButtonClicked() )
	{
		return;
	}

	switch( menuId )
	{
    case 0: // database type none
        return;

	default:
        wasDeleted = m_MyApp.getEngine().fromGuiDeleteDatabase( databaseType );
	}

    if( wasDeleted )
    {
        okMessageBox( QObject::tr( "Database Deleted" ), QObject::tr( "Database ") + GuiParams::describeDatabaseType( databaseType ) + QObject::tr( " was deleted") );
    }
    else
    {
        okMessageBox( QObject::tr( "Database Delete Failed" ), QObject::tr( "Database ") + GuiParams::describeDatabaseType( databaseType ) + QObject::tr( " could not be deleted") );
    }
}

//============================================================================
void AppletPopupMenu::addSetUnsetPreferredMenuItem( int menuId, VxGUID onlineId )
{
	if( m_MyApp.getFavoriteMgr().getIsFavorite( onlineId ) )
	{
		addMenuItem( menuId, getMyIcons().getIcon( eMyIconApp ), QObject::tr( "Unset Preferred" ) );
	}
	else
	{
		addMenuItem( menuId, getMyIcons().getIcon( eMyIconApp ), QObject::tr( "Set Preferred" ) );
	}
}

//============================================================================
void AppletPopupMenu::addChangeFriendshipMenuItem( int menuId )
{
	addMenuItem( menuId, getMyIcons().getIcon( eMyIconFriend ), QObject::tr( "Change Friendship" ) );
}

//============================================================================
void AppletPopupMenu::launchChangeFriendship( GuiUser* selectedFriend )
{
	if( !selectedFriend )
	{
		LogMsg( LOG_ERROR, "AppletPopupMenu::%s null selectedFriend", __func__ );
		vx_assert( false );
		return;
	}

	AppletPeerChangeFriendship * applet = dynamic_cast<AppletPeerChangeFriendship*>(m_MyApp.launchApplet( eAppletPeerChangeFriendship, getParentPageFrame() ));
	if( applet )
	{
		applet->setFriend( selectedFriend );
	}
}

//============================================================================
void AppletPopupMenu::addUserDetailsMenuItem( int menuId, GuiUser* guiUser )
{
	m_SelectedUserDetails = guiUser;
	addMenuItem( menuId, getMyIcons().getIcon( eMyIconAnonymous ), QObject::tr( "User Details" ) );
}

//============================================================================
void AppletPopupMenu::launchUserDetails( void )
{
	if( !m_SelectedUserDetails )
	{
		LogMsg( LOG_ERROR, "AppletPopupMenu::%s null m_SelectedUserDetails", __func__ );
		vx_assert( false );
		return;
	}

	AppletAboutUser * applet = dynamic_cast<AppletAboutUser*>(m_MyApp.launchApplet( eAppletAboutUser, getParentPageFrame() ));
	if( applet )
	{
		applet->setUser( m_SelectedUserDetails );
	}
}

//============================================================================
bool AppletPopupMenu::isMyAccessAllowed( GuiUser* guiUser, EPluginType pluginType, bool inGroup )
{
	if( !guiUser )
	{
		LogMsg( LOG_ERROR, "AppletPopupMenu::%s null guiUser", __func__ );
		return false;
	}

	if( guiUser->isMyAccessAllowedFromHim( pluginType, m_InGroup ) )
	{
		return true;
	}
	else
	{
		QString warnTitle = QObject::tr( "Insufficient Permission Level" );
		QString warmPermission =  QObject::tr( " Requires " ) + GuiParams::describePermissionLevel( guiUser->getPluginPermission( pluginType ) ) + 
			QObject::tr( " To Access Plugin " ) + GuiParams::describePluginType( pluginType );
		QMessageBox::warning( this, warnTitle, warmPermission );
	}

	return false;
}