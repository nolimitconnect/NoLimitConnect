//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AssetVoiceWidget.h"
#include "AppCommon.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>

#include <QTimer>

#include "ui_AssetVoiceWidget.h"

//============================================================================
AssetVoiceWidget::AssetVoiceWidget( QWidget* parent )
	: AssetBaseWidget( GetAppInstance(), parent )
	, ui(*(new Ui::AssetVoiceWidget))
	, m_QueueUpdateTimer( new QTimer( this  ) )
{
	initAssetVoiceWidget();
}

//============================================================================
AssetVoiceWidget::AssetVoiceWidget( AppCommon& appCommon, QWidget* parent )
: AssetBaseWidget( appCommon, parent )
, ui(*(new Ui::AssetVoiceWidget))
, m_QueueUpdateTimer( new QTimer( this  ) )
{
	initAssetVoiceWidget();
}

//============================================================================
void AssetVoiceWidget::initAssetVoiceWidget( void )
{
	ui.setupUi( this );
	setMediaModule( eMediaModuleAudioPlayWidget );

	QSize buttonSize( GuiParams::getButtonSize( eButtonSizeTiny ) );
	ui.m_PlayPauseButton->setFixedSizeAbsolute( buttonSize );

	setXferBar( ui.m_XferProgressBar );

	ui.m_PlayPauseButton->setIcons( eMyIconPlayNormal );
	ui.m_PlayPauseButton->setPressedSound( eSndDefNone );
	ui.m_PlayPosSlider->setRange( 0, 100000 );
	ui.m_PlayPosSlider->setMinimum( 0 );
	ui.m_PlayPosSlider->setMaximum( 100000 );

	connect( ui.m_PlayPauseButton, SIGNAL(clicked()), this, SLOT(slotPlayButtonClicked()) );
	connect( ui.m_LeftAvatarBar, SIGNAL(signalShredAsset()), this, SLOT(slotShredAsset()) );
	connect( ui.m_RightAvatarBar, SIGNAL(signalShredAsset()), this, SLOT(slotShredAsset()) );

	connect( ui.m_PlayPosSlider, SIGNAL(sliderPressed()), this, SLOT(slotSliderPressed()) );
	connect( ui.m_PlayPosSlider, SIGNAL(sliderReleased()), this, SLOT(slotSliderReleased()) );
	connect( ui.m_LeftAvatarBar, SIGNAL(signalResendAsset()), this, SLOT(slotResendAsset()) );
	connect( ui.m_RightAvatarBar, SIGNAL(signalResendAsset()), this, SLOT(slotResendAsset()) );

	m_QueueUpdateTimer->setInterval( 100 );
	connect( m_QueueUpdateTimer,		SIGNAL(timeout()),				this, SLOT(slotUpdatePlayerControls()) );

    ui.m_PlayPosSlider->setVisible( true );
	QSize sizeHint( 200, GuiParams::getButtonSize( eButtonSizeLarge ).height() + GuiParams::getButtonSize( eButtonSizeTiny ).height() );
	setSizeHint( sizeHint );
	setFixedHeight( sizeHint.height() );
}

//============================================================================
void AssetVoiceWidget::setAssetInfo( AssetBaseInfo& assetInfo )
{
	AssetBaseWidget::setAssetInfo( assetInfo );
	ui.m_FileNameLabel->setText( getAssetInfo().getRemoteAssetName().c_str() );

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
}

//============================================================================
void AssetVoiceWidget::toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 )
{
	switch( assetAction )
	{
	case eAssetActionPlayProgress:
		if( false == m_SliderIsPressed )
		{
			ui.m_PlayPosSlider->setValue( pos0to100000 );
			// cannot update in callback so queue
			if( true != m_IsPlaying )
			{
				queueUpdatePlayerControls( true );
			}
		}

		break;

	case eAssetActionPlayEnd:
		if( false == m_SliderIsPressed )
		{
			ui.m_PlayPosSlider->setValue( 0 );
			ui.m_PlayPauseButton->setIcon( eMyIconPlayNormal );
			// cannot update in callback so queue
			if( false != m_IsPlaying )
			{
				queueUpdatePlayerControls( false );
			}
		}

		break;

	default:
		break;
	}
}

//============================================================================
void AssetVoiceWidget::queueUpdatePlayerControls( bool enable )
{
	m_QueuedPlayerControlUpdate.push_back( enable );
	m_QueueUpdateTimer->start();
}

//============================================================================
void AssetVoiceWidget::slotUpdatePlayerControls( void )
{
	if( m_QueuedPlayerControlUpdate.size() )
	{
		bool enableControls = m_QueuedPlayerControlUpdate.front();
		m_QueuedPlayerControlUpdate.erase( m_QueuedPlayerControlUpdate.begin() );
		updateGuiPlayControls( enableControls );
	}
	else
	{
		m_QueueUpdateTimer->stop();
	}
}

//============================================================================
void AssetVoiceWidget::slotSliderPressed( void )
{
	m_SliderIsPressed = true;
}

//============================================================================
void AssetVoiceWidget::slotSliderReleased( void )
{
	int posVal = ui.m_PlayPosSlider->value();
	m_Engine.fromGuiAssetAction( eAssetActionPlayBegin, m_AssetInfo, posVal );
	m_SliderIsPressed = false;
}

//============================================================================
void AssetVoiceWidget::slotPlayButtonClicked( void )
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
void AssetVoiceWidget::startMediaPlay( int startPos )
{	
    updateGuiPlayControls( true ); // assume will start playing
    setReadyForCallbacks( true );
	bool playStarted = m_Engine.fromGuiAssetAction( eAssetActionPlayBegin, m_AssetInfo, startPos );
	updateGuiPlayControls( playStarted ); // in case failed to start
    setReadyForCallbacks( playStarted );
	if( false == playStarted )
	{
		m_MyApp.toGuiStatusMessage( "Voice Play FAILED TO Begin" );
	}
}

//========================================================================
void AssetVoiceWidget::updateGuiPlayControls( bool isPlaying )
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
            setReadyForCallbacks( false );
			ui.m_PlayPauseButton->setIcons( eMyIconPlayNormal );
			ui.m_PlayPosSlider->setValue( 0 );        
		}
	}
}

//============================================================================
void AssetVoiceWidget::onActivityStop( void )
{
	setReadyForCallbacks( false );
	stopMediaIfPlaying();
}

//============================================================================
void AssetVoiceWidget::stopMediaIfPlaying( void )
{
	if( m_IsPlaying )
	{
		m_MyApp.toGuiStatusMessage( "" );
		m_Engine.fromGuiAssetAction( eAssetActionPlayEnd, m_AssetInfo, 0 );
	}

	updateGuiPlayControls( false );
}

//============================================================================
void AssetVoiceWidget::setReadyForCallbacks( bool isReady )
{
	if( m_ActivityCallbacksEnabled != isReady )
	{
		m_ActivityCallbacksEnabled = isReady;
		wantActivityCallbacks( m_ActivityCallbacksEnabled );
	}
}

//============================================================================
void AssetVoiceWidget::showSendFail( bool show, bool permissionErr )
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
void AssetVoiceWidget::showResendButton( bool show )
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
void AssetVoiceWidget::showShredder( bool show )
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
void AssetVoiceWidget::showXferProgress( bool show )
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
void AssetVoiceWidget::setXferProgress( int xferProgress )
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
