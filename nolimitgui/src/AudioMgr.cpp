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
#include "AppSettings.h"

#include "VxSndInstance.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>

#include <QTimer>

#include <algorithm>

//============================================================================
AudioMgr::AudioMgr( AppCommon& app )
    : MiniAudioDevices()
    , IAudioRequests()
    , ToGuiHardwareControlInterface()
    , m_MyApp( app )
    , m_AudioInIo( *this )
    , m_AudioOutIo( *this )
    , m_TestFile("")
	// : MiniAudioMgr( app, app, &app )
    // , m_AudioOutIo( *this )
    // , m_AudioInIo( *this )
	// , m_AudioLevelPeekTimer( new QTimer( this ) )
{
	// m_AudioLevelPeekTimer->setInterval( 500 );
	// connect( m_AudioLevelPeekTimer, SIGNAL(timeout()), this, SLOT(slotAudioPeekTimeout()) );
}

//============================================================================
void AudioMgr::audioIoSystemStartup()
{
    if( !m_AudioIoInitialized )
    {
        int startTime = GetApplicationAliveMs();
        LogMsg( LOG_DEBUG, "%s begin at %d", __func__, startTime );

        startupMiniAudio();

        if( isSpeakerDeviceAvailable() )
        {
            int deviceIndex = 0;
            getSoundOutDeviceIndex( deviceIndex );

            m_AudioOutIo.initAudioOut( m_AudioOutFormat, deviceIndex );
            //m_ToneGenerator.setAudioFormat( m_AudioOutFormat );
        }

        if( isMicrophoneDeviceAvailable() )
        {
            int deviceIndex = 0;
            getSoundInDeviceIndex( deviceIndex );

            m_AudioInIo.initAudioIn( m_AudioInFormat, deviceIndex );
        }

        m_AudioIoInitialized = true;
        //m_ProcessAudioThread.startThread( (VX_THREAD_FUNCTION_T)AudioMiniAudioMgrProcessThreadFunc, this, "ProcessAudioThread" );
        int endTime = GetApplicationAliveMs();
        LogMsg( LOG_DEBUG, "%s took %d ms at %d", __func__, endTime - startTime, endTime );


        m_MyApp.wantToGuiHardwareCtrlCallbacks( this, true );

        bool mutedMic = m_MyApp.getAppSettings().getMicMuted();
        m_MyApp.fromGuiMuteMicrophone( mutedMic );

        bool mutedSpeaker = m_MyApp.getAppSettings().getSpeakerMuted();
        m_MyApp.fromGuiMuteSpeaker( mutedSpeaker );
    }
}

//============================================================================
void AudioMgr::audioIoSystemShutdown()
{
    if( m_AudioIoInitialized )
    {
        m_AudioIoInitialized = false;
        m_MyApp.wantToGuiHardwareCtrlCallbacks( this, false );
	    audioIoSystemShutdown();

        m_AudioInIo.audioInShutdown();
        m_AudioOutIo.audioOutShutdown();
        shutdownMiniAudio();
    }
}

//============================================================================
void AudioMgr::callbackAudioDeviceWrite( int16_t* pcmData, int sampleCnt )
{
    if( !pcmData || sampleCnt <= 0 )
    {
        return;
    }

    // 1. Append the new data to our residual buffer
    {
        std::lock_guard<std::mutex> lk(m_ResidualInBufferMutex);
        m_ResidualInBuffer.insert(m_ResidualInBuffer.end(), pcmData, pcmData + sampleCnt);
    }

    const int sampleRate = m_AudioInFormat.sampleRate() ? m_AudioInFormat.sampleRate()
                                                        : AUDIO_DEVICE_SAMPLE_RATE;
    m_Aec.setStreamFormat(sampleRate, AUDIO_CHANNELS);

    // 2. Process as many 10ms frames as we can
    while (true)
    {
        std::vector<int16_t> frame(FRAME_SIZE_10MS);
        {
            std::lock_guard<std::mutex> lk(m_ResidualInBufferMutex);
            if (m_ResidualInBuffer.size() < FRAME_SIZE_10MS)
                break;
            std::copy_n(m_ResidualInBuffer.data(), FRAME_SIZE_10MS, frame.data());
            m_ResidualInBuffer.erase(m_ResidualInBuffer.begin(), m_ResidualInBuffer.begin() + FRAME_SIZE_10MS);
        }

        int16_t* framePtr = frame.data();

        if( m_IsPlayingTestFile )
        {
            // If playing test file, override frame with test file data
            if( !m_TestFile.getNextAudioFrame(framePtr, FRAME_SIZE_10MS) )
            {
                setIsPlayingTestFile( false );
                setIsMicrophoneWanted( true );
            }
        }

        if( m_MicrophoneMuted )
        {
            // If loopback is disabled, zero out the frame to prevent it from being heard in the speakers
            std::fill_n(framePtr, FRAME_SIZE_10MS, 0);
        }

        // after processing the contents of framePtr will change so push to m_AudioInFrameBuffer first
        // that way Waveform display shows pre-processed audio
        m_AudioInFrameBuffer.pushFrame(framePtr, FRAME_SIZE_10MS, 0.0f, 0.0f, false);

        if( m_DirectMicToSpeakerEnabled )
        {
            // skip AEC processing and push the raw mic audio directly to visualization
            m_AudioEchoCanceledFrameBuffer.pushFrame(framePtr, FRAME_SIZE_10MS, 0.0f, 0.0f, false);
            // also send to speaker output callback to be played back immediately
            callbackAecProcessedAudio( framePtr, FRAME_SIZE_10MS );
            continue;
        }

        // 1. Run through WebRTC AudioProcessing (AEC3, AGC2, VAD)
        m_Aec.processCapture(framePtr, FRAME_SIZE_10MS);

        // 2. Get the results (e.g., processed PCM, VAD probability)
        m_CurrentVadProb = m_Aec.lastVadProbability();
        m_CurrentErl = m_Aec.lastEchoReturnLoss();

        // 3. Push the processed audio to the buffer for visualization
        m_AudioEchoCanceledFrameBuffer.pushFrame(framePtr, FRAME_SIZE_10MS,
                                   m_CurrentVadProb, m_CurrentErl, true);

        // 4. Process the echo-cancelled audio for 
        //  - speaker output (e.g., apply loopback or direct mic-to-speaker if enabled) 
        //  - send to opus encoder to send over network if connected to a peer
        callbackAecProcessedAudio( framePtr, FRAME_SIZE_10MS );
    }

    // Any remaining samples (less than 160) stay in m_residualBuffer
    // until the next callback arrives.
}

//============================================================================
void AudioMgr::callbackReadSpeakerData( int16_t* pcmData, int sampleCnt )
{
    if( !pcmData || sampleCnt <= 0 )
    {
        return;
    }

    // The m_SpeakerOutBuffer always contains a multiple of FRAME_SIZE_10MS samples that are ready to be played back
    // MiniAudio will request samples and depending on the OS it might request in chunks that are not a multiple of FRAME_SIZE_10MS, 
    // so we need to handle that case by copying only the available samples and zero filling the rest if necessary

    const size_t requestedSamples = static_cast<size_t>( sampleCnt );
    size_t samplesToCopy = 0;

    {
        std::lock_guard<std::mutex> lk(m_SpeakerOutBufferMutex);
        const size_t availableSamples = (m_SpeakerOutBuffer.size() > m_SpeakerOutReadOffset)
                                      ? (m_SpeakerOutBuffer.size() - m_SpeakerOutReadOffset)
                                      : 0;
        samplesToCopy = std::min( availableSamples, requestedSamples );

        if( samplesToCopy > 0 )
        {
            std::copy_n( m_SpeakerOutBuffer.data() + m_SpeakerOutReadOffset, samplesToCopy, pcmData );
            m_SpeakerOutReadOffset += samplesToCopy;

            if( m_SpeakerOutReadOffset >= m_SpeakerOutBuffer.size() )
            {
                m_SpeakerOutBuffer.clear();
                m_SpeakerOutReadOffset = 0;
            }
            else if( m_SpeakerOutReadOffset > 1024 && (m_SpeakerOutReadOffset * 2) >= m_SpeakerOutBuffer.size() )
            {
                m_SpeakerOutBuffer.erase( m_SpeakerOutBuffer.begin(), m_SpeakerOutBuffer.begin() + static_cast<std::vector<int16_t>::difference_type>(m_SpeakerOutReadOffset) );
                m_SpeakerOutReadOffset = 0;
            }
        }
    }

    if( samplesToCopy < requestedSamples )
    {
        LogMsg( LOG_VERBOSE, "%s: Underflow! Requested %zu samples but only %zu available. Zero filling the rest.", __func__, 
            requestedSamples, samplesToCopy );
        std::fill( pcmData + samplesToCopy, pcmData + sampleCnt, 0 );
    }

    if( m_SpeakersMuted)
    {
        // If muted, zero out the entire buffer to prevent any sound from being heard in the speakers
        std::fill_n( pcmData, sampleCnt, 0 );
    }

    // Tell WebRTC AEC about the samples that were just played out so it can update its internal state 
    // and do better echo cancellation on subsequent frames
    const int sampleRate = m_AudioOutFormat.sampleRate() ? m_AudioOutFormat.sampleRate()
                                                         : AUDIO_DEVICE_SAMPLE_RATE;
    m_Aec.setStreamFormat(sampleRate, AUDIO_CHANNELS);

    size_t processedOffset = 0;
    while( processedOffset + static_cast<size_t>( FRAME_SIZE_10MS ) <= samplesToCopy )
    {
        m_Aec.processRender( pcmData + processedOffset, FRAME_SIZE_10MS );
        processedOffset += static_cast<size_t>( FRAME_SIZE_10MS );
    }

    // samples already erased under lock above
}

//============================================================================
void AudioMgr::callbackAecProcessedAudio( int16_t* pcmData, int sampleCnt )
{
    if( !pcmData || sampleCnt <= 0 )
    {
        return;
    }

    if( m_AudioLoopbackEnabled || m_DirectMicToSpeakerEnabled )
    {
        // make available to speaker output callback to be played back immediately
        std::lock_guard<std::mutex> lk(m_SpeakerOutBufferMutex);
        m_SpeakerOutBuffer.insert(m_SpeakerOutBuffer.end(), pcmData, pcmData + sampleCnt);
    }

}

//============================================================================
void AudioMgr::playTestFile( TestFileWav& testFile )
{
    if( getIsPlayingTestFile() )
    {
        LogMsg( LOG_WARNING, "%s Already playing a test file. Ignoring new request.", __func__ );
        return;
    }
    
    m_TestFile = testFile;
    m_TestFile.resetPlayback();

    setIsMicrophoneWanted( false );
    setIsPlayingTestFile( true );
}

//============================================================================
void AudioMgr::callbackPeerNetworkAudio( int16_t* pcmData, int sampleCnt )
{
    if( !pcmData || sampleCnt <= 0 )
    {
        return;
    }


    if( m_AudioLoopbackEnabled || m_DirectMicToSpeakerEnabled  )
    {
        // If loopback or direct mic-to-speaker is enabled, we don't want to play back peer audio to avoid confusion, so just return early
        return;
    }

    // Process the received network audio
    {
        std::lock_guard<std::mutex> lk(m_SpeakerOutBufferMutex);
        m_SpeakerOutBuffer.insert(m_SpeakerOutBuffer.end(), pcmData, pcmData + sampleCnt);
    }
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

	int micLevel = getIsMicrophoneRunning() && !getMicrophoneMuted() ? getAudioInPeakAmplitude() : 0;

	for( auto& client : m_AudioLevelClientList )
	{
		client->callbackGuiMicrophoneLevel( micLevel );
	}
}

//============================================================================
void AudioMgr::setPlayerNlcActive( bool isActive )
{
    if( isActive != m_PlayerNlcActive )
    {
        m_PlayerNlcActive = isActive;
        if( m_PlayerNlcActive )
        {
            lockPlayerCache();

            m_PlayerCacheBuf.clear();

            unlockPlayerCache();

            lockModuleMixerBuffer();

            AudioMixerBuf& mixerBuf = getAudioMixerBuf( eMediaModulePlayerNlc );
            mixerBuf.clear();

            unlockModuleMixerBuffer();
        }
        
        toGuiWantSpeakerOutput( eMediaModulePlayerNlc, m_PlayerNlcActive );
    }
}

//============================================================================
void AudioMgr::wantMicrophoneCountChanged( int wantMicCnt )
{
	m_WantMicCnt = wantMicCnt;
	for( auto& client : m_AudioLevelClientList )
	{
		client->callbackWantMicrophoneCount( m_WantMicCnt );
	}
}

//============================================================================
void AudioMgr::wantSpeakerCountChanged( int wantSpeakerCnt )
{
	m_WantSpeakerCnt = wantSpeakerCnt;
	for( auto& client : m_AudioLevelClientList )
	{
		client->callbackWantSpeakerCount( m_WantSpeakerCnt );
	}
}
