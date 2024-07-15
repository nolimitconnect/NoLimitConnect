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

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
    class ChooseTestWebsiteUrlDlg;
}
QT_END_NAMESPACE

class P2PEngine;

class ActivityChooseTestWebsiteUrl : public ActivityBase
{
	Q_OBJECT
public:
	ActivityChooseTestWebsiteUrl( AppCommon& app, QWidget* parent = nullptr );
	virtual ~ActivityChooseTestWebsiteUrl() override = default;

    // overrides required for dialogs with there own title bar and bottom bar widgets
	TitleBarWidget*				getTitleBarWidget( void ) override;
	BottomBarWidget*			getBottomBarWidget( void ) override;

public slots:
	void						updateValues();

	void						applyResultsAndExit();

protected:
	//=== vars ===//
	Ui::ChooseTestWebsiteUrlDlg&	ui;
	AppCommon&					m_MyApp;
	P2PEngine&					m_Engine;
};
