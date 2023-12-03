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
#include "SoundMgr.h"

#include <ptop_src/ptop_engine_src/MediaProcessor/MediaProcessor.h>

#include <CoreLib/VxGlobals.h>


//============================================================================
bool AppCommon::toGuiIsMicrophoneDeviceAvailable( void )
{
	LogMsg( LOG_INFO, "#### AppCommon::toGuiIsMicrophoneDeviceAvailable" );
	if( VxIsAppShuttingDown() )
	{
		return false;
	}

	return getSoundMgr().toGuiIsMicrophoneDeviceAvailable();
}

//============================================================================
void AppCommon::toGuiWantMicrophoneRecording( EAppModule appModule, bool wantMicInput )
{
	LogMsg( LOG_INFO, "#### AppCommon::toGuiWantMicrophoneRecording %d", wantMicInput );
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalWantMicrophoneRecording( appModule, wantMicInput );
}

//============================================================================
void AppCommon::slotInternalWantMicrophoneRecording( EAppModule appModule, bool wantMicInput )
{
	VxGUID nullGuid;
	slotInternalWantUserVoiceMicrophone( appModule, nullGuid, wantMicInput );
}

//============================================================================
void AppCommon::toGuiWantUserVoiceMicrophone( EAppModule appModule, VxGUID& onlineId, bool wantMicInput )
{
	LogMsg( LOG_INFO, "#### AppCommon::toGuiWantUserVoiceMicrophone %d", wantMicInput );
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalWantUserVoiceMicrophone( appModule, onlineId, wantMicInput );
}

//============================================================================
void AppCommon::slotInternalWantUserVoiceMicrophone( EAppModule appModule, VxGUID onlineId, bool wantMicInput )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	bool wasMicAvailable = m_SoundMgr.isMicrophoneEnabled();
	m_SoundMgr.toGuiWantUserVoiceMicrophone( appModule, onlineId, wantMicInput );
	bool isMicAvailable = m_SoundMgr.isMicrophoneEnabled();
	if( wasMicAvailable != isMicAvailable )
	{
		m_ToGuiHardwareCtrlBusy = true;
		for( auto toGuiClient : m_ToGuiHardwareCtrlList )
		{
			toGuiClient->callbackToGuiWantMicrophoneRecording( wantMicInput );
		}

		m_ToGuiHardwareCtrlBusy = false;
		LogMsg( LOG_INFO, "#### AppCommon::slotEnableMicrophoneRecording %d done", isMicAvailable );
	}
}

//============================================================================
void AppCommon::toGuiWantSpeakerOutput( EAppModule appModule, bool wantSpeakerOutput )
{
	LogMsg( LOG_INFO, "#### AppCommon::toGuiWantSpeakerOutput %d", wantSpeakerOutput );
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalWantSpeakerOutput( appModule, wantSpeakerOutput );
}

//============================================================================
void AppCommon::slotInternalWantSpeakerOutput( EAppModule appModule, bool wantSpeakerOutput )
{
	VxGUID nullGuid;
	slotInternalWantUserVoiceSpeaker( appModule, nullGuid, wantSpeakerOutput );
}

//============================================================================
void AppCommon::toGuiWantUserVoiceSpeaker( EAppModule appModule, VxGUID& onlineId, bool wantSpeakerOutput )
{
	LogMsg( LOG_INFO, "#### AppCommon::toGuiWantUserVoiceSpeaker %d", wantSpeakerOutput );
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalWantUserVoiceSpeaker( appModule, onlineId, wantSpeakerOutput );
}

//============================================================================
void AppCommon::slotInternalWantUserVoiceSpeaker( EAppModule appModule, VxGUID onlineId, bool wantSpeakerOutput )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	bool wasSpeakerAvailable = m_SoundMgr.isSpeakerEnabled();
	m_SoundMgr.toGuiWantUserVoiceSpeaker( appModule, onlineId, wantSpeakerOutput );
	bool isSpeakerAvailable = m_SoundMgr.isSpeakerEnabled();

	if( wasSpeakerAvailable != isSpeakerAvailable )
	{
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
int AppCommon::toGuiModuleAudioFrame( EAppModule appModule, int16_t * pu16PcmData, int pcmDataLenInBytes, bool isSilence )
{
    if( VxIsAppShuttingDown() )
    {
        return 0;
    }

    return m_SoundMgr.toGuiModuleAudioFrame( appModule, pu16PcmData, pcmDataLenInBytes, isSilence );
}

#if defined(ENABLE_KODI) || defined(ENABLE_NLC_PLAYER)
//============================================================================
//! playback audio
int AppCommon::toGuiPlayerNlcAudio( EAppModule appModule, float* audioDataFloat, int audioDataLenInBytes )
{
	if( VxIsAppShuttingDown() )
	{
		return 0;
	}

	return m_SoundMgr.toGuiPlayerNlcAudio( appModule, audioDataFloat, audioDataLenInBytes );
}

//============================================================================
float AppCommon::toGuiGetAudioDelaySeconds( EAppModule appModule )
{
	if( VxIsAppShuttingDown() )
	{
		return 0.0f;
	}

	return m_SoundMgr.toGuiGetAudioDelaySeconds( appModule );
}

//============================================================================
float AppCommon::toGuiGetAudioCacheFreeSpace( EAppModule appModule )
{
	if( VxIsAppShuttingDown() )
	{
		return 0.0f;
	}

	return m_SoundMgr.toGuiGetAudioCacheFreeSpace( appModule );
}

//============================================================================
float AppCommon::toGuiGetAudioCacheTotalSeconds( EAppModule appModule )
{
	if( VxIsAppShuttingDown() )
	{
		return 0.0f;
	}

	return m_SoundMgr.toGuiGetAudioCacheTotalSeconds( appModule );
}

#endif // ENABLE_KODI

//============================================================================
/// Mute/Unmute microphone
void AppCommon::fromGuiMuteMicrophone( bool muteMic )
{
	m_SoundMgr.setMuteMicrophone( muteMic );
    getEngine().fromGuiMuteMicrophone( muteMic );

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
	m_SoundMgr.setMuteSpeaker( muteSpeaker );
    getEngine().fromGuiMuteSpeaker( muteSpeaker );

	m_ToGuiHardwareCtrlBusy = true;
	for( auto toGuiClient : m_ToGuiHardwareCtrlList )
	{
		toGuiClient->callbackToGuiSpeakerMuted( muteSpeaker );
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
