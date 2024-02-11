//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AssetVideoWidget.h"
#include "AppCommon.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxGlobals.h>

//============================================================================
AssetVideoWidget::AssetVideoWidget( QWidget* parent )
	: AssetBaseWidget( GetAppInstance(), parent )
	, m_ReadyForVideoTimer( new QTimer( this ) )
{
	initAssetVideoWidget();
}

//============================================================================
AssetVideoWidget::AssetVideoWidget( AppCommon& appCommon, QWidget* parent )
	: AssetBaseWidget( appCommon, parent )
	, m_ReadyForVideoTimer( new QTimer( this ) )
{
	initAssetVideoWidget();
}

//============================================================================
void AssetVideoWidget::initAssetVideoWidget( void )
{
	ui.setupUi( this );
	setXferBar( ui.m_XferProgressBar );

	ui.m_VidWidget->showAllControls( false );
	ui.m_VidWidget->disablePreview( true );
	ui.m_VidWidget->disableRecordControls( true );

	ui.m_PlayPauseButton->setIcons( eMyIconPlayNormal );
	ui.m_PlayPauseButton->setPressedSound( eSndDefNone );
	ui.m_PlayPosSlider->setRange( 0, 100000 );

	connect( ui.m_PlayPauseButton,	SIGNAL(clicked()),						this, SLOT(slotPlayButtonClicked()) );
	connect( ui.m_LeftAvatarBar,	SIGNAL(signalShredAsset()),				this, SLOT(slotShredAsset()) );
	connect( ui.m_RightAvatarBar,	SIGNAL(signalShredAsset()),				this, SLOT(slotShredAsset()) );
	connect( ui.m_RightAvatarBar,	SIGNAL(signalResendAsset()),			this, SLOT(slotResendAsset()) );
	connect( ui.m_PlayPosSlider,	SIGNAL(sliderPressed()),				this, SLOT(slotSliderPressed()) );
	connect( ui.m_PlayPosSlider,	SIGNAL(sliderReleased()),				this, SLOT(slotSliderReleased()) );

	connect( this,					SIGNAL(signalPlayProgress(int)),		this, SLOT(slotPlayProgress(int)) );
	connect( this,					SIGNAL(signalPlayEnd()),				this, SLOT(slotPlayEnd()) );
	connect( ui.m_LeftAvatarBar,	SIGNAL(signalResendAsset()),			this, SLOT(slotResendAsset()) );

	connect( ui.m_VidWidget,		SIGNAL(signalFeedRotationChanged(int)), this, SLOT(slotFeedRotationChanged(int)) );
	connect( ui.m_VidWidget,		SIGNAL(signalCamRotationChanged(int)),  this, SLOT(slotCamRotationChanged(int)) );

	connect( m_ReadyForVideoTimer,	SIGNAL(timeout()),						this, SLOT(slotReadyForVideo()) );
	m_ReadyForVideoTimer->setInterval( 50 );
}

//============================================================================
void AssetVideoWidget::setAssetInfo( AssetBaseInfo& assetInfo )
{
	AssetBaseWidget::setAssetInfo( assetInfo );

	ui.m_FileNameLabel->setText( getAssetInfo().getRemoteAssetName().c_str() );

	ui.m_VidWidget->setVideoFeedId( m_AssetInfo.getAssetUniqueId(), getAppModule() );
	this->setSizeHint( QSize( 100 * GuiParams::getGuiScale(), 224 * GuiParams::getGuiScale() ) );

	ui.m_LeftAvatarBar->setOnlineId( m_AssetInfo.getOnlineId() );
	ui.m_RightAvatarBar->setOnlineId( m_AssetInfo.getOnlineId() );
	GuiUser* guiUser = m_MyApp.getUserMgr().getUser( m_AssetInfo.getHistoryId() );
	if( assetInfo.isMine() )
	{
		ui.m_LeftAvatarBar->setTime( m_AssetInfo.getCreationTime(), m_AssetInfo.getIsQueued() );
		ui.m_RightAvatarBar->showAvatar( true );
		if( guiUser )
		{
			ui.m_RightUserNameLabel->setText( guiUser->getOnlineName().c_str() );
		}
	}
	else
	{
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

	m_ReadyForVideoTimer->start();
}

//============================================================================
void AssetVideoWidget::slotReadyForVideo( void )
{
	m_ReadyForVideoTimer->stop();
	setReadyForCallbacks( true );

	m_Engine.fromGuiAssetAction( eAssetActionPlayOneFrame, m_AssetInfo, 0 );
}

//============================================================================
void AssetVideoWidget::resizeEvent( QResizeEvent* ev )
{
	AssetBaseWidget::resizeEvent( ev );
	/*
	if( ( false == VxIsAppShuttingDown() )
		&& m_AssetInfo.isValid()
		&& !m_IsPlaying
		&& isVisible() )
	{
		setReadyForCallbacks( true );

		m_Engine.fromGuiAssetAction( eAssetActionPlayOneFrame, m_AssetInfo, 0 );
	}
	*/
}

//============================================================================
void AssetVideoWidget::toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 )
{
	AssetBaseWidget::toGuiClientAssetAction( assetAction, assetId, pos0to100000 );
	switch( assetAction )
	{
	case eAssetActionPlayProgress:
		if( false == m_SliderIsPressed )
		{
			updateGuiPlayControls( true );
			ui.m_PlayPosSlider->setValue( pos0to100000 );
		}

		break;

	case eAssetActionPlayEnd:
		if( false == m_SliderIsPressed )
		{
			updateGuiPlayControls( false );
		}

		break;

	default:
		break;
	}
}

//============================================================================
void AssetVideoWidget::slotSliderPressed( void )
{
	m_SliderIsPressed = true;
}

//============================================================================
void AssetVideoWidget::slotSliderReleased( void )
{
	m_SliderIsPressed = false;
	int posVal = ui.m_PlayPosSlider->value();
	startMediaPlay( posVal );
}

//============================================================================
void AssetVideoWidget::slotPlayButtonClicked( void )
{
	if( m_IsPlaying )
	{
		stopMediaIfPlaying();
	}
	else
	{
		startMediaPlay( 0 );
	}
}

//========================================================================
void AssetVideoWidget::startMediaPlay( int startPos )
{	
	bool playStarted = m_Engine.fromGuiAssetAction( eAssetActionPlayBegin, m_AssetInfo, startPos );
	updateGuiPlayControls( playStarted );
	if( false == playStarted )
	{
		m_MyApp.toGuiStatusMessage( "Video Play FAILED TO Begin" );
	}
}

//========================================================================
void AssetVideoWidget::updateGuiPlayControls( bool isPlaying )
{	
	if( m_IsPlaying != isPlaying )
	{
		m_IsPlaying = isPlaying;
		if( m_IsPlaying )
		{
			// start playing
			ui.m_PlayPauseButton->setIcons( eMyIconPauseNormal );
			setReadyForCallbacks( true );
		}
		else
		{
			// stop playing
			ui.m_PlayPauseButton->setIcons( eMyIconPlayNormal );
			ui.m_PlayPosSlider->setValue( 0 );
		}
	}
}

//============================================================================
void AssetVideoWidget::onActivityStop( void )
{
	setReadyForCallbacks( false );
	stopMediaIfPlaying();
}

//============================================================================
void AssetVideoWidget::stopMediaIfPlaying( void )
{
	if( m_IsPlaying )
	{
		m_MyApp.toGuiStatusMessage( "" );
		m_Engine.fromGuiAssetAction( eAssetActionPlayEnd, m_AssetInfo, 0 );
	}

	updateGuiPlayControls( false );
}

//============================================================================
void AssetVideoWidget::setReadyForCallbacks( bool isReady )
{
	if( m_ActivityCallbacksEnabled != isReady )
	{
		m_ActivityCallbacksEnabled = isReady;
		m_MyApp.wantToGuiActivityCallbacks( this, isReady );
	}
}

//============================================================================
void AssetVideoWidget::slotShredAsset( void )
{
	onActivityStop();
	emit signalShreddingAsset( this );
}

//============================================================================
void AssetVideoWidget::slotPlayProgress( int pos0to100000 )
{
	if( m_IsPlaying && ( false == m_SliderIsPressed ) )
	{
		ui.m_PlayPosSlider->setValue( pos0to100000 );
	}
}

//============================================================================
void AssetVideoWidget::slotPlayEnd( void )
{
	//updateGuiPlayControls( false );
}

//============================================================================
void AssetVideoWidget::showSendFail( bool show, bool permissionErr )
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
void AssetVideoWidget::showResendButton( bool show )
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
void AssetVideoWidget::showShredder( bool show )
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
void AssetVideoWidget::showXferProgress( bool show )
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
void AssetVideoWidget::setXferProgress( int xferProgress )
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
void AssetVideoWidget::slotFeedRotationChanged( int feedRotation )
{
}

//============================================================================
void AssetVideoWidget::slotCamRotationChanged( int camRotation )
{
	m_Engine.fromGuiAssetAction( eAssetActionPlayOneFrame, m_AssetInfo, 0 );
}

//============================================================================
void AssetVideoWidget::onAssetWidgetVisibleAndReady( bool isVisible, bool isReady )
{
	AssetBaseWidget::onAssetWidgetVisibleAndReady( isVisible, isReady );
	setReadyForCallbacks( isVisible );
	if( isReady && isVisible )
	{
		if( (false == VxIsAppShuttingDown())
			&& m_AssetInfo.isValid()
			&& !m_IsPlaying )
		{
			setReadyForCallbacks( true );
			m_Engine.fromGuiAssetAction( eAssetActionPlayOneFrame, m_AssetInfo, 0 );
		}
	}
}