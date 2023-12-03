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
#include "AssetPhotoWidget.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

//============================================================================
AssetPhotoWidget::AssetPhotoWidget( QWidget* parent )
	: AssetBaseWidget( GetAppInstance(), parent )
{
	initAssetPhotoWidget();
}

//============================================================================
AssetPhotoWidget::AssetPhotoWidget( AppCommon& appCommon, QWidget* parent )
	: AssetBaseWidget( appCommon, parent )
{
	initAssetPhotoWidget();
}

//============================================================================
void AssetPhotoWidget::initAssetPhotoWidget( void )
{
	ui.setupUi( this );
	setXferBar( ui.m_XferProgressBar );

	ui.m_SendButton->setVisible( false );
	ui.m_ButtonFrame->setVisible( false );
	ui.m_VidWidget->disablePreview( true );
	ui.m_VidWidget->disableRecordControls( true );
	
	ui.m_ShredButton->setIcon( eMyIconShredderNormal );

	connect( ui.m_VidWidget,		SIGNAL(clicked()),				this, SLOT(slotAssetWasClicked()) );
	connect( ui.m_ShredButton,		SIGNAL(clicked()),				this, SLOT(slotShredAsset()) );
	connect( ui.m_LeftAvatarBar,	SIGNAL(signalShredAsset()),		this, SLOT(slotShredAsset()) );
	connect( ui.m_RightAvatarBar,	SIGNAL(signalShredAsset()),		this, SLOT(slotShredAsset()) );
	connect( ui.m_LeftAvatarBar,	SIGNAL(signalResendAsset()),	this, SLOT(slotResendAsset()) );
}

//============================================================================
void AssetPhotoWidget::setAssetInfo( AssetBaseInfo& assetInfo )
{
	AssetBaseWidget::setAssetInfo( assetInfo );
	this->setSizeHint( QSize( 100, 224 - 16 ) );
	ui.m_VidWidget->setImageFromFile( assetInfo.getAssetName().c_str() );
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
		ui.m_RightAvatarBar->setShredButtonIcon( eMyIconTrash );
	}

	if( !assetInfo.getAssetTag().empty() )
	{
		ui.m_TagUserTextLabel->setText( assetInfo.getAssetTag().c_str() );
	}

	updateFromAssetInfo();
}

//============================================================================
void AssetPhotoWidget::slotAssetWasClicked( void )
{
	bool isVisible = ui.m_ButtonFrame->isVisible();
	ui.m_ButtonFrame->setVisible( !isVisible );
}

//============================================================================
void AssetPhotoWidget::showSendFail( bool show, bool permissionErr )
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
void AssetPhotoWidget::showResendButton( bool show )
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
void AssetPhotoWidget::showShredder( bool show )
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
void AssetPhotoWidget::showXferProgress( bool show )
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
void AssetPhotoWidget::setXferProgress( int xferProgress )
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
