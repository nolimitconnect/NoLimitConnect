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

#include "AppletUploads.h"
#include "ActivityYesNoMsgBox.h"

#include "FileXferWidget.h"
#include "GuiFileXferSession.h"
#include "GuiHelpers.h"	
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>

#include <PktLib/VxSearchDefs.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxFileInfo.h>

#include "ui_AppletUploads.h"

//============================================================================
AppletUploads::AppletUploads( AppCommon& app, QWidget*	parent )
: AppletPeerBase( OBJNAME_ACTIVITY_UPLOADS, app, parent )
, ui(*(new Ui::AppletUploadsUi))
{
    setAppletType( eAppletUploads );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    connectBarWidgets();

    connect(ui.m_FileItemList,	SIGNAL(itemClicked(QListWidgetItem*)),         this, SLOT(slotFileXferItemClicked(QListWidgetItem*)));
    connect(ui.m_FileItemList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),    this, SLOT(slotFileXferItemClicked(QListWidgetItem*)));

	populateList( m_MyApp.getFileXferMgr().getUploadSessions() );

	m_MyApp.getFileXferMgr().wantToGuiFileXferCallbacks( this, true );
	checkDiskSpace();
    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletUploads::~AppletUploads()
{
	wantActivityCallbacks( false );
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
FileXferWidget* AppletUploads::sessionToWidget( GuiFileXferSession* xferSession )
{
	FileXferWidget* item = new FileXferWidget(ui.m_FileItemList);
	item->setSizeHint( QSize( (int)(GuiParams::getGuiScale() * 200), GuiParams::getFileListEntryHeight() ) );

    item->QListWidgetItem::setData( Qt::UserRole + 1, QVariant((quint64)xferSession) );

    connect( item, SIGNAL(signalFileXferItemClicked(QListWidgetItem*)), this, SLOT(slotFileXferItemClicked(QListWidgetItem*)));
	connect( item, SIGNAL(signalAcceptButtonClicked(QListWidgetItem*)), this, SLOT(slotAcceptButtonClicked(QListWidgetItem*)) );
    connect( item, SIGNAL(signalCancelButtonClicked(QListWidgetItem*)), this, SLOT(slotCancelButtonClicked(QListWidgetItem*)));

	updateListEntryWidget( item, xferSession );
	return item;
}

//============================================================================
void AppletUploads::updateListEntryWidget( FileXferWidget* item, GuiFileXferSession* xferSession )
{
	xferSession->setWidget( item );
	item->updateWidgetFromInfo();
}

//============================================================================
GuiFileXferSession* AppletUploads::widgetToSession( FileXferWidget* item )
{
	return (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
}

//============================================================================
GuiFileXferSession* AppletUploads::findSession( VxGUID lclSessionId )
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
				LogMsg( LOG_ERROR, "AppletUploads::findSession null xferSession" );
			}
		}
		else
		{
			LogMsg( LOG_ERROR, "AppletUploads::findSession null item" );
		}
	}

	return NULL;
}

//============================================================================
FileXferWidget* AppletUploads::findListEntryWidget( VxGUID lclSessionId )
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
					return (FileXferWidget*)item;
				}
			}
			else
			{
				LogMsg( LOG_ERROR, "AppletUploads::findListEntryWidget null xferSession" );
			}
		}
		else
		{
			LogMsg( LOG_ERROR, "AppletUploads::findListEntryWidget null item" );
		}
	}

	return NULL;
}

//============================================================================
FileXferWidget* AppletUploads::addUpload( GuiFileXferSession* xferSession )
{
	FileXferWidget* item = sessionToWidget( xferSession );

	ui.m_FileItemList->addItem( item );
	ui.m_FileItemList->setItemWidget( item, item );
	if( eXferStateUploadNotStarted == xferSession->getXferState() || eXferStateDownloadNotStarted == xferSession->getXferState() )
	{
		std::string fileName = xferSession->getFileNameAndPath().toUtf8().constData();
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

	return item;
}

//============================================================================
bool AppletUploads::isUploadInProgress( VxGUID fileInstance )
{
	if( findSession( fileInstance ) )
	{
		return true;
	}

	return false;
}

//============================================================================
void AppletUploads::toGuiFileUploadStart( GuiFileXferSession* xferSession )
{
	updateSession( xferSession );
}

//============================================================================
void AppletUploads::toGuiFileXferState( EPluginType pluginType, VxGUID& lclSessionId, EXferDirection xferDir, EXferState xferState, EXferError xferErr, int param1 )
{
    if( eXferDirectionTx == xferDir )
    {
        FileXferWidget* item = findListEntryWidget( lclSessionId );
        if( item )
        {
            item->setXferState( xferState, xferErr, param1 );
        }
    }
}

//============================================================================
void AppletUploads::toGuiFileUploadComplete( EPluginType pluginType, VxGUID& lclSessionId, EXferError xferError )
{
	FileXferWidget* item = findListEntryWidget( lclSessionId );
	if( item )
	{
        item->setXferState( eXferStateCompletedUpload, xferError, xferError );
	}
}

//============================================================================
void AppletUploads::slotFileXferItemClicked(QListWidgetItem* item)
{
}

//============================================================================
void AppletUploads::slotFileIconButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1).toULongLong();
	if( xferSession )
	{

	}	
}

//============================================================================
void AppletUploads::slotAcceptButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
	if( xferSession )
	{
		m_MyApp.getFileXferMgr().acceptUpload( getAppletType(), xferSession );
	}

	ui.m_FileItemList->removeItemWidget( item );
}

//============================================================================
void AppletUploads::slotCancelButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1).toULongLong();
	if( xferSession )
	{
		if( eXferStateCompletedUpload != xferSession->getXferState() )
		{
            xferSession->setXferState( eXferStateUserCanceledUpload, eXferErrorNone, 0 );
            ((FileXferWidget*)item)->setXferState( eXferStateUserCanceledUpload, eXferErrorNone, 0 );
			m_Engine.fromGuiCancelUpload( xferSession->getLclSessionId() );
		}
	}

	ui.m_FileItemList->removeItemWidget( item );
}

//============================================================================
void AppletUploads::slotPlayButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1).toULongLong();
	if( xferSession )
	{
		this->playFile( xferSession->getFileNameAndPath(), 0, false, false );
	}
}

//============================================================================
void AppletUploads::slotPlayExternButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1).toULongLong();
	if( xferSession )
	{
		this->playFile( xferSession->getFileNameAndPath(), 0, false, true );
	}
}

//============================================================================
void AppletUploads::slotLibraryButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1).toULongLong();
	if( xferSession )
	{
		bool isInLibrary = xferSession->getIsInLibrary();
		isInLibrary = !isInLibrary;
		xferSession->setIsInLibrary( isInLibrary );
		m_Engine.fromGuiSetFileIsInLibrary( xferSession->getFileInfo(), isInLibrary );
		((FileXferWidget*)item)->updateWidgetFromInfo();
	}	
}

//============================================================================
void AppletUploads::slotFileShareButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1).toULongLong();
	if( xferSession )
	{
		bool isShared = xferSession->getIsSharedFile();
		isShared = !isShared;
		xferSession->setIsSharedFile( isShared );
		m_Engine.fromGuiSetFileIsShared(  xferSession->getFileInfo(), isShared );
		((FileXferWidget*)item)->updateWidgetFromInfo();
	}	
}

//============================================================================
void AppletUploads::slotShredButtonClicked( QListWidgetItem* item )
{
	GuiFileXferSession* xferSession = (GuiFileXferSession*)item->QListWidgetItem::data( Qt::UserRole + 1).toULongLong();
	if( xferSession )
	{
		QString fileName = xferSession->getFileNameAndPath();
		if( confirmDeleteFile( true ) )
		{
			ui.m_FileItemList->removeItemWidget( item );
			m_Engine.fromGuiDeleteFile( fileName.toUtf8().constData(), true );
		}
	}	
}

//============================================================================
bool AppletUploads::confirmDeleteFile( bool shredFile )
{
	bool acceptAction = true;
	bool isConfirmDisabled = m_MyApp.getAppSettings().getIsConfirmDeleteDisabled();
	if( false == isConfirmDisabled )
	{
		QString title = shredFile ?  QObject::tr("Confirm Shred File") :  QObject::tr("Confirm Delete File");
		QString bodyText = "";
		if( shredFile )
		{
			bodyText =  QObject::tr("Are You Sure You Want To Write Random Data Into The File Then Delete From The Device?");
		}
		else
		{
			bodyText =  QObject::tr("Are You Sure To Delete The File From The Device?");
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
void AppletUploads::populateList( std::vector<GuiFileXferSession*>& sessionList )
{
	for( auto xferSession : sessionList )
	{
		updateSession( xferSession );
	}
}

//============================================================================
void AppletUploads::updateSession( GuiFileXferSession* xferSession, bool resetState )
{
	GuiFileXferSession* existingSession = findSession( xferSession->getLclSessionId() );
	if( existingSession )
	{
		existingSession->updateSession( xferSession );
	}
	else
	{
		GuiFileXferSession* newSession = new GuiFileXferSession( *xferSession );
		newSession->setXferDirection( eXferDirectionTx );

		FileXferWidget* item = addUpload( newSession );
		if( resetState )
		{
			item->setXferState( eXferStateInUploadXfer, eXferErrorNone, 0 );
		}
	}
}
