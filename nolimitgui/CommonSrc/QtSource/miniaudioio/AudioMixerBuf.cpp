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
				DescribeAppModule( m_AppModule ), timeInterval );
		}
	}

	return AudioSampleBuf::writeSamples( samplesBuf, sampleCnt, isSilent );
}
