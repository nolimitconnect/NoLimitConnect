#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ui_GuiUserMultiListWidget.h"

#include "AppletPeerBase.h"
#include "GuiOfferCallback.h"
#include "GuiMemberActiveCallback.h"
#include "MultiSessionState.h"
#include "TodGameLogic.h"

class EngineSettings;
class GuiHosted;
class GuiHostSession;
class GuiMemberActiveMgr;
class GuiOfferSession;
class MultiSessionState;
class P2PEngine;

class GuiUserMultiListWidget : public QWidget, public GuiMemberActiveCallback
{
	Q_OBJECT
public:
	GuiUserMultiListWidget( QWidget* parent = nullptr );
	~GuiUserMultiListWidget() override;

	void						setHostViewType( EHostType hostType );
	void                        setUserViewType( EUserViewType viewType );
	EUserViewType               getUserViewType( void )						{ return ui.m_UserListWidget->getUserViewType(); };

	void						setHostAdminId( GroupieId& adminId );
    GroupieId&                  getHostAdminId( void )                      { return m_HostAdminId; };
	bool						hasHostAdmin( void )						{ return m_HostAdminId.isValid(); };
	void						clearHostAdmin( void )						{ m_HostAdminId.clear(); };

	void                        setAllowMyselfInList( bool allowMyself );
	bool                        getAllowMyselfInList( void );

	void						setSelectedUser( GuiUser* guiUser );

	void						userJoinedHost( GuiHosted* guiHosted );

	void						hideEyeSession( void );

	GuiUserListWidget*			getUserListWidget( void );

signals:
	void						signalUserSelected( GuiUser* guiUser );
	void						signalSetSessionVisible( bool makeVisible );
	void						signalViewChanged( EUserViewType viewType );
	void						signalHostedMembersView( GroupieId& hostAdminId );

protected slots:
    void						slotEyeHostButtonClicked( void );
    void						slotEyeSessionButtonClicked( void );
	void						slotEyeSearchButtonClicked( void );

	void						slotEverybodyButtonClicked( void );
    void						slotFriendsButtonClicked( void );
    void						slotGroupHostButtonClicked( void );
    void						slotChatRoomHostButtonClicked( void );
    void						slotRandomConnectHostButtonClicked( void );

	void						slotUserSelected( GuiUser* guiUser );
	void						slotSearchTextChanged( QString searchText );

protected:
	void						setStatusMsg( QString strStatus );

	void						onSelectedUserChanged( GuiUser* guiUser );

	void						updateSelectionByViewType( EUserViewType viewType );

	void						updateUsersByViewType( EUserViewType viewType );

	void						callbackGuiMemberIsJoinedToHost( VxGUID& onlineId, EHostType hostType, bool isJoined ) override;

	//=== vars ===//
	Ui::GuiUserMultiListWidgetUi	ui;

	AppCommon&					m_MyApp;
	GuiMemberActiveMgr&			m_MemberActiveMgr;

    QFrame*						m_OffersFrame{ nullptr }; 
	GuiUser*					m_SelectedUser{ nullptr };
	bool						m_SessionVisible{ true };
	GroupieId					m_HostAdminId;
	EUserViewType				m_UserViewType{ eUserViewTypeNone };
};

