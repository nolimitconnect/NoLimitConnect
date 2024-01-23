//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletDownloads.h"

#include "ActivityYesNoMsgBox.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiHelpers.h"	
#include "GuiParams.h"

#include "FileXferWidget.h"
#include "GuiFileXferSession.h"
#include "MyIcons.h"
#include "ActivityDownloadItemMenu.h"

#include <P2PEngine/P2PEngine.h>

#include <PktLib/VxSearchDefs.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxFileInfo.h>

//============================================================================
AppletDownloads::AppletDownloads( AppCommon& app,  QWidget* parent )
: AppletPeerBase( OBJNAME_ACTIVITY_DOWNLOADS, app, parent ) 
{
    setAppletType( eAppletDownloads );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    connect( ui.m_FileItemList, SIGNAL(itemClicked(QListWidgetItem*)),		                this, SLOT(slotFileXferItemClicked(QListWidgetItem*)));
    connect( ui.m_FileItemList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),	            this, SLOT(slotFileXferItemClicked(QListWidgetItem*)));

	populateList( m_MyApp.getFileXferMgr().getDownloadSessions() );

	m_MyApp.getFileXferMgr().wantToGuiFileXferCallbacks( this, true );
	checkDiskSpace();
	
    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletDownloads::~AppletDownloads()
{
	m_MyApp.getFileXferMgr().wantToGuiFileXferCallbacks( this, false );
    m_MyApp.activityStateChange( this, false );
}

////============================================================================
//void AppletDownloads::slotItemClicked(QListWidgetItem* item)
//{
//	GuiFileXferSession* poSession = (GuiFileXferSession*)item->data( Qt::UserRole + 1).toULongLong();
//	if( poSession )
//	{
//		ActivityDownloadItemMenu oDlg( m_MyApp, poSession, this );
//		if( QDialog::Rejected == oDlg.exec() )
//		{
//			m_FromGui.fromGuiCancelDownload( poSession->getLclSessionId() );
//			ui.m_FileItemList->removeItemWidget( item );
//			delete poSession;
//		}
//	}
//}

//============================================================================
FileXferWidget* AppletDownloads::sessionToWidget( GuiFileXferSession* poSession )
{
	FileXferWidget* item = new FileXferWidget(ui.m_FileItemList);
	item->setSizeHint( QSize( (int)(GuiParams::getGuiScale() * 200), GuiParams::getFileListEntryHeight() ) );

    item->setFileItemInfo( poSession );

	connect( item, SIGNAL(signalFileXferItemClicked(QListWidgetItem*)),		this, SLOT(slotFileXferItemClicked(QListWidgetItem*)) );
	connect( item, SIGNAL(signalFileIconButtonClicked(QListWidgetItem*)),	this, SLOT(slotFileIconButtonClicked(QListWidgetItem*)) );
	connect( item, SIGNAL(signalAcceptButtonClicked(QListWidgetItem*) ),    this, SLOT(slotAcceptButtonClicked(QListWidgetItem*) ) );
	connect( item, SIGNAL(signalCancelButtonClicked(QListWidgetItem*)),		this, SLOT(slotCancelButtonClicked(QListWidgetItem*)) );
	connect( item, SIGNAL(signalPlayButtonClicked(QListWidgetItem*)),		this, SLOT(slotPlayButtonClicked(QListWidgetItem*)) );
	connect( item, SIGNAL(signalPlayExternButtonClicked(QListWidgetItem*)),	this, SLOT(slotPlayExternButtonClicked(QListWidgetItem*)) );
	connect( item, SIGNAL(signalLibraryButtonClicked(QListWidgetItem*)),	this, SLOT(slotLibraryButtonClicked(QListWidgetItem*)) );
	connect( item, SIGNAL(signalFileShareButtonClicked(QListWidgetItem*)),	this, SLOT(slotFileShareButtonClicked(QListWidgetItem*)) );
	connect( item, SIGNAL(signalShredButtonClicked(QListWidgetItem*)),		this, SLOT(slotShredButtonClicked(QListWidgetItem*)) );

	updateListEntryWidget( item, poSession );

	return item;
}

//============================================================================
void AppletDownloads::updateListEntryWidget( FileXferWidget* item, GuiFileXferSession* poSession )
{
	poSession->setWidget( item );
	item->updateWidgetFromInfo();
}

//============================================================================
GuiFileXferSession* AppletDownloads::widgetToSession( FileXferWidget* item )
{
	return item->getFileItemInfo();
}

//============================================================================
GuiFileXferSession* AppletDownloads::findSession( VxGUID lclSessionId )
{
	int iCnt = ui.m_FileItemList->count();
	for( int iRow = 0; iRow < iCnt; iRow++ )
	{
		QListWidgetItem* item =  ui.m_FileItemList->item( iRow );
		if( item )
		{
			GuiFileXferSession* xferSession = (GuiFileXferSession*)item->data( Qt::UserRole + 1 ).toULongLong();
			if( xferSession )
			{
				if( xferSession->getLclSessionId() == lclSessionId )
				{
					return xferSession;
				}
			}
			else
			{
				LogMsg( LOG_ERROR, "AppletDownloads::findSession null xferSession" );
			}
		}
		else
		{
			LogMsg( LOG_ERROR, "AppletDownloads::findSession null item" );
		}
	}

	return nullptr;
}

//============================================================================
FileXferWidget* AppletDownloads::findListEntryWidget( VxGUID lclSessionId )
{
	int iCnt = ui.m_FileItemList->count();
	for( int iRow = 0; iRow < iCnt; iRow++ )
	{
		QListWidgetItem* item =  ui.m_FileItemList->item( iRow );
		if( item )
		{
			GuiFileXferSession* xferSession = (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1).toULongLong();
			if( xferSession )
			{
				if( xferSession && ( xferSession->getLclSessionId() == lclSessionId ) )
				{
					return (FileXferWidget*)item;
				}
			}
			else
			{
				LogMsg( LOG_ERROR, "AppletDownloads::findListEntryWidget null xferSession" );
			}
		}
		else
		{
			LogMsg( LOG_ERROR, "AppletDownloads::findListEntryWidget null item" );
		}
	}

	return nullptr;
}

//============================================================================
FileXferWidget* AppletDownloads::addDownload( GuiFileXferSession* xferSession )
{
	FileXferWidget* item = findListEntryWidget( xferSession->getLclSessionId() );
	if( item )
	{
		GuiFileXferSession* xferSession = (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1).toULongLong();
		delete xferSession;
        item->QListWidgetItem::setData( Qt::UserRole + 1, QVariant( (qint64)xferSession ) );
		updateListEntryWidget( item, xferSession );
	}
	else
	{
		item = sessionToWidget( xferSession );
        item->QListWidgetItem::setData( Qt::UserRole + 1, QVariant( (qint64)xferSession ) );

		ui.m_FileItemList->addItem( item );
		ui.m_FileItemList->setItemWidget( item, item );
		if( eXferStateUploadNotStarted == xferSession->getXferState() || eXferStateDownloadNotStarted == xferSession->getXferState() )
		{
			std::string fileName = xferSession->getFullFileName().toUtf8().constData();
            int64_t fileLen = (int64_t)VxFileUtil::fileExists( fileName.c_str() );
			if( fileLen && fileLen == xferSession->getFileInfo().getFileLength() )
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

	return item;
}

//============================================================================
bool AppletDownloads::isXferInProgress( VxGUID lclSessionId )
{
	if( findSession( lclSessionId ) )
	{
		return true;
	}

	return false;
}

//============================================================================
void AppletDownloads::toGuiFileDownloadStart( GuiFileXferSession* xferSession )
{
	updateSession( xferSession );
}

//============================================================================
void AppletDownloads::toGuiFileXferState( EPluginType pluginType, VxGUID& lclSessionId, EXferDirection xferDir, EXferState xferState, EXferError xferErr, int param1 )
{
    if( eXferDirectionRx == xferDir )
    {
        FileXferWidget* item = findListEntryWidget( lclSessionId );
        if( item )
        {
            item->setXferState( xferState, xferErr, param1 );
        }
    }
}

//============================================================================
void AppletDownloads::toGuiFileDownloadComplete( EPluginType pluginType, VxGUID& lclSessionId, QString newFileName, EXferError xferError )
{
	GuiFileXferSession* xferSession = findSession( lclSessionId );
	if( xferSession )
	{
		if( !newFileName.isEmpty() )
		{
			xferSession->setFullFileName( newFileName );
		}

        xferSession->setXferState( eXferStateCompletedDownload, xferError, 0 );
		FileXferWidget* item = findListEntryWidget( lclSessionId );
		if( item )
		{
			item->updateWidgetFromInfo();
		}
	}
}

//============================================================================
void AppletDownloads::slotFileXferItemClicked(QListWidgetItem* item)
{
}

//============================================================================
void AppletDownloads::slotFileIconButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1).toULongLong();
	if( xferSession )
	{

	}	
}

//============================================================================
void AppletDownloads::slotAcceptButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
	if( xferSession )
	{
		m_MyApp.getFileXferMgr().acceptDownload( getAppletType(), xferSession );
	}

	ui.m_FileItemList->removeItemWidget( item );
}

//============================================================================
void AppletDownloads::slotCancelButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1).toULongLong();
	if( xferSession )
	{
		m_MyApp.getFileXferMgr().cancelDownload( getAppletType(), xferSession );
	}

	ui.m_FileItemList->removeItemWidget( item );
}

//============================================================================
void AppletDownloads::slotPlayButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1).toULongLong();
	if( xferSession )
	{
		this->playFile( xferSession->getFullFileName(), 0, false, false );
	}
}

//============================================================================
void AppletDownloads::slotPlayExternButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1).toULongLong();
	if( xferSession )
	{
		this->playFile( xferSession->getFullFileName(), 0, false, true );
	}
}

//============================================================================
void AppletDownloads::slotLibraryButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1).toULongLong();
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
void AppletDownloads::slotFileShareButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1).toULongLong();
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
void AppletDownloads::slotShredButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1).toULongLong();
	if( xferSession )
	{
		QString fileName = xferSession->getFullFileName();
		if( confirmDeleteFile( true ) )
		{
			ui.m_FileItemList->removeItemWidget( item );
			m_Engine.fromGuiDeleteFile( fileName.toUtf8().constData(), true );
		}
	}	
}

//============================================================================
bool AppletDownloads::confirmDeleteFile( bool shredFile )
{
	bool acceptAction = true;
	bool isConfirmDisabled = m_MyApp.getAppSettings().getIsConfirmDeleteDisabled();
	if( false == isConfirmDisabled )
	{
		QString title = shredFile ?  "Confirm Shred File" : "Confirm Delete File";
		QString bodyText = "";
		if( shredFile )
		{
			bodyText = "Are You Sure You Want To Write Random Data Into The File Then Delete From The Device?";
		}
		else
		{
			bodyText = "Are You Sure To Delete The File From The Device?";
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
void AppletDownloads::populateList( std::vector<GuiFileXferSession*>& sessionList )
{
	for( auto xferSession : sessionList )
	{
		updateSession( xferSession );
	}
}

//============================================================================
void AppletDownloads::updateSession( GuiFileXferSession* xferSession, bool resetState )
{
	GuiFileXferSession* existingSession = findSession( xferSession->getLclSessionId() );
	if( existingSession )
	{
		existingSession->updateSession( xferSession );
	}
	else
	{
		GuiFileXferSession* newSession = new GuiFileXferSession( *xferSession );
		newSession->setXferDirection( eXferDirectionRx );

		FileXferWidget* item = addDownload( newSession );
		if( resetState )
		{
			item->setXferState( eXferStateInDownloadXfer, eXferErrorNone, 0 );
		}
	}
}
