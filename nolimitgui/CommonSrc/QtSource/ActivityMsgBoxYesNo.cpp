//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityMsgBoxYesNo.h"
#include "MyIconsDefs.h"
#include "AppGlobals.h"
#include "AppCommon.h"
#include "GuiHelpers.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <PktLib/VxCommon.h>
#include <CoreLib/VxDebug.h>

#include "ui_ActivityYesNo.h"

TitleBarWidget*		ActivityMsgBoxYesNo::getTitleBarWidget( void ) { return ui.m_TitleBarWidget; }
BottomBarWidget*	ActivityMsgBoxYesNo::getBottomBarWidget( void ) { return ui.m_BottomBarWidget; }
QFrame*				ActivityMsgBoxYesNo::getContentItemsFrame( void ) { return ui.m_ItemsFrame; }

//============================================================================
ActivityMsgBoxYesNo::ActivityMsgBoxYesNo(	AppCommon&		app, 
											QWidget*		parent, 
											QString			title, 
											QString			bodyText,
											bool			showNeverAgainCheckBox )
: ActivityBase( OBJNAME_ACTIVITY_MSG_BOX_YES_NO, app, parent, eAppletMessengerFrame, true )
, ui(*(new Ui::YesNoMsgBoxClass))
{
	ui.setupUi(this);
	ui.m_NeverShowAgainCheckBox->setVisible( showNeverAgainCheckBox );

	ui.m_AcceptButton->setIconOverrideColor( m_MyApp.getAppTheme().getAcceptColor() );
	ui.m_AcceptButton->setIcon( eMyIconAcceptNormal );
	ui.m_CancelButton->setIconOverrideColor( m_MyApp.getAppTheme().getCancelColor() );
	ui.m_CancelButton->setIcon( eMyIconCancelNormal );
	setTitle( title );
	ui.m_BodyTextLabel->setText( bodyText );

	getBottomBarWidget()->setVisible( false );
    getTitleBarWidget()->setPopupVisibility( true );

	connect( ui.m_AcceptButton, SIGNAL(clicked()), this, SLOT(accept()) );
	connect( ui.m_CancelButton, SIGNAL(clicked()), this, SLOT(reject()) );
    this->setFocus(); // required for android or just shuts down when prompting are you an adult
    ui.m_AcceptButton->setFocus();
}

//============================================================================
void ActivityMsgBoxYesNo::setTitle( QString strTitle )
{
	ui.m_TitleBarWidget->setTitleBarText(strTitle);
}

//============================================================================
void ActivityMsgBoxYesNo::setBodyText( QString strBodyText )
{
	ui.m_BodyTextLabel->setText( strBodyText );
}

//============================================================================
void ActivityMsgBoxYesNo::hideCancelButton( void )
{
	ui.m_CancelButton->setVisible( false );
}

//============================================================================
void ActivityMsgBoxYesNo::makeNeverShowAgainVisible( bool makeVisible )
{
	ui.m_NeverShowAgainCheckBox->setVisible( makeVisible );
}

//============================================================================
bool ActivityMsgBoxYesNo::wasNeverShowAgainChecked( void )
{
	return ui.m_NeverShowAgainCheckBox->isChecked();
}
