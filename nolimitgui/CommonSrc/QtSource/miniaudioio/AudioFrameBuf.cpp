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

#include "AudioFrameBuf.h"
#include "AudioUtils.h"

#include <CoreLib/VxDebug.h>

#include <algorithm>

//============================================================================
int AudioFrameBuf::readSamples( int16_t* retSamplesBuf, int sampleCnt )
{
	int samplesToRead = std::min( sampleCnt, m_SampleCnt );
	if( samplesToRead )
	{
		memcpy( retSamplesBuf, m_PcmData, samplesToRead * AUDIO_BYTES_PER_SAMPLE );
		samplesWereRead( samplesToRead );
	}
	else
	{
		LogMsg( LOG_ERROR, "AudioFrameBuf::readSamples ERROR samplesToRead %d m_SampleCnt %d", sampleCnt, m_SampleCnt );
	}

	return samplesToRead;
}

//============================================================================
void AudioFrameBuf::samplesWereRead( int samplesRead )
{
	if( samplesRead == m_SampleCnt )
	{
		m_SampleCnt = 0;
	}
	else if( samplesRead < m_SampleCnt )
	{
		int samplesToMove = m_SampleCnt - samplesRead;
		memcpy( m_PcmData, &m_PcmData[ samplesRead ], samplesToMove * AUDIO_BYTES_PER_SAMPLE );
		m_SampleCnt = samplesToMove;
	}
	else
	{
		LogMsg( LOG_ERROR, "AudioFrameBuf::sampleWereRead ERROR samplesRead %d m_SampleCnt %d", samplesRead, m_SampleCnt );
	}
}

//============================================================================
int AudioFrameBuf::writeSamples( int16_t* srcSamplesBuf, int sampleCnt )
{
	int writtenSamples = 0;
	if( sampleCnt > m_MaxSamples )
	{
		LogMsg( LOG_ERROR, "AudioFrameBuf::writeSamples ERROR samplesToWrite %d greater than max samples %d", sampleCnt, m_MaxSamples );
		return 0;
	}
	else if( sampleCnt <= ( m_MaxSamples - m_SampleCnt ) )
	{
		// can just append
		memcpy( &m_PcmData[ m_SampleCnt ], srcSamplesBuf, sampleCnt * AUDIO_BYTES_PER_SAMPLE );
		m_SampleCnt += sampleCnt;
		writtenSamples = sampleCnt;
	}
	else
	{
		// make room for samples
		int samplesToRemove = std::abs( m_MaxSamples - ( m_SampleCnt + sampleCnt ) );
		LogMsg( LOG_WARNING, "AudioFrameBuf::writeSamples removing %d samples to fit %d samples", samplesToRemove, sampleCnt );
		samplesWereRead( samplesToRemove );
		memcpy( &m_PcmData[ m_SampleCnt ], srcSamplesBuf, sampleCnt * AUDIO_BYTES_PER_SAMPLE );
		m_SampleCnt += sampleCnt;
		writtenSamples = sampleCnt;
	}

	return writtenSamples;
}

//============================================================================
int16_t AudioFrameBuf::getLastSample( void )
{
	if( m_SampleCnt )
	{
		return m_PcmData[ m_SampleCnt - 1 ];
	}

	return 0;
}

//============================================================================
int AudioFrameBuf::getAudioDurationMs( int sampleRate )
{
	return AudioUtils::audioDurationMs( sampleRate, m_SampleCnt );
}

//============================================================================
void AudioFrameBuf::truncateSamples( int sampleCnt )
{
	if( m_SampleCnt > sampleCnt )
	{
		samplesWereRead( m_SampleCnt - sampleCnt );
	}
}