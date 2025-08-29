#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "SndDefs.h"
#include "MyOpusHeader.h"
#include "OpusCallbackInterface.h"

#include <vector>

struct
	OpusEncoder;

class OpusAudioEncoder
{
public:
	OpusAudioEncoder( int sampleRate = MY_OPUS_SAMPLE_RATE, int channels = 1  );
	virtual ~OpusAudioEncoder() = default;

	bool						isInitialized( void )						{ return m_EncoderInitialized; }
	MyOpusHeader&				getOpusHeader( void )						{ return m_OpusHeader; }

	int							encodePcmData(		int16_t*	pcmData, 
													int16_t		pcmDataLen, 
													uint8_t*	outBuf,	
													std::vector<uint16_t>& opusEncodedLenList );
protected:
	void						initEncoder( void );

	//=== vars ===//
	int							m_SampleRate{ 0 };
	int							m_Channels{ 0 };
	bool						m_EncoderInitialized{ false };

	OpusEncoder*				m_Encoder{ nullptr };
	int							m_OpusErr{ 0 };
	int							m_BitRate{ MY_OPUS_TARGET_BIT_RATE };
	int							m_FrameSize{ MY_OPUS_FRAME_SAMPLE_CNT };
	MyOpusHeader				m_OpusHeader;
	int16_t						m_OpusInBuf[ MY_PCM_SAMPLE_CNT ];
};

