#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityBase.h"

#include <PktLib/VxCommon.h>

#include <QMessageBox>
#include "ui_ActivityMessageBox.h"

class ActivityMessageBox : public ActivityBase
{
	Q_OBJECT
public:
	ActivityMessageBox( AppCommon& app, QWidget* parent = nullptr );
	ActivityMessageBox( AppCommon& app, QWidget* parent, int infoLevel, const char* msgFormat, ... );
	ActivityMessageBox( AppCommon& app, QWidget* parent, int infoLevel, QString msg );
	virtual ~ActivityMessageBox() override = default;

    // overrides required for dialogs with there own title bar and bottom bar widgets
    virtual TitleBarWidget *	getTitleBarWidget( void ) override { return ui.m_TitleBarWidget; }
    virtual BottomBarWidget *	getBottomBarWidget( void ) override { return ui.m_BottomBarWidget; }

	void						setTitleText( QString titleText );
	void						setBodyText( QString bodyText );

	bool						wasOkButtonClicked( void )						{ return m_OkButtonClicked; }
	void						showCancelButton( bool showButton );

	QMessageBox::StandardButton getResultButton( void )							{ return m_ResultButton; }

public slots:
	void						onOkButClick( void );
	void						onCancelButClick( void ); 

protected:
	//=== vars ===//
	Ui::MessageBoxDialogClass	ui;
	bool						m_OkButtonClicked;
	QMessageBox::StandardButton m_ResultButton{ QMessageBox::NoButton };
};