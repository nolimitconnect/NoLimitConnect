#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
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

	int							m_MaxSamples{ AUDIO_SAMPLES_PER_FRAME };
	int							m_SampleCnt{ 0 };
	int							m_NotSilentCnt{ 0 };
};
