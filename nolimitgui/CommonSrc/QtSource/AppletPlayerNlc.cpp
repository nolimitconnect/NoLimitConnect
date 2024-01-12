//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletPlayerNlc.h"
#include "ActivityBrowseFiles.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"
#include "GuiPlayerMgr.h"
#include <AppInterface/INlc.h>

#include <P2PEngine/P2PEngine.h>
#include "MediaPlayerNlc.h"

#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

#include "AppletPlayerNlc.h"

#include "ActivityBrowseFiles.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"
#include "AppCommon.h"
#include "SoundMgr.h"

#include "RenderPlayerNlcThread.h"

#include <AppInterface/INlc.h>
#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

#include <QThread>

namespace
{
	const int PROCESS_QT_DEFAULT_MS = 50;

	void ProcessQtEvents( int ms = PROCESS_QT_DEFAULT_MS )
	{
		QCoreApplication::processEvents( QEventLoop::AllEvents, ms );
	}
}

//============================================================================
AppletPlayerNlc::AppletPlayerNlc( AppCommon& app, QWidget* parent )
: AppletPlayerBase( OBJNAME_APPLET_PLAYER_NLC, app, parent )
{
	initAppletPlayerNlc();
}

//============================================================================
void AppletPlayerNlc::initAppletPlayerNlc( void )
{
	setAppletType( eAppletPlayerNlc );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	connect( this, SIGNAL( signalBackButtonClicked() ), this, SLOT( closeApplet() ) );

	ui.setupUi( getContentItemsFrame() );
	ui.m_PlayPosSlider->setVisible( false );
    setMenuBottomVisibility( true );

    BottomBarWidget * bottomBar = getBottomBarWidget();
    if( bottomBar )
    {
        setupBottomMenu( bottomBar->getMenuButton() );
    }

#if defined(DEBUG)

	ui.m_FilesComboBox->setEnabled( false ); // do not enable until media player is ready
	ui.m_FilesComboBox->addItem( "Debug Media Files" );

	std::string mediaTestFilesPath{""};

	#if defined(TARGET_OS_WINDOWS)
		mediaTestFilesPath = "F:/";
	#elif defined(TARGET_OS_LINUX)
		mediaTestFilesPath = "/home/nolimit/";
    #elif defined(TARGET_OS_ANDROID)
    # if defined(TABLET_K117)
        mediaTestFilesPath = "/storage/41B0-1007/";
    # else
        mediaTestFilesPath = "/storage/emulated/0/NoLimitConnectData/";
    # endif

	#endif // defined(TARGET_OS_WINDOWS)

	std::vector<std::string> mediaTestFiles;

	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/NlcTestAudioVbrOff.opus" );
	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/NlcTestAudioVbrOn.opus" );
	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/MJPEGWithAAC.avi" );

	// these are not checked into git
	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/Drop Dead Gorgeous 1999.flv" );
	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/Grey's.Anatomy.S14E12.720p.HDTV.2CH.x265.HEVC-PSA.mkv" );
	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/Agents.of.S.H.I.E.L.D.S05E09/Marvels.Agents.of.S.H.I.E.L.D.S05E09.WEBRip.x264-ION10.mp4" );
	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/PleaseStandByAC3-EVO.avi" );
	// end not checked into git

	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/NlcTestAudio.mp3" );
	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/NlcTestAudio.wav" );
	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/DummyFileDoesNotExist.wav" );

	for( auto& mediaFile : mediaTestFiles )
	{
		if( VxFileUtil::fileExists( mediaFile.c_str() ) )
		{
			ui.m_FilesComboBox->addItem( mediaFile.c_str() );
		}
	}

#else
    ui.m_FilesComboBox->setVisible( false );
    ui.m_BrowseButton->setVisible( false );
#endif // defined(DEBUG)

	ui.m_PlayPosSlider->setRange( 0, 100000 );

	connect( ui.m_PlayPosSlider, SIGNAL( sliderPressed() ), this, SLOT( slotSliderPressed() ) );
	connect( ui.m_PlayPosSlider, SIGNAL( sliderReleased() ), this, SLOT( slotSliderReleased() ) );

	connect( this, SIGNAL( signalPlayProgress(int) ), this, SLOT( slotPlayProgress(int) ) );
	connect( this, SIGNAL( signalPlayEnd() ), this, SLOT( slotPlayEnd() ) );

	connect( ui.m_FilesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotMediaFileComboBoxSelectionChange(int)) );

	connect( ui.m_ReplayButton, SIGNAL( clicked() ), this, SLOT( slotReplayButtonClick() ) );

	connect( ui.m_BrowseButton, SIGNAL( clicked() ), this, SLOT( slotBrowseButtonClick() ) );

	m_MyApp.activityStateChange( this, true );
	m_MyApp.getSoundMgr().setPlayerNlcActive( true );
	m_MyApp.getPlayerMgr().wantPlayVideoCallbacks( this, true );
}

//============================================================================
AppletPlayerNlc::~AppletPlayerNlc()
{
	stopMediaIfPlaying();
	m_MyApp.getPlayerMgr().wantPlayVideoCallbacks( this, false );
	m_MyApp.getSoundMgr().setPlayerNlcActive( false );
	m_MyApp.activityStateChange( this, false );
}

//============================================================================
RenderGlWidget* AppletPlayerNlc::getRenderConsumer( void )
{
	return ui.m_RenderWidget;
}

//============================================================================
void AppletPlayerNlc::setupBottomMenu( VxMenuButton* menuButton )
{
	if( menuButton )
	{
		menuButton->setMenuId( 1 );
		menuButton->addMenuItem( eMenuItemShowRecent );
		menuButton->addMenuItem( eMenuItemShowWatched );
		menuButton->addMenuItem( eMenuItemShowLibrary );
		menuButton->addMenuItem( eMenuItemBrowse );
	}

	connect( menuButton, SIGNAL( signalMenuItemSelected( int, EMenuItemType ) ),
		this, SLOT( slotMenuItemSelected( int, EMenuItemType ) ) );
}

//============================================================================
void AppletPlayerNlc::slotMenuItemSelected( int menuId, EMenuItemType menuItemType )
{
	ActivityBrowseFiles* dlgBrowse;
	switch( menuItemType )
	{
	case eMenuItemBrowse:
		dlgBrowse = new ActivityBrowseFiles( m_MyApp, eFileFilterVideoOnly, this, true );
		dlgBrowse->setAppletType( getAppletType() );
		dlgBrowse->exec();
		if( dlgBrowse->getWasFileSelected() )
		{
            std::string fileStr = dlgBrowse->getSelectedFileInfo().getFullFileName();
            QString fileName = dlgBrowse->getSelectedFileInfo().getFullFileName().c_str();
            if( VxFileUtil::fileExists(fileStr.c_str()) )
            {
                playFile( fileName );
            }
            else
            {
                QMessageBox::information( this, QObject::tr("File does not exist"), fileName, QMessageBox::Ok );
            }
		}

		break;

	default:
		break;
	}
}

//============================================================================
void AppletPlayerNlc::showEvent( QShowEvent* showEvent )
{
	AppletBase::showEvent( showEvent );
}

//============================================================================
void AppletPlayerNlc::hideEvent( QHideEvent* hideEvent )
{
	AppletBase::hideEvent( hideEvent );
}

//============================================================================
void AppletPlayerNlc::resizeEvent( QResizeEvent* ev )
{
	AppletBase::resizeEvent( ev );
}

//============================================================================
void AppletPlayerNlc::setReadyForCallbacks( bool isReady )
{
	if( m_ActivityCallbacksEnabled != isReady )
	{
		m_ActivityCallbacksEnabled = isReady;
		m_MyApp.wantToGuiActivityCallbacks( this, isReady );
	}
}

//============================================================================
void AppletPlayerNlc::slotMediaFileComboBoxSelectionChange( int cbIdx )
{
    std::string mediaFile = ui.m_FilesComboBox->currentText().toUtf8().constData();
	playMediaFile( mediaFile, 0 );
}

//============================================================================
bool AppletPlayerNlc::playMedia( AssetBaseInfo& assetInfo, int pos0to100000 )
{
	if( !waitForPlayerThread() )
	{
		return false;
	}

	AppletPlayerBase::setAssetInfo( assetInfo );

	return INlc::getINlc().getNlcPlayer().fromGuiPlayMedia( assetInfo, pos0to100000 );
}

//============================================================================
bool AppletPlayerNlc::playMediaFile(std::string mediaFile, int pos0to100000 )
{
	if( !waitForPlayerThread() )
	{
		return false;
	}

	if( VxFileUtil::fileExists( mediaFile.c_str() ) )
    {
		return AppletPlayerBase::playFile( mediaFile.c_str(), pos0to100000 );
	}
    else
    {
		QString fileName( mediaFile.c_str() );
        QMessageBox::information( this, QObject::tr("File does not exist"), fileName, QMessageBox::Ok );
		return false;
    }
}

//============================================================================
bool AppletPlayerNlc::waitForPlayerThread( void )
{
	m_ElapsedTimer.start();
	while( !INlc::getINlc().getNlcPlayer().fromGuiIsModuleRunning( eAppModulePlayerNlc ) )
	{
		ProcessQtEvents( 100 );
		if( m_ElapsedTimer.elapsed() > 6000 )
		{
			LogMsg( LOG_ERROR, "Media Player Failed To Start" );
			return false;
		}
	}

	return true;
}

//============================================================================
void AppletPlayerNlc::toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 )
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
void AppletPlayerNlc::slotSliderPressed( void )
{
	m_SliderIsPressed = true;
}

//============================================================================
void AppletPlayerNlc::slotSliderReleased( void )
{
	m_SliderIsPressed = false;
	int posVal = ui.m_PlayPosSlider->value();
	startMediaPlay( posVal );
}

//============================================================================
void AppletPlayerNlc::slotPlayButtonClicked( void )
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
void AppletPlayerNlc::startMediaPlay( int startPos )
{
	bool playStarted = m_Engine.fromGuiAssetAction( eAssetActionPlayBegin, m_AssetInfo, startPos );
	updateGuiPlayControls( playStarted );
	if( false == playStarted )
	{
		m_MyApp.toGuiStatusMessage( "Video Play FAILED TO Begin" );
	}
}

//========================================================================
void AppletPlayerNlc::updateGuiPlayControls( bool isPlaying )
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
void AppletPlayerNlc::stopMediaIfPlaying( void )
{
	if( m_IsPlaying )
	{
		m_MyApp.toGuiStatusMessage( "" );
		m_Engine.fromGuiAssetAction( eAssetActionPlayEnd, m_AssetInfo, 0 );
	}

	updateGuiPlayControls( false );
}

//============================================================================
void AppletPlayerNlc::slotPlayProgress( int pos0to100000 )
{
	if( m_IsPlaying && (false == m_SliderIsPressed) )
	{
		ui.m_PlayPosSlider->setValue( pos0to100000 );
	}
}

//============================================================================
void AppletPlayerNlc::slotPlayEnd( void )
{
	//updateGuiPlayControls( false );
}

//============================================================================
void AppletPlayerNlc::slotReplayButtonClick( void )
{
	std::string fileStr = ui.m_FilesComboBox->currentText().toUtf8().constData();
	QString mediaFile = ui.m_FilesComboBox->currentText().toUtf8().constData();
	if( VxFileUtil::fileExists( fileStr.c_str() ) )
	{
		playFile( mediaFile );
	}
	else
	{
		QMessageBox::information( this, QObject::tr( "File does not exist" ), mediaFile, QMessageBox::Ok );
	}
}


//============================================================================
void AppletPlayerNlc::slotBrowseButtonClick( void )
{
    ActivityBrowseFiles dlg( m_MyApp, eFileFilterVideo, this, true );

    dlg.exec();
    if( dlg.getWasFileSelected() )
    {
        onFileSelected( dlg.getSelectedFileInfo() );
    }
}

//============================================================================
void AppletPlayerNlc::onFileSelected( FileInfo& fileInfo )
{
	if( VXFILE_TYPE_AUDIO == fileInfo.getFileType() || VXFILE_TYPE_VIDEO == fileInfo.getFileType() )
	{
		ui.m_FilesComboBox->addItem( fileInfo.getFileName().c_str() );
	}
	else
	{
		QMessageBox::information( this, QObject::tr("Unknown Media File Type"), fileInfo.getFileName().c_str(), QMessageBox::Ok );
	}
}

//============================================================================
void AppletPlayerNlc::callbackGuiMediaPlayerNlcReady( bool isReady )
{
	LogMsg( LOG_DEBUG, "%s %d", __func__, isReady );
	ui.m_FilesComboBox->setEnabled( isReady );
}
