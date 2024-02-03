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
#include "ChatEntryWidget.h"
#include "DialogAddComment.h"
#include "InputClientCallback.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxFileUtil.h>

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include <time.h>

//============================================================================
ChatEntryWidget::ChatEntryWidget( QWidget* parent, EAssetType inputMode )
: QWidget( parent )
, m_InputMode( inputMode )
{
	qDebug() << "ChatEntryWidget input mode " << inputMode;
	ui.setupUi(this);


	connect( ui.m_InputAllWidget,		SIGNAL(signalUserInputButtonClicked()),		this, SIGNAL(signalUserInputButtonClicked()) );

	connect( ui.m_InputAllWidget,		SIGNAL(signalAllTextButtonClicked()),		this, SLOT(slotAllTextButtonClicked()) );
	connect( ui.m_InputAllWidget,		SIGNAL(signalAllEmoteButtonClicked()),		this, SLOT(slotAllEmoteButtonClicked()) );
	connect( ui.m_InputAllWidget,		SIGNAL(signalAllVideoButtonClicked()),		this, SLOT(slotAllVideoButtonClicked()) );
	connect( ui.m_InputAllWidget,		SIGNAL(signalAllCameraButtonClicked()),		this, SLOT(slotAllCameraButtonClicked()) );
	connect( ui.m_InputAllWidget,		SIGNAL(signalAllGalleryButtonClicked()),	this, SLOT(slotAllGalleryButtonClicked()) );
	connect( ui.m_InputAllWidget,		SIGNAL(signalAllMicButtonPressed()),		this, SLOT(slotAllMicButtonPressed()) );
	connect( ui.m_InputAllWidget,		SIGNAL(signalAllMicButtonReleased()),		this, SLOT(slotAllMicButtonReleased()) );

	connect( ui.m_InputPhotoWidget,		SIGNAL(signalInputCompleted()),				this, SLOT(slotInputCompleted()) );
	connect( ui.m_InputTextWidget,		SIGNAL(signalInputCompleted()),				this, SLOT(slotInputCompleted()) );
	connect( ui.m_InputFaceWidget,		SIGNAL(signalInputCompleted()),				this, SLOT(slotInputCompleted()) );
	connect( ui.m_InputVideoWidget,		SIGNAL(signalInputCompleted()),				this, SLOT(slotInputCompleted()) );
	connect( ui.m_InputVoiceWidget,		SIGNAL(signalInputCompleted()),				this, SLOT(slotInputCompleted()) );

	setEntryMode( m_InputMode );
}

//============================================================================
void ChatEntryWidget::setAppModule( EAppModule appModule )
{
	m_AppModule = appModule;

	ui.m_InputAllWidget->setAppModule( appModule );
	ui.m_InputFaceWidget->setAppModule( appModule );
	ui.m_InputPhotoWidget->setAppModule( appModule );
	ui.m_InputTextWidget->setAppModule( appModule );
	ui.m_InputVideoWidget->setAppModule( appModule );
	ui.m_InputVoiceWidget->setAppModule( appModule );
}

//============================================================================
void ChatEntryWidget::setPluginType( EPluginType pluginType )
{
	m_PluginType = pluginType;

	ui.m_InputAllWidget->setPluginType( pluginType );
	ui.m_InputFaceWidget->setPluginType( pluginType );
	ui.m_InputPhotoWidget->setPluginType( pluginType );
	ui.m_InputTextWidget->setPluginType( pluginType );
	ui.m_InputVideoWidget->setPluginType( pluginType );
	ui.m_InputVoiceWidget->setPluginType( pluginType );
}

//============================================================================
void ChatEntryWidget::setGroupieId( GroupieId& groupieId )
{
	m_GroupieId = groupieId;

	ui.m_InputAllWidget->setGroupieId( groupieId );
	ui.m_InputFaceWidget->setGroupieId( groupieId );
	ui.m_InputPhotoWidget->setGroupieId( groupieId );
	ui.m_InputTextWidget->setGroupieId( groupieId );
	ui.m_InputVideoWidget->setGroupieId( groupieId );
	ui.m_InputVoiceWidget->setGroupieId( groupieId );
}

//============================================================================
void ChatEntryWidget::setCanSend( bool canSend )
{
	if( false == canSend )
	{
		setEntryMode( eAssetTypeUnknown );
	}

	ui.m_InputAllWidget->setCanSend( canSend );
	ui.m_InputFaceWidget->setCanSend( canSend );
	ui.m_InputPhotoWidget->setCanSend( canSend );
	ui.m_InputTextWidget->setCanSend( canSend );
	ui.m_InputVideoWidget->setCanSend( canSend );
	ui.m_InputVoiceWidget->setCanSend( canSend );
}

//============================================================================
void ChatEntryWidget::hideVideoCaptureInput( void )
{
	ui.m_InputAllWidget->hideVideoCaptureInput();
}

//============================================================================
void ChatEntryWidget::callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 )
{
	switch( m_InputMode )
	{
	case eAssetTypeVideo:
		ui.m_InputVideoWidget->callbackGuiPlayMotionVideoFrame( feedOnlineId, vidFrame, motion0To100000 );
		break;
	case eAssetTypePhoto:
		ui.m_InputPhotoWidget->callbackGuiPlayMotionVideoFrame( feedOnlineId, vidFrame, motion0To100000 );
		break;
	default:
		break;
	}
}

//============================================================================
void ChatEntryWidget::slotAllTextButtonClicked( void )
{
	setEntryMode( eAssetTypeChatText );	
}

//============================================================================
void ChatEntryWidget::slotAllEmoteButtonClicked( void )
{
	setEntryMode( eAssetTypeChatFace );	
}

//============================================================================
void ChatEntryWidget::slotAllVideoButtonClicked( void )
{
	setEntryMode( eAssetTypeVideo );	
}

//============================================================================
void ChatEntryWidget::slotAllCameraButtonClicked( void )
{
	setEntryMode( eAssetTypePhoto );	
}

#define SUPPORTED_IMAGE_FILES "Image files (*.bmp *.jpg *.jpeg *.png)"

//============================================================================
void ChatEntryWidget::slotAllGalleryButtonClicked( void )
{
    if( !m_GroupieId.isHostedIdValid() )
	{
		return;
	}

	AppCommon& myApp = GetAppInstance();
	QString startPath = QDir::current().path();
	std::string lastGalleryPath;
	myApp.getAppSettings().getLastGalleryDir( lastGalleryPath );
	if( ( 0 != lastGalleryPath.length() )
		&& ( VxFileUtil::directoryExists( lastGalleryPath.c_str() ) ) )
	{
		startPath = lastGalleryPath.c_str();
	}

	// Get a filename from the file dialog.
	QString filename = QFileDialog::getOpenFileName(	this,
		QObject::tr("Open Image"),
		startPath,
		SUPPORTED_IMAGE_FILES);

	if( filename.length() > 0 )
	{
		QPixmap oBitmap;
		if( false == oBitmap.load(filename) )
		{
			QString msgText = QObject::tr("Failed To Read Image File ") + filename;
			QMessageBox::critical( this, QObject::tr("Error Reading Image"), msgText );
		}
		else
		{
			std::string justFileName;
			VxFileUtil::seperatePathAndFile( filename.toUtf8().constData(), lastGalleryPath, justFileName );
			if( ( 0 != lastGalleryPath.length() )
				&& ( VxFileUtil::directoryExists( lastGalleryPath.c_str() ) ) )
			{
				myApp.getAppSettings().setLastGalleryDir( lastGalleryPath );
			}

			AssetInfo assetInfo;
			assetInfo.setAssetType( eAssetTypePhoto );
			assetInfo.setAssetLength( VxFileUtil::getFileLen( filename.toUtf8().constData() ) );
			if( 0 == assetInfo.getAssetLength() )
			{
				LogMsg( LOG_ERROR, "InputBaseWidget::photo file has zero len %s", filename.toUtf8().constData() );
				return;
			}

			assetInfo.setAssetName( filename.toUtf8().constData() );
            assetInfo.setCreatorId( GetAppInstance().getUserMgr().getMyOnlineId() );
            assetInfo.setAdminId( m_GroupieId.getHostOnlineId() );
            assetInfo.setHistoryId( m_GroupieId.getUserOnlineId() );
			assetInfo.generateNewUniqueId();
			assetInfo.setCreationTime( GetTimeStampMs() );
			assetInfo.setPluginType( getPluginType() );
			DialogAddComment addCommentDlg( assetInfo, this );
			if( QDialog::Accepted == addCommentDlg.exec() )
			{
				QString commentText = addCommentDlg.getCommentText();
				if( !commentText.isEmpty() )
				{
					std::string tagText = commentText.toUtf8().constData();
					assetInfo.setAssetTag( tagText );
				}
				else
				{
					assetInfo.setAssetTag( "" );
				}

				GetAppInstance().getEngine().fromGuiAssetAction( eAssetActionAddToAssetMgr, assetInfo );
			}
		}
	}	
}

//============================================================================
void ChatEntryWidget::slotAllMicButtonPressed( void )
{
	setEntryMode( eAssetTypeAudio );	
}

//============================================================================
void ChatEntryWidget::slotInputCompleted( void )
{
	setEntryMode( eAssetTypeUnknown );	
    setVisible( true );
}

//============================================================================
void ChatEntryWidget::slotAllMicButtonReleased( void )
{
	//cancelInput();
}

//============================================================================
void ChatEntryWidget::setEntryMode( EAssetType inputMode )
{
	ui.m_InputAllWidget->setVisible( false );
	ui.m_InputFaceWidget->setVisible( false );
	ui.m_InputPhotoWidget->setVisible( false );
	ui.m_InputVoiceWidget->setVisible( false );
	ui.m_InputVideoWidget->setVisible( false );
	ui.m_InputTextWidget->setVisible( false );

	switch( inputMode )
	{
	case eAssetTypeChatText:
		ui.m_InputTextWidget->setVisible( true );
		ui.m_InputTextWidget->setFocus();
		break;

	case eAssetTypeChatFace:
		ui.m_InputFaceWidget->setVisible( true );
		break;

	case eAssetTypePhoto:
		ui.m_InputPhotoWidget->setVisible( true );
		break;

	case eAssetTypeAudio:
		ui.m_InputVoiceWidget->setVisible( true );
		ui.m_InputVoiceWidget->startRecording();
		break;

	case eAssetTypeVideo:
		ui.m_InputVideoWidget->setVisible( true );
		break;

	default:

		if( eAssetTypeVideo == m_InputMode )
		{
			AppCommon& myApp = GetAppInstance();
			QString useMsg = QObject::tr( "Use camcorder button on left to start and stop recording video." );
			myApp.toGuiUserMessage( useMsg.toUtf8().constData() );
			//QMessageBox::information( this, QObject::tr( "Information" ), QObject::tr( "Use camcorder button on left to start and stop recording video." ), QObject::tr( "OK" ) );
		}

		ui.m_InputTextWidget->setVisible( false );
		ui.m_InputFaceWidget->setVisible( false );
		ui.m_InputPhotoWidget->setVisible( false );
		ui.m_InputVoiceWidget->setVisible( false );
		ui.m_InputVideoWidget->setVisible( false );
		ui.m_InputAllWidget->setVisible( true );
		break;
	}

	m_InputMode = inputMode;
}

//============================================================================
void ChatEntryWidget::setIsPersonalRecorder( bool isPersonal )
{
	m_IsPersonalRecorder = isPersonal;
	ui.m_InputAllWidget->setIsPersonalRecorder( isPersonal );
	ui.m_InputFaceWidget->setIsPersonalRecorder( isPersonal );
	ui.m_InputPhotoWidget->setIsPersonalRecorder( isPersonal );
	ui.m_InputTextWidget->setIsPersonalRecorder( isPersonal );
	ui.m_InputVideoWidget->setIsPersonalRecorder( isPersonal );
	ui.m_InputVoiceWidget->setIsPersonalRecorder( isPersonal );
}

//============================================================================
void ChatEntryWidget::setInputClientCallback( InputClientCallback* clientCallback ) 
{ 
	m_ClientCallback = clientCallback; 
	ui.m_InputAllWidget->setInputClientCallback( clientCallback );
	ui.m_InputFaceWidget->setInputClientCallback( clientCallback );
	ui.m_InputPhotoWidget->setInputClientCallback( clientCallback );
	ui.m_InputTextWidget->setInputClientCallback( clientCallback );
	ui.m_InputVideoWidget->setInputClientCallback( clientCallback );
	ui.m_InputVoiceWidget->setInputClientCallback( clientCallback );
}

//============================================================================
bool ChatEntryWidget::checkIfCanSend( void )
{
	if( !m_ClientCallback )
	{
		QMessageBox::critical( this, QObject::tr( "Application Error" ), QObject::tr( "ChatEntryWidget client callback is not set" ) );
		return false;
	}

	if( m_IsPersonalRecorder )
	{
		return true;
	}

	return m_ClientCallback->checkIfCanSend();
}
