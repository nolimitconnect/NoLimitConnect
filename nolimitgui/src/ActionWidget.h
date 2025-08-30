#pragma once
//============================================================================
// Copyright (C) 2025 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget>

#include "AppDefs.h"
#include "MyIconsDefs.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class ActionWidgetUi;
}
QT_END_NAMESPACE

class AppCommon;
class VxPushButton;

class ActionWidget : public QWidget
{
	Q_OBJECT
public:
	ActionWidget( QWidget* parent = nullptr );
	virtual ~ActionWidget() {};

	void							setActionText( QString text );
	void							setActionIcon( EMyIcons icon );
	void							setActionButtonSize( EButtonSize buttonSize );

	VxPushButton*					getActionButton( void );

signals:
	void							clicked( void );

protected slots:
	void							slotActionClicked( void );

protected:
	Ui::ActionWidgetUi&				ui;
};

