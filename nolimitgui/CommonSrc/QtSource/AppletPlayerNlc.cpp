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

#include <QDir>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileIsTypeFunctions.h>
#include <CoreLib/VxGlobals.h>

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
	if( !lastPlayedMovie.empty() && VxFileUtil::fileExists( lastPlayedMovie.c_str() ) )
	{
		ui.m_LastPlayedFileText->setText( lastPlayedMovie.c_str() );
	}

	ui.m_FilesComboBox->setEnabled( false ); // do not enable until media player is ready

	int mediaFilesCnt{ 0 };

	QStringList downloadPathList = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation);
	for( auto downloadPath : downloadPathList )
	{
		auto mediaPath = downloadPath + "/NlcMediaFiles";
		QDir mediaDir( mediaPath );
		if( !mediaDir.exists() )
		{
			continue;
		}

		mediaFilesCnt += addMediaFilesToComboBox( mediaDir );
	}

	if( !mediaFilesCnt )
	{
		// no files to show
		ui.m_FilesComboBox->setVisible( false );
	}

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
	if( cbIdx < m_ComboBoxFileList.size() )
	{
		std::string mediaFile = m_ComboBoxFileList[ cbIdx ];
		playSelectedMovie( mediaFile );
	}
	else
	{
		LogMsg( LOG_ERROR, "%s invalid combo box index", __func__ );
	}
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
		std::string fullFileName = fileInfo.getFileName();
		std::string filePath;
		std::string	fileName;
                                                        
		VxFileUtil::seperatePathAndFile( fullFileName.c_str(), filePath, fileName );

		m_ComboBoxFileList.emplace_back( fullFileName );
		ui.m_FilesComboBox->addItem( fileName.c_str() );
		ui.m_FilesComboBox->setVisible( true );

		playSelectedMovie( fullFileName );
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
	std::string movieFile = ui.m_LastPlayedFileText->text().toUtf8().constData();
	if( VxFileUtil::fileExists( movieFile.c_str() ) )
	{
		playSelectedMovie( movieFile );
	}
	else if( ui.m_FilesComboBox->currentIndex() >= 0 && ui.m_FilesComboBox->currentIndex() < m_ComboBoxFileList.size() )
	{
		playSelectedMovie( m_ComboBoxFileList[ui.m_FilesComboBox->currentIndex()] );
	}
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

//============================================================================
int AppletPlayerNlc::addMediaFilesToComboBox( QDir& mediaDir )
{
	int addedFileCnt{ 0 };
	mediaDir.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks | QDir::Readable );
	mediaDir.setSorting( QDir::Name );
	QFileInfoList fileList = mediaDir.entryInfoList();
	for (int i = 0; i < fileList.size(); ++i) 
	{
        const QFileInfo fileInfo = fileList.at( i );
		if( fileInfo.size() > 100 && addFileToComboBox( fileInfo ) )
		{
			addedFileCnt++;
		}
    }

	return addedFileCnt;
}

//============================================================================
bool AppletPlayerNlc::addFileToComboBox( const QFileInfo& fileInfo )
{
	bool addedFile{ false };
	std::string fileName = fileInfo.absoluteFilePath().toUtf8().constData();

	if( VxIsVideoFile( fileName ) || VxIsAudioFile( fileName ) )
	{
		m_ComboBoxFileList.emplace_back( fileName );
		ui.m_FilesComboBox->addItem( fileInfo.fileName() );
		addedFile = true;
	}

	return addedFile;
}