#pragma once
//============================================================================
// Copyright (C) 2018 Brett R. Jones
// Issued to MIT style license by Brett R. Jones in 2017
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

#include <GuiInterface/IAudioDefs.h>
#include "AudioBitrate.h"

#include <CoreLib/TimeIntervalEstimator.h>

#include "MiniAudioInDevice.h"

#include <QAudioFormat>
#include <QMutex>

class AppCommon;
class MiniAudioMgr;
class P2PEngine;

class MiniAudioIn : public MiniAudioInDevice
{
    Q_OBJECT
public:
    explicit MiniAudioIn( MiniAudioMgr& mgr, QObject *parent = 0 );
    ~MiniAudioIn() = default;

    bool                        initAudioIn( QAudioFormat& audioFormat, int deviceIndex );
    bool                        soundInDeviceChanged( int deviceIndex );

    void						audioInShutdown( void )             { stopAudioIn(); }

    void                        startAudioIn( void );
    void                        stopAudioIn( void );

    void                        wantMicrophoneInput( bool enableInput );
    bool                        isMicrophoneInputWanted( void )     { return m_MicInputEnabled; }

    void                        setMicrophoneVolume( float volume ) { m_MicrophoneVolume = volume; }
 
    void                        setMicrophoneVolume( int volume0to100 );

    char *                      getMicSilence()                     { return m_MicSilence; }

    void                        setDivideSamplesCount( int cnt )    { m_DivideCnt = cnt; }
    int                         getDivideSamplesCount( void )       { return m_DivideCnt; }

    int                         getMicWriteDurationUs( void )       { return m_MicWriteDurationUs; }

    void						echoCancelSyncStateThreaded( bool inSync );

protected:

    virtual int				    callbackAudioWrite( int16_t* pcmData, int lenBytes ) override;

    AppCommon&                  m_MyApp;

    bool                        m_initialized{ false };

    bool                        m_WantAudioIn{ false };
    bool                        m_AudioInDeviceIsStarted{ false };
    QAudioFormat                m_AudioFormat;

    char                        m_MicSilence[ AUDIO_BUF_SIZE ];
    int                         m_DivideCnt{ 1 };
    float                       m_MicrophoneVolume{ 100.0f };

    TimeIntervalEstimator       m_MicWriteTimeEstimator;
    int                         m_MicWriteDurationUs{ 0 };

    AudioBitrate                m_MicInBitrate;
    AudioBitrate                m_MicOutBitrate;
    bool                        m_EchoCancelInSync{ false };
    bool                        m_MicInputEnabled{ false }; 
};
