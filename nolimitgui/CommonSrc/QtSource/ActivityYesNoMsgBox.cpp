//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityYesNoMsgBox.h"
#include "MyIcons.h"
#include "AppGlobals.h"
#include "AppCommon.h"
#include "GuiHelpers.h"

#include <PktLib/VxCommon.h>
#include <CoreLib/VxDebug.h>

//============================================================================
ActivityYesNoMsgBox::ActivityYesNoMsgBox(	AppCommon&		app, 
											QWidget*		parent, 
											QString			title, 
											QString			bodyText,
											bool			showNeverAgainCheckBox )
: ActivityBase( OBJNAME_ACTIVITY_YES_NO_MSG_BOX, app, parent, eAppletMessengerFrame, true )
{
	ui.setupUi(this);
	ui.m_NeverShowAgainCheckBox->setVisible( showNeverAgainCheckBox );

    connectBarWidgets();
	ui.m_AcceptButton->setIconOverrideColor( m_MyApp.getAppTheme().getAcceptColor() );
	ui.m_AcceptButton->setIcon( eMyIconAcceptNormal );
	ui.m_CancelButton->setIconOverrideColor( m_MyApp.getAppTheme().getCancelColor() );
	ui.m_CancelButton->setIcon( eMyIconCancelNormal );
	setTitle( title );
	ui.m_BodyTextLabel->setText( bodyText );

    connect(ui.m_TitleBarWidget, SIGNAL(signalBackButtonClicked()), this, SLOT(slotHomeButtonClicked()));

	connect( ui.m_AcceptButton, SIGNAL(clicked()), this, SLOT(accept()) );
	connect( ui.m_CancelButton, SIGNAL(clicked()), this, SLOT(reject()) );
	this->setFocus();
	ui.m_AcceptButton->setFocus();
}


//============================================================================
void ActivityYesNoMsgBox::setTitle( QString strTitle )
{
	ui.m_TitleBarWidget->setTitleBarText(strTitle);
}

//============================================================================
void ActivityYesNoMsgBox::setBodyText( QString strBodyText )
{
	ui.m_BodyTextLabel->setText( strBodyText );
}

//============================================================================
void ActivityYesNoMsgBox::hideCancelButton( void )
{
	ui.m_CancelButton->setVisible( false );
}

//============================================================================
void ActivityYesNoMsgBox::makeNeverShowAgainVisible( bool makeVisible )
{
	ui.m_NeverShowAgainCheckBox->setVisible( makeVisible );
}

//============================================================================
bool ActivityYesNoMsgBox::wasNeverShowAgainChecked( void )
{
	return ui.m_NeverShowAgainCheckBox->isChecked();
}

//============================================================================
void ActivityYesNoMsgBox::slotHomeButtonClicked( void )
{
	reject();
}
