//============================================================================
// Copyright (C) 2009 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppCommon.h"
#include "SoundFxMgr.h"

#include <P2PEngine/P2PEngine.h>
#include <MediaProcessor/MediaProcessor.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

//============================================================================
bool AppCommon::toGuiIsMicrophoneDeviceAvailable( void )
{
	if(LogEnabled(eLogAudioIo)) LogModule( eLogAudioIo, LOG_VERBOSE, "AppCommon::%s", __func__ );
	if( VxIsAppShuttingDown() )
	{
		return false;
	}

	return m_AudioMgr.isMicrophoneDeviceAvailable();
}

//============================================================================
void AppCommon::toGuiWantMicrophoneRecording( EMediaModule mediaModule, bool wantMicInput )
{
	if(LogEnabled(eLogAudioIo)) LogModule( eLogAudioIo, LOG_VERBOSE, "AppCommon::%s %d", __func__, wantMicInput );
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalWantMicrophoneRecording( mediaModule, wantMicInput );
}

//============================================================================
void AppCommon::slotInternalWantMicrophoneRecording( EMediaModule mediaModule, bool wantMicInput )
{
    bool wasMicAvailable = m_AudioMgr.getIsMicrophoneRunning();
    m_AudioMgr.toGuiWantMicrophoneRecording( mediaModule, wantMicInput );
    bool isMicAvailable = m_AudioMgr.getIsMicrophoneRunning();
    if( wasMicAvailable != isMicAvailable )
    {
        m_ToGuiHardwareCtrlBusy = true;
        for( auto toGuiClient : m_ToGuiHardwareCtrlList )
        {
            toGuiClient->callbackToGuiWantMicrophoneRecording( wantMicInput );
        }

        m_ToGuiHardwareCtrlBusy = false;
        if(LogEnabled(eLogAudioIo)) LogModule( eLogAudioIo, LOG_VERBOSE, "AppCommon::%s %d done", __func__, isMicAvailable );
    }
}

//============================================================================
void AppCommon::toGuiWantSpeakerOutput( EMediaModule mediaModule, bool wantSpeakerOutput )
{
	
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	if( mediaModule != eMediaModuleSoundEffects )
	{
		if( LogEnabled( eLogVoice ) ) LogModule( eLogVoice, LOG_VERBOSE, "AppCommon::%s want speaker? %d module %s", __func__, wantSpeakerOutput, DescribeMediaModule( mediaModule ) );
	}

	emit signalInternalWantSpeakerOutput( mediaModule, wantSpeakerOutput );
}

//============================================================================
void AppCommon::slotInternalWantSpeakerOutput( EMediaModule mediaModule, bool wantSpeakerOutput )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    bool wasSpeakerAvailable = m_AudioMgr.getIsSpeakerRunning();
    m_AudioMgr.toGuiWantSpeakerOutput( mediaModule, wantSpeakerOutput );
    bool isSpeakerAvailable = m_AudioMgr.getIsSpeakerRunning();

    if( wasSpeakerAvailable != isSpeakerAvailable )
    {
        if( m_ToGuiHardwareCtrlBusy )
        {
            LogMsg( LOG_ERROR, "AppCommon::%s ToGuiHardware busy", __func__ );
            vx_assert( false );
        }

        m_ToGuiHardwareCtrlBusy = true;
        for( auto toGuiClient : m_ToGuiHardwareCtrlList )
        {
            toGuiClient->callbackToGuiWantSpeakerOutput( isSpeakerAvailable );
        }

        m_ToGuiHardwareCtrlBusy = false;
    }
}

//============================================================================
//! playback audio
int AppCommon::toGuiModuleAudioFrame( EMediaModule mediaModule, int16_t * pu16PcmData, int pcmDataLenInBytes, bool isSilence )
{
    if( VxIsAppShuttingDown() )
    {
        return 0;
    }

    return m_AudioMgr.toGuiModuleAudioFrame( mediaModule, pu16PcmData, pcmDataLenInBytes, isSilence );
}

//============================================================================
//! playback audio
int AppCommon::toGuiPlayerNlcAudio( EMediaModule mediaModule, float* audioDataFloat, int audioDataLenInBytes )
{
	if( VxIsAppShuttingDown() )
	{
		return 0;
	}

	return m_AudioMgr.toGuiPlayerNlcAudio( mediaModule, audioDataFloat, audioDataLenInBytes );
}

//============================================================================
float AppCommon::toGuiGetAudioDelaySeconds( EMediaModule mediaModule )
{
	if( VxIsAppShuttingDown() )
	{
		return 0.0f;
	}

	return m_AudioMgr.toGuiGetAudioDelaySeconds( mediaModule );
}

//============================================================================
float AppCommon::toGuiGetAudioCacheFreeSpace( EMediaModule mediaModule )
{
	if( VxIsAppShuttingDown() )
	{
		return 0.0f;
	}

	return m_AudioMgr.toGuiGetAudioCacheFreeSpace( mediaModule );
}

//============================================================================
float AppCommon::toGuiGetAudioCacheTotalSeconds( EMediaModule mediaModule )
{
	if( VxIsAppShuttingDown() )
	{
		return 0.0f;
	}

	return m_AudioMgr.toGuiGetAudioCacheTotalSeconds( mediaModule );
}

//============================================================================
/// Mute/Unmute microphone
void AppCommon::fromGuiMuteMicrophone( bool muteMic )
{
	m_AudioMgr.setMicrophoneMuted( muteMic );
    getEngine().fromGuiMuteMicrophone( muteMic );

	if( m_ToGuiHardwareCtrlBusy )
	{
		LogMsg( LOG_ERROR, "AppCommon::%s ToGuiHardware busy", __func__ );
		vx_assert( false );
	}

	m_ToGuiHardwareCtrlBusy = true;
	for( auto toGuiClient : m_ToGuiHardwareCtrlList )
	{
		toGuiClient->callbackToGuiMicrophoneMuted( muteMic );
	}

	m_ToGuiHardwareCtrlBusy = false;
}

//============================================================================
/// Returns true if microphone is muted
bool AppCommon::fromGuiIsMicrophoneMuted( void )
{
    return getEngine().fromGuiIsMicrophoneMuted();
}

//============================================================================
/// Mute/Unmute speaker
void AppCommon::fromGuiMuteSpeaker( bool muteSpeaker )
{
	m_AudioMgr.setSpeakerMuted( muteSpeaker );
    getEngine().fromGuiMuteSpeaker( muteSpeaker );

	if( m_ToGuiHardwareCtrlBusy )
	{
		LogMsg( LOG_ERROR, "AppCommon::%s ToGuiHardware busy", __func__ );
		vx_assert( false );
	}

	m_ToGuiHardwareCtrlBusy = true;
	for( auto& toGuiClient : m_ToGuiHardwareCtrlList )
	{
		toGuiClient->callbackToGuiSpeakerMuted( muteSpeaker );
	}

	m_ToGuiHardwareCtrlBusy = false;
}

//============================================================================
void AppCommon::fromGuiCameraEnable( bool enableCamera )
{
    m_CamLogic.setCameraEnable( enableCamera );

	if( m_ToGuiHardwareCtrlBusy )
	{
		LogMsg( LOG_ERROR, "AppCommon::%s ToGuiHardware busy", __func__ );
		vx_assert( false );
	}

	m_ToGuiHardwareCtrlBusy = true;
	for( auto& toGuiClient : m_ToGuiHardwareCtrlList )
	{
		toGuiClient->callbackToGuiCameraEnable( enableCamera );
	}

	m_ToGuiHardwareCtrlBusy = false;
}

//============================================================================
void AppCommon::fromGuiCaptureRunning( bool camCaptureRunning )
{
	if( m_ToGuiHardwareCtrlBusy )
	{
		LogMsg( LOG_ERROR, "AppCommon::%s ToGuiHardware busy", __func__ );
		vx_assert( false );
	}

	m_ToGuiHardwareCtrlBusy = true;
	for( auto& toGuiClient : m_ToGuiHardwareCtrlList )
	{
		toGuiClient->callbackToGuiCaptureRunning( camCaptureRunning );
	}

	m_ToGuiHardwareCtrlBusy = false;
}

//============================================================================
/// Returns true if speaker is muted
bool AppCommon::fromGuiIsSpeakerMuted( void )
{
    return getEngine().fromGuiIsSpeakerMuted();
}

//============================================================================
void AppCommon::fromGuiAudioOutSpaceAvaiThreaded( int freeSpaceLen )
{
	getEngine().getMediaProcessor().fromGuiAudioOutSpaceAvaiThreaded( freeSpaceLen );
}

//============================================================================
void AppCommon::fromGuiEchoCanceledSamplesThreaded( int16_t* pcmData, int sampleCnt, bool isSilence )
{
	getEngine().getMediaProcessor().fromGuiEchoCanceledSamplesThreaded( pcmData, sampleCnt, isSilence );
}
