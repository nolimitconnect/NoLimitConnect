//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxConnectId.h"
#include "PktBlobEntry.h"

#include <NetLib/VxSktUtil.h>
#include <CoreLib/VxParse.h>
#include <CoreLib/VxDebug.h>

#include <memory.h>

//============================================================================
VxConnectId::VxConnectId( const VxConnectId &rhs )
: VxGUID(rhs)
, m_IPv6OnlineIp( rhs.m_IPv6OnlineIp )
, m_IPv4OnlineIp( rhs.m_IPv4OnlineIp )
, m_u16OnlinePort( rhs.m_u16OnlinePort )
{
}

//============================================================================
bool VxConnectId::addToBlob( PktBlobEntry& blob )
{
    bool result = blob.setValue( *( (VxGUID *)this ) );
    result &= m_IPv6OnlineIp.addToBlob( blob );
    result &= m_IPv4OnlineIp.addToBlob( blob );
    result &= blob.setValue( m_u16OnlinePort );
	result &= blob.setValue( m_u16ResConnectId1 );
    return result;
}

//============================================================================
bool VxConnectId::extractFromBlob( PktBlobEntry& blob )
{
    bool result = blob.getValue( *( (VxGUID *)this ) );
    result &= m_IPv6OnlineIp.extractFromBlob( blob );
    result &= m_IPv4OnlineIp.extractFromBlob( blob );
    result &= blob.getValue( m_u16OnlinePort );
	result &= blob.getValue( m_u16ResConnectId1 );
    return result;
}

//============================================================================
VxConnectId& VxConnectId::operator =( const VxConnectId &rhs )
{
	if( this != &rhs )
	{
        *((VxGUID*)this) = *((VxGUID*)&rhs);
        m_IPv6OnlineIp = rhs.m_IPv6OnlineIp;
        m_IPv4OnlineIp = rhs.m_IPv4OnlineIp;
        m_u16OnlinePort = rhs.m_u16OnlinePort;
		m_u16ResConnectId1 = rhs.m_u16ResConnectId1;
	}

	return *this;
}

//============================================================================
bool VxConnectId::operator ==( const VxConnectId &rhs )  const
{
	return ( 0 == memcmp( this, &rhs, sizeof( VxConnectId )) );
}

//============================================================================
bool VxConnectId::operator !=( const VxConnectId &rhs )  const
{
	return ( 0 != memcmp( this, &rhs, sizeof( VxConnectId )) );
}

//============================================================================
bool VxConnectId::setIpAddress( bool ipv6, std::string ipAddr, bool* retIpHasChanged  )		
{
	if( ipAddr.empty() )
	{
		return false;
	}

	if( retIpHasChanged )
	{
		*retIpHasChanged = false;
	}

    bool valid = false;
	InetAddress inetAddr;
	inetAddr.setIp( ipAddr.c_str() );
	if( inetAddr.isValid() )
	{
		if( inetAddr.isIPv6() && ipv6 )
		{
			if( retIpHasChanged )
            {
                if( m_IPv6OnlineIp.toStdString() != ipAddr )
                {
                    *retIpHasChanged = true;
                }
			}

			m_IPv6OnlineIp = inetAddr; 
			valid = m_IPv6OnlineIp.isValid();
		}
		else
		{
			bool changed = m_IPv4OnlineIp.setIp( inetAddr.getIPv4AddressInNetOrder() );
			valid = m_IPv4OnlineIp.isValid();
			if( retIpHasChanged && changed && valid )
			{
				*retIpHasChanged = true;
			}
		}
	}

	vx_assert( valid );
    return valid;
};

//============================================================================
bool VxConnectId::getIpAddress( bool ipv6, std::string& retString )
{
	if( ipv6 && m_IPv6OnlineIp.isValid() )
	{
		retString = m_IPv6OnlineIp.toStdString();
		return true;
	}
	else if( m_IPv4OnlineIp.isValid() )
	{
		retString = m_IPv4OnlineIp.toStdString(); 
		return true;
	}

	retString.clear();
	return false;
};

//============================================================================
bool VxConnectId::isIpAddressValid( bool ipv6 )
{
	return ipv6 ? m_IPv6OnlineIp.isValid() : m_IPv4OnlineIp.isValid();
}

//============================================================================
void VxConnectId::clear( void )
{
	clearVxGUID();
	m_u16OnlinePort = 0;
	m_IPv4OnlineIp.setToInvalid();
	m_IPv6OnlineIp.setToInvalid();
}
