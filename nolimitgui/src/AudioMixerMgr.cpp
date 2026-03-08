//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AudioMixerMgr.h"

#include "AudioUtils.h"

#include <P2PEngine/P2PEngine.h>
#include <MediaProcessor/MediaProcessor.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

//============================================================================
void AudioMixerMgr::callbackAudioOut60msSpaceAvail(int freeSpaceLenBytes) 
{
    std::vector<int32_t> accumulator(AUDIO_SAMPLES_PER_FRAME, 0);
    int16_t tempFrame[AUDIO_SAMPLES_PER_FRAME];
    int activeSources = 0;

    {
        std::lock_guard<std::mutex> lock(m_ModuleMixerMutex);
        for (auto& [module, buffer] : m_AppModuleToSpeakerMap) 
        {
            if (buffer.getFrame(tempFrame)) 
            {
                activeSources++;
                for (int i = 0; i < AUDIO_SAMPLES_PER_FRAME; ++i) 
                {
                    accumulator[i] += tempFrame[i];
                }
            }
        }
    }

    if (activeSources > 0) 
    {
        int16_t finalMix[AUDIO_SAMPLES_PER_FRAME];
        for (int i = 0; i < AUDIO_SAMPLES_PER_FRAME; ++i) 
        {
            // Simple Soft-Clipping / Limiting logic
            // If you have many sources, you can scale by a factor 
            // e.g., result = accumulator[i] * 0.8;
            int32_t sample = accumulator[i];
            
            if (sample > 32767) sample = 32767;
            if (sample < -32768) sample = -32768;
            
            finalMix[i] = static_cast<int16_t>(sample);
        }

        writeMixerAudioToSpeakerHardware( finalMix, AUDIO_SAMPLES_PER_FRAME );
    }

    // After writing to speaker hardware, we can call fromGuiAudioOutSpaceAvaiThreaded to notify any clients that are waiting for space to write to mixer
    fromGuiAudioOutSpaceAvaiThreaded( freeSpaceLenBytes );
}


//============================================================================
int AudioMixerMgr::toGuiPlayerNlcAudio( EMediaModule mediaModule, float* audioDataFloat, int audioDataLenInBytes )
{
    // this assumes 20ms of stereo 48000 hz at a time from kodi based player
    // unfortunately there is no good way to down sample stereo to mono so we have to pick a channel 
    // this is because if the channels are 180 degrees out of phase they cancel each other out

    // TODO rework audio out to handle 48000 hz stereo for higher quality playback

    if( VxIsAppShuttingDown() || !audioDataLenInBytes || !audioDataFloat )
    {
        if(LogEnabled(eLogAudioIo)) LogModule( eLogAudioIo, LOG_VERBOSE, "AudioMixerMgr::toGuiPlayerNlcAudio ignored len %d", audioDataLenInBytes );
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
            LogMsg( LOG_ERROR, "AudioMixerMgr::toGuiPlayerNlcAudio overrun PlayerNlc" );
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
        if( m_PlayerCacheBuf.getSampleCnt() >= AUDIO_SAMPLES_PER_FRAME )
        {
            getAudioMixerBuf( mediaModule ).writeSamples( m_PlayerCacheBuf.getSampleBuffer() );
            m_PlayerCacheBuf.samplesWereRead( AUDIO_SAMPLES_PER_FRAME );
        }

        unlockPlayerCache();
/*
        if( LogEnabled( eLogAudioIo ) )
        {
            float cachedTime = toGuiGetAudioDelaySeconds( mediaModule );
            float totalCache = toGuiGetAudioCacheTotalSeconds( mediaModule );

            if(LogEnabled(eLogAudioIo)) LogModule( eLogAudioIo, LOG_VERBOSE, "AudioMixerMgr::toGuiPlayerNlcAudio player-nlc samples %d cached sec %3.3f total cache %3.3f sec percent %d", 
                       totalSamples, cachedTime, totalCache, (int)((cachedTime / totalCache )*100) );
        }
        */
    }
    else
    {
        LogMsg( LOG_ERROR, "AudioMixerMgr::toGuiPlayerNlcAudio unknown module %s ", DescribeMediaModule( mediaModule ) );
    }

    return audioDataLenInBytes;
}

//============================================================================
float AudioMixerMgr::toGuiGetAudioDelaySeconds( EMediaModule mediaModule )
{
    if( VxIsAppShuttingDown() )
    {
        return 0;
    }

    int cachedPcmSamples = 0;
    if( eMediaModulePlayerNlc == mediaModule )
    {
        lockPlayerCache();
        cachedPcmSamples += m_PlayerCacheBuf.getSampleCnt();
        unlockPlayerCache();
    }

    lockModuleMixerBuffer();
    cachedPcmSamples += getAudioMixerBuf( mediaModule ).getSampleCnt();
    unlockModuleMixerBuffer();

    cachedPcmSamples += getSpeakerHardwareBufferedSampleCnt();

    double delayMs = calculateMsOfSamples( cachedPcmSamples );

    return delayMs / 1000;
}

//============================================================================
float AudioMixerMgr::toGuiGetAudioCacheFreeSpace( EMediaModule mediaModule )
{
    if( VxIsAppShuttingDown() )
    {
        return 0;
    }

    int availPcmSamplesCache = 0;
    if( eMediaModulePlayerNlc == mediaModule )
    {
        lockPlayerCache();
        availPcmSamplesCache += m_PlayerCacheBuf.freeSpaceSampleCount();
        unlockPlayerCache();
    }    

    lockModuleMixerBuffer();
    availPcmSamplesCache += getAudioMixerBuf( mediaModule ).freeSpaceSampleCount();
    unlockModuleMixerBuffer();

    availPcmSamplesCache += getSpeakerHardwareFreeSpaceSampleCnt();

    if( eMediaModulePlayerNlc == mediaModule )
    {
        return availPcmSamplesCache * AUDIO_BYTES_PER_SAMPLE_KODI * AUDIO_KODI_TO_NLC_DNSAMPLE_RATIO;
    } 

    return availPcmSamplesCache;
}

//============================================================================
float AudioMixerMgr::toGuiGetAudioCacheTotalSeconds( EMediaModule mediaModule )
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
int AudioMixerMgr::toGuiModuleAudioFrame( EMediaModule mediaModule, int16_t* pu16PcmData, int pcmDataLenInBytes )
{
    // assumes must be 80 ms of pcm mono
    vx_assert( pcmDataLenInBytes == AUDIO_BUF_SIZE)
    lockModuleMixerBuffer();

    AudioMixerBuf& mixerBuf = getAudioMixerBuf( mediaModule );
    int wroteSamples = mixerBuf.writeSamples( pu16PcmData );

    unlockModuleMixerBuffer();
    return wroteSamples * AUDIO_BYTES_PER_SAMPLE;
 }

//============================================================================
AudioMixerBuf& AudioMixerMgr::getAudioMixerBuf( EMediaModule mediaModule )
{
    auto iter = m_AppModuleToSpeakerMap.find( mediaModule );
    if( iter != m_AppModuleToSpeakerMap.end() ) 
    {
        // found
        return iter->second;
    }

    // not found
    // try_emplace returns a pair: {iterator, inserted}
    // If key exists, it returns the existing one; if not, it creates it.
    auto [iter2, inserted] = m_AppModuleToSpeakerMap.try_emplace( mediaModule );
    
    return iter2->second;
}

//============================================================================
void AudioMixerMgr::fromGuiAudioOutSpaceAvaiThreaded( int sampleCnt )
{
    GetPtoPEngine().getMediaProcessor().fromGuiAudioOutSpaceAvaiThreaded( sampleCnt );
}

//============================================================================
float AudioMixerMgr::calculateMsOfSamples( int sampleCount )
{
    return (float) (sampleCount * 1000 ) / ECHO_SAMPLE_RATE;
}

//============================================================================
void AudioMixerMgr::setPlayerNlcActive( bool isActive )
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
        else
        {
            removeAudioMixerBuf( eMediaModulePlayerNlc );
        }
        
        toGuiWantSpeakerOutput( eMediaModulePlayerNlc, m_PlayerNlcActive );
    }
}

//============================================================================
void AudioMixerMgr::removeAudioMixerBuf( EMediaModule mediaModule )
{
    std::lock_guard<std::mutex> lock(m_ModuleMixerMutex);
    m_AppModuleToSpeakerMap.erase(mediaModule);
}