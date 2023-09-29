
#pragma once
//============================================================================
// Copyright (C) 2010 Brett R. Jones
// Issued to MIT style license by Brett R. Jones in 2017
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

#include "SoundDefs.h"

#include "miniaudioio/MiniAudioMgr.h"

#include "ToGuiHardwareControlInterface.h"

#include <QVector>
#include <QObject>

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

signals:
	void						signalSndFinished( VxSndInstance* sndInstance );

public slots:
	void						mutePhoneRing( bool bMute );
	void						muteNotifySound( bool bMute );
	void						slotStartPhoneRinging( void );
	void						slotStopPhoneRinging( void );
	void						slotPlayNotifySound( void );
	void						slotPlayShredderSound( void );

private slots:
	void						slotSndFinished( VxSndInstance * sndInstance );

protected:

	//=== vars ===//
	bool						m_MutePhoneRing{ false };
	bool						m_MuteNotifySnd{ false };

	QVector<VxSndInstance *>	m_SndList;
	VxSndInstance *			    m_CurSndPlaying{ nullptr };
};

