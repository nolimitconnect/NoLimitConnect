#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QDialog>
#include <QTimer>
#include "ui_ActivityTimedMessage.h"

class ActivityTimedMessage : public QDialog
{
	Q_OBJECT

public:
	ActivityTimedMessage( QString strTitle, QString strMessage, int iTimeoutSeconds = 4, QWidget* parent = nullptr );
	virtual ~ActivityTimedMessage() override = default;

    // overrides required for dialogs with there own title bar and bottom bar widgets
    //virtual TitleBarWidget *	getTitleBarWidget( void ) override { return ui.m_TitleBarWidget; }
    //virtual BottomBarWidget *	getBottomBarWidget( void ) override { return ui.m_BottomBarWidget; }

private:
	Ui::TimedMessageDlg			ui;
	QTimer						m_CloseDlgTimer;

};
