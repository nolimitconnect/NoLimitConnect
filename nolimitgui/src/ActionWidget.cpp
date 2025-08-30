//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActionWidget.h"

#include <QDesktopServices>

#include "ui_ActionWidget.h"

//============================================================================
ActionWidget::ActionWidget( QWidget* parent )
: QWidget( parent )
, ui(*(new Ui::ActionWidgetUi))
{
	ui.setupUi( this );
	ui.m_ActionButton->setFixedSize( eButtonSizeSmall );

	connect( ui.m_ActionButton, SIGNAL(clicked()), this, SLOT(slotActionClicked()) );
	connect( ui.m_ActionLabel, SIGNAL(clicked()), this, SLOT(slotActionClicked()) );
}

//============================================================================
void ActionWidget::slotActionClicked( void )
{
	emit clicked();
}

//============================================================================
void ActionWidget::setActionText( QString text )
{
	ui.m_ActionLabel->setText( text );
}

//============================================================================
void ActionWidget::setActionIcon( EMyIcons icon )
{
	ui.m_ActionButton->setIcon( icon );
}

//============================================================================
void ActionWidget::setActionButtonSize( EButtonSize buttonSize )
{
	ui.m_ActionButton->setFixedSize( buttonSize );
}

//============================================================================
VxPushButton* ActionWidget::getActionButton( void )
{
	return ui.m_ActionButton;
}