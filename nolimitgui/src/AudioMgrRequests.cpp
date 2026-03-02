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
#include "miniaudio/AudioUtils.h"

#include <P2PEngine/P2PEngine.h>
#include <MediaProcessor/MediaProcessor.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

//============================================================================
// return true if any microphone device is available to be enabled
bool AudioMgr::toGuiIsMicrophoneDeviceAvailable( void )
{
    return m_MaMicDeviceCount > 0;
}

//============================================================================
// enable disable microphone data callback
void AudioMgr::toGuiWantMicrophoneRecording( EMediaModule mediaModule, bool wantMicInput )
{
    if( LogEnabled( eLogVoice ) ) LogModule( eLogVoice, LOG_DEBUG, "AudioMgr::%s want mic? %d module %s", __func__, wantMicInput, DescribeMediaModule( mediaModule ) );
    bool found{ false };
    m_WantMicMutex.lock();
    size_t prevWantMicCnt = m_WantMicList.size();
    for( auto iter = m_WantMicList.begin(); iter != m_WantMicList.end(); iter++ )
    {
        if( *iter == mediaModule )
        {
            found = true;
            if( !wantMicInput )
            {
                m_WantMicList.erase( iter );
                break;
            }
        }
    }

    if( wantMicInput && !found )
    {
        m_WantMicList.emplace_back( mediaModule );
    }

    size_t wantMicCnt = m_WantMicList.size();
    m_WantMicMutex.unlock();

    if( prevWantMicCnt != wantMicCnt )
    {
        if( !prevWantMicCnt && wantMicCnt )
        {
            // mic count went from 0 to 1, enable mic
            m_AudioInIo.startAudioIn();
        }
        else if( prevWantMicCnt && !wantMicCnt )
        {
            // mic count went from 1 to 0, disable mic
            m_AudioInIo.stopAudioIn();
        }

        updateWantMicrophoneCount( static_cast<int>(wantMicCnt) );
    }
}

//============================================================================
// enable disable sound out
void AudioMgr::toGuiWantSpeakerOutput( EMediaModule mediaModule, bool wantSpeakerOutput )
{
    bool found{ false };
    m_WantSpeakerMutex.lock();
    size_t prevWantSpeakerCnt = getWantSpeakerCount(); 
    for( auto iter = m_WantSpeakerList.begin(); iter != m_WantSpeakerList.end(); iter++ )
    {
        if( *iter == mediaModule )
        {
            found = true;
            if( !wantSpeakerOutput )
            {
                m_WantSpeakerList.erase( iter );
                break;
            }
        }
    }

    if( wantSpeakerOutput && !found )
    {
        m_WantSpeakerList.emplace_back( mediaModule );
    }

    bool enableSpeaker = !m_WantSpeakerList.empty();
    size_t wantSpeakerCnt = getWantSpeakerCount();
    m_WantSpeakerMutex.unlock();

    if( prevWantSpeakerCnt != wantSpeakerCnt )
    {
        if( !prevWantSpeakerCnt && wantSpeakerCnt )
        {
            // speaker count went from 0 to 1, enable speaker
            m_AudioOutIo.startAudioOut();
        }
        else if( prevWantSpeakerCnt && !wantSpeakerCnt )
        {
            // speaker count went from 1 to 0, disable speaker
            m_AudioOutIo.stopAudioOut();
        }

        updateWantSpeakerCount( static_cast<int>(wantSpeakerCnt) );
    }

    if( LogEnabled( eLogVoice ) ) LogModule( eLogVoice, LOG_DEBUG, "AudioMgr::%s want speaker? %d module %s cnt %d", __func__, wantSpeakerOutput, DescribeMediaModule( mediaModule ), wantSpeakerCnt );
}

//============================================================================
int AudioMgr::toGuiPlayerNlcAudio( EMediaModule mediaModule, float* audioDataFloat, int audioDataLenInBytes )
{
    // this assumes 20ms of stereo 48000 hz at a time from kodi based player
    // unfortunately there is no good way to down sample stereo to mono so we have to pick a channel 
    // this is because if the channels are 180 degrees out of phase they cancel each other out

    // TODO rework audio out to handle 48000 hz stereo for higher quality playback

    if( VxIsAppShuttingDown() || !audioDataLenInBytes || !audioDataFloat )
    {
        if(LogEnabled(eLogAudioIo)) LogModule( eLogAudioIo, LOG_VERBOSE, "AudioMgr::toGuiPlayerNlcAudio ignored len %d", audioDataLenInBytes );
        return 0;
    }

    if( eMediaModulePlayerNlc == mediaModule )
    {
        // vx_assert( AUDIO_FRAME_SIZE_KODI == audioDataLenInBytes );

        int kodiSampleCnt = audioDataLenInBytes / sizeof( float );
        int skipCnt = AUDIO_KODI_TO_NLC_DNSAMPLE_RATIO;
        int pcmSampleCnt = kodiSampleCnt / skipCnt;

        lockPlayerCache();

        if( pcmSampleCnt > m_PlayerCacheBuf.freeSpaceSampleCount() )
        {
            unlockPlayerCache();
            LogMsg( LOG_ERROR, "AudioMgr::toGuiPlayerNlcAudio overrun PlayerNlc" );
            return 0;
        }

        int16_t* pcmBuf = m_PlayerCacheBuf.getSampleBuffer();
        pcmBuf += m_PlayerCacheBuf.getSampleCnt();
        int totalSamples{ 0 };
        for( int i = 0; i < audioDataLenInBytes / sizeof( float ); i += skipCnt )
        {
            pcmBuf[ totalSamples ] = AudioUtils::floatToPcm( audioDataFloat[ i ] );
            totalSamples++;
        }

        m_PlayerCacheBuf.samplesWereWritten( totalSamples );

        unlockPlayerCache();
/*
        if( LogEnabled( eLogAudioIo ) )
        {
            float cachedTime = toGuiGetAudioDelaySeconds( mediaModule );
            float totalCache = toGuiGetAudioCacheTotalSeconds( mediaModule );

            if(LogEnabled(eLogAudioIo)) LogModule( eLogAudioIo, LOG_VERBOSE, "AudioMgr::toGuiPlayerNlcAudio player-nlc samples %d cached sec %3.3f total cache %3.3f sec percent %d", 
                       totalSamples, cachedTime, totalCache, (int)((cachedTime / totalCache )*100) );
        }
        */
    }
    else
    {
        LogMsg( LOG_ERROR, "AudioMgr::toGuiPlayerNlcAudio unknown module %s ", DescribeMediaModule( mediaModule ) );
    }

    return audioDataLenInBytes;
}

//============================================================================
float AudioMgr::toGuiGetAudioDelaySeconds( EMediaModule mediaModule )
{
    if( VxIsAppShuttingDown() )
    {
        return 0;
    }

    if( eMediaModulePlayerNlc == mediaModule )
    {
        lockPlayerCache();
        int cachedPcmSamples = m_PlayerCacheBuf.getSampleCnt();
        unlockPlayerCache();

        return calculateMsOfSamples( cachedPcmSamples ) * 1000;
    }

    lockModuleMixerBuffer();
    int usedPcmSamples = getAudioMixerBuf( mediaModule ).getSampleCnt();
    unlockModuleMixerBuffer();

    return calculateMsOfSamples( usedPcmSamples ) * 1000;
}

//============================================================================
float AudioMgr::toGuiGetAudioCacheFreeSpace( EMediaModule mediaModule )
{
    if( VxIsAppShuttingDown() )
    {
        return 0;
    }

    if( eMediaModulePlayerNlc == mediaModule )
    {
        lockPlayerCache();
        int availPcmSamplesCache = m_PlayerCacheBuf.freeSpaceSampleCount();
        unlockPlayerCache();

        return availPcmSamplesCache * AUDIO_BYTES_PER_SAMPLE_KODI * AUDIO_KODI_TO_NLC_DNSAMPLE_RATIO;
    }

    lockModuleMixerBuffer();
    int availablePcmSampleSpace = getAudioMixerBuf( mediaModule ).freeSpaceSampleCount();
    unlockModuleMixerBuffer();

    return availablePcmSampleSpace;
}

//============================================================================
float AudioMgr::toGuiGetAudioCacheTotalSeconds( EMediaModule mediaModule )
{
    if( VxIsAppShuttingDown() )
    {
        return 0;
    }

    if( eMediaModulePlayerNlc == mediaModule )
    {
        lockPlayerCache();
        int maxPcmSamplesCache = m_PlayerCacheBuf.getMaxSamples();
        unlockPlayerCache();

        return calculateMsOfSamples( maxPcmSamplesCache ) * 1000;
    }

    lockModuleMixerBuffer();
    float mixerMaxSamples = getAudioMixerBuf( mediaModule ).getMaxSamples();
    unlockModuleMixerBuffer();

    return calculateMsOfSamples( mixerMaxSamples ) * 1000;
}

//============================================================================
int AudioMgr::toGuiModuleAudioFrame( EMediaModule mediaModule, int16_t* pu16PcmData, int pcmDataLenInBytes, bool isSilence )
{
    // assumes must be 80 ms of pcm mono
    vx_assert( pcmDataLenInBytes == AUDIO_BUF_SIZE)
    lockModuleMixerBuffer();

    AudioMixerBuf& mixerBuf = getAudioMixerBuf( mediaModule );
    int wroteSamples = mixerBuf.writeSamples( pu16PcmData, pcmDataLenInBytes / 2, isSilence );

    unlockModuleMixerBuffer();
    return wroteSamples * 2;
 }

//============================================================================
AudioMixerBuf& AudioMgr::getAudioMixerBuf( EMediaModule mediaModule )
{
    auto iter = m_AppModuleToSpeakerMap.find( mediaModule );
    if( iter != m_AppModuleToSpeakerMap.end() ) 
    {
        // found
        return iter->second;
    }

    // not found
    AudioMixerBuf mixBuf;
    mixBuf.setAudioIoMgr( this );
    mixBuf.setMediaModule( mediaModule );
    m_AppModuleToSpeakerMap.insert( std::make_pair(mediaModule, mixBuf ));

    auto newIter = m_AppModuleToSpeakerMap.find( mediaModule );

    return newIter->second;
}

//============================================================================
void AudioMgr::fromGuiAudioOutSpaceAvaiThreaded( int sampleCnt )
{
    m_MyApp.getEngine().getMediaProcessor().fromGuiAudioOutSpaceAvaiThreaded( sampleCnt );
}

//============================================================================
float AudioMgr::calculateMsOfSamples( int sampleCount )
{
    return ((float)sampleCount / (ECHO_SAMPLE_RATE / AUDIO_CHANNELS) / 1000.0f);
}
