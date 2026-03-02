//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AudioMgr.h"

#include "AppCommon.h"
#include "AudioDefs.h"
#include "AudioUtils.h"
#include "GuiAudioLevelCallback.h"

#include <P2PEngine/P2PEngine.h>
#include <MediaProcessor/MediaProcessor.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>
#include <CoreLib/VxTimer.h>

//============================================================================
void AudioMgr::startAudioInWorker( void )
{
    if( m_AudioInWorkerRunning )
    {
        return;
    }

    m_AudioInWorkerStopping = false;
    m_AudioInWorkerThread = std::thread( &AudioMgr::audioInWorkerLoop, this );
    m_AudioInWorkerRunning = true;
}

//============================================================================
void AudioMgr::stopAudioInWorker( void )
{
    m_AudioInWorkerStopping = true;
    m_AudioInWorkSemaphore.signal();

    if( m_AudioInWorkerThread.joinable() )
    {
        m_AudioInWorkerThread.join();
    }

    m_AudioInWorkerRunning = false;

    {
        std::lock_guard<std::mutex> lk( m_PendingInBuffersMutex );
        m_PendingInBuffers.clear();
    }

    {
        std::lock_guard<std::mutex> lk( m_ResidualInBufferMutex );
        m_ResidualInBuffer.clear();
    }
}

//============================================================================
void AudioMgr::audioInWorkerLoop( void )
{
    while( true )
    {
        m_AudioInWorkSemaphore.wait();

        std::vector<std::vector<int16_t>> pendingBuffers;
        {
            std::lock_guard<std::mutex> lk( m_PendingInBuffersMutex );
            pendingBuffers.swap( m_PendingInBuffers );
        }

        for( auto& buffer : pendingBuffers )
        {
            if( !buffer.empty() )
            {
                processQueuedAudioInput( buffer.data(), static_cast<int>(buffer.size()) );
            }
        }

        if( m_AudioInWorkerStopping )
        {
            std::lock_guard<std::mutex> lk( m_PendingInBuffersMutex );
            if( m_PendingInBuffers.empty() )
            {
                break;
            }
        }
    }
}

//============================================================================
void AudioMgr::callbackAudioDeviceWrite( int16_t* pcmData, int sampleCnt )
{
    if( !pcmData || sampleCnt <= 0 )
    {
        return;
    }

    constexpr size_t MAX_PENDING_AUDIO_IN_BUFFERS = 3;

    std::vector<int16_t> capturedSamples( pcmData, pcmData + sampleCnt );

    {
        std::lock_guard<std::mutex> lk( m_PendingInBuffersMutex );
        if( m_PendingInBuffers.size() >= MAX_PENDING_AUDIO_IN_BUFFERS )
        {
            m_PendingInBuffers.erase( m_PendingInBuffers.begin() );
            LogMsg( LOG_WARNING, "%s: Audio input queue overflow; dropping oldest pending buffer", __func__ );
        }
        m_PendingInBuffers.emplace_back( std::move(capturedSamples) );
    }

    m_AudioInWorkSemaphore.signal();
}

//============================================================================
void AudioMgr::processQueuedAudioInput( const int16_t* pcmData, int sampleCnt )
{
    if( !pcmData || sampleCnt <= 0 )
    {
        return;
    }

    // 1. Append the new data to our residual buffer
    {
        std::lock_guard<std::mutex> lk( m_ResidualInBufferMutex );
        m_ResidualInBuffer.insert( m_ResidualInBuffer.end(), pcmData, pcmData + sampleCnt );
    }

    static int lastSampleRate = 0;
    const int sampleRate = m_AudioInFormat.sampleRate() ? m_AudioInFormat.sampleRate()
                                                        : AUDIO_DEVICE_SAMPLE_RATE;
    if (sampleRate != lastSampleRate) {
        m_Aec.setStreamFormat(sampleRate, AUDIO_CHANNELS);
        lastSampleRate = sampleRate;
    }

    // 2. Process as many 10ms frames as we can
    while( true )
    {
        std::vector<int16_t> frame( ECHO_FRAME_SIZE_10MS );
        {
            std::lock_guard<std::mutex> lk( m_ResidualInBufferMutex );
            if( m_ResidualInBuffer.size() < ECHO_FRAME_SIZE_10MS )
                break;
            std::copy_n( m_ResidualInBuffer.data(), ECHO_FRAME_SIZE_10MS, frame.data() );
            m_ResidualInBuffer.erase( m_ResidualInBuffer.begin(), m_ResidualInBuffer.begin() + ECHO_FRAME_SIZE_10MS );
        }

        int16_t* framePtr = frame.data();

        if( m_IsPlayingTestFile )
        {
            // If playing test file, override frame with test file data
            if( !m_TestFile.getNextAudioFrame(framePtr, ECHO_FRAME_SIZE_10MS) )
            {
                setIsPlayingTestFile( false );
                setIsMicrophoneWanted( true );
            }
        }

        if( m_MicrophoneMuted )
        {
            // If loopback is disabled, zero out the frame to prevent it from being heard in the speakers
            std::fill_n(framePtr, ECHO_FRAME_SIZE_10MS, 0);
        }

        if( m_VisualizeInWanted )
        {
            // after processing the contents of framePtr will change so push to m_AudioInRawWaveformBuffer first
            // that way Waveform display shows pre-processed audio
            m_AudioInRawWaveformBuffer.pushFrame(framePtr, ECHO_FRAME_SIZE_10MS, 0.0f, 0.0f, false);
        }

        if( m_AudioTestState != eAudioTestStateNone )
        {
            if( m_AudioTestState == eAudioTestStateRun )
            {
                audioTestDetectTestSound( pcmData, sampleCnt );
            }

            return;
        }

        if( m_NoAecLoopbackEnabled )
        {
            if( m_VisualizeInWanted )
            {
                // skip AEC processing and push the raw mic audio directly to visualization
                m_AudioAecProcessedWaveformBuffer.pushFrame(framePtr, ECHO_FRAME_SIZE_10MS, 0.0f, 0.0f, false);
            }
                // also send to speaker output callback to be played back immediately
            callbackAecProcessedAudio( framePtr, ECHO_FRAME_SIZE_10MS );
            continue;
        }

        int echoDelayMs = m_EchoDelayMs + getEchoHardwareTotalLatencyMs();
        int outJitterMs = 0;
        if( m_OutExpectedRenderTimeMs != 0 )
        {
            m_OutExpectedRenderTimeMs += ECHO_MS_PER_FRAME;
            int64_t nowMs = GetHighResolutionTimeMs();
            outJitterMs = static_cast<int>(nowMs - m_OutExpectedRenderTimeMs);
            if( std::abs(outJitterMs) > 5 )
            {
                outJitterMs = 0;
                LogMsg( LOG_WARNING, "%s: Detected speaker output jitter of %d ms", __func__, outJitterMs );
                m_OutExpectedRenderTimeMs = nowMs;
            }
            // else if( outJitterMs != 0 )
            // {
            //     LogMsg( LOG_VERBOSE, "%s: Speaker output jitter of %d ms", __func__, outJitterMs );
            // }
        }
        else
        {
            m_OutExpectedRenderTimeMs = GetHighResolutionTimeMs();
        }

        m_Aec.setEchoDelay(echoDelayMs + outJitterMs);

        // 1. Run through WebRTC AudioProcessing (AEC3, AGC2, VAD)
        m_Aec.processCapture(framePtr, ECHO_FRAME_SIZE_10MS);

        // 2. Get the results (e.g., processed PCM, VAD probability)
        m_CurrentVadProb = m_Aec.lastVadProbability();
        m_CurrentErl = m_Aec.lastEchoReturnLoss();

        if( m_VisualizeInWanted )
        {
            // 3. Push the processed audio to the buffer for visualization
            m_AudioAecProcessedWaveformBuffer.pushFrame( framePtr, ECHO_FRAME_SIZE_10MS,
                                    m_CurrentVadProb, m_CurrentErl, true );
        }

        // 4. Process the echo-cancelled audio for 
        //  - speaker output (e.g., apply loopback or direct mic-to-speaker if enabled) 
        //  - send to opus encoder to send over network if connected to a peer
        callbackAecProcessedAudio( framePtr, ECHO_FRAME_SIZE_10MS );
    }

    // Any remaining samples (less than 160) stay in m_ResidualInBuffer
    // until the next callback arrives.
}

//============================================================================
void AudioMgr::callbackAecProcessedAudio( int16_t* pcmData, int sampleCnt )
{
    if( !pcmData || sampleCnt <= 0 )
    {
        return;
    }

    if( m_MicJitterStatsEnable)
    {
        // callbacks to write mic data to aec should happen every 10ms
        static int64_t lastMicCallbackTime = 0;
        int64_t micCallbackTime = GetHighResolutionTimeMs();
        if( lastMicCallbackTime != 0 )
        {
            const int64_t micCallbackJitter = (micCallbackTime - lastMicCallbackTime) - ECHO_MS_PER_FRAME;
            if(micCallbackJitter != 0 )
            {
                if( micCallbackJitter < m_MicInJitterLowMarkMs )
                {
                    m_MicInJitterLowMarkMs = micCallbackJitter;
                }
                if( micCallbackJitter > m_MicInJitterHighMarkMs )
                {
                    m_MicInJitterHighMarkMs = micCallbackJitter;
                }
            }
        }
        
        lastMicCallbackTime = micCallbackTime;

        const int nowMs = GetApplicationAliveMs();
        if( 0 == m_LastMicStatsLogMs )
        {
            m_LastMicStatsLogMs = nowMs;
        }
        else if( ( nowMs - m_LastMicStatsLogMs ) >= 1000 )
        {
            if(  m_MicInJitterHighMarkMs - m_MicInJitterLowMarkMs> 2)
            {
                LogMsg( LOG_WARNING, "%s: Detected mic jitter! low=%lld high=%lld",
                    __func__,
                    static_cast<unsigned long long>( m_MicInJitterLowMarkMs ),
                    static_cast<unsigned long long>( m_MicInJitterHighMarkMs ) );
                m_MicInJitterLowMarkMs = 100000000;
                m_MicInJitterHighMarkMs = 0;
            }

            m_LastMicStatsLogMs = nowMs;
        }
    }

    if( !m_AudioLevelClientList.empty() )
    {
        if( getIsMicrophoneMuted() )
        {
            m_AudioInPeakAmplitude = 0;
        }
        else
        {
            int thisPeak = AudioUtils::peakPcmAmplitude0to100( pcmData, sampleCnt );
            if( thisPeak > m_AudioInPeakAmplitude )
            {
                m_AudioInPeakAmplitude = thisPeak;
            }
        }
    }

    if( m_NoAecLoopbackEnabled || m_WithAecLoopbackEnabled )
    {
        // make available to speaker output callback to be played back immediately
        sendToSpeakerOutput( pcmData, sampleCnt );
        return;
    }

    m_OpusFrameBuffer.insert( m_OpusFrameBuffer.end(), pcmData, pcmData + ECHO_FRAME_SIZE_10MS );
    m_AudioInAecFramesProcessed++;
    if( m_AudioInAecFramesProcessed >= AEC_FRAME_COUNT_PER_OPUS_FRAME )
    {
        callbackAudioIn60msFrameAvail( m_OpusFrameBuffer.data(), static_cast<int>(m_OpusFrameBuffer.size()) );
        m_OpusFrameBuffer.clear();
        m_AudioInAecFramesProcessed = 0;
    }
}

//============================================================================
void AudioMgr::callbackAudioIn60msFrameAvail( const int16_t* pcmData, int sampleCnt )
{    
    if( !pcmData || sampleCnt <= 0 )
    {
        return;
    }

    m_MyApp.getEngine().getMediaProcessor().fromGuiEchoCanceledSamplesThreaded( pcmData, sampleCnt );
}

//============================================================================
void AudioMgr::wantMicrophoneLevelCallbacks( GuiAudioLevelCallback* client, bool enable )
{
	for( auto iter = m_AudioLevelClientList.begin(); iter != m_AudioLevelClientList.end(); ++iter )
	{
        if( client == *iter )
		{
			if( enable )
			{
				return;
			}
			else
			{
				m_AudioLevelClientList.erase( iter );
				if( 0 == m_AudioLevelClientList.size() )
				{
					m_AudioLevelPeekTimer->stop();
				}

				return;
			}
		}
	}

	if( enable )
	{
		m_AudioLevelClientList.emplace_back( client );
		if( 1 == m_AudioLevelClientList.size() )
		{
			m_AudioLevelPeekTimer->start();
		}
	}
}

//============================================================================
void AudioMgr::slotAudioPeekTimeout( void )
{
	if( m_AudioLevelClientList.empty() )
	{
		return;
	}

	int micLevel = getIsMicrophoneRunning() && !getIsMicrophoneMuted() ? getAudioInPeakAmplitude() : 0;
	setAudioInPeakAmplitude( 0 ); // reset for next peek interval	

	for( auto& client : m_AudioLevelClientList )
	{
		client->callbackGuiMicrophoneLevel( micLevel );
	}
}
