//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================
#pragma once

#include <GuiInterface/IDefs.h>
#include <CoreLib/VxGUID.h>

#include <string>

class VxSktStatRecord
{
public:
	VxSktStatRecord() = default;
	VxSktStatRecord( SOCKET skt );
	VxSktStatRecord( SOCKET skt, std::string& ipAddr );
	VxSktStatRecord( SOCKET skt, std::string& ipAddr, ESktType sktType, EConnectReason connectReason );
	VxSktStatRecord( SOCKET skt, ESktType sktType, std::string& ipAddr, VxGUID signatureId = VxGUID::nullVxGUID() );
	VxSktStatRecord( const VxSktStatRecord& rhs );
	virtual ~VxSktStatRecord() = default;

	VxSktStatRecord&			operator=( const VxSktStatRecord& rhs );

	SOCKET						getSktHandle( void )			{ return m_SktHandle; };
	void						setIpAddr( std::string& ipAddr ) { m_IpAddr = ipAddr; }
	std::string&				getIpAddr( void )				{ return m_IpAddr; }
	void						setSktType( ESktType sktType )	{ m_SktType = sktType; }
	ESktType					getSktType( void )				{ return m_SktType; }
	EConnectReason				getConnectReason( void )		{ return m_ConnectReason; }
	int64_t						getActiveTimestampMs( void )	{ return m_TimeMs; }
	VxGUID&						getSignature( void )			{ return m_SignatureId; }

protected:
	SOCKET						m_SktHandle{ INVALID_SOCKET };
	ESktType					m_SktType{ eSktTypeNone };
	EConnectReason				m_ConnectReason{ eConnectReasonUnknown };
	std::string					m_IpAddr{ "" };
	int64_t						m_TimeMs{ 0 };
	VxGUID						m_SignatureId;
};

