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

#include "AppletBrowseFiles.h"
#include "AppletMgr.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "BottomBarWidget.h"
#include "GuiHelpers.h"
#include "GuiPlayerMgr.h"
#include "SoundMgr.h"
#include "VxMenuButton.h"

#include <P2PEngine/P2PEngine.h>
#include "MediaPlayerNlc.h"

#include "PlayControlWidget.h"

#include <QDir>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileIsTypeFunctions.h>
#include <CoreLib/VxGlobals.h>

#include "ui_AppletPlayerNlc.h"

VxFileList AppletPlayerNlc::m_RecentFiles;

RenderGlWidget*		AppletPlayerNlc::getRenderConsumer( void )		{ return ui.m_RenderWidget; }
QSlider*			AppletPlayerNlc::getPlayPosSlider( void )		{ return ui.m_PlayControlWidget->getPlayPosSlider(); }
QPushButton*		AppletPlayerNlc::getReplayButton( void )		{ return ui.m_ReplayButton; }
VxPushButton*		AppletPlayerNlc::getPlayPauseButton( void )		{ return ui.m_PlayControlWidget->getPlayPauseButton(); }
PlayControlWidget*	AppletPlayerNlc::getPlayControlWidget( void )	{ return ui.m_PlayControlWidget; }

//============================================================================
AppletPlayerNlc::AppletPlayerNlc( AppCommon& app, QWidget* parent )
: AppletPlayerNlcBase( OBJNAME_APPLET_PLAYER_NLC, app, parent )
, ui(*(new Ui::AppletPlayerNlcUi))
{
	initAppletPlayerNlc();
}

//============================================================================
void AppletPlayerNlc::initAppletPlayerNlc( void )
{
	setAppletType( eAppletPlayerNlc );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

	ui.setupUi( getContentItemsFrame() );
	ui.m_PlayControlWidget->setVisible( false );
    setMenuBottomVisibility( true );
	ui.m_BrowseButton->setIcon( eMyIconFileBrowseNormal );
	ui.m_BrowseButton->setSquareButtonSize( eButtonSizeSmall );
	ui.m_ReplayButton->setIcon( eMyIconPlayNormal );
	ui.m_ReplayButton->setSquareButtonSize( eButtonSizeSmall );
    ui.m_OpenVideoFileButton->setIcon( eMyIconVideo );
    ui.m_OpenVideoFileButton->setSquareButtonSize( eButtonSizeSmall );
    ui.m_OpenAudioFileButton->setIcon( eMyIconMusic );
    ui.m_OpenAudioFileButton->setSquareButtonSize( eButtonSizeSmall );

    BottomBarWidget * bottomBar = getBottomBarWidget();
    if( bottomBar )
    {
        setupBottomMenu( bottomBar->getMenuButton() );
    }

	ui.m_FilesComboBox->setVisible( false ); // do not show until media player is ready
	ui.m_BrowseButton->setVisible( false );
    ui.m_ReplayButton->setVisible( false );
    ui.m_OpenVideoFileButton->setVisible( false );
    ui.m_OpenAudioFileButton->setVisible( false );

	QStringList downloadPathList = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation);
	for( auto downloadPath : downloadPathList )
	{
		auto mediaPath = downloadPath + "/NlcMediaFiles";
		QDir mediaDir( mediaPath );
		if( !mediaDir.exists() )
		{
			continue;
		}

		addMediaFilesToRecentList( mediaDir );
	}

	std::string lastPlayedMovie;
	m_MyApp.getAppSettings().getLastPlayedMovie( lastPlayedMovie );
	if( !lastPlayedMovie.empty() )
	{
		ui.m_LastPlayedFileText->setText( lastPlayedMovie.c_str() );
		m_RecentFiles.removeFile( lastPlayedMovie );
		m_RecentFiles.addFileToFront( lastPlayedMovie );
	}

	refreshRecentFilesComboBox();

	connect( ui.m_FilesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotMediaFileComboBoxSelectionChange(int)) );

	connect( ui.m_BrowseButton, SIGNAL(clicked()), this, SLOT(slotBrowseButtonClick()) );
	connect( ui.m_ReplayButton, SIGNAL(clicked()), this, SLOT(slotReplayButtonClick()) );
    connect( ui.m_OpenVideoFileButton, SIGNAL(clicked()), this, SLOT(slotOpenVideoFileButtonClick()) );
    connect( ui.m_OpenAudioFileButton, SIGNAL(clicked()), this, SLOT(slotOpenAudioFileButtonClick()) );


	connect( ui.m_PlayControlWidget, SIGNAL(signalRestart()), this, SLOT(slotReplayButtonClick()) );
	connect( ui.m_PlayControlWidget, SIGNAL(signalPlayPauseButtonClicked()), this, SLOT(slotPlayPauseButtonClick()) );
	connect( ui.m_PlayControlWidget, SIGNAL(signalStopButtonClicked()), this, SLOT(slotStopButtonClick()) );
	connect( ui.m_PlayControlWidget, SIGNAL(signalSliderChanged(int)), this, SLOT(slotSliderChanged(int)) );
	connect( ui.m_PlayControlWidget, SIGNAL(signalUpdateControlsTimeout()), this, SLOT(slotUpdateControlsTimeout()) );

	connect( ui.m_RenderWidget, SIGNAL(signalLeftMouseButtonClick()), this, SLOT(slotLeftMouseButtonClick()));

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

	connect( menuButton, SIGNAL(signalMenuItemSelected(int,EMenuItemType)),
		this, SLOT(slotMenuItemSelected(int,EMenuItemType)) );
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
	if( cbIdx < m_RecentFiles.size() )
	{
		std::string fullFileName = m_RecentFiles.getFileAtIndex( cbIdx );
		if( !fullFileName.empty() )
		{
			playSelectedMovie( fullFileName );
		}
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
	QString launchParam( "Single File" );
	ActivityBase* actBase = m_MyApp.getAppletMgr().launchApplet( eAppletBrowseFiles, getParentPageFrame(), launchParam);
    AppletBrowseFiles* fileBrowser = dynamic_cast<AppletBrowseFiles*>(actBase);
    if( fileBrowser )
    {
		std::string fullFileName = ui.m_LastPlayedFileText->text().toUtf8().constData();
		if( !fullFileName.empty() )
		{
			std::string filePath;
			std::string	fileName;
                                                        
			VxFileUtil::seperatePathAndFile( fullFileName.c_str(), filePath, fileName );
			fileBrowser->setCurrentDirectory( filePath.c_str() );
		}
		else
		{
			fileBrowser->setCurrentDirectory( VxGetAppDirectory( eAppDirRootDataStorage ).c_str() );
		}

		fileBrowser->setFileFilter( eFileFilterVideo );
		connect( fileBrowser, SIGNAL(signalFileWasSelected(QString)), this, SLOT(slotFileWasSelected(QString)) );
    }
}

//============================================================================
void AppletPlayerNlc::onFileSelected( FileInfo& fileInfo )
{
	if( VXFILE_TYPE_AUDIO == fileInfo.getFileType() || VXFILE_TYPE_VIDEO == fileInfo.getFileType() )
	{
		std::string fullFileName = fileInfo.getFileName();

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
	m_MediaPlayerReady = isReady;
	updateRecentListVisibility();
}

//============================================================================
void AppletPlayerNlc::slotReplayButtonClick( void )
{
	std::string movieFile = ui.m_LastPlayedFileText->text().toUtf8().constData();
	if( VxFileUtil::fileExists( movieFile.c_str() ) )
	{
		playSelectedMovie( movieFile );
	}
	else if( ui.m_FilesComboBox->currentIndex() >= 0 && ui.m_FilesComboBox->currentIndex() < m_RecentFiles.size() )
	{
		playSelectedMovie( m_RecentFiles.getFileAtIndex( ui.m_FilesComboBox->currentIndex() ) );
	}
}

//============================================================================
void AppletPlayerNlc::playSelectedMovie( std::string fullFileName )
{
	stopMediaIfPlaying();
	QString fileName = fullFileName.c_str();
	if( VxFileUtil::fileExists( fullFileName.c_str() ) )
	{
		startBusySpinner( getPlayControlWidget() );

		m_RecentFiles.removeFile( fullFileName );
		m_RecentFiles.addFileToFront( fullFileName );
        //m_RecentFiles.dumpToLog( LOG_VERBOSE );
		refreshRecentFilesComboBox();

		ui.m_LastPlayedFileText->setText( fileName );
		m_MyApp.getAppSettings().setLastPlayedMovie( fullFileName );
		playFile( fileName, 0, false, false );
	}
	else
	{
		QMessageBox::information( this, QObject::tr( "File does not exist" ), fileName, QMessageBox::Ok );
	}
}

//============================================================================
int AppletPlayerNlc::addMediaFilesToRecentList( QDir& mediaDir )
{
	int addedFileCnt{ 0 };
	mediaDir.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks | QDir::Readable );
	mediaDir.setSorting( QDir::Name );
	QFileInfoList fileList = mediaDir.entryInfoList();
	for (int i = 0; i < fileList.size(); ++i) 
	{
        const QFileInfo fileInfo = fileList.at( i );
		std::string fileName = fileInfo.absoluteFilePath().toUtf8().constData();
		if( !fileName.empty() && m_RecentFiles.addFileToBack( fileName ) )
		{
			addedFileCnt++;
		}
    }

	return addedFileCnt;
}

//============================================================================
void AppletPlayerNlc::refreshRecentFilesComboBox( void )
{
	ui.m_FilesComboBox->blockSignals(true);

	ui.m_FilesComboBox->clear();

	for( auto fullFileName : m_RecentFiles.getFileList() )
	{
		std::string filePath;
		std::string	fileName;
                                                        
		VxFileUtil::seperatePathAndFile( fullFileName.c_str(), filePath, fileName );

		ui.m_FilesComboBox->addItem( fileName.c_str() );
	}

	ui.m_FilesComboBox->setCurrentIndex( 0 );
	updateRecentListVisibility();

	ui.m_FilesComboBox->blockSignals(false);
}

//============================================================================
bool AppletPlayerNlc::isMediaPlayerReady( bool notifyIfNotReady ) 
{ 
	if( !m_MediaPlayerReady && notifyIfNotReady )
	{
		QMessageBox::information( this, QObject::tr( "Media Player not read" ), QObject::tr( "Try again when Media Player is ready" ), QMessageBox::Ok );
	}

	return m_MediaPlayerReady;  
}

//============================================================================
void AppletPlayerNlc::updateRecentListVisibility( void )
{
	if( m_RecentFiles.size() && m_MediaPlayerReady )
	{
		ui.m_FilesComboBox->setVisible( true );
	}
	else
	{
		ui.m_FilesComboBox->setVisible( false );
	}

	if( m_MediaPlayerReady )
	{
		ui.m_BrowseButton->setVisible( true );
		ui.m_ReplayButton->setVisible( true );
        ui.m_OpenVideoFileButton->setVisible( true );
        ui.m_OpenAudioFileButton->setVisible( true );
	}
}

//============================================================================
void AppletPlayerNlc::slotFileWasSelected( QString fileName )
{
	playSelectedMovie(fileName.toUtf8().constData());
}

//============================================================================
void AppletPlayerNlc::slotOpenVideoFileButtonClick( void )
{
    std::string addFileDir;
    m_MyApp.getAppSettings().getLastAddFileDir( addFileDir );
    QString curDir;
    if( !addFileDir.empty() )
    {
        curDir = addFileDir.c_str();
    }
    
    std::string fileName;
    if( GuiHelpers::browseForFile( this, eMediaFileVideo, fileName, curDir ) )
    {
        if( VxFileUtil::fileExists( fileName.c_str() ) )
        {
            std::string filePath;
            std::string justFileName;
            VxFileUtil::seperatePathAndFile(fileName, filePath, justFileName );
            m_MyApp.getAppSettings().setLastAddFileDir( filePath );
            m_MyApp.getEngine().fromGuiSetFileIsInLibrary( fileName, true );
            
            playSelectedMovie( fileName );
        }
    }
}

//============================================================================
void AppletPlayerNlc::slotOpenAudioFileButtonClick( void )
{
    std::string addFileDir;
    m_MyApp.getAppSettings().getLastAddFileDir( addFileDir );
    QString curDir;
    if( !addFileDir.empty() )
    {
        curDir = addFileDir.c_str();
    }
    
    std::string fileName;
    if( GuiHelpers::browseForFile( this, eMediaFileAudio, fileName, curDir ) )
    {
        if( VxFileUtil::fileExists( fileName.c_str() ) )
        {
            std::string filePath;
            std::string justFileName;
            VxFileUtil::seperatePathAndFile(fileName, filePath, justFileName );
            m_MyApp.getAppSettings().setLastAddFileDir( filePath );
            m_MyApp.getEngine().fromGuiSetFileIsInLibrary( fileName, true );
            
            playSelectedMovie( fileName );
        }
    }
}

//============================================================================
void AppletPlayerNlc::stopMediaIfPlaying( void )
{
    AppletPlayerNlcBase::stopMediaIfPlaying();
    IMediaPlayerRequests::getNlcPlayer().fromGuiStopButtonClicked();
}
