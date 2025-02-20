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

    //audioIoSystemStartup();

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

	if( m_MyApp.getAppSettings().getDisableSoundEffects() )
	{
		return nullptr;
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
		}

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
		m_CurSndPlaying = 0;
	}
}

//============================================================================
void SoundMgr::slotSndFinished( VxSndInstance * sndInstance )
{
	if( m_CurSndPlaying == sndInstance )
	{
		m_CurSndPlaying = 0;
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
	for( auto client : m_EchoCancelEnableClientList )
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

	for( auto client : m_AudioLevelClientList )
	{
		client->callbackGuiMicrophoneLevel( micLevel );
	}
}
