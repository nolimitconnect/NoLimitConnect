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

#include "AudioMixerFrame.h"

#include "AppCommon.h"
#include "AudioUtils.h"
#include "AudioSampleBuf.h"
#include "MiniAudioMgr.h"
#include "SoundMgr.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTimer.h>

//============================================================================
AudioMixerFrame::AudioMixerFrame()
{
	memset( m_MixerBuf, 0, sizeof( m_MixerBuf ) );
}

//============================================================================
void AudioMixerFrame::clearFrame( bool fillSilence )
{
	m_MixerSamplesWrote = 0;
	m_InputIds.clear();

	if( fillSilence )
	{
		memset( m_MixerBuf, 0, sizeof( m_MixerBuf ) );
		m_IsSilentSamples = true;
	}
}

//============================================================================
int AudioMixerFrame::toMixerPcmMonoChannel( EAppModule appModule, int16_t* pcmData, bool isSilenceIn )
{
	static int64_t timeNow = 0;
	static int64_t lastMixerPcmTime{ 0 };

	lastMixerPcmTime = timeNow;
	timeNow = GetHighResolutionTimeMs();

	if( hasModuleAudio( appModule ) )
	{
		if( m_AudioIoMgr->getFrameTimingEnable() )
		{		
			static int funcCallCnt{ 0 };
			funcCallCnt++;
			if( lastMixerPcmTime )
			{
				int timeInterval = (int)(timeNow - lastMixerPcmTime);
				LogMsg( LOG_VERBOSE, "W Frame %d call cnt %d toMixerPcm8000HzMonoChannel module %s elapsed %d ms overrrun ", getFrameIndex(), funcCallCnt,
					DescribeAppModule( appModule ), timeInterval );
			}
		}
		else
		{
			int timeInterval = (int)(timeNow - lastMixerPcmTime);
			LogMsg( LOG_WARNING, "W Frame %d AudioMixerFrame::toMixerPcm8000HzMonoChannel module %s elapsed %d ms overrun", getFrameIndex(), DescribeAppModule( appModule ), timeInterval );
		}
		
		return 0;
	}

	if( m_IsSilentSamples && isSilenceIn )
	{
		memset( m_MixerBuf, 0, AUDIO_BUF_SIZE );
	}
	else if( m_IsSilentSamples && !isSilenceIn )
	{
		memcpy( m_MixerBuf, pcmData, AUDIO_BUF_SIZE );
		m_IsSilentSamples = false;
	}
	else if( !m_IsSilentSamples && !isSilenceIn )
	{
		AudioUtils::mixPcmAudio( pcmData, (int16_t*)m_MixerBuf, AUDIO_BUF_SIZE );
		m_IsSilentSamples = false;
	}

	if( m_InputIds.empty() )
	{
		// first write after has been read.. reset partial read counters
		m_MixerSamplesWrote = AUDIO_SAMPLES_PER_FRAME;
	}
	
	m_InputIds.push_back( appModule );

	return AUDIO_BUF_SIZE;
}
