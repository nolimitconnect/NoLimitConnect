//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "OpusAudioEncoder.h"
#include "SndDefs.h"

#include "opus_multistream.h"

#include <CoreLib/IsBigEndianCpu.h>
#include <CoreLib/VxDebug.h>
#include <memory.h>

//============================================================================
OpusAudioEncoder::OpusAudioEncoder( int sampleRate, int channels )
: m_SampleRate( sampleRate )
, m_Channels( channels )
{
	initEncoder();
}

//============================================================================
void OpusAudioEncoder::initEncoder( void )
{
	// OPUS_APPLICATION_RESTRICTED_LOWDELAY, // RESTRICTED_LOWDELAY makes smaller AND takes about 1/2 the CPU time
	// OPUS_APPLICATION_VOIP // OPUS_APPLICATION_AUDIO
	m_Encoder = opus_encoder_create( m_SampleRate, m_Channels, OPUS_APPLICATION_AUDIO, &m_OpusErr );
	if( OPUS_OK != m_OpusErr )
	{
		LogMsg( LOG_ERROR, "Error opus_encoder_create returned: %s", opus_strerror( m_OpusErr ) );
		return;
	}

	m_OpusErr = opus_encoder_ctl( m_Encoder, OPUS_SET_BITRATE( m_BitRate ) );
	if( OPUS_OK != m_OpusErr )
	{
		LogMsg( LOG_ERROR, "Error OPUS_SET_BITRATE returned: %s", opus_strerror( m_OpusErr ) );
		return;
	}

	//m_OpusErr = opus_encoder_ctl( m_Encoder, OPUS_SET_FORCE_CHANNELS( 1 ) );
	//if( OPUS_OK != m_OpusErr )
	//{
	//	LogMsg( LOG_ERROR, "Error OPUS_SET_BITRATE returned: %s", opus_strerror( m_OpusErr ) );
	//	return;
	//}


	//m_OpusErr = opus_encoder_ctl( m_Encoder, OPUS_SET_LSB_DEPTH( 16 ) ); // bits of precision to determine silence or near silence
	//if( OPUS_OK != m_OpusErr )
	//{
	//	LogMsg( LOG_ERROR, "Error OPUS_SET_LSB_DEPTH returned: %s", opus_strerror( m_OpusErr ) );
	//	return;
	//}

	//m_OpusErr = opus_encoder_ctl( m_Encoder, OPUS_SET_VBR( 1 ) );
	//if( OPUS_OK != m_OpusErr )
	//{
	//	LogMsg( LOG_ERROR, "Error OPUS_SET_VBR returned: %s", opus_strerror( m_OpusErr ) );
	//	return;
	//}

	//m_OpusErr = opus_encoder_ctl( m_Encoder, OPUS_SET_VBR_CONSTRAINT( 0 ) );
	//if( OPUS_OK != m_OpusErr )
	//{
	//	LogMsg( LOG_ERROR, "Error OPUS_SET_VBR_CONSTRAINT returned: %s", opus_strerror( m_OpusErr ) );
	//	return;
	//}

	//m_OpusErr = opus_encoder_ctl( m_Encoder, OPUS_SET_COMPLEXITY( 10 ) );
	//if( OPUS_OK != m_OpusErr )
	//{
	//	LogMsg( LOG_ERROR, "Error OPUS_SET_COMPLEXITY returned: %s", opus_strerror( m_OpusErr ) );
	//	return;
	//}

	//m_OpusErr = opus_encoder_ctl( m_Encoder, OPUS_SET_PACKET_LOSS_PERC( 0 ) );
	//if( OPUS_OK != m_OpusErr )
	//{
	//	LogMsg( LOG_ERROR, "Error OPUS_SET_PACKET_LOSS_PERC returned: %s", opus_strerror( m_OpusErr ) );
	//	return;
	//}


	opus_int32 lookAhead;
	m_OpusErr = opus_encoder_ctl( m_Encoder, OPUS_GET_LOOKAHEAD( &lookAhead ) );
	if( OPUS_OK != m_OpusErr )
	{
		LogMsg( LOG_ERROR, "Error OPUS_GET_LOOKAHEAD returned: %s", opus_strerror( m_OpusErr ) );
		return;
	}

	m_OpusHeader.m_LookAhead = lookAhead;
	//Regardless of the rate we're coding at the ogg timestamping/skip is always timed at 48000.
	// this comes to 312
	m_OpusHeader.m_Preskip = (uint16_t)(m_OpusHeader.m_LookAhead * (48000.0 / m_SampleRate));
	// Extra samples that need to be read to compensate for the pre-skip 
	// this comes to 52 again
	m_OpusHeader.m_ExtraOut = (int32_t)(m_OpusHeader.m_Preskip * (m_SampleRate / 48000.0));


	m_OpusHeader.m_Channels			= m_Channels;
	m_OpusHeader.m_ChannelMapping	= m_OpusHeader.m_Channels > 8 ? 255 : m_Channels > 2;
	m_OpusHeader.m_InputSampleRate	= m_SampleRate;
	m_OpusHeader.m_Gain				= 0;


	/*
	int streamCnt; 
	int coupledCnt;

	m_Encoder = opus_multistream_surround_encoder_create( 
			m_SampleRate, 
			m_Channels, 
			m_OpusHeader.m_ChannelMapping, 
			&streamCnt, 
			&coupledCnt,
			m_OpusHeader.m_StreamMap, 
			OPUS_APPLICATION_RESTRICTED_LOWDELAY, // RESTRICTED_LOWDELAY makes smaller AND takes about 1/2 the CPU time
			&m_OpusErr );
	if( OPUS_OK != m_OpusErr )
	{
		LogMsg( LOG_ERROR, "Error cannot create encoder: %s", opus_strerror( m_OpusErr ) );
		return;
	}

	m_OpusHeader.m_StreamCnt	= (uint8_t)streamCnt;
	m_OpusHeader.m_CoupledCnt	= (uint8_t)coupledCnt;
	m_OpusHeader.m_MaxOpusPktSize = (1275*3+7) * streamCnt;
	m_OpusErr = opus_multistream_encoder_ctl( m_Encoder, OPUS_SET_BITRATE( m_BitRate ) );
	if( OPUS_OK != m_OpusErr )
	{
		LogMsg( LOG_ERROR, "Error OPUS_SET_BITRATE returned: %s", opus_strerror( m_OpusErr ));
		return;
	}

	m_OpusErr = opus_multistream_encoder_ctl( m_Encoder, OPUS_SET_VBR( 1 ) );
	if( OPUS_OK != m_OpusErr )
	{
		LogMsg( LOG_ERROR, "Error OPUS_SET_VBR returned: %s", opus_strerror( m_OpusErr ));
		return;
	}

	m_OpusErr = opus_multistream_encoder_ctl( m_Encoder, OPUS_SET_VBR_CONSTRAINT( 0 ) );
	if( OPUS_OK != m_OpusErr )
	{
		LogMsg( LOG_ERROR, "Error OPUS_SET_VBR_CONSTRAINT returned: %s", opus_strerror( m_OpusErr ));
		return;
	}

	m_OpusErr = opus_multistream_encoder_ctl( m_Encoder, OPUS_SET_COMPLEXITY( 10 ) );
	if( OPUS_OK != m_OpusErr )
	{
		LogMsg( LOG_ERROR, "Error OPUS_SET_COMPLEXITY returned: %s", opus_strerror( m_OpusErr ));
		return;
	}

	m_OpusErr = opus_multistream_encoder_ctl( m_Encoder, OPUS_SET_PACKET_LOSS_PERC( 0 ) );
	if( OPUS_OK != m_OpusErr )
	{
		LogMsg( LOG_ERROR, "Error OPUS_SET_PACKET_LOSS_PERC returned: %s", opus_strerror( m_OpusErr ));
		return;
	}

	m_OpusErr = opus_multistream_encoder_ctl( m_Encoder, OPUS_SET_LSB_DEPTH( 16 ) );
	if( OPUS_OK != m_OpusErr )
	{
		LogMsg( LOG_ERROR, "Error OPUS_SET_LSB_DEPTH returned: %s", opus_strerror( m_OpusErr ));
		return;
	}

	// lookahead = 52 ... dont know why
	opus_int32 lookAhead;
	m_OpusErr = opus_multistream_encoder_ctl( m_Encoder, OPUS_GET_LOOKAHEAD( &lookAhead ) );
	if( OPUS_OK != m_OpusErr )
	{
		LogMsg( LOG_ERROR, "Error OPUS_GET_LOOKAHEAD returned: %s", opus_strerror( m_OpusErr ));
		return;
	}

	m_OpusHeader.m_LookAhead = lookAhead;
	//Regardless of the rate we're coding at the ogg timestamping/skip is always timed at 48000.
	// this comes to 312
	m_OpusHeader.m_Preskip = (uint16_t)(m_OpusHeader.m_LookAhead * ( 48000.0 / m_SampleRate ));
	// Extra samples that need to be read to compensate for the pre-skip 
	// this comes to 52 again
	m_OpusHeader.m_ExtraOut = (int32_t)(m_OpusHeader.m_Preskip*( m_SampleRate / 48000.0 ));
	*/

	m_EncoderInitialized = true;
}

//============================================================================
int OpusAudioEncoder::encodePcmData(	int16_t*	pcmData, 
										int16_t		pcmDataLen, 
										uint8_t*	outBuf,	
										std::vector<uint16_t>& opusEncodedLenList )
{
	opusEncodedLenList.clear();
	if( false == m_EncoderInitialized )
	{
		LogMsg( LOG_ERROR, "ERROR: OpusAudioEncoder::writePcmData not initialized" );
		return 0;
	}
	
	int sampleCnt = pcmDataLen >> 1;
	if(  MY_PCM_SAMPLE_CNT  != sampleCnt  )
	{
		LogMsg( LOG_ERROR, "ERROR: OpusAudioEncoder::writePcmData not expected length" );
		return 0;
	}


	int totalEncodedLen = 0;
	int toEncodeOffs = 0;
	int opusFramesToEncode = MY_OPUS_FRAME_CNT;
	int opusSamplesPerFrame = MY_OPUS_FRAME_SAMPLE_CNT;

	for( int frameIdx = 0; frameIdx < opusFramesToEncode; frameIdx++ )
	{
		int16_t* rawPcm = &pcmData[ toEncodeOffs ];
		int16_t* toEncodPcm = rawPcm;

		int32_t opusEncodedFrameLen = opus_encode( m_Encoder, &toEncodPcm[ toEncodeOffs ], MY_OPUS_FRAME_SAMPLE_CNT, &outBuf[ totalEncodedLen ], MY_OPUS_FRAME_SAMPLE_CNT * AUDIO_BYTES_PER_SAMPLE );
		if( 0 >= opusEncodedFrameLen )
		{
			LogMsg( LOG_ERROR, "Encoding failed: %s", opus_strerror( opusEncodedFrameLen ) );
			vx_assert( false );
			return 0;
		}

		opusEncodedLenList.push_back( (int16_t)opusEncodedFrameLen );
		totalEncodedLen += opusEncodedFrameLen;
		toEncodeOffs += opusSamplesPerFrame;
	}

	// BRJ temp for debugging to see values in debugger
	int16_t tempBuf[ MY_OPUS_FRAME_SAMPLE_CNT ];
	memcpy( tempBuf, pcmData, sizeof( tempBuf ) );
	int16_t maxVal = -32000;
	int16_t minVal = 32000;
	for( int i = 0; i < MY_OPUS_FRAME_SAMPLE_CNT; i++ )
	{
		minVal = std::min( minVal, tempBuf[ i ] );
		maxVal = std::max( maxVal, tempBuf[ i ] );
	}

	if( minVal < -15000 || maxVal > 15000 )
	{
		LogMsg( LOG_ERROR, "OpusAudioEncoder::writePcmData min %d max %d len %d", minVal, maxVal, totalEncodedLen );
	}

	return totalEncodedLen;
}


