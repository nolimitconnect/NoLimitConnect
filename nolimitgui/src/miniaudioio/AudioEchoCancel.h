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

#include <QObject>

class AppCommon;
class AudioIoMgr;
class AudioEchoCancelImpl;
class AudioSampleBuf;
class AudioSpeakerBuf;

class MiniAudioMgr;


class QMutex;

class AudioEchoCancel : public QObject
{
	Q_OBJECT;

public:
	AudioEchoCancel( AppCommon& appCommon, MiniAudioMgr& audioIoMgr, QObject* parent );
	virtual ~AudioEchoCancel() = default;

	void						echoCancelStartup( void );

	void						enableEchoCancel( bool enable );
	bool						isEchoCancelEnabled( void );

	void                        setEchoDelayMsParam( int delayMs );

	void						speakerReadSamples( int16_t* speakerReadData, int sampleCnt );

	void						micWroteSamples( int16_t* micWritedData, int sampleCnt );

	void						processEchoCancelThreaded( void );

	void						setEchoCancelerNeedsReset( bool needReset );

	bool						getIsInSync( void );

	void						setPeakAmplitudeDebugEnable( bool enableDebug );
	bool						getPeakAmplitudeDebugEnable( void );

	void                        resetMicrophoneBuffers( void );
	void                        resetSpeakerBuffers( void );

	AppCommon&					m_MyApp;
	MiniAudioMgr&				m_AudioIoMgr;

	AudioEchoCancelImpl&		m_AudioEchoCancelImpl;
};
