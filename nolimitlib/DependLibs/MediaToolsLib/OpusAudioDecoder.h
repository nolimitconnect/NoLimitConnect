#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
// Issued to MIT style license by Brett R. Jones in 2017
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "SndDefs.h"
#include "MyOpusHeader.h"
#include "OpusCallbackInterface.h"

#include "SndDefs.h"
#include "MyOpusHeader.h"

#include <MediaToolsLib/include/opus.h>
#include <MediaToolsLib/include/opus_multistream.h>
#include <libogg/include/ogg/ogg.h>

#include <MediaToolsLib/include/opus_defines.h>
#include <MediaToolsLib/OpusTools/speex_resampler.h>

#include <vector>

#define OPUS_OUT_MAX_SAMPLES (MY_OPUS_FRAME_SAMPLE_CNT * 9) 
#define MIN_OPUS_FILE_LEN 1104

struct OpusDecoder;

class OpusAudioDecoder
{
public:
	OpusAudioDecoder( int sampleRate = MY_OPUS_SAMPLE_RATE, int channels = 1  );
	virtual ~OpusAudioDecoder();

	bool						isInitialized( void )						{ return m_DecoderInitialized; }
	OpusDecoder *				getDecoder( void )							{ return m_OpusDecoder; }

	bool						decodeToPcmData(	uint8_t*	encodedOpusAudio,	
													std::vector<uint16_t>& opusEncodedLenList,
													int16_t*	pcmDataBuf, 
													int32_t		pcmDataBufLen );
protected:
	bool						initDecoder( void );
	int							decodeOneFrameToPcmData(	uint8_t *	encodedOpusAudio,	
															int32_t		encodedFrameLenBytes,
															int16_t *	pcmDataBuf, 
															int32_t		pcmDataBufLen );
	int							opusFloatOutputToPcm(	float*					opusOutput, 
														int						channels, 
														int						frameSampleCnt, 
														SpeexResamplerState*	resampler,
														int*					skip, 
														uint8_t*				pcmOutBuf,
														unsigned int			maxout );

	//=== vars ===//
	int							m_SampleRate{ 0 };
	int							m_Channels{ 0 };
	bool						m_DecoderInitialized{ false };

	OpusDecoder *				m_OpusDecoder{ nullptr };
	int							m_OpusErr{ 0 };
	int							m_BitRate{ MY_OPUS_TARGET_BIT_RATE };
	int							m_FrameSize{ MY_OPUS_FRAME_SAMPLE_CNT };
	bool 						m_FirstFillSilence{ true };
	//float						m_OpusOutBuf[ MY_PCM_SAMPLE_CNT ];
	//SpeexResamplerState*		m_Resampler{ nullptr };
};

