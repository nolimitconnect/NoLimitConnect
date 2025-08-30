#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QFrame>

QT_BEGIN_NAMESPACE
namespace Ui {
    class SearchBarWidgetUi;
}
QT_END_NAMESPACE

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

	Ui::SearchBarWidgetUi&		ui;
};
