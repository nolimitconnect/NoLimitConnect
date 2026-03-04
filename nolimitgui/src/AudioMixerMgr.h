#pragma once
//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "miniaudio/AudioMixerBuf.h"
#include "miniaudio/AudioSampleBuf.h"

#include <GuiInterface/IAudioInterface.h>
#include <GuiInterface/IAudioDefs.h>

#include <vector>
#include <mutex>
#include <map>


class AudioMixerMgr : public IAudioRequests
{
public:
    //=== IAudioRequests begin ===//
    // return true if any microphone device is available to be enabled
    bool				        toGuiIsMicrophoneDeviceAvailable( void ) override{ return false; }; // this is handled by AudioMgr
    // enable disable microphone data callback
    void				        toGuiWantMicrophoneRecording( EMediaModule mediaModule, bool wantMicInput ) override{}; // this is handled by AudioMgr
    // enable disable sound out
    void				        toGuiWantSpeakerOutput( EMediaModule mediaModule, bool wantSpeakerOutput ) override{};  // this is handled by AudioMgr

    // add audio data to play.. assumes pcm mono 16000 Hz of mixer buffer length
    int				            toGuiModuleAudioFrame( EMediaModule mediaModule, int16_t* pu16PcmData, int pcmDataLenInBytes ) override;
    // add audio data to play.. assumes 20ms of float 2 channel 48000 Hz
    int				            toGuiPlayerNlcAudio( EMediaModule mediaModule, float* audioDataFloat, int audioDataLenInBytes ) override;

    float                       toGuiGetAudioDelaySeconds( EMediaModule mediaModule ) override;

    float                       toGuiGetAudioCacheFreeSpace( EMediaModule mediaModule ) override;

    float                       toGuiGetAudioCacheTotalSeconds( EMediaModule mediaModule ) override;
    //=== IAudioRequests end ===//
    
    virtual void                writeMixerAudioToSpeakerHardware( int16_t* pcmData, int sampleCount ) = 0;

    virtual void                callbackAudioOut60msSpaceAvail( int freeSpaceLenBytes );
    virtual void				fromGuiAudioOutSpaceAvaiThreaded( int freeSpaceLenBytes );

    float                       calculateMsOfSamples( int sampleCount );

	// player-nlc
  	virtual void				setPlayerNlcActive( bool isActive );
    bool						getPlayerNlcActive( void ) { return m_PlayerNlcActive; }

    void                        lockPlayerCache( void )                             { m_PlayerCacheMutex.lock(); }
    void                        unlockPlayerCache( void )                           { m_PlayerCacheMutex.unlock(); }

	// mixer
    void                        lockModuleMixerBuffer( void )                       { m_ModuleMixerMutex.lock(); }
    void                        unlockModuleMixerBuffer( void )                     { m_ModuleMixerMutex.unlock(); }
    AudioMixerBuf&              getAudioMixerBuf( EMediaModule mediaModule );

    //=== variables ===//

    // player-nlc
    bool                        m_PlayerNlcActive{ false };
    AudioSampleBuf              m_PlayerCacheBuf;
    std::mutex                  m_PlayerCacheMutex;

    // mixer
    std::map<EMediaModule, AudioMixerBuf> m_AppModuleToSpeakerMap;
    std::mutex                  m_ModuleMixerMutex;
};