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

#include "PktTypes.h"
#include "VxCommon.h"

#include <GuiInterface/IDefs.h>

#pragma pack(push)
#pragma pack(1)

class PktRelayUserDisconnect : public VxPktHdr
{
public:
	PktRelayUserDisconnect();

	void						setDestUserOnlineId( VxGUID& onlineId )	{ m_DestUserOnlineId = onlineId; }
	VxGUID&						getDestUserOnlineId( void )				{ return m_DestUserOnlineId; }

	void						setHostOnlineId( VxGUID& onlineId )		{ m_HostOnlineId = onlineId; }
	VxGUID&						getHostOnlineId( void )					{ return m_HostOnlineId; }

	void						setRelayError( ERelayErr relayErr )		{ m_RelayError = (uint8_t)relayErr; }
	ERelayErr					getRelayError( void )					{ return (ERelayErr)m_RelayError; }

	void						setPktType( uint16_t pktType )			{ m_PktType = pktType; }
	uint16_t					getPktType( void )						{ return m_PktType; }

	//=== vars ===//
	VxGUID						m_DestUserOnlineId;
	VxGUID						m_HostOnlineId;

private:
	uint8_t						m_RelayError{ 0 };
	uint8_t						m_u8Res1{ 0 };
	uint16_t					m_PktType{ 0 };
	uint32_t					m_u32Res3{ 0 };
};

#pragma pack(pop)

