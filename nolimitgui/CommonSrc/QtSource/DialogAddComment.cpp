//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
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
}

//============================================================================
void DialogAddComment::slotAccepted( void )
{
	m_CommentText = ui.m_CommentLineEdit->text();
	accept();
}
