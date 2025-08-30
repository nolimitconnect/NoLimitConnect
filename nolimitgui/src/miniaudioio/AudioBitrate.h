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

#include <stdint.h>
#include <vector>
#include <string>

class AudioBitrate
{
public:
	static const int			BITRATE_SAMPLES_PER_LOG_MSG = 10; // if enabled log the average bitrate every 10 sample/interval count

	AudioBitrate( void ) = default;
	~AudioBitrate( void ) = default;

	void						setIsBitrateLogEnabled( bool enable )			{ m_BitrateLogEnable = enable; reset(); }
	bool						isBitrateLogEnabled( void )						{ return m_BitrateLogEnable; }

	void						setLogMessagePrefix( std::string logPrefix )	{ m_LogPrefix = logPrefix; }

	void						reset( void )									{ m_SampleCntList.clear(); m_IntervalMsList.clear(); m_RatesSampled = 0; }

	void						addSamplesAndInterval( int sampleCnt, int timeIntervalMs );	

	bool						m_BitrateLogEnable{ false };
	int							m_RatesSampled{ 0 };
	std::string					m_LogPrefix{ "" };
	std::vector<int>			m_SampleCntList;
	std::vector<int>			m_IntervalMsList;
};

