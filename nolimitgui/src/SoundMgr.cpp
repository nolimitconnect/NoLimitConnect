//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "SoundMgr.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiAudioLevelCallback.h"
#include "GuiEchoCancelEnableCallback.h"
#include "VxSndInstance.h"

#include <CoreLib/VxDebug.h>
#include <P2PEngine/P2PEngine.h>

#include <QDebug>
#include <QMessageBox>

const char* DescribeSnd( ESndDef sndDef )
{
	switch( sndDef )
	{
	case eSndDefNone:
		return "Snd None";
	case eSndDefIgnore:
		return "Snd Ignore";
	case eSndDefCancel:
		return "Snd Cance";
	case eSndDefAlarmPleasant:
		return "Snd AlarmPleasant";
	case eSndDefAlarmAnoying:
		return "Snd AlarmAnoying";
	case eSndDefButtonClick:
		return "Snd ButtonClick";
	case eSndDefChoice1:
		return "Snd Choice1";
	case eSndDefChoice2:
		return "Snd Choice2";
	case eSndDefSending:
		return "Snd Sendin";
	case eSndDefNotify1:
		return "Snd Notify1";
	case eSndDefNotify2:
		return "Snd Notify2";
	case eSndDefPaperShredder:
		return "Snd PaperShredder";
	case eSndDefPhoneRing1:
		return "Snd PhoneRing1";
	case eSndDefReject:
		return "Snd Reject";
	case eSndDefShare:
		return "Snd Share";
	case eSndDefByeBye:
		return "Snd ByeBye";
	case eSndDefMessageArrived:
		return "Snd MessageArrived";
	case eSndDefOfferAccepted:
		return "Snd OfferAccepted";
	case eSndDefOfferRejected:
		return "Snd OfferRejected";
	case eSndDefCameraClick:
		return "Snd CameraClick";
	case eSndDefBusy:
		return "Snd Busy";
	case eSndDefOfferStillWaiting:
		return "Snd StillWaiting";
	case eSndDefFileXferComplete:
		return "Snd XferComplete";
	case eSndDefUserBellMessage:
		return "Snd BellMessage";
	case eSndDefNeckSnap:
		return "Snd NeckSnap";
	case eSndDefYes:
		return "Snd Yes";
	case eSndDefAppShutdown:
		return "Snd AppShutdown";
	default:
		return "Snd Invalid";
	}
}

//============================================================================
SoundMgr& GetSndMgrInstance( void )
{
	return GetAppInstance().getSoundMgr();
}

//============================================================================
SoundMgr::SoundMgr( AppCommon& app )
	: MiniAudioMgr( app, app, &app )
	, m_AudioLevelPeekTimer( new QTimer( this ) )
{
	m_AudioLevelPeekTimer->setInterval( 500 );
	connect( m_AudioLevelPeekTimer, SIGNAL(timeout()), this, SLOT(slotAudioPeekTimeout()) );
}

//============================================================================ 
void SoundMgr::slotStartPhoneRinging( void )
{
	playSnd( eSndDefPhoneRing1, true );
}

//============================================================================ 
void SoundMgr::slotStopPhoneRinging( void )
{
	if( m_CurSndPlaying 
		&& ( eSndDefPhoneRing1 == m_CurSndPlaying->getSndDef() ) )
	{
		m_CurSndPlaying->stopPlay();
		m_CurSndPlaying = nullptr;
	}
}

//============================================================================
void SoundMgr::slotPlayNotifySound( void )
{
	playSnd( eSndDefNotify1, true );
}

//============================================================================
void SoundMgr::slotPlayShredderSound( void )
{
	playSnd( eSndDefPaperShredder, true );
}

//============================================================================
void SoundMgr::mutePhoneRing( bool bMute )
{
	m_MutePhoneRing = bMute;
	if( bMute )
	{
		slotStopPhoneRinging();
	}
}

//============================================================================
void SoundMgr::muteNotifySound( bool bMute )
{
	m_MuteNotifySnd = bMute;
}

//============================================================================
bool SoundMgr::sndMgrStartup( void )
{
    for( int i = 0; i < eMaxSndDef; i++ )
    {
        VxSndInstance* sndInstance = new VxSndInstance( (ESndDef)i, this );
        m_SndList.emplace_back( sndInstance );
    }

	m_MyApp.wantToGuiHardwareCtrlCallbacks( this, true );

	bool mutedMic = m_MyApp.getAppSettings().getMicMuted();
	m_MyApp.fromGuiMuteMicrophone( mutedMic );

	bool mutedSpeaker = m_MyApp.getAppSettings().getSpeakerMuted();
	m_MyApp.fromGuiMuteSpeaker( mutedSpeaker );

	return true;
}

//============================================================================
bool SoundMgr::sndMgrShutdown( void )
{
	m_MyApp.wantToGuiHardwareCtrlCallbacks( this, false );
	audioIoSystemShutdown();
	return true;
}

//============================================================================
VxSndInstance * SoundMgr::playSnd( ESndDef sndDef, bool loopContinuous  )
{
#ifdef DISABLE_AUDIO
    return 0;
#endif // DISABLE_AUDIO

	if( m_MyApp.getAppSettings().getDisableAllSoundEffects() )
	{
		return nullptr;
	}

    if( !m_MyApp.getIsAppInitialized() )
    {
        return nullptr;
    }

	if( LogEnabled( eLogUsers ) && sndDef != eSndDefButtonClick )
	{
		LogMsg( LOG_VERBOSE, "SoundMgr::%s play sound %s", __func__, DescribeSnd( sndDef ) );
	}

	if( m_MutePhoneRing 
		&& ( eSndDefPhoneRing1 == sndDef ) )
	{
		return m_SndList[ eSndDefNone ];
	}

	if( m_MuteNotifySnd 
		&& ( ( eSndDefNotify1 == sndDef ) || ( eSndDefNotify2 == sndDef ) ) )
	{
		return m_SndList[ eSndDefNone ];
	}

    if( ( sndDef < m_SndList.size() )
		&& ( 0 <= sndDef ) )
	{
		if( m_CurSndPlaying )
		{
			m_CurSndPlaying->stopPlay();
			m_CurSndPlaying = nullptr;
		}

		if( LogEnabled( eLogUsers ) )LogModule( eLogUsers, LOG_VERBOSE, "SoundMgr::%s play sound %s", __func__, DescribeSnd( sndDef ) );
		m_CurSndPlaying = m_SndList[ sndDef ];
		m_CurSndPlaying->startPlay( loopContinuous );
		return m_CurSndPlaying;
	}
	else
	{
		return m_SndList[ eSndDefNone ];
	}
}

//============================================================================
void SoundMgr::stopSnd( ESndDef sndDef )
{
	if( m_CurSndPlaying 
		&& ( sndDef == m_CurSndPlaying->getSndDef() ) )
	{
		m_CurSndPlaying->stopPlay();
		m_CurSndPlaying = nullptr;
	}
}

//============================================================================
void SoundMgr::slotSndFinished( VxSndInstance * sndInstance )
{
	if( m_CurSndPlaying == sndInstance )
	{
		m_CurSndPlaying = nullptr;
	}
}

//============================================================================
void SoundMgr::wantEchoCancelEnableCallbacks( GuiEchoCancelEnableCallback* client, bool enable )
{
	for( auto iter = m_EchoCancelEnableClientList.begin(); iter != m_EchoCancelEnableClientList.end(); ++iter )
	{
        if( client == *iter )
		{
			if( enable )
			{
				return;
			}
			else
			{
				m_EchoCancelEnableClientList.erase( iter );
				return;
			}
		}
	}

	if( enable )
	{
		m_EchoCancelEnableClientList.emplace_back( client );
	}
}

//============================================================================
void SoundMgr::setEchoCancelEnable( bool enable )
{
	if( enable == getEchoCancelEnable() )
	{
		return;
	}

	MiniAudioMgr::setEchoCancelEnable( enable );
	for( auto& client : m_EchoCancelEnableClientList )
	{
		client->callbackGuiEchoCancelEnable( enable );
	}
}

//============================================================================
void SoundMgr::wantMicrophoneLevelCallbacks( GuiAudioLevelCallback* client, bool enable )
{
	for( auto iter = m_AudioLevelClientList.begin(); iter != m_AudioLevelClientList.end(); ++iter )
	{
        if( client == *iter )
		{
			if( enable )
			{
				return;
			}
			else
			{
				m_AudioLevelClientList.erase( iter );
				if( 0 == m_AudioLevelClientList.size() )
				{
					m_AudioLevelPeekTimer->stop();
				}

				return;
			}
		}
	}

	if( enable )
	{
		m_AudioLevelClientList.emplace_back( client );
		if( 1 == m_AudioLevelClientList.size() )
		{
			m_AudioLevelPeekTimer->start();
		}
	}
}

//============================================================================
void SoundMgr::slotAudioPeekTimeout( void )
{
	if( m_AudioLevelClientList.empty() )
	{
		return;
	}

	int micLevel = isMicrophoneEnabled() && !getIsMicrophoneMuted() ? getAudioInPeakAmplitude() : 0;

	for( auto& client : m_AudioLevelClientList )
	{
		client->callbackGuiMicrophoneLevel( micLevel );
	}
}

//============================================================================
void SoundMgr::wantMicrophoneCountChanged( int wantMicCnt )
{
	m_WantMicCnt = wantMicCnt;
	for( auto& client : m_AudioLevelClientList )
	{
		client->callbackWantMicrophoneCount( m_WantMicCnt );
	}
}

//============================================================================
void SoundMgr::wantSpeakerCountChanged( int wantSpeakerCnt )
{
	m_WantSpeakerCnt = wantSpeakerCnt;
	for( auto& client : m_AudioLevelClientList )
	{
		client->callbackWantSpeakerCount( m_WantSpeakerCnt );
	}
}