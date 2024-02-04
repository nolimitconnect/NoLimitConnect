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
#include "GuiPlayerMgr.h"
#include "SoundMgr.h"

#include <AppInterface/INlc.h>

#include <P2PEngine/P2PEngine.h>
#include "MediaPlayerNlc.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>

//============================================================================
AppletPlayerNlc::AppletPlayerNlc( AppCommon& app, QWidget* parent )
: AppletPlayerNlcBase( OBJNAME_APPLET_PLAYER_NLC, app, parent )
{
	initAppletPlayerNlc();
}

//============================================================================
void AppletPlayerNlc::initAppletPlayerNlc( void )
{
	setAppletType( eAppletPlayerNlc );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

	ui.setupUi( getContentItemsFrame() );
	ui.m_PlayPosSlider->setVisible( false );
    setMenuBottomVisibility( true );

    BottomBarWidget * bottomBar = getBottomBarWidget();
    if( bottomBar )
    {
        setupBottomMenu( bottomBar->getMenuButton() );
    }

	std::string lastPlayedMovie;
	m_MyApp.getAppSettings().getLastPlayedMovie( lastPlayedMovie );
	if( !lastPlayedMovie.empty() )
	{
		ui.m_LastPlayedFileText->setText( lastPlayedMovie.c_str() );
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

	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/MJPEGWithAAC.avi" );

	// these are not checked into git
	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/TestAnime.x264.mp4" );
	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/Test.x265.HEVC-PSA.mkv" );
	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/Test.HEVC.x265.mkv" );
	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/Test.x264-ION10-WithSubs/Test.x264-ION10.mp4" );
	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/TestMkv1.mkv" );
	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/Test.FLV.flv" );
	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/Test.x264.mkv" );

	// end not checked into git

	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/NlcTestAudio.mp3" );
	mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/NlcTestAudioVbrOn.opus" );
	// mediaTestFiles.push_back( mediaTestFilesPath + "MediaTestFiles/NlcTestAudioVbrOff.opus" );
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
#endif // defined(DEBUG)

	connect( ui.m_FilesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotMediaFileComboBoxSelectionChange(int)) );

	connect( ui.m_BrowseButton, SIGNAL(clicked()), this, SLOT( slotBrowseButtonClick() ) );
	connect( ui.m_ReplayButton, SIGNAL(clicked()), this, SLOT( slotReplayButtonClick() ) );

	onAppletInitialized();
}

//============================================================================
AppletPlayerNlc::~AppletPlayerNlc()
{
    stopMediaIfPlaying();
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
void AppletPlayerNlc::slotAppletClosing( void )
{
    m_MyApp.getAppSettings().setLastAppletLaunched( eLaunchFrameHome, eAppletUnknown );
    stopMediaIfPlaying();
    closeApplet();
}

//============================================================================
void AppletPlayerNlc::slotMenuItemSelected( int menuId, EMenuItemType menuItemType )
{
	ActivityBrowseFiles* dlgBrowse;
	switch( menuItemType )
	{
	case eMenuItemBrowse:
		browseForMovie();
		break;

	default:
		break;
	}
}

//============================================================================
void AppletPlayerNlc::slotMediaFileComboBoxSelectionChange( int cbIdx )
{
	std::string mediaFile = ui.m_FilesComboBox->currentText().toUtf8().constData();
	playMediaFile( mediaFile, 0, false );
}

//============================================================================
void AppletPlayerNlc::slotBrowseButtonClick( void )
{
	browseForMovie();
}

//============================================================================
void AppletPlayerNlc::browseForMovie( void )
{
	stopMediaIfPlaying();
	//startBusySpinner();
    ActivityBrowseFiles dlg( m_MyApp, eFileFilterVideo, getContentFrameOfOppositePageFrame(), true );
    dlg.exec();
	//stopBusySpinner();
    if( dlg.getWasFileSelected() )
    {
		playSelectedMovie( dlg.getSelectedFileInfo().getFullFileName() );
	}
}

//============================================================================
void AppletPlayerNlc::onFileSelected( FileInfo& fileInfo )
{
	if( VXFILE_TYPE_AUDIO == fileInfo.getFileType() || VXFILE_TYPE_VIDEO == fileInfo.getFileType() )
	{
		ui.m_FilesComboBox->addItem( fileInfo.getFileName().c_str() );
		playSelectedMovie( fileInfo.getFileName() );
	}
	else
	{
		QMessageBox::information( this, QObject::tr("Unknown Media File Type"), fileInfo.getFileName().c_str(), QMessageBox::Ok );
	}
}

//============================================================================
void AppletPlayerNlc::onMediaPlayerNlcReady( bool isReady )
{
	if( isReady )
	{
		ui.m_FilesComboBox->setEnabled( isReady );
	}
}

//============================================================================
void AppletPlayerNlc::slotReplayButtonClick( void )
{
	QString movieFile = ui.m_LastPlayedFileText->text();
	playSelectedMovie( movieFile.toUtf8().constData() );
}

//============================================================================
void AppletPlayerNlc::playSelectedMovie( std::string movieFile )
{
	stopMediaIfPlaying();
	QString fileName = movieFile.c_str();
	if( VxFileUtil::fileExists( movieFile.c_str() ) )
	{
		ui.m_LastPlayedFileText->setText( fileName );
		m_MyApp.getAppSettings().setLastPlayedMovie( movieFile );
		playFile( fileName, 0, false, false );
	}
	else
	{
		QMessageBox::information( this, QObject::tr( "File does not exist" ), fileName, QMessageBox::Ok );
	}
}