#pragma once
#include "ui_TestWidget.h"

#include <QFrame>

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
	Ui::TestWidgetUi				ui;
};

