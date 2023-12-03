#pragma once
//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AudioSampleBuf.h"
#include <GuiInterface/IAudioDefs.h>

#include <CoreLib/TimeIntervalEstimator.h>

#include "MiniAudioOutDevice.h"

#include <QAudioFormat>
#include <QMutex>

class AppCommon;
class MiniAudioMgr;
class QTimer;

class MiniAudioOut : public MiniAudioOutDevice
{
    Q_OBJECT

public:
    explicit MiniAudioOut( MiniAudioMgr& mgr, QObject *parent = 0 );
    ~MiniAudioOut() = default;

    bool                        initAudioOut( QAudioFormat& audioFormat, int deviceIndex );
    bool                        soundOutDeviceChanged( int deviceIndex );

    void						audioOutShutdown( void ) { stopAudioOut(); }

    void                        wantSpeakerOutput( bool enableOutput );
    bool                        isSpeakerOutputWanted( void )                   { return m_SpeakerOutputEnabled; }

    void                        setUpsampleMultiplier( int upSampleMult )       { m_UpsampleMutiplier = upSampleMult; }
    int                         getUpsampleMultiplier( void )                   { return m_UpsampleMutiplier; }

    void                        setSpeakerVolume( int volume0to100 );

    AudioSampleBuf&             getSpeakerEchoSamples( int64_t& tailOfSpeakerSamplestimeMs ) { tailOfSpeakerSamplestimeMs = m_EndOfEchoBufferTimestamp; return m_EchoFarBuffer; }

    void						echoCancelSyncStateThreaded( bool inSync );

    virtual int				    callbackAudioRead( int16_t* pcmData, int maxlen ) override;

protected:
    void                        startAudioOut( void );
    void                        stopAudioOut( void );

private:
    AppCommon&                  m_MyApp;

    bool                        m_initialized{ false };
    bool                        m_SpeakerOutputEnabled{ false };

    QAudioFormat                m_AudioFormat;
    
    qint64                      m_ProccessedMs = 0;

    int                         m_UpsampleMutiplier{ 0 };

    int16_t                     m_PrevLastsample{ 0 };
    int                         m_PrevLerpedSamplesCnt{ 0 };

    TimeIntervalEstimator       m_SpeakerReadTimeEstimator;

    int64_t                     m_EndOfEchoBufferTimestamp{ 0 };
    AudioSampleBuf              m_EchoFarBuffer;

    bool                        m_AudioOutDeviceIsStarted{ false };
};
