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

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletInviteAcceptUi;
}
QT_END_NAMESPACE

class QPlainTextEdit;

class AppletInviteAccept : public AppletBase
{
    Q_OBJECT
public:
    AppletInviteAccept( AppCommon& app, QWidget* parent );
    virtual ~AppletInviteAccept();

protected slots:
    void                        slotPasteFromClipboard( QString clipboardText );
    void                        slotAcceptInviteButtonClicked( void );
    void                        slotRejectInviteButtonClicked( void );
    void                        slotInviteChanged( void );

protected:
    QPlainTextEdit *            getInviteEdit( void );

    Ui::AppletInviteAcceptUi&   ui;

};


