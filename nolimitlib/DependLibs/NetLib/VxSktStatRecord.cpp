//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxSktStatRecord.h"

#include <CoreLib/VxTime.h>

//============================================================================
VxSktStatRecord::VxSktStatRecord( SOCKET skt )
	: m_SktHandle( skt )
{
	m_TimeMs = GetGmtTimeMs();
}

//============================================================================
VxSktStatRecord::VxSktStatRecord( SOCKET skt, std::string& ipAddr )
	: m_SktHandle( skt )
	, m_IpAddr( ipAddr )
{
	m_TimeMs = GetGmtTimeMs();
}

//============================================================================
VxSktStatRecord::VxSktStatRecord( SOCKET skt, std::string& ipAddr, ESktType sktType, EConnectReason connectReason )
	: m_SktHandle( skt )
	, m_SktType( sktType )
	, m_ConnectReason( connectReason )
	, m_IpAddr( ipAddr )
{
	m_TimeMs = GetGmtTimeMs();
}

//============================================================================
VxSktStatRecord::VxSktStatRecord( SOCKET skt, ESktType sktType, std::string& ipAddr, VxGUID signatureId )
: m_SktHandle( skt )
, m_SktType( sktType )
, m_IpAddr( ipAddr )
, m_SignatureId( signatureId )
{
	m_TimeMs = GetGmtTimeMs();
}

//============================================================================
VxSktStatRecord::VxSktStatRecord( const VxSktStatRecord& rhs )
	: m_SktHandle( rhs.m_SktHandle )
	, m_SktType( rhs.m_SktType )
	, m_ConnectReason( rhs.m_ConnectReason )
	, m_IpAddr( rhs.m_IpAddr )
	, m_TimeMs( rhs.m_TimeMs )
	, m_SignatureId( rhs.m_SignatureId )
{
}

//============================================================================
VxSktStatRecord& VxSktStatRecord::operator=( const VxSktStatRecord& rhs )
{
	if( this != &rhs )
	{
		m_SktHandle = rhs.m_SktHandle;
		m_SktType = rhs.m_SktType;
		m_ConnectReason = rhs.m_ConnectReason;
		m_IpAddr = rhs.m_IpAddr;
		m_TimeMs = rhs.m_TimeMs;
		m_SignatureId = rhs.m_SignatureId;
	}

	return *this;
}
