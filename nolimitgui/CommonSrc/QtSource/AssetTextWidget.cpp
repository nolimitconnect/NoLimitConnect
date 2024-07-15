//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppCommon.h"
#include "AssetTextWidget.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>

#include "ui_AssetTextWidget.h"

//============================================================================
AssetTextWidget::AssetTextWidget( QWidget* parent )
	: AssetBaseWidget( GetAppInstance(), parent )
	, ui(*(new Ui::AssetTextWidget))
{
	initAssetTextWidget();
}

//============================================================================
AssetTextWidget::AssetTextWidget( AppCommon& appCommon, QWidget* parent )
: AssetBaseWidget( appCommon, parent )
, ui(*(new Ui::AssetTextWidget))
{
	initAssetTextWidget();
}

//============================================================================
void AssetTextWidget::initAssetTextWidget( void )
{
	ui.setupUi( this );
    QSize buttonSize( GuiParams::getButtonSize( eButtonSizeSmall ) );
    ui.m_SendButton->setFixedSizeAbsolute( buttonSize );
    ui.m_CopyTextButton->setFixedSizeAbsolute( buttonSize );

	setXferBar( ui.m_XferProgressBar );
	ui.m_CopyTextButton->setIcon( eMyIconEditPaste );

	ui.m_SendButton->setVisible( false );
	ui.m_ButtonFrame->setVisible( false );

	connect( ui.m_ChatTextLabel,	SIGNAL(clicked()),			this, SLOT(slotAssetWasClicked()) );
	connect( ui.m_TagUserTextLabel, SIGNAL(clicked()),			this, SLOT(slotAssetWasClicked()) );
	connect( ui.m_CopyTextButton,	SIGNAL(clicked()),			this, SLOT(slotCopyTextToClipboardButClick()) );
	connect( ui.m_LeftAvatarBar,	SIGNAL(signalShredAsset()), this, SLOT(slotShredAsset()) );
	connect( ui.m_RightAvatarBar,	SIGNAL(signalShredAsset()), this, SLOT(slotShredAsset()) );
	connect( ui.m_LeftAvatarBar,	SIGNAL(signalResendAsset()), this, SLOT(slotResendAsset()) );
	connect( ui.m_RightAvatarBar,	SIGNAL(signalResendAsset()), this, SLOT(slotResendAsset()) );
	calculateHint();
}

//============================================================================
void AssetTextWidget::slotAssetWasClicked( void )
{
	bool isVisible = ui.m_ButtonFrame->isVisible();
	ui.m_ButtonFrame->setVisible( !isVisible );
	calculateHint();
}

//============================================================================
void AssetTextWidget::slotShredAsset( void )
{
	//this->m_Engine.fromGuiAssetAction( ePluginTypeInvalid, eAssetActionShreadFile, m_AssetInfo.getAssetUniqueId(), 0 );
	emit signalShreddingAsset( this );
}

//============================================================================
void AssetTextWidget::setAssetInfo( AssetBaseInfo& assetInfo )
{
	AssetBaseWidget::setAssetInfo( assetInfo );
	if( false == m_AssetInfo.isValid() )
	{
		return;
	}

	ui.m_ChatTextLabel->setText( assetInfo.getAssetName().c_str() );
	ui.m_LeftAvatarBar->setOnlineId( m_AssetInfo.getOnlineId() );
	ui.m_RightAvatarBar->setOnlineId( m_AssetInfo.getOnlineId() );
	GuiUser* guiUser = m_MyApp.getUserMgr().getUser( m_AssetInfo.getHistoryId() );
	if( assetInfo.isMine() )
	{
		ui.m_ChatTextLabel->setStyleSheet( m_MyApp.getAppTheme().getChatTextTxStyleSheet() );
		ui.m_LeftAvatarBar->setTime( m_AssetInfo.getCreationTime(), m_AssetInfo.getIsQueued() );
		ui.m_RightAvatarBar->showAvatar( true );
		if( guiUser )
		{
			ui.m_RightUserNameLabel->setText( guiUser->getOnlineName().c_str() );
		}
	}
	else
	{
		ui.m_ChatTextLabel->setStyleSheet( m_MyApp.getAppTheme().getChatTextRxStyleSheet() );
		ui.m_RightAvatarBar->setTime( m_AssetInfo.getCreationTime(), m_AssetInfo.getIsQueued() );
		ui.m_LeftAvatarBar->showAvatar( true );
		if( guiUser )
		{
			ui.m_LeftUserNameLabel->setText( guiUser->getOnlineName().c_str() );
		}
	}

	if( assetInfo.isFileAsset() )
	{
		ui.m_LeftAvatarBar->setShredButtonIcon( eMyIconShredderNormal );
		ui.m_RightAvatarBar->setShredButtonIcon(eMyIconShredderNormal );
	}
	else
	{
		ui.m_LeftAvatarBar->setShredButtonIcon( eMyIconTrash );
		ui.m_RightAvatarBar->setShredButtonIcon( eMyIconTrash );
	}

	calculateHint();
	updateFromAssetInfo();
}

//============================================================================
void AssetTextWidget::calculateHint( void )
{
	int avatarBarHeight = std::max( ui.m_LeftAvatarBar->getDesiredHeight(), ui.m_RightAvatarBar->getDesiredHeight() );

    QFontMetrics thisFontMetrics = fontMetrics();
    int totalHeight = GuiHelpers::calculateTextHeight( thisFontMetrics, ui.m_ChatTextLabel->text() ) + 4;
	ui.m_ChatTextLabel->setFixedHeight( totalHeight );

    int tagTextHeight = GuiHelpers::calculateTextHeight( thisFontMetrics, ui.m_TagUserTextLabel->text() ) + 4;
	ui.m_TagUserTextLabel->setFixedHeight( tagTextHeight );
	totalHeight += tagTextHeight;

	if( ui.m_ButtonFrame->isVisible() )
	{
		totalHeight +=  ui.m_ButtonFrame->height() + 4;
	}

	this->setSizeHint( QSize( 100, std::max( totalHeight, avatarBarHeight ) ) );
}

//============================================================================
void AssetTextWidget::showSendFail( bool show, bool permissionErr )
{
	if( m_AssetInfo.isMine() )
	{
		ui.m_LeftAvatarBar->showSendFail( show, permissionErr );
		ui.m_LeftAvatarBar->showResendButton( show );
	}
	else
	{
		ui.m_RightAvatarBar->showSendFail( show, permissionErr );
		ui.m_RightAvatarBar->showResendButton( show );
	}
}

//============================================================================
void AssetTextWidget::showResendButton( bool show )
{
	if( m_AssetInfo.isMine() )
	{
		ui.m_LeftAvatarBar->showResendButton( show );
	}
	else
	{
		ui.m_RightAvatarBar->showResendButton( show );
	}
}

//============================================================================
void AssetTextWidget::showShredder( bool show )
{
	if( m_AssetInfo.isMine() )
	{
		ui.m_LeftAvatarBar->showShredder( show );
	}
	else
	{
		ui.m_RightAvatarBar->showShredder( show );
	}
}

//============================================================================
void AssetTextWidget::showXferProgress( bool show )
{
	if( m_AssetInfo.isMine() )
	{
		ui.m_LeftAvatarBar->showShredder( show );
		ui.m_LeftAvatarBar->showXferProgress( show );
	}
	else
	{
		ui.m_RightAvatarBar->showShredder( show );
		ui.m_RightAvatarBar->showXferProgress( show );
	}
}

//============================================================================
void AssetTextWidget::setXferProgress( int xferProgress )
{
	if( m_AssetInfo.isMine() )
	{
		ui.m_LeftAvatarBar->setXferProgress( xferProgress );
	}
	else
	{
		ui.m_RightAvatarBar->setXferProgress( xferProgress );
	}
}

//============================================================================
void AssetTextWidget::slotCopyTextToClipboardButClick( void )
{
	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setText( ui.m_ChatTextLabel->text() );
	QMessageBox::information( this, QObject::tr( "Clipboard" ), QObject::tr( "Text was copied to clipboard" ), QMessageBox::Ok );
}