#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <CoreLib/VxGUID.h>

#include <CoreLib/InetAddress.h>

#include <string>

#pragma pack(push)
#pragma pack(1)

class PktBlobEntry;

//! derived from 16 bytes
//! size 
//	  16 bytes VxGUID
// +  16 bytes m_IPv6OnlineIp
// +   4 bytes m_IPv4OnlineIp
// +   2 bytes m_u16OnlinePort
// +   2 bytes m_u16ResConnectId1
// =  40 bytes total
class VxConnectId : public VxGUID
{
public:
	VxConnectId() = default;
	VxConnectId( const VxConnectId& rhs );
    bool                        addToBlob( PktBlobEntry& blob );
    bool                        extractFromBlob( PktBlobEntry& blob );

	VxConnectId&                operator = ( const VxConnectId& rhs );
	bool                        operator != (const VxConnectId& rhs ) const;
	bool                        operator == (const VxConnectId& rhs ) const;

	void						setPort( uint16_t port )			{ m_u16OnlinePort = htons( port ); }
	uint16_t					getPort( void )						{ return ntohs( m_u16OnlinePort ); }

	bool						setIpAddress( bool ipv6, std::string ipAddr, bool* retIpHasChanged = nullptr ); 
	bool						getIpAddress( bool ipv6, std::string& retIpAddr );

	bool						isIpAddressValid( bool ipv6 );

	InetAddress&				getIPv6( void )						{ return m_IPv6OnlineIp; }
	InetAddrIPv4&				getIPv4( void )						{ return m_IPv4OnlineIp; }

	void						clear( void );

	VxGUID&						getOnlineId( void )					{ return *this; }


	//=== vars ===//
	InetAddress					m_IPv6OnlineIp;						// users ip6 address
	InetAddrIPv4				m_IPv4OnlineIp;						// users ip4 address

protected:
	uint16_t					m_u16OnlinePort{ 0 };				// users incoming connection port
	uint16_t					m_u16ResConnectId1{ 0 };			// reserved for possible ipv6 port or local port
};

#pragma pack(pop)

