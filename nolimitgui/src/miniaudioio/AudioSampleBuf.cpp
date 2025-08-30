//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AudioSampleBuf.h"
#include "AudioUtils.h"

#include <CoreLib/VxDebug.h>

#include <algorithm>

//============================================================================
AudioSampleBuf::AudioSampleBuf()
{
	m_PcmData.resize( m_MaxSamples );
}

//============================================================================
AudioSampleBuf::AudioSampleBuf( const AudioSampleBuf& rhs )
	: m_PcmData( rhs.m_PcmData )
	, m_MaxSamples( rhs.m_MaxSamples )
	, m_SampleCnt( rhs.m_SampleCnt )
	, m_NotSilentCnt( rhs.m_NotSilentCnt )
{
}

//============================================================================
AudioSampleBuf& AudioSampleBuf::operator = ( const AudioSampleBuf& rhs )
{
	if( &rhs != this )
	{
		m_PcmData = rhs.m_PcmData;
		m_MaxSamples = rhs.m_MaxSamples;
		m_SampleCnt = rhs.m_SampleCnt;
		m_NotSilentCnt = rhs.m_NotSilentCnt;
	}

	return *this;
}

//============================================================================
void AudioSampleBuf::setMaxSamples( int maxSamples )
{
	m_MaxSamples = maxSamples;
	m_NotSilentCnt = 0;
	m_SampleCnt = 0;
	m_PcmData.resize( maxSamples );
}

//============================================================================
int AudioSampleBuf::readSamples( int16_t* retSamplesBuf, int sampleCnt )
{
	int samplesToRead = std::min( sampleCnt, m_SampleCnt );
	if( samplesToRead )
	{
		memcpy( retSamplesBuf, m_PcmData.data(), samplesToRead * AUDIO_BYTES_PER_SAMPLE);
		samplesWereRead( samplesToRead );
	}
	else
	{
		if(LogEnabled(eLogAudioIo)) LogModule( eLogAudioIo, LOG_ERROR, "AudioSampleBuf::readSamples ERROR samplesToRead %d m_SampleCnt %d", sampleCnt, m_SampleCnt );
	}

	return samplesToRead;
}

//============================================================================
void AudioSampleBuf::samplesWereRead( int samplesRead )
{
	if( samplesRead == m_SampleCnt )
	{
		m_SampleCnt = 0;
	}
	else if( samplesRead < m_SampleCnt )
	{
		int samplesToMove = m_SampleCnt - samplesRead;
		int16_t* dest = m_PcmData.data();
		int16_t* src = &m_PcmData[samplesRead];

		memcpy( dest, src, samplesToMove * AUDIO_BYTES_PER_SAMPLE);
		m_SampleCnt = samplesToMove;
	}
	else
	{
		if(LogEnabled(eLogAudioIo)) LogModule( eLogAudioIo, LOG_ERROR, "AudioSampleBuf::sampleWereRead ERROR samplesRead %d m_SampleCnt %d", samplesRead, m_SampleCnt );
	}

	if( samplesRead && m_NotSilentCnt )
	{
		m_NotSilentCnt = m_NotSilentCnt > samplesRead ? m_NotSilentCnt - samplesRead : 0;
	}
}

//============================================================================
int AudioSampleBuf::writeSamples( int16_t* srcSamplesBuf, int sampleCnt, bool isSilent )
{
	int writtenSamples = 0;
	if( sampleCnt > m_MaxSamples )
	{
		if(LogEnabled(eLogAudioIo)) LogModule( eLogAudioIo, LOG_ERROR, "AudioSampleBuf::writeSamples ERROR samplesToWrite %d greater than max samples %d", sampleCnt, m_MaxSamples );
		return 0;
	}
	else if( sampleCnt <= ( m_MaxSamples - m_SampleCnt ) )
	{
		// can just append
		memcpy( &m_PcmData[ m_SampleCnt ], srcSamplesBuf, sampleCnt * AUDIO_BYTES_PER_SAMPLE );
		samplesWereWritten( sampleCnt, isSilent );
		writtenSamples = sampleCnt;
	}
	else
	{
		// make room for samples
		int samplesToRemove = std::abs( m_MaxSamples - ( m_SampleCnt + sampleCnt ) );
		if(LogEnabled(eLogAudioIo)) LogModule( eLogAudioIo, LOG_WARNING, "AudioSampleBuf::writeSamples removing %d samples to fit %d samples", samplesToRemove, sampleCnt );
		samplesWereRead( samplesToRemove );
		memcpy( &m_PcmData[ m_SampleCnt ], srcSamplesBuf, sampleCnt * AUDIO_BYTES_PER_SAMPLE );
		samplesWereWritten( sampleCnt, isSilent );
		
		writtenSamples = sampleCnt;
	}

	return writtenSamples;
}

//============================================================================
void AudioSampleBuf::samplesWereWritten( int sampleCnt, bool isSilent )		
{ 
	m_SampleCnt += sampleCnt; 
	if( !isSilent )
	{
		m_NotSilentCnt = m_MaxSamples;
	}
}

//============================================================================
int16_t AudioSampleBuf::getLastSample( void )
{
	if( m_SampleCnt )
	{
		return m_PcmData[ m_SampleCnt - 1 ];
	}

	return 0;
}

//============================================================================
int AudioSampleBuf::getAudioDurationMs( int sampleRate )
{
	return AudioUtils::audioDurationMs( sampleRate, m_SampleCnt );
}

//============================================================================
void AudioSampleBuf::truncateSamples( int sampleCnt )
{
	if( m_SampleCnt > sampleCnt )
	{
		samplesWereRead( m_SampleCnt - sampleCnt );
	}
}