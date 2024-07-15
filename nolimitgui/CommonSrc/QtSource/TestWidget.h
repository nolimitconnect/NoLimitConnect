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

#include <QFrame>

#include "AppDefs.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class TestWidgetUi;
}
QT_END_NAMESPACE

class AppCommon;

class TestWidget : public QFrame
{
	Q_OBJECT
public:
	TestWidget( QWidget* parent = nullptr );
	virtual ~TestWidget() {};

	void							setTestWidgetSize( EButtonSize buttonSize );

signals:
	void							signalAccepted( void );
	void							signalCanceled( void );

protected slots:
	void							slotAccepted( void );
	void							slotCanceled( void );

protected:
	AppCommon&						m_MyApp;
	Ui::TestWidgetUi&				ui;
};

