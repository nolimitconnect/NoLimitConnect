//============================================================================
// Copyright (C) 2022 Brett R. Jones
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
#pragma once

#include <GuiInterface/IDefs.h>
#include <CoreLib/VxGUID.h>

#include <string>

class VxHackerRecord
{
public:
	VxHackerRecord() = default;
	VxHackerRecord( EHackerLevel hackLevel, EHackerReason hackReason, std::string& ipAddr, VxGUID signatureId = VxGUID::nullVxGUID() );
	VxHackerRecord( const VxHackerRecord& rhs );
	virtual ~VxHackerRecord() = default;

	VxHackerRecord&				operator=( const VxHackerRecord& rhs );

	EHackerLevel				getHackerLevel( void )			{ return m_HackerLevel; };
	EHackerReason				getHackerReason( void )			{ return m_HackerReason; };
	std::string&				getIpAddr( void )				{ return m_IpAddr; }
	int64_t						getHackTimestampMs( void )		{ return m_TimeMs; }
	VxGUID&						getSignature( void )			{ return m_SignatureId; }
	int							getOffenseCount( void )			{ return m_HackCount; }

	void						incrementHackCount( void )		{ m_HackCount++; }

protected:
	EHackerLevel				m_HackerLevel{ eHackerLevelUnknown };
	EHackerReason				m_HackerReason{ eHackerReasonUnknown };
	std::string					m_IpAddr;
	int64_t						m_TimeMs{ 0 };
	VxGUID						m_SignatureId;
	int							m_HackCount{ 1 };
};

