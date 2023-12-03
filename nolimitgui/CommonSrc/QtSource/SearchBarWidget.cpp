//============================================================================
// Copyright (C) 2022 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
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