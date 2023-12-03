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

#include "SoundDefs.h"

#include <QObject>

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
class QSoundEffect;
#else
class QSound;
#endif // QT_VERSION >= QT_VERSION_CHECK(6,0,0)

class QTimer;

class VxSndInstance : public QObject
{
	Q_OBJECT
public:
	VxSndInstance( ESndDef sndDef, QObject *parent = 0);
	virtual ~VxSndInstance();

	void						setSndDef( ESndDef sndDef )				{ m_SndDef = sndDef; }
	ESndDef						getSndDef( void )						{ return m_SndDef; }

	bool						isPlaying( void )						{ return m_IsPlaying; }
	
	void						startPlay( bool loopContinuous = false );
	void						stopPlay( void );

signals:
	void						signalSndFinished( VxSndInstance * thisInstance );

protected slots:
	void						slotCheckForFinish( void );

protected:
	virtual void				initSndInstance( void );

	//=== vars ===//
	ESndDef						m_SndDef;
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	QSoundEffect*				m_QSound;
#else
	QSound*						m_QSound;
#endif // QT_VERSION >= QT_VERSION_CHECK(6,0,0)

	QTimer *					m_CheckFinishTimer;
	bool						m_IsPlaying;
	bool						m_IsInitialized;
};

