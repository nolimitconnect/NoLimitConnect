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

#include <inttypes.h>

class AudioEchoBuf
{
public:
	const int TIME_MS_RANGE_EPSILON = 10;

	AudioEchoBuf() = default;
	AudioEchoBuf( int64_t timeStamp, int16_t* pcmData, int sampleCnt, bool isSpeakerSamples );
	AudioEchoBuf( const AudioEchoBuf& rhs );
	~AudioEchoBuf() = default;

	AudioEchoBuf& operator = ( const AudioEchoBuf& rhs );

	bool						isEmpty( void )					{ return  m_SamplesRead >= m_SampleCnt || !m_SampleCnt;  }
	int							getSamplesAvailable( void )		{ return m_SampleCnt ? m_SampleCnt - m_SamplesRead : 0; }
	bool						isInTimeRange( int64_t& timeEst );
	int							readSamples( int16_t* retSamplesBuf, int maxSampleCnt, int64_t timeEst );
	void						cleanupBuffer( void );

	int64_t						m_Timestamp{ 0 };
	int16_t*					m_PcmData{ nullptr };
	int							m_SampleCnt{ 0 };
	int							m_SamplesRead{ 0 };
	bool						m_IsSpeakerSamples{ false };
	static int					m_TotalSpeakerSamples;
};
