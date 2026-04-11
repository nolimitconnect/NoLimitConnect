//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletPlayerPhoto.h"

#include "ActivityBrowseFiles.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "BottomBarWidget.h"
#include "GuiPlayerMgr.h"
#include "VxMenuButton.h"

#include <AssetBase/AssetPlaySession.h>
#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletPlayerPhoto.h"

//============================================================================
AppletPlayerPhoto::AppletPlayerPhoto( AppCommon& app, QWidget* parent )
: AppletPlayerBase( OBJNAME_APPLET_PLAYER_PHOTO, app, parent )
, ui(*(new Ui::AppletPlayerPhotoUi ))
, m_ActivityCallbacksEnabled( false )
, m_IsPlaying( false )
, m_SliderIsPressed( false )
{
	initAppletPlayerPhoto();
}

//============================================================================
void AppletPlayerPhoto::initAppletPlayerPhoto( void )
{
	setAppletType( eAppletPlayerPhoto );
	setTitleBarText( DescribeApplet( m_EAppletType ) );	

	ui.setupUi( getContentItemsFrame() );
	ui.m_PlayPosSlider->setVisible( false );
    setMenuBottomVisibility( true );
	ui.m_VidWidget->setMediaModule( eMediaModulePhotoPlayer );

	ui.m_VidWidget->setVideoUiMode( eVideoUiModePhoto );
	ui.m_VidWidget->disablePreview( true );
	ui.m_VidWidget->disableRecordControls( true );

    BottomBarWidget * bottomBar = getBottomBarWidget();
    if( bottomBar )
    {
        setupBottomMenu( bottomBar->getMenuButton() );
    }

	//ui.m_PlayPosSlider->setRange( 0, 100000 );

	//connect( this, SIGNAL(signalBackButtonClicked()), this, SLOT(closeApplet()) );

	//connect( ui.m_PlayPosSlider, SIGNAL( sliderPressed() ), this, SLOT(slotSliderPressed() ) );
	//connect( ui.m_PlayPosSlider, SIGNAL( sliderReleased() ), this, SLOT(slotSliderReleased() ) );

	//connect( this, SIGNAL(signalPlayProgress( int ) ), this, SLOT(slotPlayProgress( int ) ) );
	//connect( this, SIGNAL(signalPlayEnd() ), this, SLOT(slotPlayEnd() ) );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
void AppletPlayerPhoto::setupBottomMenu( VxMenuButton * menuButton )
{
    if( menuButton )
    {
        menuButton->setMenuId( 1 );
        menuButton->addMenuItem( eMenuItemShowRecent );
        menuButton->addMenuItem( eMenuItemShowWatched );
        menuButton->addMenuItem( eMenuItemShowLibrary );
        menuButton->addMenuItem( eMenuItemBrowse );
    }

    connect( menuButton, SIGNAL(signalMenuItemSelected( int, EMenuItemType ) ), 
                this, SLOT(slotMenuItemSelected( int, EMenuItemType ) ) );
}

//============================================================================
bool AppletPlayerPhoto::playMedia( AssetPlaySession& assetPlaySession, bool useExternalPlayer)
{
	std::string fullFileName = assetPlaySession.getFileNameAndPath();

	QPixmap pixmap;
	pixmap.load( fullFileName.c_str() );
	if( pixmap.isNull() )
	{
		close();
		return false;
	}

	// Reset image rotation to 0 for photos (ignore any camera/feed rotation settings)
	// Users can still rotate photos manually using the rotate button
	ui.m_VidWidget->setVidImageRotation( 0 );

	if( ui.m_VidWidget->setImageFromFile( fullFileName.c_str() ) )
	{
		return true;
	}
	
	close();
	return false;
}

//============================================================================
void AppletPlayerPhoto::slotMenuItemSelected( int menuId, EMenuItemType menuItemType )
{
    ActivityBrowseFiles * dlgBrowse;
    switch( menuItemType )
    {
    case eMenuItemBrowse:
        dlgBrowse = new ActivityBrowseFiles( m_MyApp, eFileFilterPhotoOnly, this, true );
        dlgBrowse->setAppletType( getAppletType() );
        dlgBrowse->exec();
        if( dlgBrowse->getWasFileSelected() )
        {
            QString fileName = dlgBrowse->getSelectedFileInfo().getFileNameAndPath().c_str();
            playFile( VXFILE_TYPE_PHOTO, fileName, VxGUID::nullVxGUID(), false );
         }

        break;

    default:
        break;
    }
}

//============================================================================
void AppletPlayerPhoto::setAssetInfo( AssetInfo& assetInfo )
{
	AppletPlayerBase::setAssetInfo( assetInfo );
	//ui.m_FileNameLabel->setText( getAssetInfo().getRemoteAssetName().c_str() );
	//ui.m_ShredButton->setShredFile( getAssetInfo().getRemoteAssetName().c_str() );
	//ui.m_LeftAvatarBar->setShredFile( getAssetInfo().getRemoteAssetName().c_str() );
	//ui.m_RightAvatarBar->setShredFile( getAssetInfo().getRemoteAssetName().c_str() );

    ui.m_VidWidget->setVideoFeedId( m_AssetInfo.getAssetUniqueId(), eMediaModuleMediaPlayer );
	//if( ui.m_TagLabel->text().isEmpty() )
	//{
	//	ui.m_TagLabel->setVisible( false );
	//	ui.m_TagTitleLabel->setVisible( false );
	//	this->setSizeHint( QSize( 100, 224 - 16 ) );
	//}
	//else
	//{
	//	ui.m_TagLabel->setVisible( true );
	//	ui.m_TagTitleLabel->setVisible( true );
	//	this->setSizeHint( QSize( 100, 224 ) );
	//}

	//if( assetInfo.isMine() )
	//{
	//	ui.m_LeftAvatarBar->setTime( m_AssetInfo.getCreationTime() );
	//}
	//else
	//{
	//	ui.m_RightAvatarBar->setTime( m_AssetInfo.getCreationTime() );
	//}

	//if( assetInfo.isFileAsset() )
	//{
	//	ui.m_LeftAvatarBar->setShredButtonIcon( eMyIconShredderNormal );
	//	ui.m_RightAvatarBar->setShredButtonIcon( eMyIconShredderNormal );
	//}
	//else
	//{
	//	ui.m_LeftAvatarBar->setShredButtonIcon( eMyIconTrash );
	//	ui.m_RightAvatarBar->setShredButtonIcon( eMyIconTrash );
	//}

	//updateFromAssetInfo();
}

//============================================================================
void AppletPlayerPhoto::showEvent( QShowEvent* showEvent )
{
	AppletPlayerBase::showEvent( showEvent );
}

//============================================================================
void AppletPlayerPhoto::hideEvent( QHideEvent* hideEvent )
{
    AppletPlayerBase::hideEvent( hideEvent );
 }

//============================================================================
void AppletPlayerPhoto::resizeEvent( QResizeEvent* ev )
{
	AppletPlayerBase::resizeEvent( ev );
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
void AppletPlayerPhoto::toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 )
{
	AppletPlayerBase::toGuiClientAssetAction( assetAction, assetId, pos0to100000 );
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
void AppletPlayerPhoto::slotSliderPressed( void )
{
	m_SliderIsPressed = true;
}

//============================================================================
void AppletPlayerPhoto::slotSliderReleased( void )
{
	m_SliderIsPressed = false;
	int posVal = ui.m_PlayPosSlider->value();
	startMediaPlay( posVal );
}

//============================================================================
void AppletPlayerPhoto::slotPlayButtonClicked( void )
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
void AppletPlayerPhoto::startMediaPlay( int startPos )
{
	bool playStarted = m_Engine.fromGuiAssetAction( eAssetActionPlayBegin, m_AssetInfo, startPos );
	updateGuiPlayControls( playStarted );
	if( false == playStarted )
	{
		m_MyApp.toGuiStatusMessage( "Video Play FAILED TO Begin" );
	}
}

//========================================================================
void AppletPlayerPhoto::updateGuiPlayControls( bool isPlaying )
{
	if( m_IsPlaying != isPlaying )
	{
		m_IsPlaying = isPlaying;
		if( m_IsPlaying )
		{
			// start playing
			//ui.m_PlayPauseButton->setIcons( eMyIconPauseNormal );
			setReadyForCallbacks( true );
		}
		else
		{
			// stop playing
			//ui.m_PlayPauseButton->setIcons( eMyIconPlayNormal );
			ui.m_PlayPosSlider->setValue( 0 );
		}
	}
}

//============================================================================
void AppletPlayerPhoto::onAppletStop( void )
{
	setReadyForCallbacks( false );
	stopMediaIfPlaying();
}

//============================================================================
void AppletPlayerPhoto::stopMediaIfPlaying( void )
{
	if( m_IsPlaying )
	{
		m_MyApp.toGuiStatusMessage( "" );
		m_Engine.fromGuiAssetAction( eAssetActionPlayEnd, m_AssetInfo, 0 );
	}

	updateGuiPlayControls( false );
}

//============================================================================
void AppletPlayerPhoto::setReadyForCallbacks( bool isReady )
{
	if( m_ActivityCallbacksEnabled != isReady )
	{
		m_ActivityCallbacksEnabled = isReady;
		wantActivityCallbacks( isReady );
	}
}

//============================================================================
void AppletPlayerPhoto::slotShredAsset( void )
{
	onAppletStop();
	//emit signalShreddingAsset( this );
}

//============================================================================
void AppletPlayerPhoto::slotPlayProgress( int pos0to100000 )
{
	if( m_IsPlaying && ( false == m_SliderIsPressed ) )
	{
		ui.m_PlayPosSlider->setValue( pos0to100000 );
	}
}

//============================================================================
void AppletPlayerPhoto::slotPlayEnd( void )
{
	//updateGuiPlayControls( false );
}

//============================================================================
void AppletPlayerPhoto::showShredder( bool show )
{
	//if( m_AssetInfo.isMine() )
	//{
	//	ui.m_LeftAvatarBar->showShredder( show );
	//}
	//else
	//{
	//	ui.m_RightAvatarBar->showShredder( show );
	//}
}

//============================================================================
void AppletPlayerPhoto::showXferProgress( bool show )
{
	//if( m_AssetInfo.isMine() )
	//{
	//	ui.m_LeftAvatarBar->showXferProgress( show );
	//}
	//else
	//{
	//	ui.m_RightAvatarBar->showXferProgress( show );
	//}
}

//============================================================================
void AppletPlayerPhoto::setXferProgress( int xferProgress )
{
	//if( m_AssetInfo.isMine() )
	//{
	//	ui.m_LeftAvatarBar->setXferProgress( xferProgress );
	//}
	//else
	//{
	//	ui.m_RightAvatarBar->setXferProgress( xferProgress );
	//}
}

//============================================================================
void AppletPlayerPhoto::callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 )
{

}

//============================================================================
void AppletPlayerPhoto::callbackGuiPlayVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame )
{

}