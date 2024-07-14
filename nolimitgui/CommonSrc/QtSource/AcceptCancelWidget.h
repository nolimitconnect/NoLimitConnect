#pragma once
//============================================================================
// Copyright (C) 2017 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppDefs.h"

#include <QFrame>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AcceptCancelWidgetClass;
}
QT_END_NAMESPACE

class AppCommon;

class AcceptCancelWidget : public QFrame
{
	Q_OBJECT
public:
	AcceptCancelWidget( QWidget* parent = nullptr );
	virtual ~AcceptCancelWidget() {};

	void							setAcceptCancelWidgetSize( EButtonSize buttonSize );

	void							showCancelButton( bool showButton );

signals:
	void							signalAccepted( void );
	void							signalCanceled( void );

protected slots:
	void							slotAccepted( void );
	void							slotCanceled( void );

protected:
	AppCommon&						m_MyApp;
	Ui::AcceptCancelWidgetClass&	ui;
};

