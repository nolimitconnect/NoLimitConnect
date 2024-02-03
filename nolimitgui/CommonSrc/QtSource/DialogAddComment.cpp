//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "DialogAddComment.h"

#include "MyIcons.h"
#include "AppCommon.h"
#include "GuiParams.h"

//============================================================================
DialogAddComment::DialogAddComment( AssetBaseInfo& assetInfo, QWidget* parent )
: QDialog( parent, Qt::Popup )
, m_MyApp( GetAppInstance() )
{
	ui.setupUi(this);
	// dialog does not get pallette from parent.. force pallette update
	m_MyApp.getAppTheme().applyTheme( this );

    QSize buttonSize( GuiParams::getButtonSize( eButtonSizeMedium ) );
    ui.m_ExitDlgButton->setFixedSize( buttonSize );

	connect( ui.m_ExitDlgButton,		SIGNAL(clicked()),			this, SLOT(reject()) );
	connect( ui.m_AcceptCancelWidget,	SIGNAL(signalAccepted()),	this, SLOT(slotAccepted()) );

	ui.m_CommentLineEdit->setFocus();
}

//============================================================================
void DialogAddComment::slotAccepted( void )
{
	m_CommentText = ui.m_CommentLineEdit->text();
	accept();
}
