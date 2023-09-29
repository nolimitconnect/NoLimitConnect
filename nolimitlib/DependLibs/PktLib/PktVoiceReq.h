#pragma once

//============================================================================
// Copyright (C) 2014 Brett R. Jones 
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

#include "VxPktHdr.h"
#include <CoreLib/IsBigEndianCpu.h>
#include <GuiInterface/IAudioDefs.h>

#define VOICE_PACKET_MAX_COMPRESSED_LEN AUDIO_BUF_SIZE

#pragma pack(push)
#pragma pack(1)
class PktVoiceReq : public VxPktHdr
{
public:
	PktVoiceReq();

	void calcPktLen( void );

	void						setFrame1Len( uint16_t len )			{ m_u16Frame1DataLen = htons( len ); }
	uint16_t					getFrame1Len( void )					{ return ntohs( m_u16Frame1DataLen ); }
	void						setFrame2Len( uint16_t len )			{ m_u16Frame2DataLen = htons( len ); }
	uint16_t					getFrame2Len( void )					{ return ntohs( m_u16Frame2DataLen ); }
	void						setFrame3Len( uint16_t len )			{ m_u16Frame3DataLen = htons( len ); }
	uint16_t					getFrame3Len( void )					{ return ntohs( m_u16Frame3DataLen ); }
	void						setFrame4Len( uint16_t len )			{ m_u16Frame4DataLen = htons( len ); }
	uint16_t					getFrame4Len( void )					{ return ntohs( m_u16Frame4DataLen ); }

	uint8_t *					getCompressedData( void )				{ return m_CompressedData; }

	void						setTimeMs( int64_t time )				{ m_s64TimeMs = htonU64( time ); }
	int64_t 					getTimeMs( void )						{ return ntohU64( m_s64TimeMs ); }

private:
	//=== vars ===//
	uint16_t					m_u8CompressionType{ 1 };
	uint16_t					m_u8CompressionVersion{ 1 };
	int64_t					    m_s64TimeMs{ 0 };
	uint16_t					m_u16Frame1DataLen{ 0 };
	uint16_t					m_u16Frame2DataLen{ 0 };
	uint16_t					m_u16Frame3DataLen{ 0 };
	uint16_t					m_u16Frame4DataLen{ 0 };
	uint8_t						m_CompressedData[ VOICE_PACKET_MAX_COMPRESSED_LEN ];
};

#pragma pack(pop)


