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
#include "AssetFaceWidget.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>

//============================================================================
AssetFaceWidget::AssetFaceWidget( QWidget* parent )
	: AssetBaseWidget( GetAppInstance(), parent )
{
	initAssetFaceWidget();
}

//============================================================================
AssetFaceWidget::AssetFaceWidget( AppCommon& appCommon, QWidget* parent )
: AssetBaseWidget( appCommon, parent )
{
	initAssetFaceWidget();
}

//============================================================================
void AssetFaceWidget::initAssetFaceWidget( void )
{
	ui.setupUi( this );
    QSize buttonSize(  GuiParams::getButtonSize( eButtonSizeTiny ) );
    ui.m_ShredButton->setFixedSizeAbsolute( buttonSize );
    ui.m_SendButton->setFixedSizeAbsolute( buttonSize );

	setXferBar( ui.m_XferProgressBar );
	ui.m_ShredButton->setIcon( eMyIconTrash );
	ui.m_SendButton->setVisible( false );
	ui.m_ButtonFrame->setVisible( false );

	connect( ui.m_FaceLabel, SIGNAL(clicked()), this, SLOT(slotAssetWasClicked()) );
	connect( ui.m_ShredButton, SIGNAL(clicked()), this, SLOT(slotShredAsset()) );
	connect( ui.m_LeftAvatarBar, SIGNAL(signalShredAsset()), this, SLOT(slotShredAsset()) );
	connect( ui.m_RightAvatarBar, SIGNAL(signalShredAsset()), this, SLOT(slotShredAsset()) );
	connect( ui.m_LeftAvatarBar, SIGNAL(signalResendAsset()), this, SLOT(slotResendAsset()) );
}

//============================================================================
void AssetFaceWidget::setAssetInfo( AssetBaseInfo& assetInfo )
{
	AssetBaseWidget::setAssetInfo( assetInfo );
	if( false == m_AssetInfo.isValid() )
	{
		return;
	}

	QString faceRes = ":/AppRes/Resources/";
	faceRes += m_AssetInfo.getAssetName().c_str();
	faceRes += ".svg";
	QPixmap faceImage( faceRes );
	if( faceImage.isNull() )
	{
		LogMsg( LOG_ERROR, "FAIL AssetFaceWidget::setAssetInfo null resource %s", faceRes.toUtf8().constData() );
		return;
	}

	ui.m_FaceLabel->setPixmap( faceImage.scaled( GuiParams::getButtonSize( eButtonSizeMedium ) ) );
	ui.m_FaceLabel->setFixedSize( GuiParams::getButtonSize( eButtonSizeMedium ).width() + 4, GuiParams::getButtonSize( eButtonSizeMedium ).height() + 4 );
	ui.m_LeftAvatarBar->setOnlineId( m_AssetInfo.getOnlineId() );
	ui.m_RightAvatarBar->setOnlineId( m_AssetInfo.getOnlineId() );
	GuiUser* guiUser = m_MyApp.getUserMgr().getUser( m_AssetInfo.getHistoryId() );
	if( assetInfo.isMine() )
	{	
		ui.m_LeftAvatarBar->setTime( m_AssetInfo.getCreationTime() );
		ui.m_RightAvatarBar->showAvatar( true );
		if( guiUser )
		{
			ui.m_RightUserNameLabel->setText( guiUser->getOnlineName().c_str() );
		}
	}
	else
	{
		ui.m_RightAvatarBar->setTime( m_AssetInfo.getCreationTime() );
		ui.m_LeftAvatarBar->showAvatar( true );
		if( guiUser )
		{
			ui.m_LeftUserNameLabel->setText( guiUser->getOnlineName().c_str() );
		}
	}

	if( assetInfo.isFileAsset() )
	{
		ui.m_LeftAvatarBar->setShredButtonIcon( eMyIconShredderNormal );
		ui.m_RightAvatarBar->setShredButtonIcon( eMyIconShredderNormal );
	}
	else
	{
		ui.m_LeftAvatarBar->setShredButtonIcon( eMyIconTrash );
		ui.m_RightAvatarBar->setShredButtonIcon(  eMyIconTrash );
	}

	if( !assetInfo.getAssetTag().empty() )
	{
		ui.m_TagUserTextLabel->setText( assetInfo.getAssetTag().c_str() );
	}

	calculateHint();
	updateFromAssetInfo();
}

//============================================================================
void AssetFaceWidget::slotAssetWasClicked( void )
{
	bool isVisible = ui.m_ButtonFrame->isVisible();
	ui.m_ButtonFrame->setVisible( !isVisible );
	calculateHint();
}

//============================================================================
void AssetFaceWidget::calculateHint( void )
{
	int avatarBarHeight = std::max( ui.m_LeftAvatarBar->getDesiredHeight(), ui.m_RightAvatarBar->getDesiredHeight() );
	int totalHeight = ui.m_FaceLabel->height() + 4;

    QFontMetrics thisFontMetrics = fontMetrics();
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
void AssetFaceWidget::showSendFail( bool show, bool permissionErr )
{
	if( m_AssetInfo.isMine() )
	{
		ui.m_LeftAvatarBar->showSendFail( show, permissionErr );
		ui.m_LeftAvatarBar->showResendButton( show );
		if( m_AssetInfo.isMyHistory() )
		{
			ui.m_RightAvatarBar->showSendFail( show, permissionErr );
			ui.m_RightAvatarBar->showResendButton( show );
		}
	}
	else
	{
		ui.m_RightAvatarBar->showSendFail( show, permissionErr );
		ui.m_RightAvatarBar->showResendButton( show );
	}

	calculateHint();
}

//============================================================================
void AssetFaceWidget::showResendButton( bool show )
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
void AssetFaceWidget::showShredder( bool show )
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
void AssetFaceWidget::showXferProgress( bool show )
{
	if( m_AssetInfo.isMine() )
	{
		ui.m_LeftAvatarBar->showXferProgress( show );
	}
	else
	{
		ui.m_RightAvatarBar->showXferProgress( show );
	}
}

//============================================================================
void AssetFaceWidget::setXferProgress( int xferProgress )
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
