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

#include <vector>

class AudioSampleBuf
{
public:
	static const int MAX_SAMPLES_BUFFER_SIZE = AUDIO_BUF_SIZE; // max samples before truncates beginning to make room at end of buffer

	AudioSampleBuf();
	AudioSampleBuf( const AudioSampleBuf& rhs );
	~AudioSampleBuf() = default;

	AudioSampleBuf& operator = ( const AudioSampleBuf& rhs );

	void						clear( void )							{ m_SampleCnt = 0; m_NotSilentCnt = 0; }
	bool						empty( void )							{ return 0 == m_SampleCnt; }
	int							freeSpaceSampleCount( void )			{ return m_MaxSamples - m_SampleCnt; }

	bool						isSilent( void )						{ return 0 == m_NotSilentCnt; }

	void						setMaxSamples( int maxSamples ); // also resizes m_PcmData
	int							getMaxSamples( void )					{ return m_MaxSamples; }

	void						setSampleCnt( int sampleCnt )			{ m_SampleCnt = sampleCnt; }
	int							getSampleCnt( void )					{ return m_SampleCnt; }

	int16_t*					getSampleBuffer( bool atCurIdx = false ) { return atCurIdx ? &m_PcmData[ m_SampleCnt ] :  m_PcmData.data(); }

	virtual int					writeSamples( int16_t* samplesBuf, int sampleCnt, bool isSilent = false );
	void						samplesWereWritten( int sampleCnt, bool isSilent = false );

	virtual int					readSamples( int16_t* srcSamplesBuf, int sampleCnt );
	virtual void				samplesWereRead( int samplesRead ); // move remainder samples to begining of buffer and decrement sample count

	int16_t						getLastSample( void );

	int							getAudioDurationMs( int sampleRate = ECHO_SAMPLE_RATE ); // get the samples duration in ms

	void						truncateSamples( int sampleCnt ); // if contains more samples than this then limit to sampleCnt

private:
	std::vector<int16_t>		m_PcmData;

	int							m_MaxSamples{ MAX_SAMPLES_BUFFER_SIZE };
	int							m_SampleCnt{ 0 };
	int							m_NotSilentCnt{ 0 };
};
