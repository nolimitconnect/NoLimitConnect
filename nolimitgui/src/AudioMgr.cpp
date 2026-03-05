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
#include "AudioDefs.h"
#include "AudioUtils.h"
#include "GuiAudioLevelCallback.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>
#include <CoreLib/VxTimer.h>

#include <algorithm>

//============================================================================
AudioMgr::AudioMgr( AppCommon& app )
    : m_MyApp( app )
    , MiniAudioDevices()
    , AudioMixerMgr()
    , ToGuiHardwareControlInterface()
    , m_TestFile("")
    , m_AudioInIo( *this )
    , m_AudioOutIo( *this )
	, m_AudioTestTimer( new QTimer( this ) )
    , m_AudioLevelPeekTimer( new QTimer( this ) )
{
	m_AudioLevelPeekTimer->setInterval( 500 );
	connect( m_AudioLevelPeekTimer, SIGNAL(timeout()), this, SLOT(slotAudioPeekTimeout()) );

    m_AudioTestTimer->setInterval( 1200 );
    connect( m_AudioTestTimer, SIGNAL(timeout()), this, SLOT(slotAudioTestTimer()) );
}

//============================================================================
void AudioMgr::audioIoSystemStartup()
{
    if( !m_AudioIoInitialized )
    {
        int startTime = GetApplicationAliveMs();
        LogMsg( LOG_DEBUG, "%s begin at %d", __func__, startTime );

        startupMiniAudio();
        setAgcEnabled( false );

        if( isSpeakerDeviceAvailable() )
        {
            int deviceIndex = 0;
            getSoundOutDeviceIndex( deviceIndex );

            m_AudioOutIo.initAudioOut( m_AudioOutFormat, deviceIndex );
            m_ToneGenerator.setAudioFormat( m_AudioOutFormat );
        }

        if( isMicrophoneDeviceAvailable() )
        {
            int deviceIndex = 0;
            getSoundInDeviceIndex( deviceIndex );

            m_AudioInIo.initAudioIn( m_AudioInFormat, deviceIndex );
        }

        updateHardwareTotalLatencyMs();
        LogMsg( LOG_DEBUG, "%s calculated hardware total latency is %d ms", __func__, m_EchoHardwareTotalLatencyMs );

        m_AudioIoInitialized = true;
        //m_ProcessAudioThread.startThread( (VX_THREAD_FUNCTION_T)AudioMiniAudioMgrProcessThreadFunc, this, "ProcessAudioThread" );
        int endTime = GetApplicationAliveMs();
        LogMsg( LOG_DEBUG, "%s took %d ms at %d", __func__, endTime - startTime, endTime );

        m_MyApp.wantToGuiHardwareCtrlCallbacks( this, true );

        bool mutedMic = m_MyApp.getAppSettings().getIsMicrophoneMuted();
        m_MyApp.fromGuiMuteMicrophone( mutedMic );

        bool mutedSpeaker = m_MyApp.getAppSettings().getIsSpeakerMuted();
        m_MyApp.fromGuiMuteSpeaker( mutedSpeaker );
    }
}

//============================================================================
void AudioMgr::audioIoSystemShutdown()
{
    if( m_AudioIoInitialized )
    {
        enableAudioIn( false );
        enableAudioOut( false );
        m_AudioIoInitialized = false;
        m_MyApp.wantToGuiHardwareCtrlCallbacks( this, false );
	    audioIoSystemShutdown();

        m_AudioInIo.audioInShutdown();
        m_AudioOutIo.audioOutShutdown();
        shutdownMiniAudio();
    }
}

//============================================================================
void AudioMgr::enableAudioIn( bool enable )
{
    setIsMicrophoneWanted( enable );

    if( enable )
    {
        startAudioInWorker();
        m_AudioInIo.startAudioInHardware();
    }
    else
    {
        m_AudioInIo.stopAudioInHardware();
        stopAudioInWorker();
    }
}

//============================================================================
void AudioMgr::enableAudioOut( bool enable )
{
    setIsSpeakerWanted( enable );

    if( enable )
    {
        startAudioOutWorker();
        m_AudioOutIo.startAudioOutHardware();
    }
    else
    {
        m_AudioOutIo.stopAudioOutHardware();
        stopAudioOutWorker();
    }
}

//============================================================================
void AudioMgr::sendToSpeakerOutput( int16_t* pcmData, int sampleCnt )
{    
    if( !pcmData || sampleCnt <= 0 )
    {
        return;
    }

    std::lock_guard<std::mutex> lk(m_SpeakerOutBufferMutex);
    m_SpeakerOutBuffer.insert( m_SpeakerOutBuffer.end(), pcmData, pcmData + sampleCnt );

    const uint64_t queueDepth = static_cast<uint64_t>( m_SpeakerOutBuffer.size() );
    uint64_t currentHighWater = m_SpeakerQueueHighWatermark.load();
    while( queueDepth > currentHighWater &&
            !m_SpeakerQueueHighWatermark.compare_exchange_weak( currentHighWater, queueDepth ) )
    {
    }

    // remove old samples if buffer gets too large to prevent unbounded growth
    const size_t MAX_SPEAKER_OUT_BUFFER_SAMPLES = ECHO_FRAME_SIZE_10MS * 50;
    if( m_SpeakerOutBuffer.size() > MAX_SPEAKER_OUT_BUFFER_SAMPLES )
    {        
        m_SpeakerOutBuffer.erase( m_SpeakerOutBuffer.begin(), 
            m_SpeakerOutBuffer.begin() + static_cast<std::vector<int16_t>::difference_type>(m_SpeakerOutBuffer.size() - MAX_SPEAKER_OUT_BUFFER_SAMPLES) );
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
int AudioMgr::updateHardwareTotalLatencyMs( void ) 
{
    m_EchoHardwareTotalLatencyMs = m_AudioInIo.getHardwareDelayMs() + m_AudioOutIo.getHardwareDelayMs();
    return m_EchoHardwareTotalLatencyMs;
}

//============================================================================
void AudioMgr::setEchoDelayParam( int delayMs )
{ 
    m_EchoDelayMs = delayMs; 
    m_Aec.setEchoDelay(delayMs);
    LogMsg( LOG_DEBUG, "%s set echo delay to %d ms", __func__, delayMs );
}

//============================================================================
int AudioMgr::getWantMicrophoneCount( void ) 
{ 
    m_WantMicMutex.lock();
    int count = static_cast<int>( m_WantMicList.size() );   
    m_WantMicMutex.unlock();
    return count; 
}

//============================================================================
int AudioMgr::getWantSpeakerCount( void ) 
{
    m_WantSpeakerMutex.lock();
    int count = static_cast<int>( m_WantSpeakerList.size() );
    m_WantSpeakerMutex.unlock();
    return count;
}

//============================================================================
void AudioMgr::updateWantMicrophoneCount( int wantMicCnt )
{
	m_WantMicCnt = wantMicCnt;
	for( auto& client : m_AudioLevelClientList )
	{
		client->callbackWantMicrophoneCount( wantMicCnt );
	}
}

//============================================================================
void AudioMgr::updateWantSpeakerCount( int wantSpeakerCnt )
{
	m_WantSpeakerCnt = wantSpeakerCnt;
	for( auto& client : m_AudioLevelClientList )
	{
		client->callbackWantSpeakerCount( wantSpeakerCnt );
	}
}

//============================================================================
void AudioMgr::writeMixerAudioToSpeakerHardware( int16_t* pcmData, int sampleCount )
{
    sendToSpeakerOutput( pcmData, sampleCount );
}