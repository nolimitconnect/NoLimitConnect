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

#include <QWidget> // must be declared first or linux Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include <GuiInterface/IAudioDefs.h>

#include "AudioSampleBuf.h"

#include <CoreLib/TimeIntervalEstimator.h>
#include <CoreLib/VxMutex.h>
#include <CoreLib/VxThread.h>
#include <CoreLib/VxSemaphore.h>

#if defined(USE_SPEEX_ECHO_CANCEL)
# include <libspeex/speex/speex_echo.h>
# include <libspeex/speex/speex_preprocess.h>
#elif defined(USE_WEB_RTC_ECHO_CANCEL_1)
# include <libechocancel/EchoCancel.h>
#elif defined(USE_WEB_RTC_ECHO_CANCEL_3)
# include <libdsp-webrtc/modules/audio_processing/aec3/echo_canceller3.h>
# include <api/audio/audio_frame.h>
# include <modules/audio_processing/high_pass_filter.h>
# include <modules/audio_processing/include/audio_processing.h>
#elif defined(USE_WEB_RTC_ECHO_CANCEL_MOBILE)
# include <libaemc-webrtc/AecmWebRtc.h>
#endif // USE_SPEEX_ECHO_CANCEL

#include <QObject>

class AppCommon;
class AudioIoMgr;
class AudioSpeakerBuf;
class MiniAudioMgr;
class QAudioFormat;
class QMutex;

class AudioEchoCancelImpl : public QObject
{
	Q_OBJECT;

public:
	AudioEchoCancelImpl( AppCommon& appCommon, MiniAudioMgr& audioIoMgr, QObject* parent );
	virtual ~AudioEchoCancelImpl();

	void						echoCancelStartup( int sampleFreq = 8000 );
	void						echoCancelShutdown( void );

	void						enableEchoCancel( bool enable );
	bool						isEchoCancelEnabled( void )						{ return m_EchoCancelEnable; };

	void						setEchoDelayMsConstant( int delayMs )			{ m_EchoDelayMsConstant = delayMs; }
	int							getEchoDelayMsConstant( void )					{ return m_EchoDelayMsConstant; }

	void						setPeakAmplitudeDebugEnable( bool enableDebug ) { m_PeakAmplitudeDebug = enableDebug; }
	bool						getPeakAmplitudeDebugEnable( void )				{ return m_PeakAmplitudeDebug; }

	void						setSyncDebugEnabled( bool enableSyncDebug )		{ m_SyncDebugEnabled = enableSyncDebug; }
	bool						getSyncDebugEnabled( void )						{ return m_SyncDebugEnabled; }

	bool						getIsInSync( void ) { return m_EchoCancelInSync; }

	void						speakerReadSamples( int16_t* speakerReadData, int sampleCnt );

	void						micWroteSamples( int16_t* micWriteData, int sampleCnt );

	void						processEchoCancelThreaded( void );

	void                        setEchoCancelerNeedsReset( bool needReset )		{ m_EchoCancelNeedsReset = needReset; }

	static int64_t				calculateEchoSamplesUs( int sampleCnt )			{ return (sampleCnt * 1000000) / ECHO_SAMPLE_RATE; }
	static int64_t				calculateEchoSamplesMs( int sampleCnt )			{ return (sampleCnt * 1000) / ECHO_SAMPLE_RATE; }

	void                        resetMicrophoneBuffers( void );
	void                        resetSpeakerBuffers( void );

protected:
	bool						attemptEchoSyncThreaded( void );

	void						resetEchoCanceler( int samplesPerEchoFrame );

	void						processEchoCancelFrame( int16_t* micBuf, int16_t* speakerBuf, int samplesPerFrame, int16_t* echoCanceledBuf );

	void						processWebRtc1EchoCancel( int16_t* micWriteBuf, int16_t* speakerBuf, int sampleCnt, int16_t* echoCanceledData );
	void						processWebRtc3EchoCancel( int16_t* micWriteBuf, int16_t* speakerBuf, int sampleCnt, int16_t* echoCanceledData );
	void						processWebRtcMobileEchoCancel( int16_t* micWriteBuf, int16_t* speakerBuf, int sampleCnt, int16_t* echoCanceledData );

#if defined(USE_SPEEX_ECHO_CANCEL)
	void						startupSpeex( int sampleCnt );
	void						resetSpeex( int sampleCnt );
	void						shutdownSpeex( void );
#endif // defined(USE_SPEEX_ECHO_CANCEL)
	
	//=== vars ===//
	AppCommon&					m_MyApp;
	MiniAudioMgr&				m_AudioIoMgr;

	bool						m_DbgLogEnable{ false };

	bool						m_EchoCancelEnable{ false };
	bool						m_EchoCancelNeedsReset{ false };

	int							m_EchoDelayMsConstant{ 100 };

	int							m_MicSamplesPerWrite{ 0 };
	int64_t						m_MicWriteSamplesUs{ 0 };

	int							m_SpeakerSamplesPerRead{ 0 };
	int64_t						m_SpeakerReadSamplesUs{ 0 };

	bool						m_EchoCancelInSync{ false };
	bool						m_SyncDebugEnabled{ false };
	bool						m_PeakAmplitudeDebug{ false };

	VxMutex						m_SpeakerSamplesMutex;
	AudioSampleBuf				m_SpeakerSamples;
	int64_t						m_TailSpeakerSamplesMs{ 0 };
	int64_t						m_HeadSpeakerSamplesMs{ 0 };
	int64_t						m_SpeakerLastCallTimestamp{ 0 };
	int							m_SpeakerLastCallSampleCnt{ 0 };

	VxMutex						m_MicSamplesMutex;
	AudioSampleBuf				m_MicSamples;
	int64_t						m_TailMicSamplesMs{ 0 };
	int64_t						m_HeadMicSamplesMs{ 0 };
	int64_t						m_MicLastCallTimestamp{ 0 };
	int							m_MicLastCallSampleCnt{ 0 };


#if defined(USE_SPEEX_ECHO_CANCEL)
	SpeexEchoState*				m_SpeexState{ nullptr };
	SpeexPreprocessState*		m_SpeexPreprocess{ nullptr };
	bool						m_SpeexInitialized{ false };

#elif defined(USE_WEB_RTC_ECHO_CANCEL_1)
	EchoCancel					m_EchoCancel;
#elif defined(USE_WEB_RTC_ECHO_CANCEL_3)
	std::unique_ptr<webrtc::EchoControl>	m_EchoControler;
	std::unique_ptr<webrtc::HighPassFilter> m_HighPassFilter;

	webrtc::StreamConfig					m_MicStreamConfig;
	std::unique_ptr<webrtc::AudioBuffer>	m_MicAudioBuf;
	webrtc::AudioFrame						m_MicFrame;

	webrtc::StreamConfig					m_SpeakerStreamConfig;
	std::unique_ptr<webrtc::AudioBuffer>	m_SpeakerAudioBuf;
	webrtc::AudioFrame						m_SpeakerFrame;
#elif defined(USE_WEB_RTC_ECHO_CANCEL_MOBILE)
	AecmWebRtc								m_AecmWebRtc;
#endif // USE_SPEEX_ECHO_CANCEL

};
