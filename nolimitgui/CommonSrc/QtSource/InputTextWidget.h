#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "InputBaseWidget.h"

#include "ui_InputTextWidget.h"

class InputTextWidget : public InputBaseWidget
{
    Q_OBJECT

public:
    InputTextWidget( QWidget* parent=0);

	void						setCanSend( bool canSend ) {};

private slots:
    void						slotCancelButtonClicked( void );
	void						slotSendButtonClicked( void );

private:
	Ui::InputTextWidgetClass	ui;
};
