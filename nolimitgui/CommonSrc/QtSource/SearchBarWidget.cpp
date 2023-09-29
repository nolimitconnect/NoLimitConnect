//============================================================================
// Copyright (C) 2022 Brett R. Jones 
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

#include "SearchBarWidget.h"
#include "GuiOfferMgr.h"

#include "MyIcons.h"
#include "AppCommon.h"

#include <CoreLib/VxGlobals.h>

#include <QCloseEvent>

//============================================================================
SearchBarWidget::SearchBarWidget( QWidget* parent )
: QFrame( parent )
{
	ui.setupUi( this );
	ui.m_SearchButton->setFixedSize( eButtonSizeTiny );
	ui.m_SearchButton->setIcon( eMyIconEyeSearchEnabled );
	ui.m_RefreshButton->setFixedSize( eButtonSizeTiny );
	ui.m_RefreshButton->setIcon( eMyIconRefresh );

	connect( ui.m_SearchButton,		SIGNAL(clicked()),						this, SLOT(slotSearchButtonClicked()) );
	connect( ui.m_RefreshButton,	SIGNAL(clicked()),						this, SLOT(slotRefreshButtonClicked()) );
	connect( ui.m_SearchLineEdit,	SIGNAL(textChanged(const QString&)),	this, SLOT(slotTextChanged(const QString&)) );
}

//============================================================================
SearchBarWidget::~SearchBarWidget()
{
	stopSearch();
}

//============================================================================
void SearchBarWidget::setSearchText( QString searchText )
{
	ui.m_SearchLineEdit->setText( searchText );
}

//============================================================================
void SearchBarWidget::updateSearch( void )
{
	if( false == VxIsAppShuttingDown() )
	{
		QString searchText = ui.m_SearchLineEdit->text();
		emit signalSearchTextChanged( searchText );
		emit signalSearchText( searchText );
	}
}

//============================================================================
void SearchBarWidget::stopSearch( void )
{
	if( false == VxIsAppShuttingDown() )
	{
		emit signalSearchTextChanged( "" );
		emit signalSearchText( "" );
	}
}

//============================================================================
void SearchBarWidget::showEvent( QShowEvent* ev )
{
	QWidget::showEvent( ev );
}

//============================================================================
void SearchBarWidget::hideEvent( QHideEvent* ev )
{
	stopSearch();
	QWidget::hideEvent( ev );
}

//============================================================================
void SearchBarWidget::slotSearchButtonClicked( void )
{
	updateSearch();
}

//============================================================================
void SearchBarWidget::slotRefreshButtonClicked( void )
{
	updateSearch();
}

//============================================================================
void SearchBarWidget::slotTextChanged( const QString& text )
{
	emit signalSearchTextChanged( text );
}