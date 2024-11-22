//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "SndWriter.h"
#include "OpusAudioEncoder.h"
#include "OpusFileEncoder.h"

#include <P2PEngine/P2PEngine.h>
#include <AssetMgr/AssetInfo.h>
#include <MediaProcessor/MediaProcessor.h>

#include <PktLib/PktVoiceReq.h>
#include <PktLib/PktVoiceReply.h>
#include <PktLib/PktChatReq.h>

#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxFileShredder.h>
#include <CoreLib/VxMacros.h>


//============================================================================
SndWriter::SndWriter( P2PEngine& engine, MediaProcessor& mediaProcessor )
: m_Engine( engine )
, m_MediaProcessor( mediaProcessor )
, m_EPluginType( ePluginTypeSndWriter )
, m_IsRecording( false )
, m_IsRecordingPaused( false )
, m_FileName( "" )
, m_FileHandle( 0 )
, m_MicroSecBetweenFrames( 0 )
, m_TotalElapsedMs( 0 )
, m_IsFirstFrameAfterResumeRecording( 0 )
, m_OpusEncoder( * ( new OpusAudioEncoder( MY_OPUS_SAMPLE_RATE, 1 ) ) )
, m_OpusFileEncoder( * ( new OpusFileEncoder() ) )
{
}

//============================================================================
SndWriter::~SndWriter()
{
	delete &m_OpusEncoder;
}

//============================================================================
bool SndWriter::fromGuiAssetAction( AssetBaseInfo& assetInfo, EAssetAction assetAction, int pos0to100000 )
{
	bool result = false;
	switch( assetAction )
	{
	case eAssetActionRecordBegin:
		return fromGuiSndRecord( eSndRecordStateStartRecording, assetInfo.getHistoryId(), assetInfo.getAssetName().c_str() );

	case eAssetActionRecordEnd:
		return fromGuiSndRecord( eSndRecordStateStopRecording, assetInfo.getHistoryId(), assetInfo.getAssetName().c_str() );

	case eAssetActionRecordCancel:
		fromGuiSndRecord( eSndRecordStateStopRecording, assetInfo.getHistoryId(), assetInfo.getAssetName().c_str() );
		GetVxFileShredder().shredFile( assetInfo.getAssetName() );
		break;

	default:
		break;
	}

	return result;

}

//============================================================================
bool SndWriter::fromGuiSndRecord( ESndRecordState eRecState, VxGUID& feedId, const char* fileName )
{
	bool result = false;
	switch( eRecState )
	{
	case eSndRecordStateStopRecording:
		if( getIsRecording() )
		{
			stopSndWrite();
			result = true;
		}
		break;

	case eSndRecordStateStartRecording:
		if( !getIsRecording() )
		{
			result = startSndWrite( fileName, false );
		}

		break;

	case eSndRecordStateStartRecordingInPausedState:
		if( !getIsRecording() )
		{
			result = startSndWrite( fileName, true );
		}

		break;

	case eSndRecordStatePauseRecording:
		setIsRecordingPaused( true );
		break;

	case eSndRecordStateResumeRecording:
		setIsRecordingPaused( false );
		break;

	case eSndRecordStateCancelRecording:
		result = true;
		if( getIsRecording() )
		{
			stopSndWrite();
			std::string strFileName( fileName );
			GetVxFileShredder().shredFile( strFileName );
		}

		break;

	case eSndRecordStateDisabled:
	case eSndRecordStateError:
	default:
		break;
	}

	return result;
}

//============================================================================
bool SndWriter::startSndWrite( const char* fileName, bool beginInPausedState )
{
	stopSndWrite();
#ifdef MAKE_PCM_INSTEAD_OF_OPUS
	bool result = true;
	m_FileHandle = fopen( fileName, "wb+" );
	if( 0 == m_FileHandle )
	{
		LogMsg( LOG_ERROR, "startSndWrite:: pcm Write could not open file to write %s\n", fileName );
		result = false;
	}
#else
	m_FileName = fileName;
	bool result = m_OpusFileEncoder.beginFileEncode( fileName );
#endif //MAKE_PCM_INSTEAD_OF_OPUS
	if( result )
	{
		setIsRecordingPaused( beginInPausedState );
		setIsRecording( true );
		m_Engine.getMediaProcessor().wantMediaInput( m_Engine.getMyOnlineId(), eMediaInputAudioOpus, this,  eAppModuleMediaWriter, true );
	}

	return result;
}

//============================================================================
void SndWriter::setIsRecordingPaused( bool pause )
{
	if( pause )
	{
		if( getIsRecording() )
		{
			if( !m_IsFirstFrameAfterResumeRecording )
			{
				m_TotalElapsedMs += m_RecordElapseTimer.elapsedMs();
			}
		}
		
		m_IsRecordingPaused = true;
	}
	else
	{
		m_IsFirstFrameAfterResumeRecording = true;
		m_IsRecordingPaused = false;
	}
}

//============================================================================
void SndWriter::stopSndWrite( void )
{
	if( getIsRecording() )
	{
		setIsRecordingPaused( true );
		m_Engine.getMediaProcessor().wantMediaInput( m_Engine.getMyOnlineId(), eMediaInputAudioOpus, this,  eAppModuleMediaWriter, false );

#ifdef MAKE_PCM_INSTEAD_OF_OPUS
		if( m_FileHandle )
		{
			fclose( m_FileHandle );
			m_FileHandle = 0;
		}
#else
		m_OpusFileEncoder.finishFileEncode();
#endif // MAKE_PCM_INSTEAD_OF_OPUS		

		setIsRecording( false );
	}
}

//============================================================================
void SndWriter::closeSndFile( void )
{
	if( 0 != m_FileHandle )
	{
		fclose( m_FileHandle );
		m_FileHandle = 0;
	}
}

//============================================================================
void SndWriter::callbackOpusEncoded( uint8_t* encodedAudio, std::vector<uint16_t>& encodedLenList )
{
	if( getIsRecording() )
	{
#ifdef MAKE_PCM_INSTEAD_OF_OPUS
		if( m_FileHandle )
		{
			fwrite( pu16PcmData, 1, u16PcmDataLen, m_FileHandle );
		}
#else
		int encodedOffs{ 0 };
		for( auto encodedLen : encodedLenList )
		{
			if( encodedLen )
			{
				m_OpusFileEncoder.writeEncodedFrame( &encodedAudio[ encodedOffs ], encodedLen );
				encodedOffs += encodedLen;
			}
		}
#endif // MAKE_PCM_INSTEAD_OF_OPUS
	}
}
