//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AudioBitrate.h"
#include <CoreLib/VxDebug.h>

//============================================================================
void AudioBitrate::addSamplesAndInterval( int sampleCnt, int timeIntervalMs )
{
	if( !m_BitrateLogEnable )
	{
		return;
	}

	if( !sampleCnt || !timeIntervalMs )
	{
		LogMsg( LOG_WARNING, "AudioBitrate::addSamplesAndInterval invalid param" );
		return;
	}

	if( m_SampleCntList.size() >= BITRATE_SAMPLES_PER_LOG_MSG )
	{
		m_SampleCntList.erase( m_SampleCntList.begin() );
	}

	m_SampleCntList.push_back( sampleCnt );

	if( m_IntervalMsList.size() >= BITRATE_SAMPLES_PER_LOG_MSG )
	{
		m_IntervalMsList.erase( m_IntervalMsList.begin() );
	}

	m_IntervalMsList.push_back( timeIntervalMs );

	m_RatesSampled++;
	if( m_RatesSampled >= BITRATE_SAMPLES_PER_LOG_MSG )
	{
		m_RatesSampled = 0;

		int samplesTotal = 0;
		for( auto sampleCnt : m_SampleCntList )
		{
			samplesTotal += sampleCnt;
		}

		int intervalTotalMs = 0;
		for( auto intervalMs : m_IntervalMsList )
		{
			intervalTotalMs += intervalMs;
		}

		int sampleRate = (samplesTotal * 1000) / intervalTotalMs;

		if( sampleRate > 40000 )
		{
			if( sampleRate < 47000 || sampleRate > 49000 )
			{
				LogMsg( LOG_VERBOSE, "%s samples per second %d", m_LogPrefix.c_str(), (samplesTotal * 1000) / intervalTotalMs );
			}		
		}
		else
		{
			if( sampleRate < 7000 || sampleRate > 9000 )
			{
				LogMsg( LOG_VERBOSE, "%s samples per second %d", m_LogPrefix.c_str(), (samplesTotal * 1000) / intervalTotalMs );
			}
		}		
	}
}
