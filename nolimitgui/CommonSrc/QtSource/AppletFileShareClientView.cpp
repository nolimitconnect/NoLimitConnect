//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


#include "AppCommon.h"
#include "AppSettings.h"

#include "AppletFileShareClientView.h"

#include "AppletDownloads.h"
#include "ActivityMessageBox.h"
#include "ActivityYesNoMsgBox.h"

#include "FileXferWidget.h"
#include "MyIcons.h"
#include "AppletPopupMenu.h"
#include "AppGlobals.h"
#include "GuiFileXferSession.h"
#include "FileListReplySession.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>
#include <PktLib/VxSearchDefs.h>
#include <NetLib/VxFileXferInfo.h>
#include <CoreLib/VxFileInfo.h>
#include <CoreLib/VxFileUtil.h>

//============================================================================
AppletFileShareClientView::AppletFileShareClientView( AppCommon& app, QWidget*	parent )
: AppletPeerBase( OBJNAME_ACTIVITY_TO_FRIEND_VIEW_SHARED_FILES, app, parent )
, m_u8FileFilter( VXFILE_TYPE_ALLNOTEXE )
{
	m_LclSessionId.initializeWithNewVxGUID();
    setPluginType( ePluginTypeFileShareClient );
    setAppletType( eAppletFileShareClientView );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    connectBarWidgets();

    connect(ui.FileItemList, SIGNAL(itemClicked(QListWidgetItem*)),							this, SLOT(slotItemClicked(QListWidgetItem*)));
    connect(ui.FileItemList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),					this, SLOT(slotItemClicked(QListWidgetItem*)));
	connect( ui.m_FileFilterComboBox, SIGNAL(signalApplyFileFilter(unsigned char)),			this,  SLOT(slotApplyFileFilter(unsigned char)) );

	m_MyApp.activityStateChange( this, true );
	m_MyApp.wantToGuiActivityCallbacks( this, true );
	m_MyApp.getFileXferMgr().wantToGuiFileXferCallbacks( this, true );

	checkDiskSpace();
}

//============================================================================
AppletFileShareClientView::~AppletFileShareClientView()
{
	if( m_HisOnlineId.isVxGUIDValid() )
	{
		m_MyApp.getEngine().fromGuiDownloadFileListCancel( getPluginType(), m_HisOnlineId, m_LclSessionId );
	}

	m_MyApp.getFileXferMgr().wantToGuiFileXferCallbacks( this, false );
	m_MyApp.wantToGuiActivityCallbacks( this, false );
	m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletFileShareClientView::setIdentity( GuiUser* guiUser )
{
	if( guiUser )
	{
		ui.m_IdentWidget->setupIdentLogic();
		ui.m_IdentWidget->updateIdentity( guiUser );
		m_HisOnlineId = guiUser->getMyOnlineId();
		if( !m_MyApp.getEngine().fromGuiDownloadFileList( getPluginType(), m_HisOnlineId, m_LclSessionId, m_u8FileFilter ) )
		{
			GuiHelpers::errorMsgBox( eErrMsgUserUnavailable, this, guiUser );
			close();
		}
	}
}

//============================================================================
void AppletFileShareClientView::showEvent( QShowEvent* ev )
{
	ActivityBase::showEvent( ev );
}

//============================================================================
void AppletFileShareClientView::hideEvent( QHideEvent* ev )
{
	m_MyApp.getFileXferMgr().wantToGuiFileXferCallbacks( this, false );
	ActivityBase::hideEvent( ev );
}

//============================================================================
void AppletFileShareClientView::toGuiFileListReply( FileListReplySession* replySession )
{
	addFile( replySession->getIdent(), replySession->getPluginType(), replySession->getFileInfo() );
}

//============================================================================
void AppletFileShareClientView::toGuiSearchResultFileSearch( GuiUser* guiUser, EPluginType pluginType, VxGUID& lclSessionId, FileInfo& fileInfo )
{
	addFile( guiUser, pluginType, fileInfo );
}

//============================================================================
void AppletFileShareClientView::toGuiFileXferState( EPluginType pluginType, VxGUID& lclSessionId, EXferDirection xferDir, EXferState xferState, EXferError xferErr, int param1 )
{
    if( eXferDirectionRx == xferDir )
    {
        LogMsg( LOG_INFO, "Got Update File Download" );
        FileXferWidget* item = findListEntryWidget( lclSessionId );
        if( item )
        {
            item->setXferState( xferState, xferErr, param1 );
        }
    }
}

//============================================================================
void AppletFileShareClientView::toGuiFileDownloadStart( GuiFileXferSession* xferSessionIn )
{
	GuiFileXferSession* xferSession = findSession( xferSessionIn->getLclSessionId() );
	if( xferSession )
	{
        xferSession->setXferState( eXferStateInDownloadXfer, eXferErrorNone, 0 );
		FileXferWidget* item = findListEntryWidget( xferSession->getLclSessionId() );
		if( item )
		{
			item->updateWidgetFromInfo();
		}
	}
}

//============================================================================
GuiFileXferSession* AppletFileShareClientView::findSession( VxGUID lclSessionId )
{
	int iCnt = ui.FileItemList->count();
	for( int iRow = 0; iRow < iCnt; iRow++ )
	{
		QListWidgetItem* item =  ui.FileItemList->item( iRow );
		GuiFileXferSession* poCurInfo = (GuiFileXferSession*)item->data( Qt::UserRole + 1).toULongLong();
		if( poCurInfo->getLclSessionId() == lclSessionId )
		{
			return poCurInfo;
		}
	}

	return NULL;
}

//============================================================================
void AppletFileShareClientView::toGuiFileDownloadComplete( EPluginType pluginType, VxGUID& lclSessionId, QString newFileName, EXferError xferError )
{
	GuiFileXferSession* xferSession = findSession( lclSessionId );
	if( xferSession )
	{
		if( !newFileName.isEmpty() )
		{
			xferSession->setFullFileName( newFileName.toUtf8().constData() );
		}

        xferSession->setXferState( eXferStateCompletedDownload, xferError, 100 );
		FileXferWidget* item = findListEntryWidget( lclSessionId );
		if( item )
		{
			item->updateWidgetFromInfo();
		}
	}
}

//============================================================================
void AppletFileShareClientView::statusMsg( QString strMsg )
{
	ui.m_StatusMsgLabel->setText( strMsg );
}

//============================================================================
void AppletFileShareClientView::slotApplyFileFilter( unsigned char fileTypeMask )
{
	m_u8FileFilter = fileTypeMask;
	int iIdx = 0;
	FileXferWidget* poWidget;
	while( iIdx < ui.FileItemList->count() )
	{
		poWidget = (FileXferWidget*)ui.FileItemList->item(iIdx);
		if( poWidget )
		{
			GuiFileXferSession* poFileInfo = (GuiFileXferSession*)poWidget->QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
			updateListEntryWidget( poWidget, poFileInfo );
		}

		iIdx++;
	}
}

//============================================================================
FileXferWidget* AppletFileShareClientView::fileToWidget( GuiUser* guiUser, EPluginType pluginType, FileInfo& fileInfo )
{
	FileXferWidget* item = new FileXferWidget(ui.FileItemList);
	item->setSizeHint( QSize( (int)(GuiParams::getGuiScale() * 200), GuiParams::getFileListEntryHeight() ) );

    VxGUID lclSessionId;
    lclSessionId.initializeWithNewVxGUID();
    GuiFileXferSession* xferSession = new GuiFileXferSession( eXferDirectionRx, pluginType, guiUser, lclSessionId, fileInfo );
	xferSession->setXferState( eXferStateDownloadNotStarted );
	xferSession->setWidget( item );
    item->QListWidgetItem::setData( Qt::UserRole + 1, QVariant((quint64)xferSession) );

    connect( item, SIGNAL(signalFileXferItemClicked(QListWidgetItem*)), this, SLOT(slotItemClicked(QListWidgetItem*)));
    connect( item, SIGNAL(signalFileIconButtonClicked(QListWidgetItem*)), this, SLOT(slotItemClicked(QListWidgetItem*)));

	connect( item, SIGNAL( signalAcceptButtonClicked(QListWidgetItem*) ),   this, SLOT( slotAcceptButtonClicked(QListWidgetItem*) ) );
	connect( item, SIGNAL(signalCancelButtonClicked(QListWidgetItem*)),		this, SLOT(slotCancelButtonClicked(QListWidgetItem*)) );
	connect( item, SIGNAL(signalPlayButtonClicked(QListWidgetItem*)),		this, SLOT(slotPlayButtonClicked(QListWidgetItem*)) );
	connect( item, SIGNAL(signalLibraryButtonClicked(QListWidgetItem*)),	this, SLOT(slotLibraryButtonClicked(QListWidgetItem*)) );
	connect( item, SIGNAL(signalFileShareButtonClicked(QListWidgetItem*)),	this, SLOT(slotFileShareButtonClicked(QListWidgetItem*)) );
	connect( item, SIGNAL(signalShredButtonClicked(QListWidgetItem*)),		this, SLOT(slotShredButtonClicked(QListWidgetItem*)) );

	updateListEntryWidget( item, xferSession );

	return item;
}

//============================================================================
void AppletFileShareClientView::updateListEntryWidget( FileXferWidget* item, GuiFileXferSession* xferSession )
{
	if( !item || !xferSession )
	{
		return;
	}

	item->updateWidgetFromInfo();
	if( 0 == ( xferSession->getFileType() & m_u8FileFilter ) )
	{
		item->setVisible( false );
	}
	else
	{
		item->setVisible( true );
	}

	if( xferSession->getXferState() != eXferStateDownloadNotStarted && xferSession->getXferState() != eXferStateUploadNotStarted )
	{
		int percent = m_FromGui.fromGuiGetFileDownloadState( xferSession->getFileHashId().getHashData() );
		if( percent < 0 )
		{
			item->setFileIconButtonEnabled( true );
			item->setFileProgressBarValue( 0 );
		}
		else
		{
			item->setFileIconButtonEnabled( false );
			item->setFileProgressBarValue( percent );
		}
	}
}

//============================================================================
GuiFileXferSession* AppletFileShareClientView::widgetToFileItemInfo( FileXferWidget* item )
{
	return (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
}

//============================================================================
FileXferWidget* AppletFileShareClientView::findListEntryWidget( VxGUID lclSessionId )
{
	int iIdx = 0;
	FileXferWidget* poWidget;
	while( iIdx < ui.FileItemList->count() )
	{
		poWidget = (FileXferWidget*)ui.FileItemList->item(iIdx);
		if( poWidget )
		{
			GuiFileXferSession* poFileInfo = (GuiFileXferSession*)poWidget->QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
			if( poFileInfo && ( poFileInfo->getLclSessionId() == lclSessionId ) )
			{
				return poWidget;
			}
		}

		iIdx++;
	}

	return NULL;
}

//============================================================================
void AppletFileShareClientView::addFile( GuiUser* guiUser, EPluginType pluginType, FileInfo& fileInfo )
{
    if( fileInfo.getFileLength() && !fileInfo.getFullFileName().empty() )
	{
        FileXferWidget* item = fileToWidget( guiUser, pluginType, fileInfo );
		if( item )
		{
			//LogMsg( LOG_INFO, "AppletFileShareClientView::addFile: adding widget\n");
			ui.FileItemList->addItem( item );
			ui.FileItemList->setItemWidget( item, item );
			GuiFileXferSession* xferSession = widgetToFileItemInfo( item );
			if( xferSession )
			{
				if( eXferStateUploadNotStarted == xferSession->getXferState() || eXferStateDownloadNotStarted == xferSession->getXferState() )
				{
					std::string fileName = xferSession->getFullFileName().toUtf8().constData();
                    int64_t fileLen = (int64_t)VxFileUtil::fileExists( fileName.c_str() );
					if( fileLen && fileLen == fileInfo.getFileLength() )
					{
						if( eXferStateUploadNotStarted == xferSession->getXferState() )
						{
							xferSession->setXferState( eXferStateCompletedUpload, eXferErrorNone, 100 );
							item->updateWidgetFromInfo();
						}
						else
						{
							xferSession->setXferState( eXferStateCompletedDownload, eXferErrorNone, 100 );
							item->updateWidgetFromInfo();
						}
					}
				}			
			}
		}
	}
	else
	{
		statusMsg( "File List Complete" );
	}
}

//============================================================================
void AppletFileShareClientView::slotHomeButtonClicked( void )
{
	accept();
}

//============================================================================
void AppletFileShareClientView::slotItemClicked(QListWidgetItem* item)
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->data(Qt::UserRole + 1).toLongLong();
	if( xferSession )
	{
		if( -1 == m_FromGui.fromGuiGetFileDownloadState( xferSession->getFileHashId().getHashData() ) )
		{
			promptForDownload( xferSession );
		}
	}
}

//============================================================================
void AppletFileShareClientView::slotCancelButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->data(Qt::UserRole + 1).toLongLong();
	if( !xferSession )
	{
		return;
	}

	switch( xferSession->getXferState() )
	{
	case eXferStateDownloadNotStarted:
		beginDownload( xferSession, item );
		break;

	case eXferStateCompletedUpload:
	case eXferStateCompletedDownload:
		removeDownload( xferSession, item );
		break;
	case eXferStateWaitingOfferResponse:
	case eXferStateInUploadQue:
	case eXferStateBeginUpload:
	case eXferStateInUploadXfer:
	case eXferStateUserCanceledUpload:
	case eXferStateUploadOfferRejected:
	case eXferStateUploadError:
		cancelUpload( xferSession, item );
		break;

	case eXferStateInDownloadQue:
	case eXferStateBeginDownload:
	case eXferStateInDownloadXfer:
	case eXferStateUserCanceledDownload:
	case eXferStateDownloadError:
		cancelDownload( xferSession, item );
	default:
		break;
	}
}

//============================================================================
void AppletFileShareClientView::beginDownload( GuiFileXferSession* xferSession, QListWidgetItem* item  )
{
	if( !xferSession || !xferSession->getIdent() )
	{
		LogMsg( LOG_ERROR, "AppletFileShareClientView::beginDownload invalid param" );
		return;
	}

	if(	-1 != m_FromGui.fromGuiGetFileDownloadState( xferSession->getFileHashId().getHashData() ) )
	{
		ActivityMessageBox errMsgBox( m_MyApp, this, LOG_INFO, "File is already downloading" );
		errMsgBox.exec();
	}
	else
	{
		GuiUser* guiUser = xferSession->getIdent();
		if( false == xferSession->getLclSessionId().isVxGUIDValid() )
		{
			xferSession->getLclSessionId().initializeWithNewVxGUID();
		}

		FileInfo& fileInfo = xferSession->getFileInfo();
		// make sure is correct user to transfer from
		fileInfo.setOnlineId( guiUser->getMyOnlineId() );
		fileInfo.setXferSessionId( xferSession->getLclSessionId() );

		EXferError xferError = m_FromGui.fromGuiFileXferControl( xferSession->getPluginType(), eXferActionDownload, fileInfo );
		if( eXferErrorNone != xferError )
		{
			ActivityMessageBox errMsgBox( m_MyApp, this, LOG_INFO, GuiParams::describeEXferError( xferError ) );
			errMsgBox.exec();
		}
		else
		{
			m_MyApp.getFileXferMgr().beginDownload( getAppletType(), xferSession );
		}
	}
}

//============================================================================
void AppletFileShareClientView::removeDownload( GuiFileXferSession* xferSession, QListWidgetItem* item  )
{
	ui.FileItemList->removeItemWidget( item );
}

//============================================================================
void AppletFileShareClientView::cancelDownload( GuiFileXferSession* xferSession, QListWidgetItem* item )
{
    xferSession->setXferState(  eXferStateUserCanceledDownload, eXferErrorNone, 0 );
    ((FileXferWidget*)item)->setXferState( eXferStateUserCanceledDownload, eXferErrorNone, 0 );
	m_MyApp.getFileXferMgr().cancelDownload( getAppletType(), xferSession );
	std::string fileName = xferSession->getFileInfo().getFullFileName();
	if( VxFileUtil::fileExists( fileName.c_str() ) )
	{
		if( confirmDeleteFile( true ) )
		{
			removeDownload( xferSession, item );
			m_MyApp.getEngine().fromGuiDeleteFile( fileName, true );
		}
	}
}

//============================================================================
void AppletFileShareClientView::cancelUpload( GuiFileXferSession* xferSession, QListWidgetItem* item )
{
    xferSession->setXferState( eXferStateUserCanceledUpload, eXferErrorNone, 0 );
    ((FileXferWidget*)item)->setXferState( eXferStateUserCanceledUpload, eXferErrorNone, 0 );
	m_MyApp.getFileXferMgr().cancelUpload( getAppletType(), xferSession );

	removeDownload( xferSession, item );
}

//============================================================================
void AppletFileShareClientView::slotPlayButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->data(Qt::UserRole + 1).toLongLong();
	if( xferSession )
	{
		this->playFile( xferSession->getFullFileName() );
	}
}

//============================================================================
void AppletFileShareClientView::slotLibraryButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->data(Qt::UserRole + 1).toLongLong();
	if( xferSession )
	{
		bool inLibary = xferSession->getIsInLibrary();
		inLibary = !inLibary;
		xferSession->setIsInLibrary( inLibary );
		m_Engine.fromGuiSetFileIsInLibrary( xferSession->getFileInfo(), inLibary );
		((FileXferWidget*)item)->updateWidgetFromInfo();
	}
}

//============================================================================
void AppletFileShareClientView::slotFileShareButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->data(Qt::UserRole + 1).toLongLong();
	if( xferSession )
	{
		bool isShared = xferSession->getIsSharedFile();
		isShared = !isShared;
		xferSession->setIsSharedFile( isShared );
		m_Engine.fromGuiSetFileIsShared( xferSession->getFileInfo(), isShared );
		((FileXferWidget*)item)->updateWidgetFromInfo();
	}
}

//============================================================================
void AppletFileShareClientView::slotShredButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->data(Qt::UserRole + 1).toLongLong();
	if( xferSession )
	{
		QString fileName = xferSession->getFullFileName();
		if( confirmDeleteFile( true ) )
		{
			removeDownload( xferSession, item );
			m_Engine.fromGuiDeleteFile( fileName.toUtf8().constData(), true );
		}
	}
}

//============================================================================
bool AppletFileShareClientView::confirmDeleteFile( bool shredFile )
{
	bool acceptAction = true;
	bool isConfirmDisabled = m_MyApp.getAppSettings().getIsConfirmDeleteDisabled();
	if( false == isConfirmDisabled )
	{
		QString title = shredFile ?  QObject::tr( "Confirm Shred File" ) : QObject::tr( "Confirm Delete File" );
		QString bodyText = "";
		if( shredFile )
		{
			bodyText = QObject::tr( "Are You Sure You Want To Write Random Data Into The File Then Delete From The Device?" );
		}
		else
		{
			bodyText = QObject::tr( "Are You Sure To Delete The File From The Device?" );
		}

		ActivityYesNoMsgBox dlg( m_MyApp, &m_MyApp, title, bodyText );
		dlg.makeNeverShowAgainVisible( true );
		if( false == (QDialog::Accepted == dlg.exec()))
		{
			acceptAction = false;
		}

		if( dlg.wasNeverShowAgainChecked() )
		{
			m_MyApp.getAppSettings().setIsConfirmDeleteDisabled( true );
		}
	}

	return acceptAction;
}

//============================================================================
void AppletFileShareClientView::promptForDownload( GuiFileXferSession* poInfo )
{
	/*
	m_SelectedFileInfo = poInfo;
	PopupMenu popupMenu( m_MyApp, this );
    popupMenu.setTitleBarWidget( this->getTitleBarWidget() );
    popupMenu.setBottomBarWidget( this->getBottomBarWidget() );
    popupMenu.setTitle( QObject::tr( "Download A File" ) );
    popupMenu.addMenuItem( 1, getMyIcons().getIcon(eMyIconFileDownload), QObject::tr( "Download A File" ) );
    popupMenu.addMenuItem( 2, getMyIcons().getIcon(getMyIcons().getFileIcon(poInfo->getFileType())), poInfo->getJustFileName() );
    connect( &popupMenu, SIGNAL(menuItemClicked(int, PopupMenu *, ActivityBase*)), this, SLOT(slotDownloadFileSelected(int, PopupMenu *, ActivityBase*)));

    popupMenu.exec();*/
}

//============================================================================
void AppletFileShareClientView::slotDownloadFileSelected( int iMenuId, QWidget* popupMenu )
{
	if( m_SelectedFileInfo && m_Friend )
	{
		if(	-1 != m_FromGui.fromGuiGetFileDownloadState( m_SelectedFileInfo->getFileHashId().getHashData() ) )
		{
			ActivityMessageBox errMsgBox( m_MyApp, this, LOG_INFO, "File is already downloading" );
			errMsgBox.exec();
		}
		else
		{
			if( false == m_SelectedFileInfo->getLclSessionId().isVxGUIDValid() )
			{
				m_SelectedFileInfo->getLclSessionId().initializeWithNewVxGUID();
			}

			FileInfo& fileInfo = m_SelectedFileInfo->getFileInfo();
			fileInfo.setOnlineId( m_Friend->getMyOnlineId() );

			EXferError xferError = m_FromGui.fromGuiFileXferControl( ePluginTypeFileShareServer, eXferActionDownload, fileInfo );

			if( eXferErrorNone != xferError )
			{
				ActivityMessageBox errMsgBox( m_MyApp, this, LOG_INFO, GuiParams::describeEXferError( xferError ) );
				errMsgBox.exec();
			}
		}
	}
}

//============================================================================
void AppletFileShareClientView::clearFileList( void )
{
	ui.FileItemList->clear();
}

//============================================================================
void AppletFileShareClientView::moveUpOneFolder( void )
{
	if( m_strCurrentDirectory.length() )
	{
		char * pBuf = new char[ m_strCurrentDirectory.length() + 1 ];
		strcpy( pBuf, m_strCurrentDirectory.c_str() );
		char * pTemp = strrchr( pBuf, '/' );
		if( pTemp )
		{
			pTemp[0] = 0;
		}
		m_strCurrentDirectory = pBuf;
        delete[] pBuf;
	}
}

//============================================================================
void AppletFileShareClientView::slotAcceptButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
	if( xferSession )
	{
		m_MyApp.getFileXferMgr().acceptDownload( getAppletType(), xferSession );
		ui.FileItemList->removeItemWidget( item );
	}
}
