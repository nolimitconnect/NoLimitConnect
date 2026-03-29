//============================================================================
// Copyright (C) 2016 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "DialogConfirmRemoveMessage.h"

#include "MyIconsDefs.h"
#include "AppCommon.h"
#include "GuiParams.h"

#include <AssetMgr/AssetInfo.h>

#include "ui_DialogConfirmRemoveMessage.h"

//============================================================================
DialogConfirmRemoveMessage::DialogConfirmRemoveMessage( AssetBaseInfo& assetInfo, bool shredOnly, QWidget* parent )
: QDialog( parent, Qt::Popup )
, ui(*(new Ui::ConfirmRemoveMessageClass))
, m_MyApp( GetAppInstance() )
, m_AssetInfo( assetInfo )
, m_ShredOnly(shredOnly)
{
	ui.setupUi(this);
	// dialog does not get pallette from parent.. force pallette update
	m_MyApp.getAppTheme().applyTheme( this );

    QSize buttonSize( GuiParams::getButtonSize( eButtonSizeMedium ) );
    ui.m_ExitDlgButton->setFixedSize( buttonSize );
    ui.m_CancelButton->setFixedSizeAbsolute( buttonSize );
    ui.m_RemoveButton->setFixedSizeAbsolute( buttonSize );
    ui.m_ShredButton->setFixedSizeAbsolute( buttonSize );

	ui.m_CancelButton->setIconOverrideColor( m_MyApp.getAppTheme().getCancelColor() );
	ui.m_CancelButton->setIcon( eMyIconRejectRedX );

	ui.m_RemoveButton->setIcon( eMyIconTrash );
	ui.m_ShredButton->setIcon( eMyIconShredderNormal );

	connect( ui.m_ExitDlgButton,		SIGNAL(clicked()), this, SLOT(reject()) );
	connect( ui.m_CancelButton,			SIGNAL(clicked()), this, SLOT(reject()) );
	connect( ui.m_RemoveButton,			SIGNAL(clicked()), this, SLOT(slotRemoveAssetButtonClicked()) );
	connect( ui.m_ShredButton,			SIGNAL(clicked()), this, SLOT(slotShredFileButtonClicked()) );
	if( false == m_AssetInfo.isFileAsset() )
	{
		ui.m_ShredButton->setVisible( false );
        ui.m_ShreadButtonTextLabel->setVisible( false );
		ui.m_ShredTextLabel->setVisible( false );
	}

	if( m_ShredOnly )
	{
		ui.m_RemoveButton->setVisible( false );
		ui.m_RemoveButtonTextLabel->setVisible( false );
		ui.m_TrashTextLabel->setVisible( false );
	}
}

//============================================================================
void DialogConfirmRemoveMessage::slotRemoveAssetButtonClicked( void )
{
	m_AssetAction = eAssetActionRemoveFromAssetMgr;
	accept();
}

//============================================================================
void DialogConfirmRemoveMessage::slotShredFileButtonClicked( void )
{
	m_AssetAction = eAssetActionShreadFile;
	accept();
}
