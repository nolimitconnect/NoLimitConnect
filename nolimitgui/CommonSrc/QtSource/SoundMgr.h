
#pragma once
//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "SoundDefs.h"

#include "miniaudioio/MiniAudioMgr.h"

#include "ToGuiHardwareControlInterface.h"

#include <QVector>
#include <QObject>

class GuiAudioLevelCallback;
class P2PEngine;
class QIODevice;
class QSound;
class MyQtSoundInput;
class MyQtSoundOutput;
class VxSndInstance;

class SoundMgr : public MiniAudioMgr, public ToGuiHardwareControlInterface
{
	Q_OBJECT
public:
	SoundMgr( AppCommon& app );
	virtual ~SoundMgr() override = default;

	bool						sndMgrStartup( void );
	bool						sndMgrShutdown( void );

	virtual void 				callbackToGuiWantMicrophoneRecording( bool wantMicInput ) override {};
	virtual void 				callbackToGuiWantSpeakerOutput( bool wantSpeakerOutput ) override {};

	VxSndInstance*			    playSnd( ESndDef sndDef, bool loopContinuous = false );
	void						stopSnd( ESndDef sndDef );

	void						wantAudioLevelCallbacks( GuiAudioLevelCallback *client, bool enable );

signals:
	void						signalSndFinished( VxSndInstance* sndInstance );

public slots:
	void						mutePhoneRing( bool bMute );
	void						muteNotifySound( bool bMute );
	void						slotStartPhoneRinging( void );
	void						slotStopPhoneRinging( void );
	void						slotPlayNotifySound( void );
	void						slotPlayShredderSound( void );

	void						slotAudioPeekTimeout( void );

private slots:
	void						slotSndFinished( VxSndInstance * sndInstance );

protected:

	//=== vars ===//
	bool						m_MutePhoneRing{ false };
	bool						m_MuteNotifySnd{ false };

	QVector<VxSndInstance *>	m_SndList;
	VxSndInstance *			    m_CurSndPlaying{ nullptr };

	QTimer*						m_AudioLevelPeekTimer;
	std::vector<GuiAudioLevelCallback*> m_AudioLevelClientList;
};

