//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletLibrary.h"

#include "ActivityBrowseFiles.h"
#include "ActivityMsgBoxYesNo.h"

#include "AppCommon.h"
#include "AppGlobals.h"
#include "AppSettings.h"

#include "AppletAboutFile.h"
#include "AppletMgr.h"

#include "FileShareItemWidget.h"
#include "FileItemInfo.h"
#include "FileActionMenu.h"
#include "GuiHelpers.h"
#include "GuiParams.h"
#include "GuiPlayerMgr.h"

#include <AssetBase/AssetPlaySession.h>
#include <AssetMgr/AssetMgr.h>
#include <AssetMgr/AssetInfo.h>
#include <P2PEngine/P2PEngine.h>

#include <PktLib/VxSearchDefs.h>
#include <NetLib/VxFileXferInfo.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxFileInfo.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletLibrary.h"

//============================================================================
AppletLibrary::AppletLibrary( AppCommon& app, QWidget* parent, QString launchParam )
    : AppletBase( OBJNAME_APPLET_LIBRARY, app, parent )
    , ui(*(new Ui::AppletLibraryUi))
    , m_ePluginType( ePluginTypeInvalid )
    , m_IsSelectAFileMode( !launchParam.isEmpty() ? true : false )
{
    setAppletType( eAppletLibrary );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    m_eFileFilterType = getAppletFileFilter( getAppletType() );
    setFileFilter( m_eFileFilterType );

    ui.m_DoubleTapInstructionLabel->setVisible( m_IsSelectAFileMode );

	ui.m_AddFileButton->setIcon( eMyIconLibraryCancel );
	ui.m_AddFileButton->setSquareButtonSize( eButtonSizeMedium );
	ui.m_BrowseButton->setIcon( eMyIconFileAdd );
	ui.m_BrowseButton->setSquareButtonSize( eButtonSizeMedium );

    connect( ui.m_AddFileButton, SIGNAL(clicked()), this, SLOT(slotAddFileButtonClicked()) );
    connect( ui.m_AddFileLabel, SIGNAL(clicked()), this, SLOT(slotAddFileLabelClicked()) );
    connect( ui.m_BrowseButton, SIGNAL(clicked()), this, SLOT(slotBrowseButtonClicked()) );
    connect( ui.m_BrowseFilesLabel, SIGNAL(clicked()), this, SLOT(slotBrowseLabelClicked()) );
    ui.m_BrowseFrame->setVisible( false ); // might remove browse altogether if select media browse works out

    connect( ui.m_FileFilterSelectWidget, SIGNAL(signalFileFilterChanged(EFileFilterType)), this, SLOT(slotApplyFileFilter(EFileFilterType)) );

    connect( ui.m_FileMediaSelectWidget, SIGNAL(signalFileMediaSelected(EMediaFileType)), this, SLOT(slotFileMediaSelected(EMediaFileType)) );

    statusMsg( "Requesting Library File List " );

    slotApplyFileFilter( m_eFileFilterType );
    connectBarWidgets();

    wantFileXferCallbacks( true );
    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletLibrary::~AppletLibrary()
{
    wantFileXferCallbacks( false );
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletLibrary::showEvent( QShowEvent* ev )
{
    AppletBase::showEvent( ev );
    m_MyApp.setIsLibraryActivityActive( true );
    wantFileXferCallbacks( true );
    slotRequestFileList();
}

//============================================================================
void AppletLibrary::hideEvent( QHideEvent* ev )
{
    wantFileXferCallbacks( false );
    AppletBase::hideEvent( ev );
    m_MyApp.setIsLibraryActivityActive( false );
}

//============================================================================
void AppletLibrary::callbackToGuiFileList( VxGUID& appInstId, FileInfo& fileInfo )
{
    if( appInstId == getAppletInstId() )
    {
        if( ui.m_FileItemList->count() == 0 )
        {
            updateStorageSpace( fileInfo.getFileNameAndPath() );
        }

        addFile( fileInfo );
    }
}

//============================================================================
void AppletLibrary::callbackToGuiFileListCompleted( VxGUID& appInstId )
{
    if( appInstId == getAppletInstId() )
    {
        statusMsg( "List Get Completed" );
    }
}

//============================================================================
void AppletLibrary::toGuiFileDeleted( QString& fileName )
{
    FileShareItemWidget* poWidget;
    int iIdx = 0;
    while( iIdx < ui.m_FileItemList->count() )
    {
        poWidget = (FileShareItemWidget*)ui.m_FileItemList->item( iIdx );
        if( poWidget )
        {
            FileItemInfo* poFileInfo = (FileItemInfo*)poWidget->QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
            if( poFileInfo && (poFileInfo->getFileNameAndPath() == fileName) )
            {
                poWidget->deleteLater();
                break;
            }
        }

        iIdx++;
    }

    updateStorageSpace( fileName.toUtf8().constData() );
}

//============================================================================
void AppletLibrary::setFileFilter( EFileFilterType fileFilter )
{
    m_eFileFilterType = fileFilter;
    ui.m_FileFilterSelectWidget->setFileFilter( m_eFileFilterType );
}

//============================================================================
void AppletLibrary::slotApplyFileFilter( EFileFilterType fileFilter )
{
    m_eFileFilterType = fileFilter;
    slotRequestFileList();
}

//============================================================================
void AppletLibrary::slotRequestFileList( void )
{
    clearFileList();
    setAppletFileFilter( getAppletType(), m_eFileFilterType );
    m_FromGui.fromGuiGetFileLibraryList( getAppletInstId(), FileFilterToVxFileType( m_eFileFilterType ) );
}

//============================================================================
FileShareItemWidget* AppletLibrary::fileToWidget( FileInfo& fileInfo )
{
    FileShareItemWidget* item = new FileShareItemWidget( ui.m_FileItemList );
    item->setSizeHint( QSize( (int)(GuiParams::getGuiScale() * 200), GuiParams::getFileListEntryHeight() ) );

    FileItemInfo* poItemInfo = new FileItemInfo( fileInfo );
    item->QListWidgetItem::setData( Qt::UserRole + 1, QVariant( ( quint64 )poItemInfo ) );

    connect( item,
             SIGNAL(signalFileShareItemClicked(QListWidgetItem*)),
             this,
             SLOT(slotListItemClicked(QListWidgetItem*)) );

    connect( item,
             SIGNAL(signalFileIconClicked(QListWidgetItem*)),
             this,
             SLOT(slotListFileIconClicked(QListWidgetItem*)) );

    connect( item,
             SIGNAL(signalPlayButtonClicked(QListWidgetItem*)),
             this,
             SLOT(slotListPlayIconClicked(QListWidgetItem*)) );
        
    connect( item,
             SIGNAL(signalPlayExternButtonClicked(QListWidgetItem*)),
             this,
             SLOT(slotListPlayExternIconClicked(QListWidgetItem*)) );

    connect( item,
             SIGNAL(signalLibraryButtonClicked(QListWidgetItem*)),
             this,
             SLOT(slotListLibraryIconClicked(QListWidgetItem*)) );

    connect( item,
             SIGNAL(signalFileShareButtonClicked(QListWidgetItem*)),
             this,
             SLOT(slotListShareFileIconClicked(QListWidgetItem*)) );

    connect( item,
            SIGNAL(signalAboutFileButtonClicked(QListWidgetItem*)),
            this,
            SLOT(slotListAboutFileClicked(QListWidgetItem*)) );

    connect( item,
             SIGNAL(signalShredButtonClicked(QListWidgetItem*)),
             this,
             SLOT(slotListShredIconClicked(QListWidgetItem*)) );

    item->updateWidgetFromInfo();
    return item;
}

//============================================================================
void AppletLibrary::slotListFileIconClicked( QListWidgetItem* item )
{
    slotListPlayIconClicked( item );
}

//============================================================================
void AppletLibrary::slotListShareFileIconClicked( QListWidgetItem* item )
{
    FileItemInfo* poInfo = ( ( FileShareItemWidget* )item )->getFileItemInfo();
    if( poInfo )
    {
        if( VXFILE_TYPE_DIRECTORY == poInfo->getFileType() )
        {
        }
        else
        {
            // is file
            poInfo->toggleIsShared();
            ( ( FileShareItemWidget* )item )->updateWidgetFromInfo();
            m_Engine.fromGuiSetFileIsShared( poInfo->getFileInfo(), poInfo->getIsSharedFile() );
        }
    }
}

//============================================================================
void AppletLibrary::slotListAboutFileClicked( QListWidgetItem* item )
{
    FileItemInfo* poInfo = ( (FileShareItemWidget*)item )->getFileItemInfo();
    if( poInfo )
    {
        if( VXFILE_TYPE_DIRECTORY == poInfo->getFileType() )
        {
        }
        else
        {
            AppletAboutFile* aboutFile = dynamic_cast<AppletAboutFile*>( m_MyApp.getAppletMgr().launchApplet( eAppletAboutFile, getParentPageFrame() ) );
            if( aboutFile )
            {
                aboutFile->setFileInfo( poInfo->getFileInfo() );
            }
        }
    }
}

//============================================================================
void AppletLibrary::slotListLibraryIconClicked( QListWidgetItem* item )
{
    FileItemInfo* poInfo = ( ( FileShareItemWidget* )item )->getFileItemInfo();
    if( poInfo )
    {
        if( VXFILE_TYPE_DIRECTORY == poInfo->getFileType() )
        {
        }
        else
        {
            // is file
            poInfo->toggleIsInLibrary();
            ( ( FileShareItemWidget* )item )->updateWidgetFromInfo();
            m_Engine.fromGuiSetFileIsInLibrary( poInfo->getFileInfo(), poInfo->getIsInLibrary() );
        }
    }
}

//============================================================================
void AppletLibrary::slotListPlayIconClicked( QListWidgetItem* item )
{
    FileItemInfo* poInfo = ( ( FileShareItemWidget* )item )->getFileItemInfo();
    if( poInfo )
    {
        if( VXFILE_TYPE_DIRECTORY == poInfo->getFileType() )
        {
        }
        else
        {
            bool newAsset{ false };
            AssetInfo assetInfo;
            if( poInfo->toAsssetInfo( m_MyApp, assetInfo, &newAsset ) )
            {
                if( newAsset )
                {
                    assetInfo.setLocationFlags( ASSET_LOC_FLAG_LIBRARY );
                    m_MyApp.getEngine().fromGuiAssetAction( eAssetActionAddToAssetMgr, assetInfo );
                }

                AssetPlaySession playSession( assetInfo );
                m_MyApp.getPlayerMgr().playMedia( playSession, false );
            }
            else
            {
                QMessageBox::information( this, QObject::tr( "File Not Found" ), poInfo->getFileNameAndPath().toUtf8().constData(), QMessageBox::Ok );
                ui.m_FileItemList->removeItemWidget( item );
            }
        }
    }
}

//============================================================================
void AppletLibrary::slotListPlayExternIconClicked( QListWidgetItem* item )
{
    FileItemInfo* poInfo = ( ( FileShareItemWidget* )item )->getFileItemInfo();
    if( poInfo )
    {
        if( VXFILE_TYPE_DIRECTORY == poInfo->getFileType() )
        {
        }
        else
        {
            bool newAsset{ false };
            AssetInfo assetInfo;
            if( poInfo->toAsssetInfo( m_MyApp, assetInfo, &newAsset ) )
            {
                if( newAsset )
                {
                    assetInfo.setLocationFlags( ASSET_LOC_FLAG_LIBRARY );
                    m_MyApp.getEngine().fromGuiAssetAction( eAssetActionAddToAssetMgr, assetInfo );
                }

                AssetPlaySession playSession( assetInfo );
                m_MyApp.getPlayerMgr().playMedia( playSession, true );
            }
            else
            {
                QMessageBox::information( this, QObject::tr( "File Not Found" ), poInfo->getFileNameAndPath().toUtf8().constData(), QMessageBox::Ok );
                ui.m_FileItemList->removeItemWidget( item );
            }
        }
    }
}

//============================================================================
void AppletLibrary::slotListShredIconClicked( QListWidgetItem* item )
{
    FileItemInfo* poInfo = ( ( FileShareItemWidget* )item )->getFileItemInfo();
    if( poInfo )
    {
        if( VXFILE_TYPE_DIRECTORY == poInfo->getFileType() )
        {
        }
        else
        {
            // shred file
            AssetInfo assetInfo;
            if( poInfo->toAsssetInfo( m_MyApp, assetInfo ) )
            {
                if( confirmDeleteFile( assetInfo, true ) )
                {
                    ui.m_FileItemList->removeItemWidget( item );
                    m_Engine.fromGuiDeleteFile( assetInfo.getAssetNameAndPath().c_str(), true );
                }
            }
        }
    }
}

//============================================================================
//!	get friend from QListWidgetItem data
FileItemInfo* AppletLibrary::widgetToFileItemInfo( FileShareItemWidget* item )
{
    return ( FileItemInfo* )item->QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
}

//============================================================================
FileShareItemWidget* AppletLibrary::findListEntryWidget( FileInfo& fileInfo )
{
    int iIdx = 0;
    FileShareItemWidget* poWidget;
    while( iIdx < ui.m_FileItemList->count() )
    {
        poWidget = ( FileShareItemWidget* )ui.m_FileItemList->item( iIdx );
        if( poWidget )
        {
            FileItemInfo* poFileInfo = ( FileItemInfo* )poWidget->QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
            if( poFileInfo && ( poFileInfo->getFileInfo().getFileNameAndPath() == fileInfo.getFileNameAndPath() ) )
            {
                return poWidget;
            }
        }

        iIdx++;
    }

    return NULL;
}

//============================================================================
void AppletLibrary::slotAddFileButtonClicked( void )
{
    std::string addFileDir;
    m_MyApp.getAppSettings().getLastAddFileDir( addFileDir );
    QString curDir;
    if( !addFileDir.empty() )
    {
        curDir = addFileDir.c_str();
    }

    FileInfo fileInfo;
    if( GuiHelpers::browseForFile( this, eMediaFileAny, fileInfo, curDir ) )
    {
        std::string fileName = fileInfo.getFileNameAndPath();
        if( !fileInfo.getFilePath().empty() )
        {
            m_MyApp.getAppSettings().setLastAddFileDir( fileInfo.getFilePath() );
        }
        
        m_MyApp.getEngine().fromGuiSetFileIsInLibrary( fileInfo, true );

        // see if file is already a asset
        AssetMgr& assetMgr = m_MyApp.getEngine().getAssetMgr();
        assetMgr.lockResources();
        AssetBaseInfo* assetInfo = assetMgr.findAsset( fileInfo.getFileNameAndPath() );
        if( assetInfo )
        {
            if( assetInfo->isInLibrary() )
            {
                assetMgr.unlockResources();
                QMessageBox::information( this, QObject::tr("Already in library"), QObject::tr( "File is already in library " ), QMessageBox::Ok );
            }
            else
            {
                assetInfo->setIsInLibrary( true );
                FileInfo fileInfoInLibrary =  assetInfo->getFileInfo();
                assetMgr.unlockResources();

                addFile( fileInfoInLibrary );
            }
        }
        else
        {
            assetMgr.unlockResources();

            AssetInfo newAsset( fileInfo );
            newAsset.setIsInLibrary( true );

            AssetBaseInfo* createdAsset{ nullptr };
            bool result = assetMgr.addAsset( newAsset, createdAsset );
            if( result && createdAsset )
            {
                FileInfo fileInfoInLibrary = createdAsset->getFileInfo();
                addFile( fileInfoInLibrary );
            }
            else
            {
                QMessageBox::information( this, QObject::tr("File Error"), QObject::tr( "Could not add file to library " ), QMessageBox::Ok );
            }
        }
    }
}

//============================================================================
void AppletLibrary::slotAddFileLabelClicked( void )
{
    ui.m_AddFileButton->emulateUserClicked();
}

//============================================================================
void AppletLibrary::slotBrowseButtonClicked( void )
{
    ActivityBrowseFiles dlg( m_MyApp, m_eFileFilterType, this );
    dlg.exec();
    clearFileList();
    statusMsg( "Requesting Library File List " );
    m_FromGui.fromGuiGetFileLibraryList( getAppletInstId(), FileFilterToVxFileType( m_eFileFilterType ) );
}

//============================================================================
void AppletLibrary::slotBrowseLabelClicked( void )
{
    ui.m_BrowseButton->emulateUserClicked();
}

//============================================================================
void AppletLibrary::addFile( FileInfo& fileInfo )
{
    FileShareItemWidget* existingItem = findItemByFileName( fileInfo.getFileNameAndPath().c_str() );
    if( existingItem )
    {
        FileItemInfo* poItemInfo = existingItem->getFileItemInfo();
        if( poItemInfo )
        {
            existingItem->update();
        }
    }

    if( !existingItem )
    {
        FileShareItemWidget* item = fileToWidget( fileInfo );
        if( item )
        {
            //LogMsg( LOG_INFO, "AppletLibrary::addFile: adding widget");
            ui.m_FileItemList->addItem( item );
            ui.m_FileItemList->setItemWidget( item, item );
        }
    }
}

//============================================================================
//! user selected menu item
void AppletLibrary::slotListItemClicked( QListWidgetItem* item )
{
    FileItemInfo* poInfo = ( FileItemInfo* )item->data( Qt::UserRole + 1 ).toLongLong();
    if( poInfo )
    {
        FileInfo& fileInfo = poInfo->getFileInfo();
        if( m_IsSelectAFileMode )
        {
            m_FileWasSelected = true;
            m_SelectedFileType = fileInfo.getFileType();
            m_SelectedFileName = fileInfo.getFileNameAndPath().c_str();
            m_SelectedFileLen = fileInfo.getFileLength();
            m_SelectedFileIsShared = poInfo->getIsSharedFile();
            m_SelectedFileIsInLibrary = poInfo->getIsInLibrary();
            accept();
        }
        else
        {
            playFile( fileInfo.getFileNameAndPath().c_str(), 0, false, false );
        }
    }
}

//============================================================================
//! user double clicked menu item
void AppletLibrary::slotListItemDoubleClicked( QListWidgetItem* item )
{
    slotListItemClicked( item );
}

//============================================================================
void AppletLibrary::clearFileList( void )
{
    ui.m_FileItemList->clear();
}

//============================================================================
FileShareItemWidget* AppletLibrary::findItemByFileName( QString fileName )
{
    int iIdx = 0;
    FileShareItemWidget* poWidget;
    while( iIdx < ui.m_FileItemList->count() )
    {
        poWidget = (FileShareItemWidget*)ui.m_FileItemList->item( iIdx );
        if( poWidget )
        {
            FileItemInfo* poFileInfo = (FileItemInfo*)poWidget->QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
            if( poFileInfo && (poFileInfo->getFileNameAndPath() == fileName) )
            {
                return poWidget;
            }
        }

        iIdx++;
    }

    return nullptr;
}

//============================================================================
void AppletLibrary::updateStorageSpace( std::string fileNameAndPath )
{
	uint64_t diskFreeSpace = m_Engine.fromGuiGetDiskFreeSpace( fileNameAndPath.c_str() );
	if( ( 0 != diskFreeSpace ) && ( diskFreeSpace < 1000000000 ) )
	{
        m_MyApp.toGuiUserMessage( "Storage Space is low %s", GuiParams::describeFileLength( diskFreeSpace ).toUtf8().constData() );
	}

    ui.m_StatusLabel->setText( QObject::tr( "Storage Space Available: " ) + GuiParams::describeFileLength( diskFreeSpace ) );
}

//============================================================================
void AppletLibrary::statusMsg( QString strMsg )
{
    ui.m_StatusLabel->setText( strMsg );
}

//============================================================================
void AppletLibrary::slotFileMediaSelected( EMediaFileType mediaFileType )
{
    browseForFile( mediaFileType );
}

//============================================================================
void AppletLibrary::browseForFile( EMediaFileType mediaFileType )
{
    std::string lastDir;
    switch( mediaFileType )
    {
    case eMediaFileVideo:
        m_MyApp.getAppSettings().getLastLibraryVideoDir( lastDir );
        break;
    case eMediaFileAudio:
        m_MyApp.getAppSettings().getLastLibraryAudioDir( lastDir );
        break;
    case eMediaFileImage:
        m_MyApp.getAppSettings().getLastLibraryImageDir( lastDir );
        break;
    default:
        QMessageBox::information( this, QObject::tr( "Error" ), QObject::tr( "Unknown Media Type" ), QMessageBox::Ok );
        return;
    }

    QString startDir;
    if( !lastDir.empty() )
    {
        startDir = lastDir.c_str();
    }
    
    VxFileInfo fileInfo;
    if( GuiHelpers::browseForFile( this, mediaFileType, fileInfo, startDir ) )
    {
        std::string fileNameAndPath = fileInfo.getFileNameAndPath();
        std::string justPath = fileInfo.getFilePath();
        std::string justName = fileInfo.getFileName();
        if( !justPath.empty() )
        {
            switch( mediaFileType )
            {
            case eMediaFileVideo:
                m_MyApp.getAppSettings().setLastLibraryVideoDir( justPath );
                break;
            case eMediaFileAudio:
                m_MyApp.getAppSettings().setLastLibraryAudioDir( justPath );
                break;
            case eMediaFileImage:
                m_MyApp.getAppSettings().setLastLibraryImageDir( justPath );
                break;
            default:
                QMessageBox::information( this, QObject::tr( "Error" ), QObject::tr( "Unknown Media Type" ), QMessageBox::Ok );
                return;
            }
        }

        // see if file is already a asset
        AssetMgr& assetMgr = m_MyApp.getEngine().getAssetMgr();
        assetMgr.lockResources();
        AssetBaseInfo* assetInfo = assetMgr.findAsset( fileInfo.getFileNameAndPath() );
        if( assetInfo )
        {
            if( assetInfo->isInLibrary() )
            {
                assetMgr.unlockResources();
                QMessageBox::information( this, QObject::tr("Already in library"), QObject::tr( "File is already in library " ), QMessageBox::Ok );
            }
            else
            {
                assetInfo->setIsInLibrary( true );
                FileInfo fileInfoInLibrary = assetInfo->getFileInfo();
                if( !fileInfoInLibrary.getThumbId().isVxGUIDValid() )
                {
                    QString thumbFileName;
                    if( GuiHelpers::generateMediaThumbnail( assetInfo, thumbFileName ) )
                    {
                        ThumbInfo thumbInfo;
                        if( !GuiHelpers::addThumbAsset( m_MyApp, thumbFileName, assetInfo->getFileInfo().getThumbId(), thumbInfo ) )
                        {
                            QString msgText = QObject::tr( "Could not get thumbnail file info" );
                            QMessageBox::information( this, QObject::tr( "Error occured creating thumbnail asset " ) + thumbFileName, msgText );
                            assetInfo->getFileInfo().getThumbId().clear();
                        }
                        else
                        {
                            assetMgr.updateDatabase( assetInfo );
                        }
                    }
                }

                
                assetMgr.unlockResources();

                addFile( fileInfoInLibrary );
            }
        }
        else
        {
            assetMgr.unlockResources();

            AssetInfo newAsset( fileInfo );
            newAsset.setIsInLibrary( true );

            if( !newAsset.getThumbId().isVxGUIDValid() )
            {
                QString thumbFileName;
                if( GuiHelpers::generateMediaThumbnail( &newAsset, thumbFileName ) )
                {
                    ThumbInfo thumbInfo;
                    if( !GuiHelpers::addThumbAsset( m_MyApp, thumbFileName, newAsset.getFileInfo().getThumbId(), thumbInfo ) )
                    {
                        QString msgText = QObject::tr( "Could not get thumbnail file info" );
                        QMessageBox::information( this, QObject::tr( "Error occured creating thumbnail asset " ) + thumbFileName, msgText );
                        assetInfo->getFileInfo().getThumbId().clear();
                    }
                }
            }

            AssetBaseInfo* createdAsset{ nullptr };
            bool result = assetMgr.addAsset( newAsset, createdAsset );
            if( result && createdAsset )
            {
                FileInfo fileInfoInLibrary = createdAsset->getFileInfo();
                addFile( fileInfoInLibrary );
            }
            else
            {
                QMessageBox::information( this, QObject::tr("File Error"), QObject::tr( "Could not add file to library " ), QMessageBox::Ok );
            }
        }
    }
}

//============================================================================
void AppletLibrary::wantFileXferCallbacks( bool enable )
{
    if( m_FileXferCallbacksRequested != enable )
    {
        m_FileXferCallbacksRequested = enable;
        m_MyApp.getFileXferMgr().wantToGuiFileXferCallbacks( this, enable );
    }
}
