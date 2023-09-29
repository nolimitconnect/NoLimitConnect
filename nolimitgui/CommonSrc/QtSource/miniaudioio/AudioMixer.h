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
#pragma once

#include "AudioMixerInterface.h"
#include "AudioMixerFrame.h"

#include "AudioBitrate.h"
#include "AudioSampleBuf.h"
#include "AudioSpeakerBuf.h"

#include <QMutex>
#include <QWidget>
#include <QElapsedTimer>
#include <QAudioFormat>

#include <CoreLib/VxMutex.h>
#include <CoreLib/VxThread.h>
#include <CoreLib/VxSemaphore.h>

class AppCommon;
class AudioIoMgr;
class AudioSampleBuf;
class IAudioCallbacks;
class MiniAudioMgr;

class AudioMixer : public QWidget, public AudioMixerInterface
{
    Q_OBJECT
public:
    explicit AudioMixer( MiniAudioMgr& audioIoMgr, IAudioCallbacks& audioCallbacks, QWidget* parent );

    void                        shutdownAudioMixer( void );
    void                        resetMixer( void );

    void                        lockMixer( void )                       { m_MixerMutex.lock(); }
    void                        unlockMixer( void )                     { m_MixerMutex.unlock(); }

    IAudioCallbacks&            getAudioCallbacks()                     { return m_AudioCallbacks; }
    void                        lockAudioCallbacks()                    { m_AudioCallbackMutex.lock(); }
    void                        unlockAudioCallbacks()                  { m_AudioCallbackMutex.unlock(); }

    int                         getMixerSamplesPerFrame( void )         { return AUDIO_SAMPLES_PER_FRAME; }

    QAudioFormat&               getMixerFormat( void )                  { return m_MixerFormat; }

    // assumes 80 ms of pcm 8000hz mono audio
    virtual int				    toGuiAudioFrameThreaded( EAppModule appModule, int16_t* pcmData, bool isSilenceIn ) override;

    int                         incrementMixerWriteIndex( void );
    AudioMixerFrame&            getAudioWriteFrame( void )              { return m_MixerFrames[ m_MixerWriteIdx ]; };

    int                         incrementMixerReadIndex( void );
    AudioMixerFrame&            getAudioReadFrame( void )               { return m_MixerFrames[ m_MixerReadIdx ]; };

protected:
    //=== vars ===//
    MiniAudioMgr&               m_AudioIoMgr;
    AppCommon&                  m_MyApp;
    IAudioCallbacks&            m_AudioCallbacks;
    QMutex                      m_AudioCallbackMutex;

    QMutex                      m_MixerMutex;

    int16_t						m_MixerBuf[ AUDIO_SAMPLES_PER_FRAME ];
    int16_t						m_QuietEchoBuf[ AUDIO_SAMPLES_PER_FRAME * 8 ];
    bool                        m_WasReset{ true };

    int                         m_PrevLerpedSamplesCnt{ 0 };
    int16_t                     m_PrevLerpedSampleValue{ 0 };

    AudioMixerFrame             m_MixerFrames[ MAX_GUI_MIXER_FRAMES ];
    int                         m_MixerWriteIdx{ 0 };
    int                         m_MixerReadIdx{ 0 };

    QElapsedTimer               m_ElapsedTimer;
    QAudioFormat                m_MixerFormat;
 };
    
