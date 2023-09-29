#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
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

#include <GuiInterface/IDefs.h>
#include <GuiInterface/IAudioDefs.h>

#include <inttypes.h>

class AudioSampleBuf
{
public:
	static const int MAX_SAMPLES_BUFFER_SIZE = AUDIO_BUF_SIZE; // max samples before truncates beginning to make room at end of buffer

	AudioSampleBuf() = default;
	AudioSampleBuf( const AudioSampleBuf& rhs ) = delete;
	~AudioSampleBuf() = default;

	AudioSampleBuf& operator = ( const AudioSampleBuf& rhs ) = delete;

	void						clear( void )							{ m_SampleCnt = 0; }
	bool						empty( void )							{ return 0 == m_SampleCnt; }
	int							freeSpaceSampleCount( void )			{ return m_MaxSamples - m_SampleCnt; }

	void						setMaxSamples( int maxSamples )			{ m_MaxSamples = maxSamples; } // cannot be more than 4 * AUDIO_SAMPLES_PER_FRAME
	int							getMaxSamples( void )					{ return m_MaxSamples; }

	void						setSampleCnt( int sampleCnt )			{ m_SampleCnt = sampleCnt; }
	int							getSampleCnt( void )					{ return m_SampleCnt; }

	int16_t*					getSampleBuffer( void )					{ return m_PcmData; }

	int							writeSamples( int16_t* retSamplesBuf, int sampleCnt );
	void						samplesWereWritten( int sampleCnt );

	int							readSamples( int16_t* srcSamplesBuf, int sampleCnt );
	void						samplesWereRead( int samplesRead ); // move remainder samples to begining of buffer and decrement sample count

	int16_t						getLastSample( void );

	int							getAudioDurationMs( int sampleRate = ECHO_SAMPLE_RATE ); // get the samples duration in ms

	void						truncateSamples( int sampleCnt ); // if contains more samples than this then limit to sampleCnt

	int16_t					    m_PcmData[ MAX_SAMPLES_BUFFER_SIZE ];

	int							m_MaxSamples{ MAX_SAMPLES_BUFFER_SIZE };
	int							m_SampleCnt{ 0 };
};
