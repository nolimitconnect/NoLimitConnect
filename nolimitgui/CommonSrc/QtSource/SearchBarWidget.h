#pragma once
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

#include <QFrame>

#include "ui_SearchBarWidget.h"

class SearchBarWidget : public QFrame
{
	Q_OBJECT

public:
	SearchBarWidget( QWidget* parent = nullptr );
	virtual ~SearchBarWidget();

	void						setSearchText( QString searchText );

signals:
	void						signalSearchText( QString searchText );
	void						signalSearchTextChanged( QString searchText );

protected slots:
	void						slotSearchButtonClicked( void );
	void						slotRefreshButtonClicked( void );
	void						slotTextChanged( const QString& text );

protected:
	virtual void				showEvent( QShowEvent* ev );
	virtual void				hideEvent( QHideEvent* ev );

	void						updateSearch( void );
	void						stopSearch( void );

	Ui::SearchBarWidgetUi		ui;
};
