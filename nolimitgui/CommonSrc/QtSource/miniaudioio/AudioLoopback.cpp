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
// https://www.nolimitconnect.com
//============================================================================

#include "AudioLoopback.h"

#include "AppCommon.h"
#if defined(USE_MINI_AUDIO)
#include "MiniAudioMgr.h"
#else
#include "AudioIoMgr.h"
#endif // defined(USE_MINI_AUDIO)
#include "AudioUtils.h"

#include <ptop_src/ptop_engine_src/MediaProcessor/RawAudio.h>

#include <CoreLib/VxTime.h>
#include <CoreLib/VxTimer.h>

namespace
{
	//============================================================================
	static void* AudioLoopbackProcessThreadFunc( void* pvContext )
	{
		VxThread* poThread = (VxThread*)pvContext;
		poThread->setIsThreadRunning( true );
		AudioLoopback* processor = (AudioLoopback*)poThread->getThreadUserParam();
		if( processor )
		{
			processor->processAudioLoopbackThreaded();
		}

		poThread->threadAboutToExit();
		return nullptr;
	}
}

//============================================================================
#if defined(USE_MINI_AUDIO)
AudioLoopback::AudioLoopback( MiniAudioMgr& audioIoMgr, QObject* parent )
#else
AudioLoopback::AudioLoopback( AudioIoMgr& audioIoMgr, QObject* parent )
#endif // defined(USE_MINI_AUDIO)
	: QObject( parent )
	, m_AudioIoMgr( audioIoMgr )
	, m_MyApp( audioIoMgr.getMyApp() )
{
	memset( m_MixerBuf, 0, AUDIO_SAMPLES_PER_FRAME * 2 );
	memset( m_QuietEchoBuf, 0, AUDIO_SAMPLES_PER_FRAME * 8 * 2 );

	for( int i = 0; i < MAX_GUI_MIXER_FRAMES; i++ )
	{
		m_MixerFrames[ i ].setFrameIndex( i );
		m_MixerFrames[ i ].setAudioIoMgr( &audioIoMgr );
	}

	m_EchoCanceledBitrate.setLogMessagePrefix( "Echo Canceled " );
	m_ProcessFrameBitrate.setLogMessagePrefix( "Process Frame " );
	m_ProcessSpeakerBitrate.setLogMessagePrefix( "Process Speaker " );
	m_SpeakerReadBitrate.setLogMessagePrefix( "Speaker Read " );

	//m_ProcessAudioLoopbackThread.startThread( (VX_THREAD_FUNCTION_T)AudioLoopbackProcessThreadFunc, this, "AudioLoopbackProcessor" );
}

//============================================================================
void AudioLoopback::audioLoopbackShutdown( void )
{
	m_ProcessAudioLoopbackThread.abortThreadRun( true );
	m_AudioLoopbackSemaphore.signal();
}

//============================================================================
int AudioLoopback::readRequestFromSpeaker( int16_t* readReqPcmBuf, int reqSpeakerSampleCnt )
{
	if( reqSpeakerSampleCnt <= 0 )
	{
		LogMsg( LOG_DEBUG, "readRequestFromSpeaker 0 sampleCnt " );
		return 0;
	}

	//int64_t timeNow = GetHighResolutionTimeMs();
	//static int64_t lastCallTime = 0;
	//int callTimeElapsed = lastCallTime ? (int)(timeNow - lastCallTime) : 0;
	//lastCallTime = timeNow;

	//m_SpeakerReadBitrate.addSamplesAndInterval( reqSpeakerSampleCnt, callTimeElapsed );

	// LogMsg( LOG_VERBOSE, " AudioLoopback::readRequestFromSpeaker speaker buf m_ProcessedBufMutex.lock()" );
	//m_ProcessedBufMutex.lock();
	if( reqSpeakerSampleCnt > m_SpeakerProcessedBuf.getSampleCnt() )
	{
		//static int64_t lastTime = 0;
		//int timeElapsed = lastTime ? (int)(timeNow - lastTime) : 0;
		//lastTime = timeNow;

		//LogMsg( LOG_DEBUG, "AudioLoopback::readRequestFromSpeaker underrun speaker requested %d available %d elapsed %d ms",
		//	reqSpeakerSampleCnt, m_SpeakerProcessedBuf.getSampleCnt(), timeElapsed );

		memset( readReqPcmBuf, 0, reqSpeakerSampleCnt * AUDIO_BYTES_PER_SAMPLE );
		// LogMsg( LOG_VERBOSE, " AudioLoopback::readRequestFromSpeaker speaker buf m_ProcessedBufMutex.unlock()" );
		//m_ProcessedBufMutex.unlock();
		return 0;
	}
	else
	{
		//static int64_t lastTime = 0;
		//int timeElapsed = lastTime ? (int)(timeNow - lastTime) : 0;
		//lastTime = timeNow;

		memcpy( readReqPcmBuf, m_SpeakerProcessedBuf.getSampleBuffer(), reqSpeakerSampleCnt * AUDIO_BYTES_PER_SAMPLE );
		m_SpeakerProcessedBuf.samplesWereRead( reqSpeakerSampleCnt );

		//if( timeElapsed > 300 )
		//{
		//	LogMsg( LOG_DEBUG, "AudioLoopback::readRequestFromSpeaker delayed success speaker samples left %d elapsed %d ms",
		//		m_SpeakerProcessedBuf.getSampleCnt(), timeElapsed );
		//}
	}

	// LogMsg( LOG_VERBOSE, " AudioLoopback::readRequestFromSpeaker speaker buf m_ProcessedBufMutex.unlock()" );
	//m_ProcessedBufMutex.unlock();

	return reqSpeakerSampleCnt;
}

//============================================================================
int AudioLoopback::toGuiAudioFrameThreaded( EAppModule appModule, int16_t* pcmData, bool isSilenceIn )
{
	lockMixer();
	AudioLoopbackFrame& audioFrame = getAudioWriteFrame();
	int result = audioFrame.toMixerPcm8000HzMonoChannel( eAppModuleMicrophone, pcmData, isSilenceIn );
	unlockMixer();
	return result;
}

//============================================================================
void AudioLoopback::fromGuiEchoCanceledSamplesThreaded( int16_t* pcmData, int sampleCnt, bool isSilence )
{
	vx_assert( AUDIO_SAMPLES_PER_FRAME == sampleCnt );

	static int64_t lastTime = 0;
	int64_t timeNow = GetHighResolutionTimeMs();
	int timeElapsed = lastTime ? (int)(timeNow - lastTime) : 0;
	lastTime = timeNow;

	m_EchoCanceledBitrate.addSamplesAndInterval( sampleCnt, timeElapsed );

	if( m_AudioIoMgr.getSampleCntDebugEnable() )
	{
		LogMsg( LOG_VERBOSE, "fromGuiEchoCanceledSamplesThreaded samples %d elapsed %d ms", sampleCnt, timeElapsed );
	}

	toGuiAudioFrameThreaded( eAppModuleMicrophone, pcmData, isSilence );
}

//============================================================================
void AudioLoopback::fromGuiAudioOutSpaceAvaiThreaded( int sampleCnt )
{
	m_AudioLoopbackSemaphore.signal();
}


//============================================================================
void AudioLoopback::frame80msElapsed( void )
{
	m_AudioLoopbackSemaphore.signal();
}

//============================================================================
void AudioLoopback::processAudioLoopbackThreaded( void )
{
	while( false == m_ProcessAudioLoopbackThread.isAborted() )
	{
		m_AudioLoopbackSemaphore.wait();
		if( m_ProcessAudioLoopbackThread.isAborted() )
		{
			LogMsg( LOG_VERBOSE, "AudioLoopback::processAudioLoopbackThreaded aborting1" );
			break;
		}

		static int64_t lastTime = 0;
		int64_t timeNow = GetHighResolutionTimeMs();
		int timeElapsed = lastTime ? (int)(timeNow - lastTime) : 0;
		lastTime = timeNow;

		if( m_AudioIoMgr.getAudioTimingDebugEnable() )
		{
			static int64_t lastSpaceAvailableTime{ 0 };
			static int funcCallCnt{ 0 };
			funcCallCnt++;
			if( lastSpaceAvailableTime )
			{
				int timeInterval = (int)(timeNow - lastSpaceAvailableTime);
                LogMsg( LOG_VERBOSE, "processAudioOutSpaceAvailableThreaded %d elapsed %d ms app %d ms", funcCallCnt, (int)timeInterval, (int)m_MyApp.elapsedMilliseconds() );
			}

			lastSpaceAvailableTime = timeNow;
		}

		static int16_t prevFrameSample = 0;

		// make current frame ready for read by speakers
		lockMixer();
		AudioLoopbackFrame& audioFrame = getAudioWriteFrame();
		audioFrame.processFrameForSpeakerOutputThreaded( prevFrameSample );
		prevFrameSample = audioFrame.getLastEchoSample();

		// LogMsg( LOG_VERBOSE, " AudioLoopback::processAudioLoopbackThreaded speaker buf m_ProcessedBufMutex.lock()" );
		m_ProcessedBufMutex.lock();

		if( audioFrame.echoSamplesAvailable() != AUDIO_SAMPLES_PER_FRAME )
		{
			LogMsg( LOG_ERROR, "P Frame %d AudioLoopback::processAudioLoopbackThreaded incorrect buffer processing should have %d samples but has %d samples elapsed %d ms",
				audioFrame.getFrameIndex(), AUDIO_SAMPLES_PER_FRAME, audioFrame.echoSamplesAvailable(), timeElapsed );
		}

		if( audioFrame.echoSamplesAvailable() * AUDIO_FRAME_TO_DEVICE_RATE_MULTIPLIER != audioFrame.speakerSamplesAvailable() )
		{
			LogMsg( LOG_ERROR, "P Frame %d AudioLoopback::processAudioLoopbackThreaded incorrect upsampling should be %d samples is %d samples elapsed %d ms", 
				audioFrame.getFrameIndex(), audioFrame.echoSamplesAvailable() * AUDIO_FRAME_TO_DEVICE_RATE_MULTIPLIER, audioFrame.speakerSamplesAvailable(), timeElapsed );
		}

		if( m_AudioIoMgr.getBitrateDebugEnable() )
		{
			m_ProcessFrameBitrate.addSamplesAndInterval( audioFrame.echoSamplesAvailable(), timeElapsed );
			m_ProcessSpeakerBitrate.addSamplesAndInterval( audioFrame.speakerSamplesAvailable(), timeElapsed );
		}

		if( m_AudioIoMgr.getSampleCntDebugEnable() )
		{
			m_AudioIoMgr.getAudioMasterClock().audioSpeakerReadSampleCnt( audioFrame.speakerSamplesAvailable() );
		}

		m_SpeakerProcessedBuf.writeSamples( audioFrame.getSpeakerSampleBuf(), audioFrame.speakerSamplesAvailable() );
		// LogMsg( LOG_VERBOSE, " AudioLoopback::processAudioLoopbackThreaded speaker buf m_ProcessedBufMutex.unlock()" );
		m_ProcessedBufMutex.unlock();
		m_EchoProcessedBuf.writeSamples( audioFrame.getEchoSampleBuf(), audioFrame.echoSamplesAvailable() );

		if( m_AudioIoMgr.getSampleCntDebugEnable() )
		{
			LogMsg( LOG_ERROR, "P Frame %d AudioLoopback::processAudioLoopbackThreaded processed samples available echo %d speaker %d elapsed %d ms",
				audioFrame.getFrameIndex(), m_EchoProcessedBuf.getSampleCnt(), m_SpeakerProcessedBuf.getSampleCnt(), timeElapsed );
		}

		// move to next frame and clear it so is ready to write to
		incrementMixerWriteIndex();
		AudioLoopbackFrame& nextAudioFrame = getAudioWriteFrame();
		nextAudioFrame.clearFrame(false);
		incrementMixerReadIndex();
		unlockMixer();

		// let the echo canceler unlock the processed speaker samples as soon as possible to avoid
		// stalling the qt audio device read or write call
		m_AudioIoMgr.getAudioEchoCancel().processEchoCancelThreaded( m_EchoProcessedBuf, m_ProcessedBufMutex );

		// do output space available processing
		processOutSpaceAvailable();

		if( m_ProcessAudioLoopbackThread.isAborted() )
		{
			LogMsg( LOG_VERBOSE, "AudioLoopback::processAudioLoopbackThreaded aborting3" );
			break;
		}
	}

	LogMsg( LOG_VERBOSE, "AudioLoopback::processAudioLoopbackThreaded leaving function" );
}

//============================================================================
int AudioLoopback::incrementMixerWriteIndex( void ) 
{ 
	m_MixerWriteIdx++; 
	if( m_MixerWriteIdx >= MAX_GUI_MIXER_FRAMES )
	{
		m_MixerWriteIdx = 0;
	}
		
	if( m_AudioIoMgr.getFrameIndexDebugEnable() )
	{
		int64_t timeNow = GetHighResolutionTimeMs();
		static int64_t lastMixerPcmTime{ 0 };
		static int funcCallCnt{ 0 };
		funcCallCnt++;
		if( lastMixerPcmTime )
		{
			int timeInterval = (int)(timeNow - lastMixerPcmTime);
			int avgTimeInterval = timeInterval;
			static std::vector<int> intervalList;
			intervalList.push_back( timeInterval );
			if( intervalList.size() > 20 )
			{
				int totalTime = 0;
				intervalList.erase( intervalList.begin() );
				for( auto interval : intervalList )
				{
					totalTime += interval;
				}

				avgTimeInterval = totalTime / 20;
			}


			LogMsg( LOG_VERBOSE, "W Frame %d call cnt %d incrementMixerWriteIndex elapsed %d ms avg elapsed %d", m_MixerWriteIdx, funcCallCnt, timeInterval, avgTimeInterval );
		}

		lastMixerPcmTime = timeNow;
	}

	return m_MixerWriteIdx; 
}

//============================================================================
int AudioLoopback::incrementMixerReadIndex( void )
{
	m_MixerReadIdx++;
	if( m_MixerReadIdx >= MAX_GUI_MIXER_FRAMES )
	{
		m_MixerReadIdx = 0;
	}

	if( m_AudioIoMgr.getFrameIndexDebugEnable() )
	{
		int64_t timeNow = GetHighResolutionTimeMs();
		static int64_t lastMixerPcmTime{ 0 };
		static int funcCallCnt{ 0 };
		funcCallCnt++;
		if( lastMixerPcmTime )
		{
			int timeInterval = (int)(timeNow - lastMixerPcmTime);
			LogMsg( LOG_VERBOSE, "R Frame %d call cnt %d incrementMixerReadIndex elapsed %d ms", m_MixerReadIdx, funcCallCnt, timeInterval );
		}

		lastMixerPcmTime = timeNow;
	}

	return m_MixerReadIdx;
}

//============================================================================
void AudioLoopback::echoCancelSyncStateThreaded( bool inSync )
{
	if( inSync )
	{
		lockMixer();

		m_EchoCanceledBitrate.setIsBitrateLogEnabled( inSync && m_AudioIoMgr.getBitrateDebugEnable() );
		m_ProcessFrameBitrate.setIsBitrateLogEnabled( inSync && m_AudioIoMgr.getBitrateDebugEnable() );
		m_ProcessSpeakerBitrate.setIsBitrateLogEnabled( inSync && m_AudioIoMgr.getBitrateDebugEnable() );
		m_SpeakerReadBitrate.setIsBitrateLogEnabled( inSync && m_AudioIoMgr.getBitrateDebugEnable() );

		unlockMixer();
	}
}

//============================================================================
void AudioLoopback::microphoneDeviceEnabled( bool isEnabled )
{
	if( isEnabled )
	{
		m_EchoCanceledBitrate.reset();
	}
}

//============================================================================
void AudioLoopback::speakerDeviceEnabled( bool isEnabled )
{
	if( isEnabled )
	{	
		m_ProcessFrameBitrate.reset();
		m_ProcessSpeakerBitrate.reset();
		m_SpeakerReadBitrate.reset();
	}
}

//============================================================================
void AudioLoopback::writeProcessedSamples( int16_t* pcmData, int sampleCnt )
{
	m_ProcessedBufMutex.lock();
	m_SpeakerProcessedBuf.writeSamples( pcmData, sampleCnt );
	m_ProcessedBufMutex.unlock();
}