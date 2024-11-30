#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget>

#include "SoundDefs.h"

#include <CoreLib/MediaCallbackInterface.h>
#include <CoreLib/VxGUID.h>

#include <vector>

class MediaProcessor;

class VxSndInstance : public QObject, public MediaCallbackInterface
{
public:
	VxSndInstance( ESndDef sndDef, QObject* parent = nullptr );
	virtual ~VxSndInstance();

	void						setSndDef( ESndDef sndDef ) { m_SndDef = sndDef; }
	ESndDef						getSndDef( void ) { return m_SndDef; }

	bool						isPlaying( void ) { return m_IsPlaying; }

	bool						startPlay( bool loopContinuous = false );
	void						stopPlay( void );

protected:
	bool						initSndInstance( void );

	void						callbackAudioOutSpaceAvail( int freeSpaceLen ) override;

	void						wantAudioCallbacks( bool wantCallbacks );

	//=== vars ===//
	ESndDef						m_SndDef{ eSndDefNone };
	MediaProcessor*				m_MediaProcessor{ nullptr };
	VxGUID						m_MyOnlineId;

	bool						m_IsPlaying{ false };
	bool						m_IsInitialized{ false };
	std::string					m_WavFileName;
	VxGUID						m_MediaSessionId;
	std::vector<int16_t>		m_WavSamples;
	int							m_WavRate{ 0 };
	int							m_WavChannels{ 0 };
	int							m_PlaySndIdx{ 0 };
	bool						m_AudioCallbacksRequested{ 0 };
};

