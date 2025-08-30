//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AudioMixerBuf.h"

#include "AppCommon.h"
#include "AudioUtils.h"
#include "AudioSampleBuf.h"
#include "MiniAudioMgr.h"
#include "SoundMgr.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTimer.h>

//============================================================================
int	AudioMixerBuf::writeSamples( int16_t* samplesBuf, int sampleCnt, bool isSilent )
{
	if( m_AudioIoMgr && m_AudioIoMgr->getFrameTimingEnable() )
	{
		m_LastMixerPcmTime = m_TimeNow;
		m_TimeNow = GetHighResolutionTimeMs();

		m_FuncCallCnt++;
		if( m_LastMixerPcmTime )
		{
			int timeInterval = (int)(m_TimeNow - m_LastMixerPcmTime);
			LogMsg( LOG_VERBOSE, "AudioMixerBuf::writeSamples Frame call cnt %d  module %s elapsed %d ms overrrun ", m_FuncCallCnt,
				DescribeMediaModule( m_MediaModule ), timeInterval );
		}
	}

	return AudioSampleBuf::writeSamples( samplesBuf, sampleCnt, isSilent );
}
