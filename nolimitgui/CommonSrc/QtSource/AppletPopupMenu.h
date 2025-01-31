#pragma once
//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"
#include "AppDefs.h"

#include <CoreLib/MediaCallbackInterface.h>

enum class EPopupMenuType
{
	ePopupMenuUnknown,
	ePopupMenuTitleBarAppMenu,
	ePopupMenuTitleBarUserMenu,
	ePopupMenuFriend,
	ePopupMenuHostSession,
	ePopupMenuGroupieListSession,
	ePopupMenuHostedListSession,
	ePopupMenuOfferFriendship,
	ePopupMenuUserSession,
	ePopupMenuDeleteDb,

	eMaxPopupMenu
};

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletPopupMenuUi;
}
QT_END_NAMESPACE

class GuiHostSession;
class GuiHostedListSession;
class GuiGroupieListSession;
class GuiUserSessionBase;
class QListWidgetItem;

class AppletPopupMenu : public AppletBase, public MediaCallbackInterface
{
	Q_OBJECT
public:
	AppletPopupMenu( AppCommon& app, QWidget* parent = nullptr, VxGUID assetId = VxGUID::nullVxGUID() );
	virtual ~AppletPopupMenu() override;

	void						setTitle( QString strTitle );
	void						setMenuType( EPopupMenuType menuType );
	EPopupMenuType				getMenuType( void )									{ return m_MenuType; };
    void						setAppletType( EApplet appletType ) override		{ m_AppletType = appletType; };
    EApplet						getAppletType( void ) override						{ return m_AppletType; };

	void						clearMenulList( void );
	void						addMenuItem( int iItemId, QIcon& oIcon, QString strMenuItemText );

	void						showTitleBarAppMenu( void );
	void						showTitleBarUserMenu( void );

	void						showFriendMenu( GuiUser* poSelectedFriend, bool inGroup = false );
	void						showPersonOfferMenu( GuiUser* poSelectedFriend );
	void						showUserSessionMenu( EApplet appletType, GuiUserSessionBase* userSession );

	void						showGroupieListSessionMenu( GuiGroupieListSession* hostSession, bool inGroup = false );
	void						showHostedListSessionMenu( GuiHostedListSession* hostSession );
	void						showHostSessionMenu( GuiHostSession* hostSession );

	void						showDeleteDbMenu( void );

signals:
	void						menuItemClicked( int iItemId, AppletPopupMenu* popupMenu, AppletBase* );

public slots:
	void						onTitleBarAppMenuSelected( int iMenuId );
	void						onTitleBarUserMenuSelected( int iMenuId );
	void						onFriendActionSelected( int iMenuId );
	void						onGroupieSessionActionSelected( int iMenuId );
	void						onHostSessionActionSelected( int iMenuId );
	void						onHostListSessionMenu( int iMenuId );
	void						onPersonActionSelected( int iMenuId );
	void						onUserSessionActionSelected( int iMenuId );

	void						onDeleteDbSelected( int menuId );

private slots:
	//! user clicked the upper right x button
	void						slotBackButtonClicked( void ) override;
;
	//! user selected menu item
	void						itemClicked( QListWidgetItem* item );

protected:
	void						initAppletPopupMenu( void );

	void						addChangeFriendshipMenuItem( int menuId );
	void						addSetUnsetPreferredMenuItem( int menuId, VxGUID onlineId );
	void						addUserDetailsMenuItem( int menuId, GuiUser* guiUser );

	void						launchChangeFriendship( GuiUser* selectedFriend );
	void						launchUserDetails( void );

	bool						isMyAccessAllowed( GuiUser* guiUser, EPluginType pluginType, bool inGroup );

    //=== vars ===//
    Ui::AppletPopupMenuUi&		ui;
	AppletBase*					m_ParentActivity{ nullptr };
	int							m_iMenuItemHeight{ 48 };
	GuiHostSession*				m_HostSession{ nullptr };
	GuiGroupieListSession*		m_GroupieListSession{ nullptr };
	GuiHostedListSession*		m_HostedListSession{ nullptr };
	GuiUser*					m_SelectedFriend{ nullptr };
	GuiUserSessionBase*			m_UserSession{ nullptr };
	QFrame*						m_ContentItemsFrame{ nullptr };
	EPopupMenuType				m_MenuType{ EPopupMenuType::ePopupMenuUnknown };
	EApplet						m_AppletType{ eAppletUnknown };
	bool						m_InGroup{ false };
	GuiUser*					m_SelectedUserDetails{ nullptr };
};
