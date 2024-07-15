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

#include "AppCommon.h"
#include "AppGlobals.h"
#include "AppSettings.h"

#include "AppletDownloads.h"
#include "ActivityBrowseFiles.h"
#include "ActivityYesNoMsgBox.h"

#include "FileShareItemWidget.h"
#include "FileItemInfo.h"
#include "FileActionMenu.h"
#include "GuiHelpers.h"
#include "GuiParams.h"
#include "GuiPlayerMgr.h"

#include <P2PEngine/P2PEngine.h>
#include <AssetMgr/AssetMgr.h>
#include <AssetMgr/AssetInfo.h>

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
    , m_FileWasSelected( false )
    , m_SelectedFileType( 0 )
    , m_SelectedFileName( "" )
    , m_SelectedFileLen( 0 )
    , m_SelectedFileIsShared( false )
    , m_SelectedFileIsInLibrary( false )
    , m_eFileFilterType( eFileFilterAll )
    , m_FileFilterMask( VXFILE_TYPE_ALLNOTEXE )
{
    setAppletType( eAppletLibrary );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    m_eFileFilterType = GuiParams::fileFilterToEnum( launchParam );
    setFileFilter( m_eFileFilterType );

    ui.m_DoubleTapInstructionLabel->setVisible( m_IsSelectAFileMode );

	ui.m_AddFileButton->setIcon( eMyIconLibraryCancel );
	ui.m_AddFileButton->setSquareButtonSize( eButtonSizeMedium );
	ui.m_AddFilesButton->setIcon( eMyIconFileAdd );
	ui.m_AddFilesButton->setSquareButtonSize( eButtonSizeMedium );

    connect( ui.m_AddFileButton, SIGNAL(clicked()), this, SLOT( slotAddFileButtonClicked() ) );
    connect( ui.m_AddFilesButton, SIGNAL(clicked()), this, SLOT( slotAddFilesButtonClicked() ) );

    connect( ui.m_FileFilterComboBox, SIGNAL(signalApplyFileFilter(unsigned char)), this, SLOT(slotApplyFileFilter(unsigned char)) );
    statusMsg( "Requesting Library File List " );
    m_MyApp.getFileXferMgr().wantToGuiFileXferCallbacks( this, true );
    slotApplyFileFilter( ui.m_FileFilterComboBox->getCurrentFileFilterMask() );
    connectBarWidgets();

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletLibrary::~AppletLibrary()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletLibrary::statusMsg( QString strMsg )
{
    //LogMsg( LOG_INFO, strMsg.toStdString().c_str() );
    ui.m_StatusMsgLabel->setText( strMsg );
}

//============================================================================
void AppletLibrary::showEvent( QShowEvent* ev )
{
    AppletBase::showEvent( ev );
    m_MyApp.setIsLibraryActivityActive( true );
    m_MyApp.getFileXferMgr().wantToGuiFileXferCallbacks( this, true );
    slotRequestFileList();
}

//============================================================================
void AppletLibrary::hideEvent( QHideEvent* ev )
{
    m_MyApp.getFileXferMgr().wantToGuiFileXferCallbacks( this, false );
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
            updateStorageSpace( fileInfo.getFullFileName() );
        }

        addFile( fileInfo );
    }
}

//============================================================================
void AppletLibrary::callbackToGuiFileListCompleted( VxGUID& appInstId )
{
    if( appInstId == getAppletInstId() )
    {
        //setActionEnable( true );
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
            if( poFileInfo && (poFileInfo->getFullFileName() == fileName) )
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
void AppletLibrary::setFileFilter( EFileFilterType eFileFilter )
{
    m_eFileFilterType = eFileFilter;
    m_FileFilterMask = ui.m_FileFilterComboBox->getMaskFromFileFilterType( m_eFileFilterType );
    ui.m_FileFilterComboBox->setFileFilter( eFileFilter );
}

//============================================================================
void AppletLibrary::slotApplyFileFilter( unsigned char fileTypeMask )
{
    m_FileFilterMask = fileTypeMask;
    slotRequestFileList();
}

//============================================================================
void AppletLibrary::slotRequestFileList( void )
{
    clearFileList();
    m_FromGui.fromGuiGetFileLibraryList( getAppletInstId(), m_FileFilterMask );
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

                m_MyApp.getPlayerMgr().playMedia( assetInfo, false );
            }
            else
            {
                QMessageBox::information( this, QObject::tr( "File Not Found" ), poInfo->getFullFileName().toUtf8().constData(), QMessageBox::Ok );
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

                m_MyApp.getPlayerMgr().playMedia( assetInfo, true );
            }
            else
            {
                QMessageBox::information( this, QObject::tr( "File Not Found" ), poInfo->getFullFileName().toUtf8().constData(), QMessageBox::Ok );
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
                    m_Engine.fromGuiDeleteFile( assetInfo.getAssetName().c_str(), true );
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
            if( poFileInfo && ( poFileInfo->getFileInfo().getFullFileName() == fileInfo.getFullFileName() ) )
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
    if( GuiHelpers::browseForFile( this, fileInfo, curDir ) )
    {
        std::string fileName = fileInfo.getFullFileName();
        m_MyApp.getAppSettings().setLastAddFileDir( fileInfo.getFilePath() );
        m_MyApp.getEngine().fromGuiSetFileIsInLibrary( fileName, true );

        // see if file is already a asset
        AssetMgr& assetMgr = m_MyApp.getEngine().getAssetMgr();
        assetMgr.lockResources();
        AssetBaseInfo* assetInfo = assetMgr.findAsset( fileInfo.getFullFileName() );
        if( assetInfo )
        {
            if( assetInfo->isInLibary() )
            {
                assetMgr.unlockResources();
                QMessageBox::information( this, QObject::tr("Already in library"), QObject::tr( "File is already in library " ), QMessageBox::Ok );
            }
            else
            {
                assetInfo->setIsInLibary( true );
                FileInfo fileInfoInLibrary =  assetInfo->getFileInfo();
                assetMgr.unlockResources();

                addFile( fileInfoInLibrary );
            }
        }
        else
        {
            assetMgr.unlockResources();

            AssetInfo newAsset( fileInfo );
            newAsset.setIsInLibary( true );

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
void AppletLibrary::slotAddFilesButtonClicked( void )
{
    ActivityBrowseFiles dlg( m_MyApp, eFileFilterAll, this );
    dlg.exec();
    clearFileList();
    statusMsg( "Requesting Library File List " );
    m_FromGui.fromGuiGetFileLibraryList( getAppletInstId(), m_FileFilterMask );
}

//============================================================================
void AppletLibrary::addFile( FileInfo& fileInfo )
{
    FileShareItemWidget* existingItem = findItemByFileName( fileInfo.getFullFileName().c_str() );
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
void AppletLibrary::slotHomeButtonClicked( void )
{
    closeApplet();
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
            m_SelectedFileName = fileInfo.getFullFileName().c_str();
            m_SelectedFileLen = fileInfo.getFileLength();
            m_SelectedFileIsShared = poInfo->getIsSharedFile();
            m_SelectedFileIsInLibrary = poInfo->getIsInLibrary();
            accept();
        }
        else
        {
            //FileActionMenu fileActionMenuDialog(	m_MyApp, 
            //										this, 
            //										poInfo->getMyFileInfo(),
            //										poInfo->getIsSharedFile(),
            //										poInfo->getIsInLibrary() );
            //fileActionMenuDialog.exec();
            playFile( fileInfo.getFullFileName().c_str(), 0, false, false );
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
            if( poFileInfo && (poFileInfo->getFullFileName() == fileName) )
            {
                return poWidget;
            }
        }

        iIdx++;
    }

    return nullptr;
}

//============================================================================
void AppletLibrary::updateStorageSpace( std::string fileName )
{
	uint64_t diskFreeSpace = m_Engine.fromGuiGetDiskFreeSpace( fileName.c_str() );
	if( ( 0 != diskFreeSpace ) && ( diskFreeSpace < 1000000000 ) )
	{
        m_MyApp.toGuiUserMessage( "Storage Space is low %s", GuiParams::describeFileLength( diskFreeSpace ).toUtf8().constData() );
	}

    ui.m_StatusLabel->setText( QObject::tr( "Storage Space Available: " ) + GuiParams::describeFileLength( diskFreeSpace ) );
}