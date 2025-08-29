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

#include <GuiInterface/IDefs.h>
#include <memory.h>

class RawAudio
{
public:
	RawAudio( int16_t *	pcmData, uint16_t pcmDataLen, EMediaModule mediaModule )
		: m_PcmData( (int16_t*)new uint8_t[ pcmDataLen ] )
		, m_PcmDataLen( pcmDataLen )
		, m_MediaModule( mediaModule )
	{
		memcpy( m_PcmData, pcmData, pcmDataLen );
	}

	RawAudio( int16_t pcmDataLen, bool clearData, EMediaModule mediaModule )
		: m_PcmData( (int16_t*)new uint8_t[ pcmDataLen ] )
		, m_PcmDataLen( pcmDataLen )
		, m_MediaModule( mediaModule )
	{
		if( clearData )
		{
			muteAudioContents();
		}
	}

	~RawAudio()
	{
		delete m_PcmData;
	}

	EMediaModule				getMediaModule( void )		{ return m_MediaModule; };
	int16_t *					getDataBuf( void )			{ return m_PcmData; }
	bool						getIsSilence( void )		{ return m_IsSilence; }

	void						muteAudioContents( void )	{ memset( m_PcmData, 0, m_PcmDataLen ); m_IsSilence = true; }

	//=== vars ===//
	int16_t *					m_PcmData;
	uint16_t					m_PcmDataLen; 
	EMediaModule				m_MediaModule{ eMediaModuleInvalid };
	bool						m_IsSilence{ false };
};
