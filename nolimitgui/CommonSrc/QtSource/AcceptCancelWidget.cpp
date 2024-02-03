//============================================================================
// Copyright (C) 2017 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AcceptCancelWidget.h"

#include "AppCommon.h"
#include "VxAppTheme.h"
#include "GuiParams.h"
#include "VxAppTheme.h"

//============================================================================
AcceptCancelWidget::AcceptCancelWidget( QWidget* parent )
: QFrame( parent )
, m_MyApp(GetAppInstance())
{
	ui.setupUi( this );
	ui.m_AcceptButton->setIconOverrideColor( m_MyApp.getAppTheme().getAcceptColor() );
	ui.m_AcceptButton->setIcon( eMyIconCheckMark );
	ui.m_CancelButton->setIconOverrideColor( m_MyApp.getAppTheme().getCancelColor() );
	ui.m_CancelButton->setIcon( eMyIconRedX );
	connect( ui.m_AcceptButton, SIGNAL( clicked() ), this, SLOT( slotAccepted() ) );
	connect( ui.m_CancelButton, SIGNAL( clicked() ), this, SLOT( slotCanceled() ) );
	setAcceptCancelWidgetSize( eButtonSizeMedium );

	ui.m_CancelButton->setFocus();
}

//============================================================================
void AcceptCancelWidget::slotAccepted( void )
{
	emit signalAccepted();
}

//============================================================================
void AcceptCancelWidget::slotCanceled( void )
{
	emit signalCanceled();
}

//============================================================================
void AcceptCancelWidget::setAcceptCancelWidgetSize( EButtonSize buttonSize )
{
	QSize butSize = GuiParams::getButtonSize( buttonSize );

	setFixedHeight( butSize.height() + 4 );
	ui.m_AcceptButton->setFixedSize( buttonSize );
	ui.m_CancelButton->setFixedSize( buttonSize );
}

//============================================================================
void AcceptCancelWidget::showCancelButton( bool showButton )
{
	ui.m_CancelButton->setVisible( showButton );
}
