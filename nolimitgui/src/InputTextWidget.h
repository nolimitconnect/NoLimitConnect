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

QT_BEGIN_NAMESPACE
namespace Ui {
    class InputTextWidgetClass;
}
QT_END_NAMESPACE

class InputTextWidget : public InputBaseWidget
{
    Q_OBJECT

public:
    InputTextWidget( QWidget* parent=0);
    void						setFocusOnText( void );

	void						setCanSend( bool canSend ) {};

protected:
    bool						eventFilter( QObject* watched, QEvent* event ) override;

private slots:
    void						slotCancelButtonClicked( void );
	void						slotSendButtonClicked( void );

private:
	Ui::InputTextWidgetClass&	ui;
};
