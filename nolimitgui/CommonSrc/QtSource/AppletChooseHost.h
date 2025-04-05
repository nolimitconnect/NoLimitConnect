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

#include "AppletBase.h"

enum EChooseUserReason
{
    eChooseUserReasonUnknown,

    eChooseUserReasonGroupHost,
    eChooseUserReasonChatRoomHost,
    eChooseUserReasonRandomConnectHost,

    eChooseUserReasonTest,

    eMaxChooseUserReason
};

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletChooseHostUi;
}
QT_END_NAMESPACE

class GuiUserSessionBase;
class GuiUserListItem;

class AppletChooseHost : public AppletBase
{

	Q_OBJECT
public:
    AppletChooseHost( AppCommon& app, QWidget* parent );
	virtual ~AppletChooseHost() override;

    void                        setChooseUserReason( EChooseUserReason chooseUserReason );
    EChooseUserReason           getChooseUserReason( void )                                 { return m_ChooseUserReason; }

    void                        addUser( VxGUID& onlineId );
    void                        updateUser( GuiUser* guiUser );

    void                        setChooseInstructionsText( QString instructionText );

signals:
    void                        signalUserChoosen( VxGUID onlineId );

protected slots:
    void                        slotUserSelected( GuiUserSessionBase* userSession, GuiUserListItem* userItem );

protected:
    void                        showEvent( QShowEvent* ev ) override;

    //=== vars ===//
    Ui::AppletChooseHostUi&	    ui;
    EChooseUserReason           m_ChooseUserReason{ eChooseUserReasonUnknown };
};


