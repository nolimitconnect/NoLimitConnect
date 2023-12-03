//============================================================================
// Copyright (C) 2017 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "TestWidget.h"

#include "AppCommon.h"
#include "VxAppTheme.h"
#include "GuiParams.h"
#include "VxAppTheme.h"

//============================================================================
TestWidget::TestWidget( QWidget* parent )
: QFrame( parent )
, m_MyApp(GetAppInstance())
{
	ui.setupUi( this );
	ui.m_AcceptButton->setIconOverrideColor( m_MyApp.getAppTheme().getAcceptColor() );
	ui.m_AcceptButton->setIcon( eMyIconCheckMark );

	connect( ui.m_AcceptButton, SIGNAL( clicked() ), this, SLOT( slotAccepted() ) );

	setTestWidgetSize( eButtonSizeMedium );
}

//============================================================================
void TestWidget::slotAccepted( void )
{
	static bool textVisible{ true };
	textVisible = !textVisible;
	ui.m_LineEdit->setVisible( textVisible );
}

//============================================================================
void TestWidget::slotCanceled( void )
{
	emit signalCanceled();
}

//============================================================================
void TestWidget::setTestWidgetSize( EButtonSize buttonSize )
{
	QSize butSize = GuiParams::getButtonSize( buttonSize );

	setFixedHeight( butSize.height() + 4 );
	ui.m_AcceptButton->setFixedSize( buttonSize );
}
