#pragma once
//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class LanguageSelectUi;
}
QT_END_NAMESPACE

class AppletLanguageSelect : public AppletBase
{
	Q_OBJECT
public:
    AppletLanguageSelect( AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletLanguageSelect() override = default;



private slots:
	void						slotApplyButtonClicked( void );

protected:
	virtual void				onLanguageSelected( void );

	//=== vars ===//
	Ui::LanguageSelectUi&		ui;
};


