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

#include <CoreLib/VxDefs.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class ShowHelpDialog;
}
QT_END_NAMESPACE

class P2PEngine;

class ActivityShowHelp : public ActivityBase
{
	Q_OBJECT

public:
	ActivityShowHelp( AppCommon& app, QWidget* parent = nullptr );
	virtual ~ActivityShowHelp() override = default;

    // overrides required for dialogs with there own title bar and bottom bar widgets
    virtual TitleBarWidget*     getTitleBarWidget( void ) override;
    virtual BottomBarWidget*    getBottomBarWidget( void ) override;

protected slots:
	void						slotReject();
	void						slotClipboard();

protected:
	void						loadHelpFile( const char* pFileName, uint64_t u64FileLen );
	void						loadHelp( void );


	//=== vars ===//
	Ui::ShowHelpDialog&			ui;
};
