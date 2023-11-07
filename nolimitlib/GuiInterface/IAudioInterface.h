#pragma once
//============================================================================
// Copyright (C) 2019 Brett R. Jones
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

#include <GuiInterface/IDefs.h>
#include <GuiInterface/IAudioDefs.h>

#include <typeinfo>
#include <inttypes.h>

class VxGUID;

class IAudioCallbacks
{
public:
    /// Mute/Unmute microphone
    virtual void				fromGuiMuteMicrophone( bool muteMic ) = 0;
    /// Returns true if microphone is muted
    virtual bool				fromGuiIsMicrophoneMuted( void ) = 0;
    /// Mute/Unmute speaker
    virtual void				fromGuiMuteSpeaker( bool muteSpeaker ) = 0;
    /// Returns true if speaker is muted
    virtual bool				fromGuiIsSpeakerMuted( void ) = 0;
};

class IAudioRequests
{
public:
    // return true if any microphone device is available to be enabled
    virtual bool				toGuiIsMicrophoneDeviceAvailable( void ) = 0;

    // enable disable fromGuiMicrophoneData callback
    virtual void				toGuiWantMicrophoneRecording( EAppModule appModule, bool wantMicInput ) = 0;
    // enable disable microphone for specific user communicaion (usually push to talk)
    virtual void				toGuiWantUserVoiceMicrophone( EAppModule appModule, VxGUID& onlineId, bool wantMicInput ) = 0;

    // enable disable speaker sound out
    virtual void				toGuiWantSpeakerOutput( EAppModule appModule, bool wantSpeakerOutput ) = 0;
    // enable disable speaker for specific user communicaion (usually push to talk)
    virtual void				toGuiWantUserVoiceSpeaker( EAppModule appModule, VxGUID& onlineId, bool wantSpeakerOutput ) = 0;

    // add audio data to play.. assumes pcm mono 
    virtual int				    toGuiModuleAudioFrame( EAppModule appModule, int16_t * pu16PcmData, int pcmDataLenInBytes, bool isSilence ) = 0;

#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
    // add audio data to play.. assumes float 2 channel 48000 Hz
    virtual int				    toGuiPlayerNlcAudio( EAppModule appModule, float * audioSamples48000, int dataLenInBytes ) = 0;

    virtual float               toGuiGetAudioDelaySeconds( EAppModule appModule ) = 0;

    virtual float               toGuiGetAudioCacheFreeSpace( EAppModule appModule ) = 0;

    virtual float               toGuiGetAudioCacheTotalSeconds( EAppModule appModule ) = 0;

#endif // ENABLE_KODI
};
