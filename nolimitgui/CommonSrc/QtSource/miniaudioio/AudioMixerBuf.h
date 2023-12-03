#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AudioSampleBuf.h"

class MiniAudioMgr;

class AudioMixerBuf : public AudioSampleBuf
{
public:
    AudioMixerBuf() = default;
    AudioMixerBuf( const AudioMixerBuf& rhs ) = default;
    virtual ~AudioMixerBuf() = default;

    AudioMixerBuf& operator = ( const AudioMixerBuf& rhs ) = default;

    void						setAudioIoMgr( MiniAudioMgr* audioIoMgr )	{ m_AudioIoMgr = audioIoMgr; }

    void						setAppModule( EAppModule appModule )	    { m_AppModule = appModule; }
    EAppModule                  getAppModule( void )	                    { return m_AppModule; }
    
    int					        writeSamples( int16_t* samplesBuf, int sampleCnt, bool isSilent = false ) override;

    MiniAudioMgr*				m_AudioIoMgr{ nullptr };
    EAppModule                  m_AppModule{ eAppModuleInvalid };
    int64_t                     m_TimeNow{ 0 };
	int64_t                     m_LastMixerPcmTime{ 0 };
    int                         m_FuncCallCnt{ 0 };
};
