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

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QElapsedTimer>
#include <QWidget>

#include "AudioEchoCancel.h"
#include "AudioFrameBuf.h"
#include "AudioMasterClock.h"
#include "AudioMixer.h"
#include "AudioSampleBuf.h"
#include "AudioTestGenerator.h"
#include "MiniAudioOut.h"
#include "MiniAudioIn.h"
#include "MiniAudioDevices.h"

#include <GuiInterface/IAudioInterface.h>

#include <CoreLib/VxGUID.h>

#include <atomic>
#include <utility>
#include <vector>

class AppCommon;

// Qt is unuseable for audio echo cancelation because both input and output are done in the ui thread which affects timing
// Use minaudio from https://miniaud.io/ as alternative

class MiniAudioMgr : public QWidget, public MiniAudioDevices, public IAudioRequests
{
    Q_OBJECT
public:
    explicit MiniAudioMgr( AppCommon& app, IAudioCallbacks& audioCallbacks, QWidget* parent );

    ~MiniAudioMgr() override = default;

    void                        audioIoSystemStartup();
    void                        audioIoSystemShutdown();

    AppCommon&                  getMyApp( void )                            { return m_MyApp; }

    AudioEchoCancel&            getAudioEchoCancel( void )                  { return m_AudioEchoCancel; }

    void                        setMicrophoneVolume( float volume )         { m_MicrophoneVolume = volume; }

    int                         getMicrophonePeakValue( void )              { return m_PeakAudioInAmplitude; } // get peak value 0 - 100
    int                         getSpeakerPeakValue( void )                 { return m_PeakAudioOutAmplitude; } // get peak value 0 - 100

    bool                        isAudioInitialized( void )                  { return m_AudioIoInitialized;  }
    IAudioCallbacks&            getAudioCallbacks( void )                   { return m_AudioCallbacks; }
    QAudioFormat&               getAudioOutFormat( void )                   { return m_AudioOutFormat; }
    QAudioFormat&               getAudioInFormat( void )                    { return m_AudioInFormat; }

    AudioMixer&                 getAudioOutMixer( void )                    { return m_AudioOutMixer; }
    MiniAudioIn&                getAudioInIo( void )                        { return m_AudioInIo; }
    MiniAudioOut&               getAudioOutIo( void )                       { return m_AudioOutIo; }

    AudioMasterClock&           getAudioMasterClock( void )                 { return m_AudioMasterClock; }

    float                       calculateMsOfSamples( int sampleCount );

    bool                        isSilentSamples( int16_t* pcmData, int sampleCnt );

    void                        setPlayerNlcActive( bool isActive );
    bool                        getPlayerNlcActive( void )                  { return m_PlayerNlcActive; }

    // volume is from 0.0 to 1.0
    void						setSpeakerVolume( float volume0to1 );

    void                        microphoneDeviceEnabled( bool isEnabled );

    bool                        isMicrophoneEnabled( void )                 { return m_WantMicrophone; }
    bool                        isMicrophoneInputWanted( void )             { return m_WantMicrophone; }

    void                        setMuteMicrophone( bool mute );
    bool                        getIsMicrophoneMuted( void )                { return m_MicrophoneMuted; }

    void                        speakerDeviceEnabled( bool isEnabled );

    bool                        isSpeakerEnabled( void )                    { return m_WantSpeakerOutput; }
    bool                        isSpeakerOutputWanted( void )               { return m_WantSpeakerOutput; }

    void                        setMuteSpeaker( bool mute );
    bool                        getIsSpeakerMuted()                         { return m_SpeakersMuted; }

    /// start debug enables
    void                        setAudioTimingDebugEnable( bool enable )    { m_AudioTimingEnabled = enable; }
    bool                        getAudioTimingDebugEnable( void )           { return m_AudioTimingEnabled; }

    void                        setFrameTimingEnable( bool enable )         { m_FrameTimingEnabled = enable; }
    bool                        getFrameTimingEnable( void );

    void                        setFrameIndexDebugEnable( bool enable )     { m_FrameIndexDebugEnabled = enable; }
    bool                        getFrameIndexDebugEnable( void )            { return m_FrameIndexDebugEnabled; }

    void                        setBitrateDebugEnable( bool enable )        { m_BitrateDebugEnabled = enable; }
    bool                        getBitrateDebugEnable( void )               { return m_BitrateDebugEnabled; }

    void                        setSampleCntDebugEnable( bool enable )      { m_SampleCntDebugEnabled = enable; }
    bool                        getSampleCntDebugEnable( void )             { return m_SampleCntDebugEnabled; }

    void                        setPeakAmplitudeDebugEnable( bool enable );
    bool                        getPeakAmplitudeDebugEnable( void )             { return m_PeakAmplitudeDebug; }

    void                        setEnableSpeakerTestTone( int enableTestTone )  { m_SpeakerTestToneEnable = enableTestTone; }
    bool                        getEnableSpeakerTestTone( void )                { return m_SpeakerTestToneEnable; }
    /// end debug enables

    bool                        getAudioLoopbackEnable( void )                  { return m_DirectLoopbackEnable; }

    void                        setDirectLoopbackEnable( int enable )           { m_DirectLoopbackEnable = enable; }
    bool                        getDirectLoopbackEnable( void )                 { return m_DirectLoopbackEnable; }

    void                        setEchoCancelEnable( bool enable );
    bool                        getEchoCancelEnable( void )                     { return m_EchoCancelEnabled; }

    void                        setEchoCancelerNeedsReset( bool needReset );

    void                        echoCancelSyncStateThreaded( bool inSync );
    bool                        isEchoCancelInSync( void );

    //=== IAudioRequests ===//
    // return true if any microphone device is available to be enabled
    virtual bool				toGuiIsMicrophoneDeviceAvailable( void ) override;
    // enable disable microphone data callback
    virtual void				toGuiWantMicrophoneRecording( EAppModule appModule, bool wantMicInput ) override;
    // enable disable sound out
    virtual void				toGuiWantSpeakerOutput( EAppModule appModule, bool wantSpeakerOutput ) override;
    // add audio data to play.. assumes pcm mono 16000 Hz of mixer buffer length
    virtual int				    toGuiPlayAudioFrame( EAppModule appModule, int16_t* pu16PcmData, int pcmDataLenInBytes, bool isSilence ) override;
    // enable disable microphone for specific user communicaion (usually push to talk)
    virtual void				toGuiWantUserVoiceMicrophone( EAppModule appModule, VxGUID& onlineId, bool wantMicInput ) override;
    // enable disable speaker for specific user communicaion (usually push to talk)
    virtual void				toGuiWantUserVoiceSpeaker( EAppModule appModule, VxGUID& onlineId, bool wantSpeakerOutput ) override;
#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
    virtual int				    toGuiPlayAudio( EAppModule appModule, float* audioDataFloat, int audioDataLenInBytes ) override;

    virtual float               toGuiGetAudioDelaySeconds( EAppModule appModule ) override;

    virtual float               toGuiGetAudioCacheFreeSpace( EAppModule appModule ) override;

    virtual float               toGuiGetAudioCacheTotalSeconds( EAppModule appModule ) override;

#endif // defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)

    void						fromGuiEchoCanceledSamplesThreaded( int16_t* pcmData, int sampleCnts, bool isSilence );
    virtual void				fromGuiAudioOutSpaceAvaiThreaded( int freeSpaceLen );

    int                         getAudioInPeakAmplitude( void );
    int                         getAudioOutPeakAmplitude( void );

    bool                        setSoundInDeviceIndex( int sndInDeviceIndex );
    bool                        getSoundInDeviceIndex( int& retDeviceIndex );

    bool                        setSoundOutDeviceIndex( int sndOutDeviceIndex );
    bool                        getSoundOutDeviceIndex( int& retDeviceIndex );

    bool                        soundInDeviceChanged( int deviceIndex );
    bool                        soundOutDeviceChanged( int deviceIndex );

    bool                        runAudioDelayTest( void );

    void                        setEchoDelayMsParam( int delayMs )      { m_AudioEchoCancel.setEchoDelayMsParam( delayMs ); }
    bool                        getIsEchoCancelInSync( void )           { return m_IsEchoCancelInSync; }

    void                        readTestToneSamples( int16_t* pcmData, int sampleCnt );

    void                        directLoopbackWrite( int16_t* pcmData, int sampleCnt ) {};
    void                        directLoopbackRead( int16_t* pcmData, int sampleCnt ){};

    void                        setInSampleRate( int actualRate )       { m_AudioInFormat.setSampleRate( actualRate ); }
    void                        setOutSampleRate( int actualRate )      { m_AudioOutFormat.setSampleRate( actualRate ); }

    void                        callbackAudioDeviceWrite( int16_t* pcmData, int sampleCnt );
    void                        callbackAudioDeviceRead( int16_t* pcmData, int sampleCnt );

    void                        processAudioThreaded( void );

    void                        addReadSpeakerCount( int readCnt )              { m_SpeakerReadSampleCnt += readCnt; }
    void                        subtractReadSpeakerCount( int processedCnt ) 
                                    { int val = m_SpeakerReadSampleCnt; if( val >= processedCnt ) m_SpeakerReadSampleCnt -= processedCnt; else m_SpeakerReadSampleCnt = 0; }
    int                         getReadSpeakerCount( void )                     { return m_SpeakerReadSampleCnt; }

    void                        lockMixer( void )                               { m_AudioOutMixer.lockMixer(); }
    void                        unlockMixer( void )                             { m_AudioOutMixer.unlockMixer(); }

    void                        addEchoCanceledSamples( int16_t* pcmData, int sampleCnt );

    void                        clearAllBuffers();

signals:
    void                        signalNeedMoreAudioData( int requiredLen );
    void                        signalAudioTestState( EAudioTestState audioTestState );
    void                        signalTestedSoundDelay( int echoDelayMs );
    void                        signalAudioTestMsg( QString audioTestMsg );

protected slots:
    void                        slotAudioTestTimer( void );

protected:
    void                        aboutToDestroy();
    // update speakers to current mode and output
    void                        enableSpeakers( bool enable );
    // update microphone output
    void                        enableMicrophone( bool enable );

    void                        setAudioTestState( EAudioTestState audioTestState );
    void                        setAudioTestSentTime( int64_t sentTime )        { m_AudioTestSentTimeMs = sentTime; }
    int64_t                     getAudioTestSentTime( void )                    { return m_AudioTestSentTimeMs; }
    void                        audioTestDetectTestSound( int16_t* sampleInData, int inSampleCnt, int64_t micWriteTime );
    int64_t                     getAudioTestDetectTime( int& peakValue );

    bool                        handleAudioTestResult( int64_t soundOutTimeMs, int64_t soundDetectTimeMs, int peakVal0to100 );

    void                        resetMicrophoneBuffers( void );
    void                        resetSpeakerBuffers( void );

    void                        processMixerFrames( void );

    AppCommon&                  m_MyApp;

    IAudioCallbacks&            m_AudioCallbacks;

    bool                        m_AudioIoInitialized{ false };

    bool                        m_MicrophoneMuted{ false };
    bool                        m_WantMicrophone{ false };
    int                         m_PeakAudioInAmplitude{ 0 };
    std::vector<std::pair<EAppModule, VxGUID>> m_WantMicList;
    VxMutex                     m_WantMicMutex;

    bool                        m_SpeakersMuted{ false };
    bool                        m_WantSpeakerOutput{ false };
    int                         m_PeakAudioOutAmplitude{ 0 };   
    std::vector<std::pair<EAppModule, VxGUID>> m_WantSpeakerList;
    VxMutex                     m_WantSpeakerMutex;

    bool                        m_EchoCancelEnabled{ false };

    bool                        m_DirectLoopbackEnable{ false };

    bool                        m_SpeakerTestToneEnable{ false };

    QAudioFormat                m_AudioOutFormat;
    MiniAudioOut                m_AudioOutIo;

    QAudioFormat                m_AudioInFormat;
    MiniAudioIn                 m_AudioInIo;

    bool                        m_IsOutPaused{ false };
    bool                        m_IsTestMode{ false };
    int                         m_CacheAuidioLen = 0;
    QMutex                      m_AudioQueueMutex;
    int                         m_OutWriteCount = 0;

    float                       m_MicrophoneVolume{ 100.0f };

    int16_t                     m_MyLastAudioOutSample[ eMaxAppModule ];

    AudioEchoCancel             m_AudioEchoCancel;

    QTimer*                     m_AudioTestTimer{ nullptr };
    EAudioTestState             m_AudioTestState{ eAudioTestStateNone };
    int64_t                     m_AudioTestSentTimeMs{ 0 };
    std::vector<std::pair<int64_t, int>> m_DelayTestDetectList; // pairs of time and peak value detected

    bool                        m_AudioTestMicEnable{ false };
    bool                        m_AudioTestSpeakerEnable{ false };
    int                         m_EchoDelayTestMaxInterations{ 3 };
    int                         m_EchoDelayCurrentInteration{ 0 };
    std::vector<int>            m_EchoDelayResultList;

    AudioMasterClock            m_AudioMasterClock;

    // AudioLoopback               m_AudioLoopback;
    bool                        m_AudioLoopbackEnabled{ false };
    bool                        m_AudioTimingEnabled{ false };
    bool                        m_FrameTimingEnabled{ false };
    bool                        m_FrameIndexDebugEnabled{ false };
    bool                        m_BitrateDebugEnabled{ false };
    bool                        m_PeakAmplitudeDebug{ false };
    bool                        m_IsEchoCancelInSync{ false };
    bool                        m_SampleCntDebugEnabled{ false };

    bool                        m_UseFixedAudioOutBufferSize{ false };

    MiniAudioDevices            m_MiniAudioDevices;
    AudioTestGenerator          m_ToneGenerator;
    int16_t                     m_SilenceBuf[ AUDIO_SAMPLES_PER_FRAME ];

    AudioSampleBuf              m_AudioWriteBuf;
    VxMutex                     m_AudioWriteMutex;

    AudioSampleBuf              m_AudioReadBuf;
    VxMutex                     m_AudioReadMutex;

    AudioMixer                  m_AudioOutMixer;
    AudioFrameBuf               m_AudioMixerBuf;
    VxMutex                     m_AudioMixerMutex;

    AudioSampleBuf              m_EchoCanceledBuf;
    VxMutex                     m_EchoCanceledBufMutex;

    VxThread					m_ProcessAudioThread;
    VxSemaphore					m_ProcessAudioSemaphore;

    std::atomic<int>            m_SpeakerReadSampleCnt{ 0 };

    bool                        m_PlayerNlcActive{ false };

#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
    AudioSampleBuf              m_PlayerNlcCache;
    VxMutex                     m_PlayerNlcMutex;
#endif // defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
    bool                        m_LastSpeakerRequestWasFullfilled{false};
};
