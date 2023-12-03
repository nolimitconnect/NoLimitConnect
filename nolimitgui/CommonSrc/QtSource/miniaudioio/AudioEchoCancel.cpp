
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AudioEchoCancel.h"
#include "AudioEchoCancelImpl.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MiniAudioMgr.h"

//============================================================================
AudioEchoCancel::AudioEchoCancel( AppCommon& appCommon, MiniAudioMgr& audioIoMgr, QObject* parent )
	: QObject(parent)
	, m_MyApp( appCommon )
	, m_AudioIoMgr( audioIoMgr )
	, m_AudioEchoCancelImpl( *new AudioEchoCancelImpl(appCommon, audioIoMgr, this ))
{	
}

//============================================================================
void AudioEchoCancel::echoCancelStartup( void )
{
	m_AudioEchoCancelImpl.setEchoDelayMsConstant( m_MyApp.getAppSettings().getEchoDelayParam() );
	m_AudioEchoCancelImpl.echoCancelStartup();
}

//============================================================================
void AudioEchoCancel::enableEchoCancel( bool enable )
{
	m_AudioEchoCancelImpl.enableEchoCancel( enable );
}

//============================================================================
bool AudioEchoCancel::isEchoCancelEnabled( void )
{
	return m_AudioEchoCancelImpl.isEchoCancelEnabled();
}

//============================================================================
void AudioEchoCancel::setEchoDelayMsParam( int delayMs )
{
	m_AudioEchoCancelImpl.setEchoDelayMsConstant( delayMs );
}

//============================================================================
void AudioEchoCancel::speakerReadSamples( int16_t* speakerReadData, int sampleCnt )
{
	m_AudioEchoCancelImpl.speakerReadSamples( speakerReadData, sampleCnt );
}

//============================================================================
void AudioEchoCancel::micWroteSamples( int16_t* micWritedData, int sampleCnt )
{
	m_AudioEchoCancelImpl.micWroteSamples( micWritedData, sampleCnt );
}

//============================================================================
void AudioEchoCancel::processEchoCancelThreaded( void )
{
	m_AudioEchoCancelImpl.processEchoCancelThreaded();
}

//============================================================================
void AudioEchoCancel::setEchoCancelerNeedsReset( bool needReset )
{
	m_AudioEchoCancelImpl.setEchoCancelerNeedsReset( needReset );
}

//============================================================================
bool AudioEchoCancel::getIsInSync( void )
{
	return m_AudioEchoCancelImpl.getIsInSync();
}

//============================================================================
void AudioEchoCancel::setPeakAmplitudeDebugEnable( bool enableDebug ) 
{ 
	m_AudioEchoCancelImpl.setPeakAmplitudeDebugEnable( enableDebug );
}

//============================================================================
bool AudioEchoCancel::getPeakAmplitudeDebugEnable( void ) 
{ 
	return m_AudioEchoCancelImpl.getPeakAmplitudeDebugEnable();
}

//============================================================================
void AudioEchoCancel::resetMicrophoneBuffers( void )
{
	m_AudioEchoCancelImpl.resetMicrophoneBuffers();
}

//============================================================================
void AudioEchoCancel::resetSpeakerBuffers( void )
{
	m_AudioEchoCancelImpl.resetSpeakerBuffers();
}
