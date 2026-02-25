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


#include "AudioFrameBuffer.h"
#include <libwebrtc-aec/apm/WebRtcAec.h>

#include "miniaudio/AudioMixerBuf.h"

#include "miniaudio/MiniAudioDevices.h"
#include "miniaudio/MiniAudioIn.h"
#include "miniaudio/MiniAudioOut.h"

#include "TestFileWav.h"

#include "GuiAudioLevelCallback.h"
#include "ToGuiHardwareControlInterface.h"

#include <GuiInterface/IAudioInterface.h>

#include <CoreLib/VxMutex.h>

#include <vector>
#include <mutex>

#include <QObject>

class AppCommon;
class GuiAudioLevelCallback;
class GuiEchoCancelEnableCallback;

class AudioMgr : public QObject, public MiniAudioDevices, public IAudioRequests, public ToGuiHardwareControlInterface
{
	Q_OBJECT
public:
    const int FRAME_SIZE_10MS = 160; // PCM Sample Count for 10ms at 16kHz

    AudioMgr( AppCommon& app );
    ~AudioMgr() = default;

    void                        audioIoSystemStartup( void );
    void                        audioIoSystemShutdown( void );

    bool                        isAudioInitialized( void )                  { return m_AudioIoInitialized;  }

    AudioFrameBuffer&           getAudioInBuffer( void )                    { return m_AudioInFrameBuffer; }
    AudioFrameBuffer&           getAudioOutBuffer( void )                   { return m_AudioEchoCanceledFrameBuffer; }

    MiniAudioIn&                getAudioInIo( void )                        { return m_AudioInIo; }
    MiniAudioOut&               getAudioOutIo( void )                       { return m_AudioOutIo; }

    void                        setSpeakerMuted( bool muteSpeaker )         { m_SpeakersMuted = muteSpeaker; };
    bool                        getSpeakerMuted( void )                     { return m_SpeakersMuted; };

    void                        setAudioLoopbackEnable( bool enable )       { m_AudioLoopbackEnabled = enable; }
    bool                        getAudioLoopbackEnable( void )              { return m_AudioLoopbackEnabled; };

    void                        setDirectMicToSpeakerEnable( bool enable )  { m_DirectMicToSpeakerEnabled = enable; }
    bool                        getDirectMicToSpeakerEnable( void )         { return m_DirectMicToSpeakerEnabled; }

    bool                        setSoundInDeviceIndex( int sndInDeviceIndex ) { m_SndInDeviceIndex = sndInDeviceIndex; return true; };
    bool                        getSoundInDeviceIndex( int& retDeviceIndex ){ retDeviceIndex = m_SndInDeviceIndex; return true; };

    bool                        setSoundOutDeviceIndex( int sndOutDeviceIndex ) { m_SndOutDeviceIndex = sndOutDeviceIndex; return true; };
    bool                        getSoundOutDeviceIndex( int& retDeviceIndex ){ retDeviceIndex = m_SndOutDeviceIndex; return true; };

    bool                        soundInDeviceChanged( int deviceIndex )     { m_SndInDeviceIndex = deviceIndex; return true; };
    bool                        soundOutDeviceChanged( int deviceIndex )    { m_SndOutDeviceIndex = deviceIndex; return true; };

    bool                        runAudioDelayTest( void ){return true; };
    void                        setEnableSpeakerTestTone( int enableTestTone )  {}

    void                        setMicrophoneMuted( bool muteMic )          { m_MicrophoneMuted = muteMic; };
    bool                        getMicrophoneMuted( void )                  { return m_MicrophoneMuted; };

    void                        setIsMicrophoneWanted( bool wanted ){ m_MicrophoneWanted = wanted; };
    bool                        getIsMicrophoneWanted( void ) { return m_MicrophoneWanted; }

    void                        setIsMicrophoneRunning( bool running ){ m_MicrophoneRunning = running; };
    bool                        getIsMicrophoneRunning( void ) { return m_MicrophoneRunning; }

    void                        setIsSpeakerWanted( bool wanted ){ m_SpeakerWanted = wanted; };
    bool                        getIsSpeakerWanted( void ) { return m_SpeakerWanted; }

    void                        setIsSpeakerRunning( bool running ){ m_SpeakerRunning = running; };
    bool                        getIsSpeakerRunning( void ) { return m_SpeakerRunning; }

    void                        setIsPlayingTestFile( bool isPlaying ){ m_IsPlayingTestFile = isPlaying; };
    bool                        getIsPlayingTestFile( void ) { return m_IsPlayingTestFile; }

    void                        setNeedAudioOutDeviceStop( bool enabled ){ };
    bool                        speakerDeviceEnabled( bool enabled ){ return true; };

    void                        callbackAudioDeviceWrite( int16_t* pcmData, int sampleCnt );
    void                        callbackReadSpeakerData( int16_t* pcmData, int sampleCnt );

    void                        callbackAecProcessedAudio( int16_t* pcmData, int sampleCnt );

    void                        callbackPeerNetworkAudio( int16_t* pcmData, int sampleCnt );

    void                        playTestFile( TestFileWav& testFile );  

	virtual void 				callbackToGuiWantMicrophoneRecording( bool wantMicInput ) override {};
	virtual void 				callbackToGuiWantSpeakerOutput( bool wantSpeakerOutput ) override {};

	void						wantMicrophoneLevelCallbacks( GuiAudioLevelCallback *client, bool enable );

	int							getWantMicrophoneCount( void ) { return m_WantMicCnt; }

	int							getWantSpeakerCount( void ) { return m_WantSpeakerCnt; }
 
    void						setPlayerNlcActive( bool isActive );
    bool						getPlayerNlcActive( void ) { return m_PlayerNlcActive; }

    float                       calculateMsOfSamples( int sampleCount ) { return ((float)sampleCount / (ECHO_SAMPLE_RATE / AUDIO_CHANNELS) / 1000.0f);}

    //=== IAudioRequests begine ===//
    // return true if any microphone device is available to be enabled
    virtual bool				toGuiIsMicrophoneDeviceAvailable( void ) override;
    // enable disable microphone data callback
    virtual void				toGuiWantMicrophoneRecording( EMediaModule mediaModule, bool wantMicInput ) override;
    // enable disable sound out
    virtual void				toGuiWantSpeakerOutput( EMediaModule mediaModule, bool wantSpeakerOutput ) override;
    // add audio data to play.. assumes pcm mono 16000 Hz of mixer buffer length
    virtual int				    toGuiModuleAudioFrame( EMediaModule mediaModule, int16_t* pu16PcmData, int pcmDataLenInBytes, bool isSilence ) override;

    virtual int				    toGuiPlayerNlcAudio( EMediaModule mediaModule, float* audioDataFloat, int audioDataLenInBytes ) override;

    virtual float               toGuiGetAudioDelaySeconds( EMediaModule mediaModule ) override;

    virtual float               toGuiGetAudioCacheFreeSpace( EMediaModule mediaModule ) override;

    virtual float               toGuiGetAudioCacheTotalSeconds( EMediaModule mediaModule ) override;

    void						fromGuiEchoCanceledSamplesThreaded( int16_t* pcmData, int sampleCnts, bool isSilence );
    virtual void				fromGuiAudioOutSpaceAvaiThreaded( int freeSpaceLen );
    //=== IAudioRequests end ===//

    void                        setAudioInPeakAmplitude( int peakAmplitude ) { m_PeakAudioInAmplitude = peakAmplitude; }
    int                         getAudioInPeakAmplitude( void ) { return m_PeakAudioInAmplitude; }

    void                        lockPlayerCache( void )                             { m_PlayerCacheMutex.lock(); }
    void                        unlockPlayerCache( void )                           { m_PlayerCacheMutex.unlock(); }

    void                        lockModuleMixerBuffer( void )                       { m_ModuleMixerMutex.lock(); }
    void                        unlockModuleMixerBuffer( void )                     { m_ModuleMixerMutex.unlock(); }
    AudioMixerBuf&              getAudioMixerBuf( EMediaModule mediaModule );

protected slots:
    void						slotAudioPeekTimeout( void );

protected:
    bool                        playFromTestFile( void );

    void                        wantMicrophoneCountChanged( int wantMicCnt );
    void                        wantSpeakerCountChanged( int wantSpeakerCnt );

    AppCommon&                  m_MyApp;

    bool                        m_AudioIoInitialized{false};

    bool                        m_MicrophoneMuted{false};
    bool                        m_SpeakersMuted{false};

    bool                        m_AudioLoopbackEnabled{false};

    bool                        m_DirectMicToSpeakerEnabled{false};

    bool                        m_MicrophoneWanted{true};
    bool                        m_SpeakerWanted{true};

    bool                        m_MicrophoneRunning{false};
    bool                        m_SpeakerRunning{false};

    VxAudioFormat               m_AudioOutFormat;
    MiniAudioOut                m_AudioOutIo;

    VxAudioFormat               m_AudioInFormat;
    MiniAudioIn                 m_AudioInIo;

    int                         m_SndOutDeviceIndex{0};
    int                         m_SndInDeviceIndex{0};

    AudioFrameBuffer            m_AudioInFrameBuffer;
    AudioFrameBuffer            m_AudioEchoCanceledFrameBuffer;

    WebRtcAec                   m_Aec;

    std::vector<int16_t>        m_ResidualInBuffer;
    std::vector<int16_t>        m_SpeakerOutBuffer;
    size_t                      m_SpeakerOutReadOffset{0};
    std::mutex                  m_ResidualInBufferMutex;
    std::mutex                  m_SpeakerOutBufferMutex;

    // Activity Probability: 0.0–1.0 signal from WebRTC APM stats, shown on a QProgressBar (0–100 range).
    float                       m_CurrentVadProb{0};
    // Echo Return Loss (ERL): Get this from WebRTC APM stats. If ERL is high, the AEC is working hard.
    float                       m_CurrentErl{0};
    // Input Level: Calculate the RMS (Root Mean Square) of the 10ms buffer.
    float                       m_CurrentRms{0};


    TestFileWav                 m_TestFile;
    int                         m_TestFileIndex{0};
    bool                        m_IsPlayingTestFile{false};

    QTimer*						m_AudioLevelPeekTimer;
	std::vector<GuiAudioLevelCallback*> m_AudioLevelClientList;
    int                         m_PeakAudioInAmplitude{ 0 };

    int							m_WantMicCnt{ 0 };
    std::vector<EMediaModule>   m_WantMicList;
    VxMutex                     m_WantMicMutex;

    int							m_WantSpeakerCnt{ 0 };
    std::vector<EMediaModule>   m_WantSpeakerList;
    VxMutex                     m_WantSpeakerMutex;

    bool                        m_PlayerNlcActive{ false };


    std::map<EMediaModule, AudioMixerBuf> m_AppModuleToSpeakerMap;
    VxMutex                     m_ModuleMixerMutex;

    AudioSampleBuf              m_PlayerCacheBuf;
    VxMutex                     m_PlayerCacheMutex;
};
