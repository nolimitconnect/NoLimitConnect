#pragma once
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
