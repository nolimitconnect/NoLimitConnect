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

	connect( ui.m_FilesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotMediaFileComboBoxSelectionChange(int)) );

	connect( ui.m_BrowseButton, SIGNAL( clicked() ), this, SLOT( slotBrowseButtonClick() ) );

	onAppletInitialized();
}

//============================================================================
AppletPlayerNlc::~AppletPlayerNlc()
{
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
                playFile( fileName, 0, false );
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
void AppletPlayerNlc::slotMediaFileComboBoxSelectionChange( int cbIdx )
{
	std::string mediaFile = ui.m_FilesComboBox->currentText().toUtf8().constData();
	playMediaFile( mediaFile, 0, false );
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
void AppletPlayerNlc::onMediaPlayerNlcReady( bool isReady )
{
	if( isReady )
	{
		ui.m_FilesComboBox->setEnabled( isReady );
	}
}