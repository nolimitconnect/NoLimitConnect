#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityBase.h"
#include "ui_ActivityYesNo.h"

class ActivityYesNoMsgBox : public ActivityBase
{
	Q_OBJECT
public:
	ActivityYesNoMsgBox(	AppCommon&		app, 
							QWidget*		parent, 
							QString			title, 
							QString			bodyText,
							bool			showNeverAgainCheckBox = false); 
	virtual ~ActivityYesNoMsgBox() override = default;

    // overrides required for dialogs with there own title bar and bottom bar widgets
    virtual TitleBarWidget *	getTitleBarWidget( void ) override { return ui.m_TitleBarWidget; }
    virtual BottomBarWidget *	getBottomBarWidget( void ) override { return ui.m_BottomBarWidget; }

	void						setTitle( QString strTitle );
	void						setBodyText( QString strBodyText );
	void						hideCancelButton( void );
	void						makeNeverShowAgainVisible( bool makeVisible );
	bool						wasNeverShowAgainChecked( void );

private slots:
    void						slotHomeButtonClicked( void ) override;

protected:

	//=== vars ===//
	Ui::YesNoMsgBoxClass		ui;
};


