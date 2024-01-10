#pragma once
//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ui_AppletUserConnections.h"

#include "AppletClientBase.h"
#include "GuiUserJoinCallback.h"
#include "GuiHostJoinCallback.h"

enum EUpdateType
{
    eUpdateTypeUserAny,
    eUpdateTypeClient,
    eUpdateTypeHost,
};

class AppletUserConnections : public AppletClientBase, public GuiUserJoinCallback, public GuiHostJoinCallback
{
	Q_OBJECT
public:
    AppletUserConnections( AppCommon& app, QWidget* parent );
	virtual ~AppletUserConnections() override;

    EUserViewType               getSelectedUserViewType( void );

protected slots:
    void                        slotUserViewTypeSelectionChange( int comboIdx );

protected:
    void                        showEvent( QShowEvent* ev ) override;

    //! GuiUserUpdateCallback
    void				        callbackIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp ) override;
    void				        callbackIndentListRemove( EUserViewType listType, VxGUID& onlineId ) override;
    void				        callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline ) override;

    void				        callbackUserAdded( GuiUser* guiUser ) override;
    void				        callbackUserUpdated( GuiUser* guiUser ) override;
    void				        callbackUserRemoved( VxGUID& onlineId ) override;
    void				        callbackMyIdentUpdated( GuiUser* guiUser ) override;

    //! GuiUserJoinCallback
    void				        callbackGuiUserJoinRequested( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;
    void				        callbackGuiUserJoinWasGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;
    void				        callbackGuiUserJoinIsGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;
    void				        callbackGuiUserUnJoinGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;
    void				        callbackGuiUserJoinDenied( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) override;
    void				        callbackGuiUserJoinLeaveHost( GroupieId& groupieId ) override;
    void				        callbackGuiUserJoinRemoved( GroupieId& groupieId ) override;

    //! GuiHostJoinCallback
    void				        callbackJoinRequestCount( int requestCnt ) override;
    void				        callbackGuiHostJoinRequested( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) override;
    void				        callbackGuiHostJoinWasGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) override;
    void				        callbackGuiHostJoinIsGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) override;
    void				        callbackGuiHostUnJoinGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) override;
    void				        callbackGuiHostJoinDenied( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) override;
    void				        callbackGuiHostJoinLeaveHost( GroupieId& groupieId ) override;
    void				        callbackGuiHostUnJoin( GroupieId& groupieId ) override;
    void				        callbackGuiHostJoinRemoved( GroupieId& groupieId ) override;

    void                        refreshList( void );

    //=== vars ===//
    Ui::AppletUserConnectionsUi	ui;
};


