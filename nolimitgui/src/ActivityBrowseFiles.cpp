//============================================================================
// Copyright (C) 2010 Brett R. Jones 
// 
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityBrowseFiles.h"
#include "ActivityMsgBoxYesNo.h"
#include "AppCommon.h"
#include "AppSettings.h"

#include "FileShareItemWidget.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileLists.h>
#include <CoreLib/VxGlobals.h>

#include <QDesktopServices>
#include <QUrl>
#include <QTimer>
#include <QFileDialog>

#include "ui_ActivityBrowseFiles.h"

TitleBarWidget *  ActivityBrowseFiles::getTitleBarWidget( void ) { return ui.m_TitleBarWidget; }
BottomBarWidget * ActivityBrowseFiles::getBottomBarWidget( void ) { return ui.m_BottomBarWidget; }

//============================================================================
ActivityBrowseFiles::ActivityBrowseFiles( AppCommon& app, EFileFilterType fileFilter, QWidget* parent, bool isSelectAFileMode )
: ActivityBase( OBJNAME_ACTIVITY_BROWSE_FILES, app, parent, eActivityBrowseFiles, true )
, ui(*(new Ui::BrowseFilesWidget))
, m_WidgetClickEventFixTimer( new QTimer( this ) )
, m_bFetchInProgress( false )
, m_IsSelectAFileMode( isSelectAFileMode )
, m_eFileFilterType( fileFilter )
{
	ui.setupUi( this );
    setFileFilter( m_eFileFilterType );

	setTitleBarText( QObject::tr("Browse Device Files") );
	ui.m_DoubleTapInstructionLabel->setVisible( m_IsSelectAFileMode );
    connectBarWidgets();

	ui.m_UpDirectoryButton->setIcon( eMyIconMoveUpDirNormal );
	ui.m_UpDirectoryButton->setSquareButtonSize( eButtonSizeMedium );
	ui.m_BrowseButton->setIcon( eMyIconFileBrowseNormal );
	ui.m_BrowseButton->setSquareButtonSize( eButtonSizeMedium );
	ui.m_AddAllButton->setIcon( eMyIconLibraryCancel );
	ui.m_AddAllButton->setSquareButtonSize( eButtonSizeMedium );
	if( m_IsSelectAFileMode )
	{
		ui.m_AddAllButton->setVisible( false );
		ui.m_AddAllLabel->setVisible( false );
	}

	m_WidgetClickEventFixTimer->setInterval( 10 );
	connect( m_WidgetClickEventFixTimer, SIGNAL(timeout()), this, SLOT(slotRequestFileList()) );

    connect( ui.FileItemList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slotListItemClicked(QListWidgetItem*)));
    connect( ui.FileItemList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(slotListItemDoubleClicked(QListWidgetItem*)));

    connect( ui.m_UpDirectoryLabel,		SIGNAL(clicked()), this, SLOT(slotUpDirectoryClicked()));

    connect( ui.m_UpDirectoryButton,    SIGNAL(clicked()), this, SLOT(slotUpDirectoryClicked()));

    connect( ui.m_BrowseButton,         SIGNAL(clicked()), this, SLOT(slotBrowseButtonClicked()));

    connect( ui.m_AddAllButton,         SIGNAL(clicked()), this, SLOT(slotAddAllButtonClicked()) );


	setFileFilter( m_eFileFilterType );
	
    wantFileXferCallbacks( true );
	setActionEnable( false );

	connect( ui.m_FileFilterSelectWidget, SIGNAL(signalFileFilterChanged(EFileFilterType)), this,  SLOT(slotApplyFileFilter(EFileFilterType)) );
	// will be requested when setCurrentBrowseDir is called // slotRequestFileList();
	setCurrentBrowseDir( getAppletFolder( getAppletType(), m_eFileFilterType ).c_str() );
}

//============================================================================
ActivityBrowseFiles::~ActivityBrowseFiles()
{
	wantFileXferCallbacks( false );
	clearFileList();
}

//============================================================================
void ActivityBrowseFiles::showEvent( QShowEvent* ev )
{
	ActivityBase::showEvent( ev );
	repositionToParent();
}

//============================================================================
void ActivityBrowseFiles::hideEvent( QHideEvent* ev )
{
	wantFileXferCallbacks( false );
	ActivityBase::hideEvent( ev );
}

//============================================================================
void ActivityBrowseFiles::callbackToGuiFileList( VxGUID& appInstId, FileInfo& fileInfo )
{
	if( appInstId == getAppletInstId() )
	{
		addFile( fileInfo );
	}
}

//============================================================================
void ActivityBrowseFiles::callbackToGuiFileListCompleted( VxGUID& appInstId )
{
	if( appInstId == getAppletInstId() )
	{
		setActionEnable( true );
		bool hasFilesInList{ false };
		for( int i = 0; i < ui.FileItemList->count(); i++ )
		{
			QListWidgetItem* itemInList = ui.FileItemList->item( i );
			FileItemInfo* poInfo = ((FileShareItemWidget*)itemInList)->getFileItemInfo();
			if( poInfo->isDirectory() )
			{
				continue;
			}

			if( poInfo->getFileType() == VXFILE_TYPE_EXECUTABLE )
			{
				continue;
			}

			hasFilesInList = true;
			break;
		}

		showAddAllToLibrary( hasFilesInList );
	}
}

//============================================================================
void ActivityBrowseFiles::toGuiFileDeleted( QString& fileName )
{
    updateStorageSpace( fileName.toUtf8().constData() );
}

//============================================================================
void ActivityBrowseFiles::setCurrentBrowseDir( QString browseDir )
{
	if( !browseDir.isEmpty() )
	{
		m_CurBrowseDirectory = browseDir.toUtf8().constData();
		ui.m_CurDirLabel->setTextBreakAnywhere( m_CurBrowseDirectory.c_str(), 4 );
		updateStorageSpace( m_CurBrowseDirectory );
		slotRequestFileList();
	}
}

//============================================================================
void ActivityBrowseFiles::setFileFilter( EFileFilterType eFileFilter )
{
	m_eFileFilterType = eFileFilter;
    ui.m_FileFilterSelectWidget->setFileFilter( eFileFilter );
}

//============================================================================
FileShareItemWidget* ActivityBrowseFiles::fileToWidget( FileInfo& fileInfo )
{
	FileShareItemWidget* item = new FileShareItemWidget(ui.FileItemList);
	item->setSizeHint(QSize( (int)(GuiParams::getGuiScale() * 200),
                             (int)(62 * GuiParams::getGuiScale()) ) );

	FileItemInfo* poItemInfo = new FileItemInfo( fileInfo );

	item->setFileItemInfo( poItemInfo );
	item->setSelectAFileMode( m_IsSelectAFileMode );

	connect(	item, SIGNAL(signalFileShareItemClicked(QListWidgetItem*)), 
				this, SLOT(slotListItemClicked(QListWidgetItem*)) );

	connect(	item, SIGNAL(signalFileIconClicked(QListWidgetItem*)), 
				this, SLOT(slotListFileIconClicked(QListWidgetItem*)) );

	connect(	item, SIGNAL(signalPlayButtonClicked(QListWidgetItem*)), 
				this, SLOT(slotListPlayIconClicked(QListWidgetItem*)) );

	connect(	item, SIGNAL(signalPlayExternButtonClicked(QListWidgetItem*)), 
				this, SLOT(slotListPlayExternIconClicked(QListWidgetItem*)) );

	connect(	item, SIGNAL(signalLibraryButtonClicked(QListWidgetItem*)), 
				this, SLOT(slotListLibraryIconClicked(QListWidgetItem*)) );

	connect(	item, SIGNAL(signalFileShareButtonClicked(QListWidgetItem*)), 
				this, SLOT(slotListShareFileIconClicked(QListWidgetItem*)) );

	connect(	item, SIGNAL(signalShredButtonClicked(QListWidgetItem*)), 
				this, SLOT(slotListShredIconClicked(QListWidgetItem*)) );

	item->updateWidgetFromInfo();

	return item;
}

//============================================================================
void ActivityBrowseFiles::addFile( FileInfo& fileInfo )
{
	if( fileExistsInList( fileInfo.getFileNameAndPath().c_str() ) )
	{
		return;
	}

	FileShareItemWidget* item = fileToWidget( fileInfo );
	if( item )
	{
		if( fileInfo.isDirectory() )
		{
			if( 0 == ui.FileItemList->count() )
			{
				LogMsg( LOG_INFO, "add directory %s", fileInfo.getFileNameAndPath().c_str() );
				ui.FileItemList->addItem( item );
			}
			else
			{
				LogMsg( LOG_INFO, "insert 0 directory %s", fileInfo.getFileNameAndPath().c_str() );
				ui.FileItemList->insertItem( 0, (QListWidgetItem*)item );
			}
		}
		else
		{
			bool itemInserted = false;
			QString justFileName = fileInfo.getFileName().c_str();
			for(int i = 0; i < ui.FileItemList->count(); i++ )
			{
				QListWidgetItem* itemInList = ui.FileItemList->item(i);
				FileItemInfo* poInfo = ((FileShareItemWidget*)itemInList)->getFileItemInfo();
				if( poInfo->isDirectory() )
				{
					continue;
				}

				if( poInfo->getFileName() > justFileName )
				{
					itemInserted = true;
					LogMsg( LOG_INFO, "inserted %d file %s", i, fileInfo.getFileName().c_str() );
					ui.FileItemList->insertItem( i, (QListWidgetItem*)item );
					break;
				}
			}

			if( false == itemInserted )
			{
				LogMsg( LOG_INFO, "add file %s", fileInfo.getFileName().c_str() );
				ui.FileItemList->addItem( item );
			}
		}

		ui.FileItemList->setItemWidget( (QListWidgetItem*)item, (QWidget*)item );
	}
}

//============================================================================
void ActivityBrowseFiles::slotUpDirectoryClicked( void )
{
	if( m_CurBrowseDirectory.length() > 1 )
	{
		// move up one directory
		char * pBuf = new char[ m_CurBrowseDirectory.length() + 1 ];
		strcpy( pBuf, m_CurBrowseDirectory.c_str() );
		if( '/' == pBuf[ m_CurBrowseDirectory.length() - 1 ] )
		{
			pBuf[ m_CurBrowseDirectory.length() - 1 ] = 0;
		}

		char * pTemp = strrchr( pBuf, '/' );
		if( pTemp )
		{
			pTemp++;
			pTemp[0] = 0;
		}

		m_CurBrowseDirectory = pBuf;
		VxFileUtil::assureTrailingDirectorySlash( m_CurBrowseDirectory );
        delete[] pBuf;
		setActionEnable( false );
		slotRequestFileList();
	}
}

//============================================================================
void ActivityBrowseFiles::slotBrowseButtonClicked( void )
{
	QString curDir = m_CurBrowseDirectory.c_str();

	std::string selectedDir = GuiHelpers::browseForDirectory( curDir, this );

	if( !selectedDir.empty() )
	{
		setCurrentBrowseDir( selectedDir.c_str() );
		setActionEnable( false );

		setAppletFolder( getAppletType(), m_eFileFilterType, selectedDir );

		slotRequestFileList();
	}
}

//============================================================================
void ActivityBrowseFiles::slotAddAllButtonClicked( void )
{
	bool acceptAction = true;

	QString title = QObject::tr( "Confirm add all Files to library" );
	QString bodyText = QObject::tr( "Do you want to add All the files in the list to the library?" );

	ActivityMsgBoxYesNo dlg( m_MyApp, &m_MyApp, title, bodyText );
	if( false == (QDialog::Accepted == dlg.exec()) )
	{
		acceptAction = false;
	}

	if( acceptAction )
	{
		for( int i = 0; i < ui.FileItemList->count(); i++ )
		{
			QListWidgetItem* itemInList = ui.FileItemList->item( i );
			FileItemInfo* poInfo = ((FileShareItemWidget*)itemInList)->getFileItemInfo();
			if( poInfo->isDirectory() )
			{
				continue;
			}

			if( poInfo->getFileType() == VXFILE_TYPE_EXECUTABLE )
			{
				continue;
			}

			m_MyApp.getEngine().fromGuiSetFileIsInLibrary( poInfo->getFileInfo(), true );
		}
	}
}

//============================================================================
void ActivityBrowseFiles::slotListItemClicked( QListWidgetItem* item )
{
	if( 1000 > m_ClickToFastTimer.elapsedMs() )
	{
		// in windows if user clicks to fast on some files the app will crash.. don't know why
		return;
	}

	FileItemInfo* poInfo = ((FileShareItemWidget*)item)->getFileItemInfo();
	if( poInfo )
	{
		if( VXFILE_TYPE_DIRECTORY == poInfo->getFileType() )
		{
            if( ! poInfo->getFileNameAndPath().isEmpty() )
			{
                m_CurBrowseDirectory =  poInfo->getFileNameAndPath().toUtf8().constData();
				VxFileUtil::assureTrailingDirectorySlash( m_CurBrowseDirectory );
				m_WidgetClickEventFixTimer->start(10);
			}
		}
		else
		{
			// is file
			if( m_IsSelectAFileMode )
			{
				// dont know why but get single click then double click so just send on to double click
				slotListItemDoubleClicked( item );
				return;
			}

			m_ClickToFastTimer.startTimer();
			if( poInfo->shouldOpenFile() )
			{
				this->playFile( poInfo->getFileNameAndPath(), 0, false, false );
			}
		}
	}
}

//============================================================================
void ActivityBrowseFiles::slotListItemDoubleClicked( QListWidgetItem* item )
{
	if( 1000 > m_ClickToFastTimer.elapsedMs() )
	{
		// in windows if user clicks to fast on some files the app will crash.. don't know why
		return;
	}

	if( m_IsSelectAFileMode )
	{
		FileItemInfo* poInfo = ((FileShareItemWidget*)item)->getFileItemInfo();
		if( poInfo )
		{
			if( VXFILE_TYPE_DIRECTORY == poInfo->getFileType() )
			{
				slotListItemClicked( item ); 
			}
			else
			{
				// is file
				m_ClickToFastTimer.startTimer();
				if( poInfo->shouldOpenFile() )
				{
					m_FileWasSelected			= true;
					m_SelectedFileInfo			= poInfo->getFileInfo();

					m_MyApp.activityStateChange( this, false );
					wantActivityCallbacks( false );					
					wantFileXferCallbacks( false );

					delayedCloseApplet();
				}
			}
		}
	}
	else
	{
		slotListItemClicked( item );
	}
}

//============================================================================
void ActivityBrowseFiles::slotListFileIconClicked( QListWidgetItem* item )
{
	slotListItemClicked( item ); 
}

//============================================================================
void ActivityBrowseFiles::slotListShareFileIconClicked( QListWidgetItem* item )
{
	FileItemInfo* poInfo = ((FileShareItemWidget*)item)->getFileItemInfo();
	if( poInfo )
	{
		if( VXFILE_TYPE_DIRECTORY == poInfo->getFileType() )
		{
			if( false == m_bFetchInProgress )
			{
				std::string strDir = poInfo->getFileNameAndPath().toUtf8().constData();
				if( strDir.length() )
				{
					m_CurBrowseDirectory = strDir;
					VxFileUtil::assureTrailingDirectorySlash( m_CurBrowseDirectory );
					setActionEnable( false );
					fromListWidgetRequestFileList();
				}
			}
		}
		else
		{
			// is file
			poInfo->toggleIsShared();
			((FileShareItemWidget*)item)->updateWidgetFromInfo();
            m_Engine.fromGuiSetFileIsShared( poInfo->getFileInfo(), poInfo->getIsSharedFile() );
		}
	}	
}

//============================================================================
void ActivityBrowseFiles::slotListLibraryIconClicked( QListWidgetItem* item )
{
	FileItemInfo* poInfo = ((FileShareItemWidget*)item)->getFileItemInfo();
	if( poInfo )
	{
		if( VXFILE_TYPE_DIRECTORY == poInfo->getFileType() )
		{
			if( false == m_bFetchInProgress )
			{
				std::string strDir = poInfo->getFileNameAndPath().toUtf8().constData();
				if( strDir.length() )
				{
					m_CurBrowseDirectory = strDir;
					VxFileUtil::assureTrailingDirectorySlash( m_CurBrowseDirectory );
					setActionEnable( false );
					fromListWidgetRequestFileList();
				}
			}
		}
		else
		{
			// is file
			poInfo->toggleIsInLibrary();
			((FileShareItemWidget*)item)->updateWidgetFromInfo();
			m_Engine.fromGuiSetFileIsInLibrary( poInfo->getFileInfo(), poInfo->getIsInLibrary() );
		}
	}	
}

//============================================================================
void ActivityBrowseFiles::slotListPlayIconClicked( QListWidgetItem* item )
{
	FileItemInfo* poInfo = ((FileShareItemWidget*)item)->getFileItemInfo();
	if( poInfo )
	{
		if( m_IsSelectAFileMode )
		{
			slotListItemDoubleClicked( item );
		}
		else if( VXFILE_TYPE_DIRECTORY == poInfo->getFileType() )
		{
			if( false == m_bFetchInProgress )
			{
				std::string strDir = poInfo->getFileNameAndPath().toUtf8().constData();
				if( strDir.length() )
				{
					m_CurBrowseDirectory = strDir;
					VxFileUtil::assureTrailingDirectorySlash( m_CurBrowseDirectory );
					setActionEnable( false );
					fromListWidgetRequestFileList();
				}
			}
		}
		else
		{
			// play file
			this->playFile( poInfo->getFileNameAndPath(), 0, false, false );
		}
	}	
}

//============================================================================
void ActivityBrowseFiles::slotListPlayExternIconClicked( QListWidgetItem* item )
{
	FileItemInfo* poInfo = ((FileShareItemWidget*)item)->getFileItemInfo();
	if( poInfo )
	{
		if( VXFILE_TYPE_DIRECTORY == poInfo->getFileType() )
		{
			if( false == m_bFetchInProgress )
			{
				std::string strDir = poInfo->getFileNameAndPath().toUtf8().constData();
				if( strDir.length() )
				{
					m_CurBrowseDirectory = strDir;
					VxFileUtil::assureTrailingDirectorySlash( m_CurBrowseDirectory );
					setActionEnable( false );
					fromListWidgetRequestFileList();
				}
			}
		}
		else
		{
			// play file
			this->playFile( poInfo->getFileNameAndPath(), 0, false, true );
		}
	}	
}

//============================================================================
void ActivityBrowseFiles::slotListShredIconClicked( QListWidgetItem* item )
{
	FileItemInfo* poInfo = ((FileShareItemWidget*)item)->getFileItemInfo();
	if( poInfo )
	{
		if( VXFILE_TYPE_DIRECTORY == poInfo->getFileType() )
		{
			if( false == m_bFetchInProgress )
			{
				std::string strDir = poInfo->getFileNameAndPath().toUtf8().constData();
				if( strDir.length() )
				{
					m_CurBrowseDirectory = strDir;
					VxFileUtil::assureTrailingDirectorySlash( m_CurBrowseDirectory );
					setActionEnable( false );
					fromListWidgetRequestFileList();
				}
			}
		}
		else
		{
			// shred file
			QString fileName = poInfo->getFileNameAndPath();
			if( confirmDeleteFile( fileName, true ) )
			{
				ui.FileItemList->removeItemWidget( item );
				m_Engine.fromGuiDeleteFile( fileName.toUtf8().constData(), true );
			}
		}
	}	
}

//============================================================================
void ActivityBrowseFiles::setActionEnable( bool enable )
{
	m_bFetchInProgress = enable ? false : true;

	LogMsg( LOG_INFO, "Fetch In Progress %d", m_bFetchInProgress );
	ui.m_UpDirectoryButton->setEnabled( enable );
}

//============================================================================
void ActivityBrowseFiles::fromListWidgetRequestFileList( void )
{
	// there is a problem in that if we clear the list in a click event from a widget in the list
	// then causes crash so emit timed event to let the click event finish
	m_WidgetClickEventFixTimer->start();
}

//============================================================================
void ActivityBrowseFiles::slotApplyFileFilter( EFileFilterType fileFilter )
{
	m_eFileFilterType = fileFilter;
	m_CurBrowseDirectory = getAppletFolder( getAppletType(), fileFilter );
	slotRequestFileList();
}

//============================================================================
void ActivityBrowseFiles::slotRequestFileList( void )
{
	m_WidgetClickEventFixTimer->stop();
	clearFileList();

	ui.m_CurDirLabel->setText( m_CurBrowseDirectory.c_str() );
	setAppletFileFilter( getAppletType(), m_eFileFilterType );
	setAppletFolder( getAppletType(), m_eFileFilterType, m_CurBrowseDirectory );
	m_Engine.getFromGuiInterface().fromGuiBrowseFiles( getAppletInstId(), m_CurBrowseDirectory, FileFilterToVxFileType( m_eFileFilterType ) | VXFILE_TYPE_DIRECTORY );
}

//============================================================================
void ActivityBrowseFiles::clearFileList( void )
{
	for(int i = 0; i < ui.FileItemList->count(); ++i)
	{
		QListWidgetItem* item = ui.FileItemList->item(i);
		delete ((FileShareItemWidget*)item);
	}
	
	ui.FileItemList->clear();
}

//============================================================================
void ActivityBrowseFiles::showAddAllToLibrary( bool visible )
{
	ui.m_AddAllWidget->setVisible( visible );
}

//============================================================================
void ActivityBrowseFiles::updateStorageSpace( std::string fileName )
{
	uint64_t diskFreeSpace = m_Engine.fromGuiGetDiskFreeSpace( fileName.c_str() );
	if( (0 != diskFreeSpace) && (diskFreeSpace < 1000000000) )
	{
		m_MyApp.toGuiUserMessage( "Storage Space is low %s", GuiParams::describeFileLength( diskFreeSpace ).toUtf8().constData() );
	}

	ui.m_StatusLabel->setText( QObject::tr( "Storage Space Available: " ) + GuiParams::describeFileLength( diskFreeSpace ) );
}

//============================================================================
bool ActivityBrowseFiles::fileExistsInList( QString fileName )
{
	for(int i = 0; i < ui.FileItemList->count(); ++i)
	{
		QListWidgetItem* item = ui.FileItemList->item(i);
		FileItemInfo * poInfo = ((FileShareItemWidget*)item)->getFileItemInfo();
		if( poInfo && poInfo->getFileNameAndPath() == fileName )
		{
			return true;
		}
	}

	return false;
}

//============================================================================
void ActivityBrowseFiles::wantFileXferCallbacks( bool enable )
{
	if( enable != m_FileXferCallbacksRequested )
	{
		m_FileXferCallbacksRequested = enable;
		m_MyApp.getFileXferMgr().wantToGuiFileXferCallbacks( this, enable );
	}
}