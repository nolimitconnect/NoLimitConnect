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

#include "ui_AppletChooseUser.h"

#include "AppletBase.h"

enum EChooseUserReason
{
    eChooseUserReasonUnknown,

    eChooseUserReasonGroupHost,
    eChooseUserReasonChatRoomHost,
    eChooseUserReasonRandomConnectHost,

    eMaxChooseUserReason
};

class AppletChooseUser : public AppletBase
{

	Q_OBJECT
public:
    AppletChooseUser( AppCommon& app, QWidget* parent );
	virtual ~AppletChooseUser() override;

    void                        setChooseUserReason( EChooseUserReason chooseUserReason );
    EChooseUserReason           getChooseUserReason( void )                                 { return m_ChooseUserReason; }

    void                        addUser( VxGUID& onlineId );

    void                        setChooseInstructionsText( QString instructionText );

protected slots:
    void                        slotUserSelected( GuiUserSessionBase* userSession, GuiUserListItem* userItem );

protected:
    void                        showEvent( QShowEvent* ev ) override;

    //=== vars ===//
    Ui::AppletChooseUserUi	    ui;
    EChooseUserReason           m_ChooseUserReason{ eChooseUserReasonUnknown };
};


