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

QT_BEGIN_NAMESPACE
namespace Ui {
    class YesNoMsgBoxClass;
}
QT_END_NAMESPACE

class ActivityMsgBoxYesNo : public ActivityBase
{
	Q_OBJECT
public:
	ActivityMsgBoxYesNo(	AppCommon&		app, 
							QWidget*		parent, 
							QString			title, 
							QString			bodyText,
							bool			showNeverAgainCheckBox = false); 
	virtual ~ActivityMsgBoxYesNo() override = default;

	void						setTitle( QString strTitle );
	void						setBodyText( QString strBodyText );
	void						hideCancelButton( void );
	void						makeNeverShowAgainVisible( bool makeVisible );
	bool						wasNeverShowAgainChecked( void );

protected:
	// overrides required for dialogs with there own title bar and bottom bar widgets
    TitleBarWidget*				getTitleBarWidget( void ) override;
    BottomBarWidget*			getBottomBarWidget( void ) override;
	QFrame*						getContentItemsFrame( void ) override;

	//=== vars ===//
	Ui::YesNoMsgBoxClass&		ui;
};


