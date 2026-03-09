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

    lockPlayerCache();
    if( !m_PlayerCacheQueue.empty() )
    {
        // move a frame of player-nlc audio from the player cache queue to the mixer buffer so it can be mixed with other sources and played out
        auto& frame = m_PlayerCacheQueue.front();  
        getAudioMixerBuf( eMediaModulePlayerNlc ).writeSamples( frame.data() ); 
        m_PlayerCacheQueue.pop_front();
    }

    unlockPlayerCache();

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

        bool printStats{ false };

        lockPlayerCache();
        if( pcmSampleCnt > m_PlayerCacheBuf.freeSpaceSampleCount() )
        {
            unlockPlayerCache();
            LogMsg( LOG_ERROR, "AudioMixerMgr::%s overrun m_PlayerCacheBuf", __func__  );
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
            // Emplace directly to avoid extra copies
            m_PlayerCacheQueue.emplace_back( m_PlayerCacheBuf.getSampleBuffer(), m_PlayerCacheBuf.getSampleBuffer() + AUDIO_SAMPLES_PER_FRAME );
            m_PlayerCacheBuf.samplesWereRead( AUDIO_SAMPLES_PER_FRAME );

            if( m_PlayerCacheQueue.size() > PLAYER_MAX_QUEUE_SIZE ) 
            {
                LogMsg( LOG_ERROR, "AudioMixerMgr::%s overrun m_PlayerCacheQueue", __func__  );
                printStats = true;                            

                m_PlayerCacheQueue.pop_front();
            }
        }

        unlockPlayerCache();

        if( printStats )
        {
            float cachedTime = toGuiGetAudioDelaySeconds( mediaModule );
            float totalCache = toGuiGetAudioCacheMaxSeconds( mediaModule );
            float cacheFreeBytes = toGuiGetAudioCacheFreeSpace( mediaModule );

            LogMsg( LOG_VERBOSE, "AudioMixerMgr::%s free space %d cached sec %3.3f total cache sec %3.3f percent %d", __func__,
                        cacheFreeBytes, cachedTime, totalCache, (int)((cachedTime / totalCache )*100) );
        }


        if( LogEnabled( eLogPlayerNlc ) )
        {
            static int lastTimeMs = 0;
            int timeNowMs = GetApplicationAliveMs();
            if( timeNowMs - lastTimeMs > 5000 )
            {
                lastTimeMs = timeNowMs;
                float cachedTime = toGuiGetAudioDelaySeconds( mediaModule );
                float totalCache = toGuiGetAudioCacheMaxSeconds( mediaModule );
                float cacheFreeBytes = toGuiGetAudioCacheFreeSpace( mediaModule );

                LogMsg( LOG_VERBOSE, "AudioMixerMgr::%s free space %d cached sec %3.3f total cache sec %3.3f percent %d", __func__,
                            cacheFreeBytes, cachedTime, totalCache, (int)((cachedTime / totalCache )*100) );
            }
        }
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
        cachedPcmSamples += m_PlayerCacheQueue.size() * AUDIO_SAMPLES_PER_FRAME;
        if( m_PlayerCacheQueue.size() == PLAYER_MAX_QUEUE_SIZE )
        {
            cachedPcmSamples -= m_PlayerCacheBuf.getSampleCnt();
        }
        else
        {
            cachedPcmSamples += m_PlayerCacheBuf.getSampleCnt();
        }
                
        unlockPlayerCache();

        return calculateMsOfSamples( cachedPcmSamples ) / 1000;
    }

    lockModuleMixerBuffer();
    cachedPcmSamples += getAudioMixerBuf( mediaModule ).getSampleCnt();
    unlockModuleMixerBuffer();

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
        availPcmSamplesCache += (PLAYER_MAX_QUEUE_SIZE - m_PlayerCacheQueue.size()) * AUDIO_SAMPLES_PER_FRAME;
        if( availPcmSamplesCache )
        {
            // player cannot handle 60ms jitter
            availPcmSamplesCache -= m_PlayerCacheBuf.getSampleCnt();
        }

        vx_assert( availPcmSamplesCache >= 0 );

        unlockPlayerCache();

        return availPcmSamplesCache * AUDIO_BYTES_PER_SAMPLE_KODI * AUDIO_KODI_TO_NLC_DNSAMPLE_RATIO;
    }    

    lockModuleMixerBuffer();
    availPcmSamplesCache += getAudioMixerBuf( mediaModule ).freeSpaceSampleCount();
    unlockModuleMixerBuffer();
    
    return availPcmSamplesCache * AUDIO_BYTES_PER_SAMPLE;
}

//============================================================================
float AudioMixerMgr::toGuiGetAudioCacheMaxSeconds( EMediaModule mediaModule )
{
    if( VxIsAppShuttingDown() )
    {
        return 0;
    }

    int maxPcmSamplesCache = 0;

    if( eMediaModulePlayerNlc == mediaModule )
    {
        maxPcmSamplesCache += (PLAYER_MAX_QUEUE_SIZE - 1) * AUDIO_SAMPLES_PER_FRAME;
    }
    else
    {
        lockModuleMixerBuffer();
        maxPcmSamplesCache += getAudioMixerBuf( mediaModule ).getMaxSamples();
        unlockModuleMixerBuffer();
    }

    return calculateMsOfSamples( maxPcmSamplesCache ) / 1000;
}

//============================================================================
int AudioMixerMgr::toGuiModuleAudioFrame( EMediaModule mediaModule, int16_t* pu16PcmData, int pcmDataLenInBytes )
{
    vx_assert( pcmDataLenInBytes == AUDIO_BUF_SIZE );
    lockModuleMixerBuffer();

    AudioMixerBuf& mixerBuf = getAudioMixerBuf( mediaModule );
    int wroteSamples = mixerBuf.writeSamples( pu16PcmData );

    unlockModuleMixerBuffer();
    vx_assert( wroteSamples == AUDIO_SAMPLES_PER_FRAME );
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
    return ( sampleCount * 1000.0f ) / ECHO_SAMPLE_RATE;
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
            m_PlayerCacheQueue.clear();

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