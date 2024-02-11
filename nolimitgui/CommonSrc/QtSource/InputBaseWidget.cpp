//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "InputBaseWidget.h"

#include "AppCommon.h"
#include "ChatEntryWidget.h"
#include "DialogAddComment.h"
#include "GuiHelpers.h"
#include "InputClientBaseCallback.h"

#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxTime.h>
#include <CoreLib/VxTimeUtil.h>

#include <P2PEngine/P2PEngine.h>

#include <MediaToolsLib/OpusAudioDecoder.h>

#include <QTimer>

//============================================================================
InputBaseWidget::InputBaseWidget( AppCommon& app, QWidget* parent )
: QWidget( parent )
, m_MyApp( app )
, m_ChatEntryWidget( (ChatEntryWidget *)parent )
, m_ElapseTimer( new QTimer( this ) )
{
	connect( this,			SIGNAL(signalChatMessage(QString)), this, SLOT(slotChatMessage(QString)) );
	connect( m_ElapseTimer, SIGNAL(timeout()),					this, SLOT(slotElapsedTimerTimeout()) );
}

//============================================================================
MyIcons& InputBaseWidget::getMyIcons( void )
{
	return m_MyApp.getMyIcons();
}

//============================================================================
void InputBaseWidget::setInputClientCallback( InputClientBaseCallback* clientCallback ) 
{ 
	m_ClientCallback = clientCallback; 
	fillAssetBaseInfo( true );
}

//============================================================================
void InputBaseWidget::setGroupieId( GroupieId& groupieId )
{ 
    m_GroupieId = groupieId;

    m_AssetInfo.setCreatorId( m_MyApp.getUserMgr().getMyOnlineId() );
    m_AssetInfo.setAdminId( groupieId.getHostOnlineId() );
    m_AssetInfo.setHistoryId( groupieId.getUserOnlineId() );
}

//============================================================================
void InputBaseWidget::setIsPersonalRecorder( bool isPersonal )
{
	m_IsPersonalRecorder = isPersonal;
	m_AssetInfo.setIsPersonalRecord( isPersonal );
}

//============================================================================
void InputBaseWidget::setIsChatRoom( bool isChatRoom )
{
    m_IsPersonalRecorder = false;
    m_AssetInfo.setIsChatRoomRecord( isChatRoom );
}

//============================================================================
bool InputBaseWidget::canAcceptInput( EAssetType assetType )
{
	return m_ClientCallback && m_ClientCallback->canAcceptInput( assetType );
}

//============================================================================
void InputBaseWidget::slotChatMessage( QString chatMsg )
{
}

//============================================================================
bool InputBaseWidget::voiceRecord( EAssetAction action )
{
	if( !checkIfCanSend() )
	{
		return false;
	}

	EAssetType assetType = eAssetTypeAudio;
	bool actionResult = true;
    switch( action )
    {
    case eAssetActionRecordBegin:
		fillAssetBaseInfo( true );
        m_AssetInfo.generateNewUniqueId();
        if( generateFileName( assetType, m_AssetInfo.getAssetUniqueId() ) )
        {
            m_AssetInfo.setAssetName( m_FileName );
            actionResult = m_MyApp.getEngine().fromGuiSndRecord( eSndRecordStateStartRecording, m_AssetInfo.getCreatorId(), m_FileName.c_str() );
            if( true == actionResult )
            {
                m_TimeRecStart = GetTimeStampMs();
                m_TimeRecCurrent = m_TimeRecStart;
                m_AssetInfo.setCreationTime( m_TimeRecCurrent );
                m_ElapseTimer->start( 400 );
            }
            else
            {
                LogMsg( LOG_ERROR, "Could Not start Audio Record" );
            }
        }

		break;

	case eAssetActionRecordEnd:
		m_ElapseTimer->stop();
		updateElapsedTime();
		if( 0 == ( m_TimeRecCurrent - m_TimeRecStart ) )
		{
			// if only pressed for a second then ignore
			m_MyApp.getEngine().fromGuiSndRecord( eSndRecordStateCancelRecording, m_AssetInfo.getCreatorId(), m_FileName.c_str() );
			actionResult = false;
		}
		else
		{
			m_MyApp.getEngine().fromGuiSndRecord( eSndRecordStateStopRecording, m_AssetInfo.getCreatorId(), m_FileName.c_str() );

			uint64_t fileLen = VxFileUtil::getFileLen( m_FileName.c_str() );
			m_AssetInfo.setAssetLength( fileLen );
			if( MIN_OPUS_FILE_LEN < m_AssetInfo.getAssetLength() )
			{
				if( addOptionalComment() )
				{
					m_ClientCallback->handleAssetAction( m_IsPersonalRecorder ? eAssetActionAddToAssetMgr : eAssetActionAddAssetAndSend, m_AssetInfo );
					actionResult = true;
				}
			}
			else
			{
				LogMsg( LOG_ERROR, "InputBaseWidget::voiceRecord file len %s is too short% s", 
						GuiParams::describeFileLength( m_AssetInfo.getAssetLength() ).toUtf8().constData(), m_FileName.c_str());
				m_MyApp.toGuiUserMessage( "Voice Record file len %s is too short %s", 
										 GuiParams::describeFileLength( m_AssetInfo.getAssetLength() ).toUtf8().constData(), m_FileName.c_str() );
				GuiHelpers::errorMsgBox( eErrMsgVoiceMessageTooShort, GuiHelpers::getParentPageFrame( this ) );
				VxFileUtil::deleteFile( m_FileName.c_str() );
				actionResult = false;
			}
		}

		break;

	case eAssetActionRecordCancel:
		m_ElapseTimer->stop();
		m_MyApp.getEngine().fromGuiSndRecord( eSndRecordStateCancelRecording, m_AssetInfo.getCreatorId(), m_FileName.c_str() );	
		actionResult = true;
		break;

    default:
        break;
	}

	return actionResult;
}

//============================================================================
bool InputBaseWidget::videoRecord( EAssetAction action )
{
	if( !checkIfCanSend() )
	{
		return false;
	}

	EAssetType assetType = eAssetTypeVideo;
	bool actionResult = true;
	switch( action )
	{
	case eAssetActionRecordBegin:
		fillAssetBaseInfo( true );
		m_AssetInfo.generateNewUniqueId();
        if( generateFileName( assetType, m_AssetInfo.getAssetUniqueId() ) )
        {
		    m_AssetInfo.setAssetName( m_FileName );
		    actionResult = m_MyApp.getEngine().fromGuiVideoRecord( eVideoRecordStateStartRecording, m_AssetInfo.getCreatorId(), m_FileName.c_str() );
		    if( true == actionResult )
		    {
			    m_TimeRecStart		= GetTimeStampMs();
			    m_TimeRecCurrent	= m_TimeRecStart;
			    m_AssetInfo.setCreationTime( m_TimeRecCurrent );
			    m_ElapseTimer->start( 400 );
		    }
		    else
		    {
			    LogMsg( LOG_ERROR, "Could Not start Video Record" );
				GuiHelpers::errorMsgBox( eErrMsgVideoClipFailedToStart, GuiHelpers::getParentPageFrame( this ) );
		    }
        }

		break;

	case eAssetActionRecordEnd:
		{
			m_ElapseTimer->stop();
			updateElapsedTime();
			if( 0 == ( m_TimeRecCurrent - m_TimeRecStart ) )
			{
				// if only pressed for a second then ignore
				m_MyApp.getEngine().fromGuiVideoRecord( eVideoRecordStateCancelRecording, m_AssetInfo.getCreatorId(), m_FileName.c_str() );	
				actionResult = false;
			}
			else
			{
				m_MyApp.getEngine().fromGuiVideoRecord( eVideoRecordStateStopRecording, m_AssetInfo.getCreatorId(), m_FileName.c_str() );
				uint64_t fileLen = VxFileUtil::getFileLen( m_FileName.c_str() );
				if( 5000 > fileLen )
				{
					// not long enough to be a recording
					VxFileUtil::deleteFile( m_FileName.c_str() );
					m_MyApp.toGuiUserMessage( "InputBaseWidget::videoRecord file %s has to short len %s", 
							m_FileName.c_str(), GuiParams::describeFileLength( fileLen ).toUtf8().constData() );
					GuiHelpers::errorMsgBox( eErrMsgVideoClipTooShort, GuiHelpers::getParentPageFrame( this ) );
					actionResult = false;
				}
				else
				{
					m_AssetInfo.setAssetLength( fileLen );
					if( addOptionalComment() )
					{
						m_ClientCallback->handleAssetAction( m_IsPersonalRecorder ? eAssetActionAddToAssetMgr : eAssetActionAddAssetAndSend, m_AssetInfo );
						actionResult = true;
					}
				}
			}
		}

		break;

	case eAssetActionRecordCancel:
		m_ElapseTimer->stop();
		m_MyApp.getEngine().fromGuiVideoRecord( eVideoRecordStateCancelRecording, m_AssetInfo.getCreatorId(), m_FileName.c_str() );	
		actionResult = true;
		break;

    default:
        break;
	}

	return actionResult;
}

//============================================================================
void InputBaseWidget::slotElapsedTimerTimeout( void )
{
	updateElapsedTime();
}

//============================================================================
void InputBaseWidget::updateElapsedTime( void )
{
	m_TimeRecCurrent = GetTimeStampMs();
	time_t timeDif = m_TimeRecCurrent - m_TimeRecStart;
	std::string elapsedTime = VxTimeUtil::formatTimeDiffIntoMinutesAndSeconds( timeDif );
	emit signalElapsedRecTime( elapsedTime.c_str() );
}

//============================================================================
bool InputBaseWidget::generateFileName( EAssetType assetType, VxGUID& uniqueId )
{
	return canAcceptInput( assetType ) && m_ClientCallback->generateFileName( assetType, uniqueId, m_FileName );
}

//============================================================================
bool InputBaseWidget::fillAssetBaseInfo( bool newAssetId )
{
	return m_ClientCallback && m_ClientCallback->fillAssetBaseInfo( m_AssetInfo, newAssetId );
}

//============================================================================
bool InputBaseWidget::addOptionalComment( void )
{
	bool wasAccepted{ false };
	DialogAddComment addCommentDlg( m_AssetInfo, this );
	if( QDialog::Accepted == addCommentDlg.exec() )
	{
		wasAccepted = true;
		QString commentText = addCommentDlg.getCommentText();
		if( !commentText.isEmpty() )
		{
			std::string tagText = commentText.toUtf8().constData();
			m_AssetInfo.setAssetTag( tagText );
		}
		else
		{
			m_AssetInfo.setAssetTag( "" );
		}
	}

	return wasAccepted;
}

//============================================================================
bool InputBaseWidget::checkIfCanSend( void )
{
	return m_ChatEntryWidget->checkIfCanSend();
}
