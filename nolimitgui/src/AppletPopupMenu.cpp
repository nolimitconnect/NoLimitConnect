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
#include "AppGlobals.h"
#include "AppSettings.h"

#include "AppletAboutMeClient.h"
#include "AppletAboutUser.h"
#include "AppletCamClient.h"
#include "AppletFileOfferSelect.h"
#include "AppletFileShareClientView.h"
#include "AppletFriendRequest.h"
#include "AppletMgr.h"
#include "AppletPeerChangeFriendship.h"
#include "AppletPeerTodGame.h"
#include "AppletPeerVideoPhone.h"
#include "AppletPeerVoicePhone.h"
#include "AppletStoryboardClient.h"

#include "FileItemInfo.h"
#include "FileActionMenu.h"
#include "FileShareItemWidget.h"
#include "GuiFavoriteMgr.h"
#include "GuiHelpers.h"
#include "GuiMemberActiveMgr.h"
#include "GuiParams.h"
#include "GuiGroupieListSession.h"
#include "GuiHostSession.h"
#include "GuiHostedListSession.h"
#include "BottomBarWidget.h"
#include "MyIcons.h"
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
void AppletPopupMenu::addMenuItem( int iItemId, QIcon oIcon, QString strMenuItemText )
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
void AppletPopupMenu::showFriendMenu( GuiUser* poSelectedFriend )
{
	setMenuType( EPopupMenuType::ePopupMenuFriend );
	m_SelectedFriend = poSelectedFriend;
	bool isMyself = poSelectedFriend->isMyself();
	// populate title
	QString strTitle = poSelectedFriend->describeMyFriendshipToHim();
	if( isMyself )
	{
		strTitle = GuiParams::describeFriendship( eFriendStateAdmin );
	}

	strTitle += ": ";
	strTitle += poSelectedFriend->getOnlineName().c_str();
	setTitle( strTitle );

	for( int i = (int)(eUserActionNone)+1; i < (int)eMaxFriendAction; ++i )
	{
		addUserAction( (EUserAction)i );
	}
}

//============================================================================
void AppletPopupMenu::addUserAction( enum EUserAction userAction )
{
    if( userAction == eUserActionBlockUnblock )
	{
		if( m_SelectedFriend->isIgnored() )
		{
			addMenuItem( (int)eUserActionBlockUnblock, getMenuIcon( eMyIconFriend ), QObject::tr( "Unblock User" ) );
			return;
		}
	}

	EPluginAccess pluginAccess{ ePluginAccessIgnored };
	if( !canPerformAction( userAction, pluginAccess ) )
	{
		return;
	}

	bool isMyself = m_SelectedFriend->isMyself();	
	QString strAction;
	if( userAction ==  eUserActionBlockUnblock )
	{
		if( !isMyself )
		{
			addMenuItem( (int)eUserActionBlockUnblock, getMenuIcon( eMyIconIgnored ), QObject::tr( "Block User" ) );
		}
	}
	else if( userAction ==  eUserActionAboutMe )
	{
		strAction = GuiParams::describePluginAction( m_SelectedFriend, ePluginTypeAboutMePageServer, pluginAccess );
		addMenuItem( (int)eUserActionAboutMe, getMenuIcon( getMyIcons().getPluginIcon( ePluginTypeAboutMePageServer, pluginAccess ) ), strAction );
	}
	else if( userAction ==  eUserActionStoryboard )
	{
		strAction = GuiParams::describePluginAction( m_SelectedFriend, ePluginTypeStoryboardServer, pluginAccess );
		addMenuItem( (int)eUserActionStoryboard, getMenuIcon( getMyIcons().getPluginIcon( ePluginTypeStoryboardServer, pluginAccess ) ), strAction );
	}
	else if( userAction ==  eUserActionCamServer )
	{
		strAction = GuiParams::describePluginAction( m_SelectedFriend, ePluginTypeCamServer, pluginAccess );
		addMenuItem( (int)eUserActionCamServer, getMenuIcon( getMyIcons().getPluginIcon( ePluginTypeCamServer, pluginAccess ) ), strAction );
	}
	else if( userAction ==  eUserActionMessenger )
	{
		strAction = GuiParams::describePluginAction( m_SelectedFriend, ePluginTypeMessenger, pluginAccess );
		addMenuItem( (int)eUserActionMessenger, getMenuIcon( getMyIcons().getPluginIcon( ePluginTypeMessenger, pluginAccess ) ), strAction );
	}
	else if( userAction ==  eUserActionVoicePhone )
	{
		strAction = GuiParams::describePluginAction( m_SelectedFriend, ePluginTypeVoicePhone, pluginAccess );
		addMenuItem( (int)eUserActionVoicePhone, getMenuIcon( getMyIcons().getPluginIcon( ePluginTypeVoicePhone, pluginAccess ) ), strAction );
	}
	else if( userAction ==  eUserActionVideoPhone )
	{
		strAction = GuiParams::describePluginAction( m_SelectedFriend, ePluginTypeVideoPhone, pluginAccess );
		addMenuItem( (int)eUserActionVideoPhone, getMenuIcon( getMyIcons().getPluginIcon( ePluginTypeVideoPhone, pluginAccess ) ), strAction );
	}
	else if( userAction ==  eUserActionTruthOrDare )
	{
		strAction = GuiParams::describePluginAction( m_SelectedFriend, ePluginTypeTruthOrDare, pluginAccess );
		addMenuItem( (int)eUserActionTruthOrDare, getMenuIcon( getMyIcons().getPluginIcon( ePluginTypeTruthOrDare, pluginAccess ) ), strAction );
	}
	else if( userAction ==  eUserActionViewSharedFiles )
	{
		strAction = GuiParams::describePluginAction( m_SelectedFriend, ePluginTypeFileShareServer, pluginAccess );
		addMenuItem( (int)eUserActionViewSharedFiles, getMenuIcon( getMyIcons().getPluginIcon( ePluginTypeFileShareServer, pluginAccess ) ), strAction );
	}
	else if( userAction ==  eUserActionOfferFile )
	{
		strAction = GuiParams::describePluginAction( m_SelectedFriend, ePluginTypePersonFileXfer, pluginAccess );
		addMenuItem( (int)eUserActionOfferFile, getMenuIcon( getMyIcons().getPluginIcon( ePluginTypePersonFileXfer, pluginAccess ) ), strAction );
	}
	else if( userAction ==  eUserActionSetUnsetPreferred )
	{
		addSetUnsetPreferredMenuItem( (int)eUserActionSetUnsetPreferred, m_SelectedFriend->getMyOnlineId() );
	}
	else if( userAction ==  eUserActionChangeFriendship )
	{
		addChangeFriendshipMenuItem( (int)eUserActionChangeFriendship );
	}
	else if( userAction ==  eUserActionRequestFriendship )
	{
		addFriendRequestMenuItem( (int)eUserActionRequestFriendship, m_SelectedFriend );
	}
	else if( userAction == eUserActionUserDetails )
	{
		addUserDetailsMenuItem( (int)eUserActionUserDetails, m_SelectedFriend );
	}
	else if( userAction == eUserActionDeleteUserFromDb )
	{
		addDeleteUserFromDbMenuItem( (int)eUserActionDeleteUserFromDb, m_SelectedFriend );
	}
	else
	{
		LogMsg( LOG_ERROR, "AppletPopupMenu::%s unknown action %d", __func__, userAction );
		vx_assert( false );
	}
}

//============================================================================
bool AppletPopupMenu::canPerformAction( enum EUserAction userAction, EPluginAccess& pluginAccess )
{
	pluginAccess = ePluginAccessIgnored;
	bool isMyself = m_SelectedFriend->isMyself();	
	if( isMyself )
	{
		if( userAction == eUserActionBlockUnblock )
		{
			return false;
		}
	}

    if( userAction == eUserActionBlockUnblock )
    {
        pluginAccess = ePluginAccessOk;
    }
    else if( userAction == eUserActionAboutMe )
	{
		if( m_SelectedFriend->hasAboutMeContent() )
		{
			pluginAccess = m_SelectedFriend->getMyAccessPermissionFromHim( ePluginTypeAboutMePageServer );
		}	
	}
    else if( userAction == eUserActionStoryboard )
	{
		if( m_SelectedFriend->hasStoryboardContent() )
		{
			pluginAccess = m_SelectedFriend->getMyAccessPermissionFromHim( ePluginTypeStoryboardServer );
		}
	}
    else if( userAction == eUserActionCamServer )
	{
		if( m_SelectedFriend->hasSharedWebCam() )
		{
			pluginAccess = m_SelectedFriend->getMyAccessPermissionFromHim( ePluginTypeCamServer );
		}
	}
    else if( userAction == eUserActionViewSharedFiles )
	{
		pluginAccess = m_SelectedFriend->getMyAccessPermissionFromHim( ePluginTypeFileShareServer );
	}
    else if( userAction == eUserActionOfferFile )
	{
		pluginAccess = m_SelectedFriend->getMyAccessPermissionFromHim( ePluginTypePersonFileXfer );
	}
    else if( userAction == eUserActionMessenger )
	{
		pluginAccess = m_SelectedFriend->getMyAccessPermissionFromHim( ePluginTypeMessenger);
	}
    else if( userAction == eUserActionVideoPhone )
	{
		pluginAccess = m_SelectedFriend->getMyAccessPermissionFromHim( ePluginTypeVideoPhone );
		if( pluginAccess == ePluginAccessOk && m_MyApp.getOfferMgr().haveActiveOffer( m_SelectedFriend->getMyOnlineId(), ePluginTypeVideoPhone ) )
		{
			pluginAccess = ePluginAccessBusy;
		}
	}
    else if( userAction == eUserActionVoicePhone )
	{
		pluginAccess = m_SelectedFriend->getMyAccessPermissionFromHim( ePluginTypeVoicePhone );
		if( pluginAccess == ePluginAccessOk && m_MyApp.getOfferMgr().haveActiveOffer( m_SelectedFriend->getMyOnlineId(), ePluginTypeVoicePhone ) )
		{
			pluginAccess = ePluginAccessBusy;
		}
	}
    else if( userAction == eUserActionTruthOrDare )
	{
		pluginAccess = m_SelectedFriend->getMyAccessPermissionFromHim( ePluginTypeTruthOrDare );
		if( pluginAccess == ePluginAccessOk && m_MyApp.getOfferMgr().haveActiveOffer( m_SelectedFriend->getMyOnlineId(), ePluginTypeTruthOrDare ) )
		{
			pluginAccess = ePluginAccessBusy;
		}
	}
    else if( userAction == eUserActionSetUnsetPreferred )
	{
		pluginAccess = ePluginAccessOk;
	}
    else if( userAction == eUserActionChangeFriendship )
	{
		pluginAccess = ePluginAccessOk;
	}
    else if( userAction == eUserActionRequestFriendship )
	{
		pluginAccess = ePluginAccessOk;
	}
	else if( userAction == eUserActionUserDetails )
	{
		pluginAccess = ePluginAccessOk;
	}
	else if( userAction == eUserActionDeleteUserFromDb )
	{
		if( VxGetCanDeleteUserFromDb() )
		{
			pluginAccess = ePluginAccessOk;
		}
	}
	else
	{
		LogMsg( LOG_ERROR, "%s Unknown user action %d", __func__, userAction );
		vx_assert( false );
		return false;
	}

	return pluginAccess == ePluginAccessOk;
}

//============================================================================
//! user selected friend action
void AppletPopupMenu::onFriendActionSelected( int iMenuId )
{
	switch( iMenuId )
	{
	case eUserActionAboutMe: 
		{
			AppletAboutMeClient* applet = dynamic_cast< AppletAboutMeClient* >( m_MyApp.launchApplet( eAppletAboutMeClient, getParentPageFrame() ) );
			if( applet )
			{
				applet->setIdentity( m_SelectedFriend );
			}
		}

		break;

	case eUserActionStoryboard:
		{
			AppletStoryboardClient* applet = dynamic_cast< AppletStoryboardClient* >( m_MyApp.launchApplet( eAppletStoryboardClient, getParentPageFrame() ) );
			if( applet )
			{
				applet->setIdentity( m_SelectedFriend );
			}
		}

		break;

	case eUserActionCamServer:
		{
			AppletCamClient* applet = dynamic_cast<AppletCamClient*>(m_MyApp.launchApplet( eAppletCamClient, getParentPageFrame() ));
			if( applet )
			{
				applet->setupCamFeed( m_SelectedFriend );
			}
		}

		break;

	case eUserActionViewSharedFiles:
		{
			AppletFileShareClientView* applet = dynamic_cast<AppletFileShareClientView*>(m_MyApp.launchApplet( eAppletFileShareClientView, getParentPageFrame() ));
			if( applet )
			{
				applet->setIdentity( m_SelectedFriend );
			}
		}

		break;
	
	case eUserActionOfferFile:
		{
			m_MyApp.offerToFriendSendFile( m_SelectedFriend, getParentPageFrame() );
		}

		break;

	case eUserActionMessenger:
		{
			m_MyApp.offerToFriendPluginSession( m_SelectedFriend, ePluginTypeMessenger, getParentPageFrame() );
		}

		break;

	case eUserActionVoicePhone:
		{
			m_MyApp.offerToFriendPluginSession( m_SelectedFriend, ePluginTypeVoicePhone, getParentPageFrame() );
		}

		break;

	case eUserActionVideoPhone:
		{
			m_MyApp.offerToFriendPluginSession( m_SelectedFriend, ePluginTypeVideoPhone, getParentPageFrame() );
		}

		break;

	case eUserActionTruthOrDare:
		{
			m_MyApp.offerToFriendPluginSession( m_SelectedFriend, ePluginTypeTruthOrDare, getParentPageFrame() );
		}

		break;

	case eUserActionSetUnsetPreferred: // make preferred
		m_MyApp.getFavoriteMgr().toggleIsFavorite( m_SelectedFriend->getMyOnlineId() );
		break;

	case eUserActionBlockUnblock: // block/unblock
	case eUserActionChangeFriendship: // change friendship
		launchChangeFriendship( m_SelectedFriend );
		break;

	case eUserActionRequestFriendship: // send friend request
		launchSendFriendRequest( m_SelectedFriend );
		break;

	case eUserActionUserDetails: // user details
		launchUserDetails();
		break;

	case eUserActionDeleteUserFromDb: // debug only .. delete user
		m_MyApp.getUserMgr().deleteUser( m_SelectedFriend, getParentPageFrame() );
		break;

	default:
		LogMsg( LOG_ERROR, "%s Unknown Menu id %d", __func__, iMenuId );
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
void AppletPopupMenu::showGroupieListSessionMenu( GuiGroupieListSession* groupieSession )
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
				showFriendMenu( guiUser );
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
	addMenuItem( 0, getMenuIcon( eMyIconFriend ), QObject::tr( "Offer Friendship" ) );
	addMenuItem( 1, getMenuIcon( eMyIconFriend ), QObject::tr( "Offer Join Group" ) );

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
	addMenuItem( 1, getMenuIcon(GetAppletIcon(eAppletAboutNoLimitConnect)), DescribeApplet(eAppletAboutNoLimitConnect));
	addMenuItem( 2, getMenuIcon( GetAppletIcon( eAppletLibrary ) ), DescribeApplet( eAppletLibrary ) );
	addMenuItem( 3, getMenuIcon( GetAppletIcon( eAppletPersonalRecorder ) ), DescribeApplet(eAppletPersonalRecorder));

	if( isPluginEnabled( ePluginTypeAboutMePageServer ) )
	{
		addMenuItem( 4, getMenuIcon(  GetAppletIcon( eAppletEditAboutMe ) ), DescribeApplet( eAppletEditAboutMe ) );
	}

	if( isPluginEnabled( ePluginTypeStoryboardServer ) )
	{
		addMenuItem( 5, getMenuIcon(  GetAppletIcon( eAppletEditStoryboard ) ), DescribeApplet( eAppletEditStoryboard ) );
	}

	if( isPluginEnabled( ePluginTypeCamServer ) )
	{
		addMenuItem( 6, getMenuIcon(  GetAppletIcon( eAppletCamServerViewMine ) ), DescribeApplet( eAppletCamServerViewMine ) );
	}

	if( isPluginEnabled( ePluginTypeMessenger ) )
	{
		addMenuItem( 7, getMenuIcon( GetAppletIcon( eAppletMultiMessenger ) ), DescribeApplet( eAppletMultiMessenger ));
	}

	addMenuItem( 8, getMenuIcon( GetAppletIcon( eAppletGroupJoin ) ), DescribeApplet( eAppletGroupJoin ) );
	addMenuItem( 9, getMenuIcon( GetAppletIcon( eAppletChatRoomJoin ) ), DescribeApplet( eAppletChatRoomJoin ) );
	addMenuItem( 10, getMenuIcon( GetAppletIcon( eAppletRandomConnectJoin ) ), DescribeApplet( eAppletRandomConnectJoin ) );
	addMenuItem( 11, getMenuIcon( GetAppletIcon( eAppletDownloads ) ), DescribeApplet( eAppletDownloads ) );
	if( isPluginEnabled( ePluginTypeFileShareServer ) )
	{
		addMenuItem( 12, getMenuIcon(  GetAppletIcon( eAppletUploads ) ), DescribeApplet( eAppletUploads ) );
	}

	if( isPluginEnabled( ePluginTypeHostGroup ) )
	{
		addMenuItem( 13, getMenuIcon(  GetAppletIcon( eAppletGroupHostAdmin ) ), DescribeApplet( eAppletGroupHostAdmin ) );
	}

	if( isPluginEnabled( ePluginTypeHostChatRoom ) )
	{
		addMenuItem( 14, getMenuIcon(  GetAppletIcon( eAppletChatRoomHostAdmin ) ), DescribeApplet( eAppletChatRoomHostAdmin ) );
	}

	if( isPluginEnabled( ePluginTypeHostRandomConnect ) )
	{
		addMenuItem( 15, getMenuIcon(  GetAppletIcon( eAppletRandomConnectHostAdmin ) ), DescribeApplet( eAppletRandomConnectHostAdmin ) );
	}
}

//============================================================================
void AppletPopupMenu::onTitleBarAppMenuSelected( int iMenuId )
{
	switch( iMenuId )
	{
	case 1: // about app
		m_MyApp.getAppletMgr().launchApplet( eAppletAboutNoLimitConnect, getParentPageFrame() );
		break;
	case 2: // library
		m_MyApp.getAppletMgr().launchApplet( eAppletLibrary, getParentPageFrame() );
		break;

	case 3: // personal recorder
		m_MyApp.getAppletMgr().launchApplet( eAppletPersonalRecorder, getParentPageFrame() );
		break;

	case 4: // about me server
		m_MyApp.getAppletMgr().launchApplet( eAppletEditAboutMe, getParentPageFrame() );
		break;

	case 5: // storyboard server
		m_MyApp.getAppletMgr().launchApplet( eAppletEditStoryboard, getParentPageFrame() );
		break;

	case 6: // cam server
		m_MyApp.getAppletMgr().launchApplet( eAppletCamServerViewMine, getParentPageFrame() );
		break;

	case 7: // messenger
		m_MyApp.getAppletMgr().launchApplet( eAppletMultiMessenger, getParentPageFrame() );
		break;

	case 8: // group host
		m_MyApp.getAppletMgr().launchApplet( eAppletGroupJoin, getParentPageFrame() );
		break;

	case 9: // chat room
		m_MyApp.getAppletMgr().launchApplet( eAppletChatRoomJoin,  getParentPageFrame() );
		break;

	case 10: // random connect
		m_MyApp.getAppletMgr().launchApplet( eAppletRandomConnectJoin, getParentPageFrame() );
		break;

	case 11: // downloads
		m_MyApp.getAppletMgr().launchApplet( eAppletDownloads, getParentPageFrame() );
		break;

	case 12: // uploads
		m_MyApp.getAppletMgr().launchApplet( eAppletUploads, getParentPageFrame() );
		break;

	case 13: // group admin
		m_MyApp.getAppletMgr().launchApplet( eAppletGroupHostAdmin, getParentPageFrame() );
		break;

	case 14: // chat room admin
		m_MyApp.getAppletMgr().launchApplet( eAppletChatRoomHostAdmin, getParentPageFrame() );
		break;

	case 15: // random connect admin
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
        addMenuItem( dbType, getMenuIcon( eMyIconShredderNormal ), GuiParams::describeDatabaseType( (EDatabaseType)dbType ) );
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

	if( !errMsgBox.wasOkButtonClicked())
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
		addMenuItem( menuId, getMenuIcon( eMyIconApp ), QObject::tr( "Unset Preferred" ) );
	}
	else
	{
		addMenuItem( menuId, getMenuIcon( eMyIconApp ), QObject::tr( "Set Preferred" ) );
	}
}

//============================================================================
void AppletPopupMenu::addChangeFriendshipMenuItem( int menuId )
{
	addMenuItem( menuId, getMenuIcon( eMyIconFriend ), QObject::tr( "Change Friendship" ) );
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
void AppletPopupMenu::addFriendRequestMenuItem( int menuId, GuiUser* guiUser )
{
	if( guiUser->getHisFriendshipToMe() < eFriendStateFriend ) // && eFriendStateIgnore != guiUser->getPluginPermission( ePluginTypeFriendRequest ) )
	{
		addMenuItem( menuId, getMenuIcon( eMyIconFriendJoined ), QObject::tr( "Friendship Request" ) );
	}
}

//============================================================================
void AppletPopupMenu::launchSendFriendRequest( GuiUser* selectedFriend )
{
	if( !selectedFriend )
	{
		LogMsg( LOG_ERROR, "AppletPopupMenu::%s null selectedFriend", __func__ );
		vx_assert( false );
		return;
	}

	if( selectedFriend->getMyFriendshipToHim() < eFriendStateFriend )
	{
		okMessageBox( QObject::tr( "Friendship Request" ), QObject::tr( "To send a frienship request you must set ") + selectedFriend->getOnlineName().c_str() + QObject::tr(" as friend or higher first"));
		return;
	}

	AppletFriendRequest * applet = dynamic_cast<AppletFriendRequest*>(m_MyApp.launchApplet( eAppletFriendRequest, getParentPageFrame() ));
	if( applet )
	{
		applet->friendRequestSetup( selectedFriend->getMyOnlineId(), false );
	}
}

//============================================================================
void AppletPopupMenu::addUserDetailsMenuItem( int menuId, GuiUser* guiUser )
{
	m_SelectedUserDetails = guiUser;
	addMenuItem( menuId, getMenuIcon( eMyIconAnonymous ), QObject::tr( "User Details" ) );
}

//============================================================================
void AppletPopupMenu::addDeleteUserFromDbMenuItem( int menuId, GuiUser* guiUser )
{
	addMenuItem( menuId, getMenuIcon( eMyIconAnonymous ), QObject::tr( "Delete User From Database" ) );
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
bool AppletPopupMenu::isMyAccessAllowed( GuiUser* guiUser, EPluginType pluginType )
{
	if( !guiUser )
	{
		LogMsg( LOG_ERROR, "AppletPopupMenu::%s null guiUser", __func__ );
		return false;
	}

	if( guiUser->isMyAccessAllowedFromHim( pluginType ) )
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

//============================================================================
QIcon AppletPopupMenu::getMenuIcon( EMyIcons iconType )
{
	if( iconType == eMyIconApp )
	{
		return getMyIcons().getIcon( iconType );
	}

	return getMyIcons().getIcon( iconType, GuiParams::getButtonSize( eButtonSizeSmall ), m_MyApp.getAppTheme().getColor( eButtonForegroundNormal ) );
}