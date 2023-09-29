//============================================================================
// Copyright (C) 2017 Brett R. Jones
// Issued to MIT style license by Brett R. Jones in 2017
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
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
