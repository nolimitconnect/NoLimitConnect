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

#include "VxHackerRecord.h"

#include <CoreLib/VxTime.h>

//============================================================================
VxHackerRecord::VxHackerRecord( EHackerLevel hackLevel, EHackerReason hackReason, std::string& ipAddr, VxGUID signatureId )
: m_HackerLevel( hackLevel )
, m_HackerReason( hackReason )
, m_IpAddr( ipAddr )
, m_SignatureId( signatureId )
{
	m_TimeMs = GetGmtTimeMs();
}

//============================================================================
VxHackerRecord::VxHackerRecord( const VxHackerRecord& rhs )
	: m_HackerLevel( rhs.m_HackerLevel )
	, m_HackerReason( rhs.m_HackerReason )
	, m_IpAddr( rhs.m_IpAddr )
	, m_TimeMs( rhs.m_TimeMs )
	, m_SignatureId( rhs.m_SignatureId )
	, m_HackCount( rhs.m_HackCount )
{
}

//============================================================================
VxHackerRecord& VxHackerRecord::operator=( const VxHackerRecord& rhs )
{
	if( this != &rhs )
	{
		m_HackerLevel = rhs.m_HackerLevel;
		m_HackerReason = rhs.m_HackerReason;
		m_IpAddr = rhs.m_IpAddr;
		m_TimeMs = rhs.m_TimeMs;
		m_SignatureId = rhs.m_SignatureId;
		m_HackCount = rhs.m_HackCount;
	}

	return *this;
}
