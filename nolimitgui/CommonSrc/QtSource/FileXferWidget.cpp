//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "FileXferWidget.h"
#include "AppCommon.h"
#include "MyIcons.h"

#include "GuiFileXferSession.h"
#include <Plugins/FileInfo.h>
#include "GuiHelpers.h"
#include "GuiParams.h"

//============================================================================
FileXferWidget::FileXferWidget(QWidget* parent)
: QWidget(parent)
, m_MyApp( GetAppInstance() )
{
	ui.setupUi(this);
	ui.m_FileIconButton->setFixedSize( eButtonSizeLarge );
	ui.m_FileAcceptButton->setFixedSize( eButtonSizeLarge );
	ui.m_FileCancelButton->setFixedSize( eButtonSizeLarge );

	ui.m_FileAcceptButton->setIcon( eMyIconAcceptNormal );
	ui.m_FileAcceptButton->setIconOverrideColor( m_MyApp.getAppTheme().getAcceptColor() );
	ui.m_FileAcceptButton->setVisible( false );

	connect( ui.m_FileAcceptButton,		SIGNAL( clicked() ),					this, SLOT( slotAcceptButtonClicked() ) );
	connect( ui.m_FileIconButton,		SIGNAL(clicked()),						this, SLOT(slotFileIconButtonClicked()) );
	connect( ui.m_FileCancelButton,		SIGNAL(clicked()),						this, SLOT(slotCancelButtonClicked()) );
	connect( ui.m_FileActionBar,		SIGNAL(signalPlayButtonClicked()),		this, SLOT(slotPlayButtonClicked()) );
	connect( ui.m_FileActionBar,		SIGNAL(signalLibraryButtonClicked()),	this, SLOT(slotLibraryButtonClicked()) );
	connect( ui.m_FileActionBar,		SIGNAL(signalFileShareButtonClicked()), this, SLOT(slotFileShareButtonClicked()) );
	connect( ui.m_FileActionBar,		SIGNAL(signalShredButtonClicked()),		this, SLOT(slotShredButtonClicked()) );
}

//============================================================================
FileXferWidget::~FileXferWidget()
{
	GuiFileXferSession* fileItemInfo = (GuiFileXferSession*)QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
    if( fileItemInfo && !fileItemInfo->parent() )
    {
        delete fileItemInfo;
    }
}

//============================================================================
MyIcons& FileXferWidget::getMyIcons( void )
{
	return m_MyApp.getMyIcons();
}

//============================================================================
void FileXferWidget::mousePressEvent(QMouseEvent * event)
{
	QWidget::mousePressEvent(event);
	emit signalFileXferItemClicked( this );
}

//============================================================================
void FileXferWidget::setXferState( EXferState xferState, EXferError xferErr, int param1 )
{
	GuiFileXferSession* sessionInfo = getFileItemInfo();
	if( sessionInfo )
	{
        sessionInfo->setXferState( xferState, xferErr, param1 );
		updateXferInfo();
	}
}

//============================================================================
void FileXferWidget::slotFileIconButtonClicked( void )
{
	emit signalFileIconButtonClicked( this );
}

//============================================================================
void FileXferWidget::slotAcceptButtonClicked( void )
{
	emit signalAcceptButtonClicked( this );
}

//============================================================================
void FileXferWidget::slotCancelButtonClicked( void )
{
	emit signalCancelButtonClicked( this );
}

//============================================================================
void FileXferWidget::slotPlayButtonClicked( void )
{
	emit signalPlayButtonClicked( this );
}

//============================================================================
void FileXferWidget::slotLibraryButtonClicked( void )
{
	emit signalLibraryButtonClicked( this );
}

//============================================================================
void FileXferWidget::slotFileShareButtonClicked( void )
{
	emit signalLibraryButtonClicked( this );
}

//============================================================================
void FileXferWidget::slotShredButtonClicked( void )
{
	emit signalShredButtonClicked( this );
}

//============================================================================
void FileXferWidget::resizeEvent(QResizeEvent* resizeEvent)
{
	QWidget::resizeEvent(resizeEvent);
	updateWidgetFromInfo();
}

//============================================================================
bool FileXferWidget::getIsShared( void )
{
	bool isShared = false;
	GuiFileXferSession* poInfo = getFileItemInfo();
	if( poInfo )
	{
		isShared = poInfo->getIsSharedFile();
	}

	return isShared;
}

//============================================================================
void FileXferWidget::setIsSharedFile( bool isShared )
{
	GuiFileXferSession* sessionInfo = getFileItemInfo();
	if( sessionInfo )
	{
		sessionInfo->setIsSharedFile( isShared );
	}

	updateWidgetFromInfo();
}

//============================================================================
bool FileXferWidget::getIsInLibrary( void )
{
	bool isInLibrary = false;
	GuiFileXferSession* sessionInfo = getFileItemInfo();
	if( sessionInfo )
	{
		isInLibrary = sessionInfo->getIsInLibrary();
	}

	return isInLibrary;
}

//============================================================================
void FileXferWidget::setIsInLibrary( bool isInLibrary )
{
	GuiFileXferSession* sessionInfo = getFileItemInfo();
	if( sessionInfo )
	{
		sessionInfo->setIsInLibrary( isInLibrary );
	}

	updateWidgetFromInfo();
}

//============================================================================
void FileXferWidget::setFileItemInfo( GuiFileXferSession* fileItemInfo )
{
    QListWidgetItem::setData( Qt::UserRole + 1, QVariant((quint64)fileItemInfo) );
}

//============================================================================
GuiFileXferSession* FileXferWidget::getFileItemInfo( void )
{
	return (GuiFileXferSession*)QListWidgetItem::data( Qt::UserRole + 1 ).toULongLong();
}

//============================================================================
void FileXferWidget::setFileIconButtonEnabled( bool enable )
{
	ui.m_FileIconButton->setEnabled( enable );
}

//============================================================================
void FileXferWidget::setFileProgressBarValue( int val )
{
	ui.m_FileProgressBar->setValue( val );
}

//============================================================================
void FileXferWidget::updateWidgetFromInfo( void )
{
	GuiFileXferSession* xferSession = getFileItemInfo();
	if( !xferSession )
	{
		return;
	}

	ui.m_FileIconButton->setIcon( getMyIcons().getFileIcon( xferSession->getFileType() ) );
	if( VXFILE_TYPE_DIRECTORY == xferSession->getFileType() )
	{
		ui.m_FileIconButton->setIcon( eMyIconFileBrowseNormal );
		ui.m_FileSizeLabel->setVisible( false );
		ui.m_FileNameLabel->setText( xferSession->getFilePath() );
		ui.m_FilePathLabel->setVisible( false );
		ui.m_FileStatus->setVisible( false );
		ui.m_FileActionBar->setVisible( false );
		ui.m_FileProgressBar->setVisible( false );
		ui.m_FileCancelButton->setVisible( false );
	}
	else
	{
		ui.m_FileNameLabel->setText( xferSession->getJustFileName() );
		ui.m_FilePathLabel->setText( xferSession->getFilePath() );
		ui.m_FileSizeLabel->setText( xferSession->describeFileLength() );
		ui.m_FilePathLabel->setTextBreakAnywhere( xferSession->getFilePath(), 3 );

		updateXferInfo();
	}

	ui.m_FileActionBar->setIsSharedFile(  xferSession->getIsSharedFile() );
	ui.m_FileActionBar->setIsInLibrary(  xferSession->getIsInLibrary() );
}

//============================================================================
void FileXferWidget::updateXferInfo( void )
{
	GuiFileXferSession* xferSession = getFileItemInfo();
	if( !xferSession )
	{
		return;
	}

	EXferError xferError = xferSession->getXferError();
	if( xferError != eXferErrorNone && xferError != eXferErrorAlreadyDownloading &&
		xferError != eXferErrorAlreadyDownloaded && xferError != eXferErrorAlreadyUploading )
	{
		ui.m_FileStatus->setText( GuiParams::describeEXferError( xferError ) );
		if( eXferDirectionRx == xferSession->getXferDirection() )
		{
			ui.m_FileCancelButton->setIcons( eMyIconFileDownloadCancel );
		}
		else
		{
			ui.m_FileCancelButton->setIcons( eMyIconSendFileCancel );
		}

		return;
	}

	EXferState xferState = xferSession->getXferState();

	bool enableCancel = false;
	bool enablePlay = false;
	bool enableProgress = false;
	bool enableCompleted = false;
	
	ui.m_FileStatus->setText( xferSession->describeXferState() );
	switch( xferState )
	{
	case eXferStateInDownloadXfer:
	case eXferStateInUploadXfer:
		enableProgress = true;
		ui.m_FileProgressBar->setValue( xferSession->getXferProgress() );
		break;

	case eXferStateUploadError:
	case eXferStateDownloadError:
		ui.m_FileStatus->setText( GuiParams::describeEXferError( xferError ) );
		break;

	case eXferStateCompletedUpload:
	case eXferStateCompletedDownload:
		ui.m_FileAcceptButton->setVisible( true );
		ui.m_FileCancelButton->setVisible( false );
		
		enableCompleted = true;
		enableCancel = true;
		enablePlay = true;
		break;

	case eXferStateUserCanceledUpload:
	case eXferStateUserCanceledDownload:
	case eXferStateUploadOfferRejected:
		enableCancel = true;
		break;

	case eXferStateBeginUpload:
	case eXferStateBeginDownload:
		enableProgress = true;
		break;

	case eXferStateUnknown:
	case eXferStateWaitingOfferResponse:
	case eXferStateUploadNotStarted:
	case eXferStateInUploadQue:
	case eXferStateDownloadNotStarted:
	case eXferStateInDownloadQue:
	default:
		break;
	}

	if( enableCancel )
	{
		ui.m_FileCancelButton->setIcon( eMyIconCancelNormal );
	}
	else if( !enableCompleted )
	{
		if( eXferStateDownloadNotStarted == xferState )
		{
			ui.m_FileCancelButton->setIcons( eMyIconFileDownloadNormal );
#if ENABLE_STREAMING
			if( xferSession->isStremable() )
			{
				ui.m_FileAcceptButton->setIcons( eMyIconPlayStream );
				ui.m_FileCancelButton->setVisible( true );
			}
#endif // ENABLE_STREAMING
		}
		else if( eXferStateUploadNotStarted == xferState )
		{
			ui.m_FileCancelButton->setIcons( eMyIconSendFileNormal );
		}
		else
		{
			ui.m_FileCancelButton->setIcons( eMyIconCancelNormal );
		}
	}

	if( enablePlay )
	{
		ui.m_FileActionBar->setVisible( true );
	}
	else
	{
		ui.m_FileActionBar->setVisible( false );
	}

	if( enableProgress )
	{
		ui.m_FileProgressBar->setVisible( true );
	}
	else
	{
		ui.m_FileProgressBar->setVisible( false );
	}
}
