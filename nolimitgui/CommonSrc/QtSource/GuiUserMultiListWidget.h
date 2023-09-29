#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ui_GuiUserMultiListWidget.h"

#include "AppletPeerBase.h"
#include "GuiOfferCallback.h"
#include "MultiSessionState.h"
#include "TodGameLogic.h"

class EngineSettings;
class GuiHosted;
class GuiHostSession;
class GuiOfferSession;
class MultiSessionState;
class P2PEngine;

class GuiUserMultiListWidget : public QWidget
{
	Q_OBJECT
public:
	GuiUserMultiListWidget( QWidget* parent = nullptr );
	virtual ~GuiUserMultiListWidget() = default;

	void                        setUserViewType( EUserViewType viewType );
	EUserViewType               getUserViewType( void )						{ return ui.m_UserListWidget->getUserViewType(); };

	void						setSelectedUser( GuiUser* guiUser );

	void						userJoinedHost( GuiHosted* guiHosted );

	void						hideEyeSession( void );

	GuiUserListWidget*			getUserListWidget( void );

signals:
	void						signalUserSelected( GuiUser* guiUser );
	void						signalSetSessionVisible( bool makeVisible );

protected slots:
    void						slotEyeHostButtonClicked( void );
    void						slotEyeSessionButtonClicked( void );
	void						slotEyeSearchButtonClicked( void );

    void						slotFriendsButtonClicked( void );
    void						slotGroupHostButtonClicked( void );
    void						slotChatRoomHostButtonClicked( void );
    void						slotRandomConnectHostButtonClicked( void );

	void						slotUserSelected( GuiUser* guiUser );
	void						slotSearchTextChanged( QString searchText );

protected:
	void						setStatusMsg( QString strStatus );

	void						onSelectedUserChanged( GuiUser* guiUser );

	//=== vars ===//
	Ui::GuiUserMultiListWidgetUi	ui;

	AppCommon&					m_MyApp;
    QFrame*						m_OffersFrame{ nullptr }; 
	GuiUser*					m_SelectedUser{ nullptr };
	bool						m_SessionVisible{ true };
};

