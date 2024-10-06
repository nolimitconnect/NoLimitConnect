//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "InetAddress.h"

#include "InetAddressParse.h"
#include "IsBigEndianCpu.h"

#include "PktBlobEntry.h"
#include "VxDebug.h"
#include "VxParse.h"
#include "VxSktUtil.h"

#include <string>
#include <array>

#ifdef TARGET_OS_WINDOWS
	#include <Winsock2.h>
	#include <Ws2tcpip.h>  
#else
    #include <arpa/inet.h>
    #include <net/if.h>
    #include <netdb.h>        /* getaddrinfo(3) et al.                       */
    #include <netinet/in.h>   /* sockaddr_in & sockaddr_in6 definition.      */
	#include <unistd.h> 
    #include <sys/socket.h>
    #include <sys/types.h>

	#ifndef TARGET_OS_ANDROID
		#include <ifaddrs.h>
	#endif
#if defined(TARGET_OS_ANDROID)
    #include "ifaddrs-android-impl.h"
#endif
    #include <stdio.h>
    #include <sys/types.h>
    #include <ifaddrs.h>
    #include <netinet/in.h>
    #include <string.h>
    #include <arpa/inet.h>
#endif

#include <memory.h>
#include <stdio.h>

namespace 
{
  const uint64_t IP4_BINARY_INDICATOR = 0xffffffffffffffffULL;
};

//============================================================================
// InetAddrIPv4
//============================================================================
InetAddrIPv4::InetAddrIPv4(const InetAddrIPv4& rhs)
: m_u32AddrIPv4(rhs.m_u32AddrIPv4)
{
}

//============================================================================
InetAddrIPv4::InetAddrIPv4( const char* pIpAddress )
: m_u32AddrIPv4(0)
{
	fromString( pIpAddress );
}

//============================================================================
InetAddrIPv4::InetAddrIPv4( uint32_t u32IpAddr )
{
	setIp( u32IpAddr );
}

//============================================================================
bool InetAddrIPv4::addToBlob( PktBlobEntry& blob )
{
    return blob.setValue( m_u32AddrIPv4 );
}

//============================================================================
bool InetAddrIPv4::extractFromBlob( PktBlobEntry& blob )
{
    return blob.getValue( m_u32AddrIPv4 );
}

//============================================================================
bool InetAddrIPv4::fromString( const char* pIpAddress )
{
	if( ( NULL == pIpAddress ) || 
		( 0 == strlen( pIpAddress ) ) ||
		( 0 == strcmp( "0.0.0.0", pIpAddress ) ) ||
		( 0 == strcmp( "::", pIpAddress ) ) )
	{
		LogMsg( LOG_ERROR, "InetAddrIPv4::%s %s is invalid", __func__, pIpAddress ? pIpAddress : "null" );
		setToInvalid();
		return false;
	}
	else if( isIPv4String( pIpAddress ) )
	{
		uint32_t u32Ip;
		VxIPv4_pton( pIpAddress, &u32Ip, false );
		setIp( u32Ip );
	}
	else
	{
		LogMsg( LOG_ERROR, "InetAddrIPv4::%s %s is not ipv4", __func__, pIpAddress );
	}
}

//============================================================================
std::string InetAddrIPv4::toString( void )
{
	std::string retIpAddress;
	char as8Buf[ INET6_MAX_STR_LEN ];
	as8Buf[0] = 0; 

	if( isValid() )
	{
		uint32_t u32Ip = getIPv4AddressInNetOrder();
		VxIPv4_ntop( &u32Ip, as8Buf, sizeof( as8Buf ), false );
			//LogMsg( LOG_INFO, "InetAddress::toString %s uint32_t 0x%x host order false\n", as8Buf, u32Ip );
		retIpAddress = as8Buf;
	}
	else
	{
		retIpAddress = "0.0.0.0";
	}

	return retIpAddress;
}

//============================================================================
InetAddrIPv4& InetAddrIPv4::operator=( const InetAddrIPv4& inetAddr ) 
{
	if( inetAddr.isValid() )
	{
		m_u32AddrIPv4 = inetAddr.getIPv4AddressInNetOrder();
	}
	else
	{
		setToInvalid();
	}

	return *this;
}

//============================================================================
bool InetAddrIPv4::operator != ( const InetAddrIPv4& inetAddr )  const
{
	return ! (inetAddr.isIPv4() && ( m_u32AddrIPv4 == inetAddr.getIPv4AddressInNetOrder() ));
}

//============================================================================
bool InetAddrIPv4::operator == (const InetAddrIPv4& inetAddr)  const
{
	return inetAddr.isIPv4() && ( m_u32AddrIPv4 == inetAddr.getIPv4AddressInNetOrder() );
}

//============================================================================
InetAddrIPv4& InetAddrIPv4::operator=( const InetAddress& inetAddr ) 
{
	if( inetAddr.isIPv4() && inetAddr.isValid() )
	{
		setIp( inetAddr.getIPv4AddressInNetOrder() );
	}
	else
	{
		setToInvalid();
		LogMsg( LOG_ERROR, "ERROR InetAddrIPv4::operator=( const InetAddress& inetAddr ) is invalid" );
	}

	return *this;
}

//============================================================================
bool InetAddrIPv4::operator != ( const InetAddress& inetAddr )  const
{
	return ! (inetAddr.isIPv4() && ( this->m_u32AddrIPv4 == inetAddr.getIPv4AddressInNetOrder() ));
}

//============================================================================
bool InetAddrIPv4::operator == (const InetAddress& inetAddr)  const
{
	return (inetAddr.isIPv4() && ( this->m_u32AddrIPv4 == inetAddr.getIPv4AddressInNetOrder() ));
}

//============================================================================
bool InetAddrIPv4::isValid( void ) const
{
	return ( 0 != m_u32AddrIPv4 );
}

//============================================================================
void InetAddrIPv4::setToInvalid( void )
{
	m_u32AddrIPv4 = 0;
}

//============================================================================
bool InetAddrIPv4::isLoopBack() const
{
	return ( 0x7f000001 != m_u32AddrIPv4 );
}

//============================================================================
InetAddress InetAddrIPv4::toInetAddress( void )
{
	InetAddress inetAddr;
	inetAddr.setIp( this->getIPv4AddressInNetOrder(), false );
	return inetAddr;
}

//============================================================================
bool InetAddrIPv4::isIPv4String( const char* pIpAddress ) const
{
	if( pIpAddress
		&& (strlen(pIpAddress) < 16 )
		&& strchr(pIpAddress, '.')
		&& ( 0 == strchr(pIpAddress, ':')) )
	{
		return true;
	}

	return false;
}

//============================================================================
uint32_t InetAddrIPv4::getIPv4AddressInHostOrder( void ) const
{
	return ntohl( m_u32AddrIPv4 );
}

//============================================================================
uint32_t InetAddrIPv4::getIPv4AddressInNetOrder( void ) const
{
	return m_u32AddrIPv4;
}

//============================================================================
// note.. internally kept in network order instead of host order
bool InetAddrIPv4::setIp( uint32_t u32IPv4Addr, bool bIsHostOrder )
{
    uint32_t oldAddrIPv4 = m_u32AddrIPv4;
	if( bIsHostOrder )
	{
		u32IPv4Addr = htonl( u32IPv4Addr );
	}

	if( u32IPv4Addr )
	{
		m_u32AddrIPv4 = u32IPv4Addr;
	}
	else
	{
		setToInvalid();
	}

    return oldAddrIPv4 != m_u32AddrIPv4;
}

//============================================================================
bool InetAddrIPv4::setIp( const char* pIp )
{
    uint32_t oldAddrIPv4 = m_u32AddrIPv4;
	if( !fromString( pIp ) )
	{
		return false;
	}

    return oldAddrIPv4 != m_u32AddrIPv4;
}

//============================================================================
//! returns port in host order
uint16_t InetAddrIPv4::setIp( struct sockaddr_in& oIPv4Addr )
{
	setIp(*((uint32_t*)&oIPv4Addr.sin_addr), true );
	return ntohs( oIPv4Addr.sin_port );
}

//============================================================================
//! returns port in host order
uint16_t InetAddrIPv4::setIp( struct sockaddr& ipAddr )
{
	if( AF_INET == ipAddr.sa_family )
	{
		return setIp( *((sockaddr_in *)&ipAddr) );
	}
	else if( AF_INET6 == ipAddr.sa_family )
	{
		LogMsg( LOG_ERROR, "ERROR InetAddrIPv4::setIp tried to set IPv6 address" );
		setToInvalid();
		return 0;
	}
	else
	{
		LogMsg( LOG_ERROR, "InetAddress::setIp unknown family" );
		return 0;
	}
}

//============================================================================
//! returns port in host order
uint16_t InetAddrIPv4::setIp( struct sockaddr_storage& oAddr )
{
	switch( oAddr.ss_family ) 
	{
	case AF_INET:
		return setIp(*((struct sockaddr_in *)&oAddr));
		break;

	case AF_INET6:
		LogMsg( LOG_ERROR, "ERROR InetAddrIPv4::setIp tried to set IPv6 address" );
		setToInvalid();
		break;

	default:
		//vx_assert(false);
		break;
	};

	return 0;
}

//============================================================================
bool InetAddrIPv4::isLocalAddress( void ) const
{
	uint32_t hostOrderIpv4 = getIPv4AddressInHostOrder();
	if( ( 0x00000000 == ( hostOrderIpv4 & 0xff000000 ) ) || // 0.
		( 0x7f000000 == ( hostOrderIpv4 & 0xff000000 ) ) || // 127.
		( 0x0a000000 == ( hostOrderIpv4 & 0xff000000 ) ) || // 10.		
		( 0xc0a80000 == ( hostOrderIpv4 & 0xffff0000 ) ) || // 192.168.
		( 0xa9fe0000 == ( hostOrderIpv4 & 0xffff0000 ) ) || // 169.254.
		( 0xac100000 == ( hostOrderIpv4 & 0xfff00000 ) ) )  // 172.16?
	{
		return true;
	}

	return false;
}

//============================================================================
//! fill address with this ip address and the given port
int InetAddrIPv4::fillAddress( struct sockaddr_storage& oAddr, uint16_t u16Port )
{
	return fillAddress( *((struct sockaddr_in*)&oAddr), u16Port );
}

//============================================================================
//! fill address with this ip address and the given port.. returns struct len
int InetAddrIPv4::fillAddress( struct sockaddr_in& oIPv4Addr, uint16_t u16Port )
{
	// setup the address and port
	memset( &oIPv4Addr, 0, sizeof( sockaddr_in ) );

	oIPv4Addr.sin_family			= AF_INET;
	*((long*)&oIPv4Addr.sin_addr)	= getIPv4AddressInHostOrder();

	oIPv4Addr.sin_port				= htons( u16Port );
	return (int)sizeof( struct sockaddr_in);
}

//============================================================================
//! returns port in host order
uint16_t InetAddrIPv4::getIpFromAddr(const struct sockaddr *sa, std::string& retStr)
{
	uint16_t u16Port = 0;
	char as8Addr[ INET6_MAX_STR_LEN ];
	switch(sa->sa_family)
	{
	case AF_INET:
		VxIPv4_ntop(&(((struct sockaddr_in *)sa)->sin_addr), as8Addr, sizeof(as8Addr), true );
		u16Port = ntohs( (((struct sockaddr_in *)sa)->sin_port) );
		break;

	case AF_INET6:
		VxIPv6_ntop( &(((struct sockaddr_in6 *)sa)->sin6_addr), as8Addr, sizeof(as8Addr) );
		u16Port = ntohs( (((struct sockaddr_in6 *)sa)->sin6_port) );
		break;

	default:
		strcpy(as8Addr, "Unknown AF");
		return 0;
	}
	retStr = as8Addr;
	return u16Port;
}

//============================================================================
bool InetAddrIPv4::isLittleEndian( void )
{
	return ( ntohl(4L) == 4L );
}

//============================================================================
uint32_t InetAddrIPv4::swap32Bit( uint32_t src )
{
	return htonl( src );
}

//============================================================================
// InetAddrIPv4AndPort
//============================================================================
InetAddrIPv4AndPort::InetAddrIPv4AndPort( const InetAddrIPv4AndPort& rhs )
    : InetAddrIPv4( rhs )
    , m_u16Port( rhs.m_u16Port )
{
}

//============================================================================
bool InetAddrIPv4AndPort::addToBlob( PktBlobEntry& blob )
{
    bool result = InetAddrIPv4::addToBlob( blob );
    result &= blob.setValue( m_u16Port );
    return result;
}

//============================================================================
bool InetAddrIPv4AndPort::extractFromBlob( PktBlobEntry& blob )
{
    bool result = InetAddrIPv4::extractFromBlob( blob );
    result &= blob.getValue( m_u16Port );
    return result;
}

//============================================================================
bool InetAddrIPv4AndPort::fromString( const char* pIpAddress )
{
	uint16_t port = 0;
	bool parsedIsIpv6 = false;
	uint8_t buf[16];
	if( ParseIPv4OrIPv6( pIpAddress, buf, port, parsedIsIpv6 ) )
	{
		if( !parsedIsIpv6 )
		{
			m_u32AddrIPv4 = *((uint32_t*)(&buf[0]));
			if( port )
			{
				m_u16Port = port;
			}
			else
			{
				LogMsg( LOG_ERROR, "%s no port specified in address %s", __func__, pIpAddress );
			}
		}
		else
		{
			LogMsg( LOG_ERROR, "%s attempted to set ip6 address %s", __func__, pIpAddress );
		}

		return port;
	}
	else
	{
		LogMsg( LOG_ERROR, "%s invalid ip %s", __func__, pIpAddress );
	}

	return false;
}

//============================================================================
std::string InetAddrIPv4AndPort::toString( bool includePort )
{
	std::string ipAddr = InetAddrIPv4::toString();
	if( includePort && !ipAddr.empty() && m_u16Port )
	{
		ipAddr += ":";
		ipAddr += std::to_string( m_u16Port );
	}

	return ipAddr;
}

//============================================================================
InetAddrIPv4AndPort& InetAddrIPv4AndPort::operator=(const InetAddrIPv4& inetAddr) 
{
	if( this != &inetAddr )
	{
		m_u32AddrIPv4	= inetAddr.getIPv4AddressInNetOrder();
	}

	return *this;
}

//============================================================================
//! equal operator
InetAddrIPv4AndPort& InetAddrIPv4AndPort::operator=(const InetAddrIPv4AndPort& inetAddr) 
{
	if( this != &inetAddr )
	{
		m_u32AddrIPv4	= inetAddr.getIPv4AddressInNetOrder();
		m_u16Port		= inetAddr.getPort();
	}

	return *this;
}

//============================================================================
InetAddrIPv4AndPort& InetAddrIPv4AndPort::operator=(const InetAddress& inetAddr) 
{
	m_u32AddrIPv4	= inetAddr.getIPv4AddressInNetOrder();
	return *this;
}

//============================================================================
InetAddrIPv4AndPort& InetAddrIPv4AndPort::operator=(const InetAddrAndPort& inetAddr) 
{
	m_u16Port		= inetAddr.getPort();
	m_u32AddrIPv4	= inetAddr.getIPv4AddressInNetOrder();
	return *this;
}

//============================================================================
void InetAddrIPv4AndPort::setIpAndPort( struct sockaddr_storage& oAddr )
{
	m_u16Port = setIp( oAddr );
}

//============================================================================
void InetAddrIPv4AndPort::setIpAndPort( struct sockaddr& oAddr )
{
	m_u16Port = setIp( oAddr );
}

//============================================================================
//============================================================================
//============================================================================
//============================================================================
// InetAddress

//============================================================================
InetAddress::InetAddress( const char* pIpAddress )
: m_u64AddrHi(0)
, m_u64AddrLo(0)
{
	fromString( pIpAddress );
}

//============================================================================
InetAddress::InetAddress( uint32_t u32IpAddr )
{
	setIp( u32IpAddr );
}

//============================================================================
InetAddress::InetAddress( const InetAddress& rhs )
    : m_u64AddrHi( rhs.m_u64AddrHi )
    , m_u64AddrLo( rhs.m_u64AddrLo )
{
}

//============================================================================
bool InetAddress::addToBlob( PktBlobEntry& blob )
{
    bool result = blob.setValue( m_u64AddrHi );
    result &= blob.setValue( m_u64AddrLo );
    return result;
}

//============================================================================
bool InetAddress::extractFromBlob( PktBlobEntry& blob )
{
    bool result = blob.getValue( m_u64AddrHi );
    result &= blob.getValue( m_u64AddrLo );
    return result;
}

//============================================================================
uint16_t InetAddress::fromString( const char* pIpAddress )
{
	if( !pIpAddress || 
		( 0 == strlen( pIpAddress ) ) ||
		( 0 == strcmp( "0.0.0.0", pIpAddress ) ) )
	{
		LogMsg( LOG_ERROR, "InetAddress::fromString invalid ip %s", pIpAddress ? pIpAddress : "NULL" );
		setToInvalid();
		return 0;
	}

	uint16_t port = 0;
	bool parsedIsIpv6 = false;
	uint8_t ipBinary[16];
	if( ParseIPv4OrIPv6( pIpAddress, ipBinary, port, parsedIsIpv6 ) )
	{
		// the parser sets in the order is is parsed which host order but we want is stored as net order
		if( parsedIsIpv6 )
		{
			m_u64AddrLo = htonU64( *((uint64_t*)ipBinary) );
			m_u64AddrHi = htonU64( *((uint64_t*)(&ipBinary[8])));
		}
		else
		{
			m_u64AddrLo = htonl( *((uint32_t*)ipBinary) );
			m_u64AddrHi = IP4_BINARY_INDICATOR; // indicator is a ipv4 address
		}

		return port;
	}

	setToInvalid();
	return 0;
}

//============================================================================
std::string InetAddress::toString( void )
{
	std::string retIpAddress;
	std::array<char, INET6_MAX_STR_LEN> as8Buf;
	as8Buf.data()[0] = 0; 

	if( !isValid() )
	{
		return retIpAddress;
	}

	if( isIPv4() )
	{
		uint32_t u32Ip = getIPv4AddressInNetOrder();
        VxIPv4_ntop( &u32Ip, as8Buf.data(), as8Buf.size(), false);
		//LogMsg( LOG_INFO, "InetAddress::toString %s uint32_t 0x%x host order false\n", as8Buf, u32Ip );
	}
	else
	{
		VxIPv6_ntop( this, as8Buf.data(), as8Buf.size());
	}

	retIpAddress = as8Buf.data();
	if( retIpAddress.empty() )
	{
		retIpAddress = "";
	}

	return retIpAddress;
}

//============================================================================
EIpAddrType InetAddress::getIpAddrType( void )
{
	EIpAddrType addrType{ eIpAddrTypeUnknown };
	if( isValid() )
	{
		addrType = isIPv4() ? eIpAddrTypeIpv4 : eIpAddrTypeIpv6;
	}

	return addrType;
}

//============================================================================
InetAddress InetAddress::getDefaultIp( void )
{
	 std::vector<InetAddress> retAddresses;
	 getAllAddresses( retAddresses );
	 if( retAddresses.size() )
	 {
		 return retAddresses[0];
	 }
	 return InetAddress();
}

//============================================================================
int InetAddress::getAllAddresses( std::vector<InetAddress>& retAddresses )
{
#if defined(TARGET_OS_WINDOWS) || defined(TARGET_OS_ANDROID)
	// for unknown reasons this code that works on windows only return loopback in android
	// NO known fix and the linux version not viable because of missing ifaddr.h
	// TODO.. if android fetch addresses from JAVA
	char as8HostName[ 1025 ];
	//first get host name
	if( gethostname( as8HostName, sizeof( as8HostName ) ) )
	{
        LogMsg( LOG_ERROR, "getAllAddresses: Unable to get host name" );
		#ifdef TARGET_OS_WINDOWS
			return WSAGetLastError();
		#else
			return VxGetLastError();
		#endif // TARGET_OS_WINDOWS
	}

	struct addrinfo Hints;
	struct addrinfo * AI;
	struct addrinfo * AddrInfo;

	memset(&Hints, 0, sizeof(Hints));

    Hints.ai_family   = PF_UNSPEC;
    Hints.ai_socktype = SOCK_STREAM;
    Hints.ai_protocol = IPPROTO_TCP;
    //Hints.ai_flags    = AI_PASSIVE;
	Hints.ai_flags = AI_ADDRCONFIG;

	char as8Buf[16];
	sprintf( as8Buf, "%d", 65000 );

	int RetVal = getaddrinfo(as8HostName, "echo", &Hints, &AddrInfo);
	if (RetVal != 0)
	{
		//char * pErr = gai_strerror(RetVal);
		//printf("getaddrinfo() failed with error %d: %s\n", RetVal, pErr );
        LogMsg( LOG_ERROR, "InetAddress::getAllAddresses getaddrinfo error %d", RetVal );
		return RetVal;
	}

	for(  AI = AddrInfo; AI != NULL; AI = AI->ai_next )
	{
		if ((AI->ai_family != PF_INET) && (AI->ai_family != PF_INET6))
		{
			continue;
		}
		// Open a socket with the correct address family for this address.
		SOCKET oSkt = socket(AI->ai_family, AI->ai_socktype, AI->ai_protocol);

		if( oSkt == INVALID_SOCKET )
		{
			continue;
		}

        struct sockaddr_storage * poSktAddr = (struct sockaddr_storage *)AI->ai_addr;
        InetAddress oTestAddr;
        VxSetSktAddressPort( poSktAddr, 0 );
        oTestAddr.setIp( *poSktAddr );
        std::string strTestIpAddress = oTestAddr.toString();        

        if( ( false == oTestAddr.isValid() ) ||
            ( oTestAddr.isLoopBack() ) )
        {
            if( ! oTestAddr.isLoopBack() )
            {
                LogMsg( LOG_INFO, "InetAddress::%s: invalid addr %s", __func__, strTestIpAddress.c_str() );
            }

            VxCloseSkt( oSkt );
            continue;
        }

		//LogMsg( LOG_INFO, "binding skt %d\n", oSkt );
		if( false == VxBindSkt( oSkt, poSktAddr ) )
        {
            LogMsg( LOG_INFO, "InetAddress::%s: could not bind addr %s", __func__, strTestIpAddress.c_str() );
            continue;
        }

        if( oTestAddr.isValid() &&
            ( false == oTestAddr.isLoopBack() ) )
        {
            retAddresses.push_back(oTestAddr);
        }

//#define TEST_CONNECTION
#ifdef TEST_CONNECTION
        if( false == VxTestConnection( oSkt, oTestAddr ) )
        {
            LogMsg( LOG_INFO, "Connection using local address %s OK", strTestIpAddress.c_str() );
        }
        else
        {
            LogMsg( LOG_INFO, "Connection using local address %s FAIL", strTestIpAddress.c_str() );
        }
#else
		//LogMsg( LOG_INFO, "closing skt %d\n", oSkt );
        VxCloseSkt( oSkt );
#endif // TEST_CONNECTION
	}

	//LogMsg( LOG_INFO, "freeing addr info\n" );
	freeaddrinfo( AddrInfo ); // free the linked list
#else
    struct ifaddrs *myaddrs, *ifa;
    struct sockaddr_storage * poSktAddr = NULL;
    char buf[64];

    if(getifaddrs(&myaddrs) != 0)
    {
        perror("InetAddress::getAllAddresses: getifaddrs");
        LogMsg( LOG_ERROR, "InetAddress::getAllAddresses: getifaddrs FAIL" );
    }

    for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;
        if (!(ifa->ifa_flags & IFF_UP))
            continue;

        switch (ifa->ifa_addr->sa_family)
        {
            case AF_INET:
            case AF_INET6:
            {
                poSktAddr = (struct sockaddr_storage *)ifa->ifa_addr;
                break;
            }

            default:
                continue;
        }

        if (!inet_ntop(ifa->ifa_addr->sa_family, poSktAddr, buf, sizeof(buf)))
        {
           LogMsg( LOG_ERROR, "InetAddress::getAllAddresses: %s: inet_ntop failed!", ifa->ifa_name);
        }
        else
        {
            // Open a socket with the correct address family for this address.
            SOCKET oSkt = socket(ifa->ifa_addr->sa_family,
                                 SOCK_STREAM,
                                 0 );

            if( oSkt == INVALID_SOCKET )
            {
                continue;
            }

            //LogMsg( LOG_INFO, "InetAddress::getAllAddresses: %s: %s\n", ifa->ifa_name, buf);
            InetAddress oTestAddr;
            VxSetSktAddressPort(poSktAddr, 0);
            oTestAddr.setIp( *poSktAddr );
            std::string strTestIpAddress = oTestAddr.toString();

            if( ( false == oTestAddr.isValid() ) ||
                ( oTestAddr.isLoopBack() ) )
            {
                //if( oTestAddr.isLoopBack() )
                //{
                //    LogMsg( LOG_INFO, "InetAddress::getAllAddresses: loopback addr %s\n", strTestIpAddress.c_str() );
                //}
                //else
                //{
                //    LogMsg( LOG_INFO, "InetAddress::getAllAddresses: invalid addr %s\n", strTestIpAddress.c_str() );
                //}
                VxCloseSkt( oSkt );
                continue;
            }

            if( false == VxBindSkt( oSkt, poSktAddr ) )
            {
                LogMsg( LOG_INFO, "InetAddress::getAllAddresses: could not bind addr %s", strTestIpAddress.c_str() );
                VxCloseSkt( oSkt );
                continue;
            }

            if( oTestAddr.isValid() &&
                ( false == oTestAddr.isLoopBack() ) )
            {
                retAddresses.push_back(oTestAddr);
            }

    //#define TEST_CONNECTION
    #ifdef TEST_CONNECTION
            if( false == VxTestConnection( oSkt, oTestAddr ) )
            {
                LogMsg( LOG_INFO, "Connection using local address %s OK", strTestIpAddress.c_str() );
            }
            else
            {
                LogMsg( LOG_INFO, "Connection using local address %s FAIL", strTestIpAddress.c_str() );
            }
    #else
            VxCloseSkt( oSkt );
    #endif // TEST_CONNECTION
        }
    }

    freeifaddrs(myaddrs);
#endif // TARGET_OS_WINDOWS

#if defined(TARGET_OS_LINUX)
    if( retAddresses.empty() )
    {
        struct ifaddrs * ifAddrStruct=NULL;
        struct ifaddrs * ifa=NULL;
        void * tmpAddrPtr=NULL;

        getifaddrs(&ifAddrStruct);

        for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
        {
            if (!ifa->ifa_addr)
            {
                continue;
            }

            if (ifa->ifa_addr->sa_family == AF_INET)
            { // check it is IP4
                // is a valid IP4 Address
                tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                char addressBuffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                //LogMsg( LOG_INFO, "%s IP Address %s\n", ifa->ifa_name, addressBuffer);
                InetAddress oTestAddr;
                oTestAddr.setIp( *addressBuffer );
                if( oTestAddr.isValid() && ( false == oTestAddr.isLoopBack() ) )
                {
                    retAddresses.push_back(oTestAddr);
                }
            }
            else if (ifa->ifa_addr->sa_family == AF_INET6)
            { // check it is IP6
                // is a valid IP6 Address
                tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
                char addressBuffer[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
                //LogMsg( LOG_INFO, "%s IP Address %s\n", ifa->ifa_name, addressBuffer);
                InetAddress oTestAddr;
                oTestAddr.setIp( *addressBuffer );
                if( oTestAddr.isValid() && ( false == oTestAddr.isLoopBack() ) )
                {
                    retAddresses.push_back(oTestAddr);
                }
            }
        }

        if (ifAddrStruct != nullptr)
        {
            freeifaddrs(ifAddrStruct);
        }
    }
#endif // defined(TARGET_OS_LINUX)
#if defined(TARGET_OS_ANDROID)
    if( retAddresses.empty() )
    {
        struct ifaddrs_android * ifAddrStruct=NULL;
        struct ifaddrs_android * ifa=NULL;
        void * tmpAddrPtr=NULL;

        getifaddrs_android(&ifAddrStruct);

        for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
        {
            if (!ifa->ifa_addr)
            {
                continue;
            }

            if (ifa->ifa_addr->sa_family == AF_INET)
            { // check it is IP4
                // is a valid IP4 Address
                tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                char addressBuffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                //LogMsg( LOG_INFO, "%s IP Address %s\n", ifa->ifa_name, addressBuffer);
                InetAddress oTestAddr;
                oTestAddr.setIp( (const char*)(&addressBuffer[0]) );
                if( oTestAddr.isValid() && ( false == oTestAddr.isLoopBack() ) )
                {
                    retAddresses.push_back(oTestAddr);
                }
            }
            else if (ifa->ifa_addr->sa_family == AF_INET6)
            { // check it is IP6
                // is a valid IP6 Address
                tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
                char addressBuffer[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
                //LogMsg( LOG_INFO, "%s IP Address %s\n", ifa->ifa_name, addressBuffer);
                InetAddress oTestAddr;
                oTestAddr.setIp( (const char*)(&addressBuffer[0]) );
                if( oTestAddr.isValid() && ( false == oTestAddr.isLoopBack() ) )
                {
                    retAddresses.push_back(oTestAddr);
                }
            }
        }

        if (ifAddrStruct != nullptr)
        {
            freeifaddrs_android(ifAddrStruct);
        }
    }

#endif // defined(TARGET_OS_ANDROID)

	return 0;
}

//============================================================================
void InetAddress::dumpAddresses( std::vector<InetAddress>& addressList )
{
    LogMsg( LOG_INFO, "InetAddress::dumpAddresses count %d", addressList.size() );
    static int addrIdx = 0;
    for( InetAddress& addr : addressList )
    {
        addrIdx++;
        LogMsg( LOG_INFO, "Addr %d - %s isIPv4 %d isLoopback %d isLocal %d", addrIdx, addr.toString().c_str(), addr.isIPv4(), addr.isLoopBack(), addr.isLocalAddress() );
    }
}

//============================================================================
bool InetAddress::isLoopBack() const
{
	bool isLoopBack = true;
	if( isIPv4() )
	{
		if( 0x7f000001 != getIPv4AddressInNetOrder() )
		{
			isLoopBack = false;
		}
	}
	else
	{
		uint8_t * pu8Bytes = (uint8_t * )this;
		for(int i = 0; i < 16; i++ )
		{
			if( pu8Bytes[i] && (i != 15) )
			{
				isLoopBack = false;
				break;
			}
			else if(  (1 == pu8Bytes[i]) && (i == 15) )
			{
				isLoopBack = true;
				break;
			}
		}
	}
	return isLoopBack;
}

//============================================================================
bool InetAddress::isLocalAddress( bool forLocalListening ) const
{
	if( false == isValid() )
	{
		LogMsg( LOG_ERROR, "InetAddrIPv4::isLocalAddress invalid address " );
		return false;
	}

	if( isIPv6() ) 
	{
		uint32_t * netOrder = (uint32_t *)this;

		uint32_t ip0 = ntohl(netOrder[0]);
		uint32_t ip1 = ntohl(netOrder[1]);
		uint32_t ip2 = ntohl(netOrder[2]);
		uint32_t ip3 = ntohl(netOrder[3]);

		if( forLocalListening && !ip0 && !ip1 && !ip2 && !ip3 ) // :: *
		{
			return false;
		}

		if( !ip0 && !ip1 && !ip2 &&
			( 0x00000000 == (ip3 & 0xfffffffe) ) )  // ::/127
		{
			return true;
		}

		if( ( 0xfe800000 == ( ip0 & 0xffc00000 ) ) || // fe80  RFC4291
			( 0xfc000000 == ( ip0 & 0xfe000000 ) ) || // fc00  RFC4193
			( 0xfec00000 == ( ip0 & 0xffc00000 ) ) )   // fec0  RFC3879
		{
			return true;
		}

		return false;
	} 
	else 
	{
		uint32_t hostOrderIpv4 = getIPv4AddressInHostOrder();
		if( ( 0x00000000 == ( hostOrderIpv4 & 0xff000000 ) ) || // 0.
			( 0x7f000000 == ( hostOrderIpv4 & 0xff000000 ) ) || // 127.
			( 0x0a000000 == ( hostOrderIpv4 & 0xff000000 ) ) || // 10.		
			( 0xc0a80000 == ( hostOrderIpv4 & 0xffff0000 ) ) || // 192.168.
			( 0xa9fe0000 == ( hostOrderIpv4 & 0xffff0000 ) ) || // 169.254.
			( 0xac100000 == ( hostOrderIpv4 & 0xfff00000 ) ) )  // 172.16?
		{
			return true;
		}

		return false;
	}
}

//============================================================================
InetAddress& InetAddress::operator=(const InetAddress& oAddr) 
{
	if( this != &oAddr )
	{
		m_u64AddrHi = oAddr.m_u64AddrHi;
		m_u64AddrLo = oAddr.m_u64AddrLo;
	}

	return *this;
}

//============================================================================
bool InetAddress::operator == (const InetAddress& oAddr)  const
{
	return (m_u64AddrHi == oAddr.m_u64AddrHi) && (m_u64AddrLo == oAddr.m_u64AddrLo);
}

//============================================================================
bool InetAddress::operator != (const InetAddress& oAddr)  const
{
	return (m_u64AddrHi != oAddr.m_u64AddrHi) || (m_u64AddrLo != oAddr.m_u64AddrLo);
}


//============================================================================
InetAddress& InetAddress::operator=(const InetAddrIPv4& inetAddr) 
{
	setIp( inetAddr.getIPv4AddressInNetOrder() );
	return *this;
}

//============================================================================
bool InetAddress::operator == (const InetAddrIPv4& inetAddr)  const
{
	return ( getIPv4AddressInNetOrder() == inetAddr.getIPv4AddressInHostOrder() );
}

//============================================================================
bool InetAddress::operator != (const InetAddrIPv4& inetAddr)  const
{
	return ( getIPv4AddressInNetOrder() != inetAddr.getIPv4AddressInHostOrder() );
}

//============================================================================
bool InetAddress::isValid( void ) const
{
	return ( 0 != m_u64AddrLo );
}

//============================================================================
void InetAddress::setToInvalid( void )
{
	m_u64AddrHi = 0;
	m_u64AddrLo = 0;
}

//============================================================================
bool InetAddress::isIPv4String( const char* pIpAddress ) const
{
	if( pIpAddress
		 && (strlen(pIpAddress) < 16 )
		 && strchr(pIpAddress, '.')
		 && ( 0 == strchr(pIpAddress, ':')) )
	{
		return true;
	}

	return false;
}

//============================================================================
bool InetAddress::isIPv4( void ) const
{
	if( IP4_BINARY_INDICATOR == m_u64AddrHi && 0 != m_u64AddrLo ) 
	{
		return true;
	}

	return false;
}

//============================================================================
bool InetAddress::isIPv6( void ) const
{
	return isValid() && !isIPv4();
}

//============================================================================
bool InetAddress::isIPv6GlobalAddress( void ) const
{
	// NOTE: bad assumption. better to check for locals instead.
	if( isIPv6() && !isLocalAddress() )
	{
		return true;
	}

	return false;
}

//============================================================================
uint32_t InetAddress::getIPv4AddressInHostOrder( void ) const
{
	return ntohl(*((uint32_t*)&m_u64AddrLo));
}

//============================================================================
uint32_t InetAddress::getIPv4AddressInNetOrder( void ) const
{
	return *((uint32_t*)&m_u64AddrLo);
}

//============================================================================
// note.. internally kept in network order instead of host order
void InetAddress::setIp( uint32_t u32IPv4Addr, bool bIsHostOrder )
{
    if( bIsHostOrder )
	{
		u32IPv4Addr = htonl( u32IPv4Addr );
	}

	if( u32IPv4Addr )
	{
		*((uint32_t*)&m_u64AddrLo) = u32IPv4Addr;
		m_u64AddrHi = IP4_BINARY_INDICATOR;
	}
	else
	{
		setToInvalid();
	}
}

//============================================================================
uint16_t InetAddress::setIp( const char* pIp )
{
	return fromString( pIp );
}

//============================================================================
//! returns port in host order
uint16_t InetAddress::setIp( struct sockaddr_in& oIPv4Addr )
{
    setIp(*((uint32_t*)&oIPv4Addr.sin_addr), true );
	return ntohs( oIPv4Addr.sin_port );
}

//============================================================================
//! returns port in host order
uint16_t InetAddress::setIp( struct sockaddr_in6& oIPv6Addr )
{
	memcpy(this, &oIPv6Addr.sin6_addr, sizeof(struct sockaddr_in6) < sizeof( InetAddress ) ? sizeof(struct sockaddr_in6) : sizeof( InetAddress ) );
	return ntohs( oIPv6Addr.sin6_port );
}

//============================================================================
//! returns port in host order
uint16_t InetAddress::setIp( struct sockaddr& ipAddr )
{
	if( AF_INET == ipAddr.sa_family )
	{
		return setIp( *((sockaddr_in *)&ipAddr) );
	}
	else if( AF_INET6 == ipAddr.sa_family )
	{
		return setIp( *((sockaddr_in6 *)&ipAddr) );
	}
	else
	{
		LogMsg( LOG_ERROR, "InetAddress::setIp unknown family" );
		return 0;
	}
}

//============================================================================
//! returns port in host order
uint16_t InetAddress::setIp( struct sockaddr_storage& oAddr )
{
	switch( oAddr.ss_family ) 
	{
	case AF_INET:
		return setIp(*((struct sockaddr_in *)&oAddr));
		break;

	case AF_INET6:
		return setIp(*((struct sockaddr_in6 *)&oAddr));
		break;

	default:
		//vx_assert(false);
		return 0;
	}
}

//============================================================================
//! fill address with this ip address and the given port
int InetAddress::fillAddress( struct sockaddr_storage& oAddr, uint16_t u16Port )
{
    memset( &oAddr, 0, sizeof( struct sockaddr_storage ) );
	if( isIPv4() )
	{
        return fillAddress( *((struct sockaddr_in*)&oAddr), u16Port );
	}
	else
	{
        return fillAddress( *((struct sockaddr_in6*)&oAddr), u16Port );
	}
}

//============================================================================
//! fill address with this ip address and the given port.. returns struct len
int InetAddress::fillAddress( struct sockaddr_in& oIPv4Addr, uint16_t u16Port )
{
	// setup the address and port
	memset( &oIPv4Addr, 0, sizeof( sockaddr_in ) );

	oIPv4Addr.sin_family			= AF_INET;
	*((long*)&oIPv4Addr.sin_addr)	= getIPv4AddressInHostOrder();

	oIPv4Addr.sin_port				= htons( u16Port );
	return (int)sizeof( struct sockaddr_in);
}

//============================================================================
//! fill address with this ip address and the given port.. returns struct len
int InetAddress::fillAddress( struct sockaddr_in6& oIPv6Addr, uint16_t u16Port )
{
	// setup the address and port
	memset( &oIPv6Addr, 0, sizeof( sockaddr_in6 ) );

	oIPv6Addr.sin6_family			= AF_INET6;

	memcpy( &oIPv6Addr.sin6_addr, this, 16 );

	oIPv6Addr.sin6_port	= htons( u16Port );
	return (int)sizeof( struct sockaddr_in6);
}

//============================================================================
//! returns port in host order
uint16_t InetAddress::getIpFromAddr(const struct sockaddr *sa, std::string& retStr)
{
	uint16_t u16Port = 0;
	std::array<char, INET6_MAX_STR_LEN> as8Addr;
    switch(sa->sa_family)
    {
		case AF_INET:
			VxIPv4_ntop(&(((struct sockaddr_in *)sa)->sin_addr), as8Addr.data(), as8Addr.size(), true);
			u16Port = ntohs( (((struct sockaddr_in *)sa)->sin_port) );
			break;

		case AF_INET6:
			VxIPv6_ntop( &(((struct sockaddr_in6 *)sa)->sin6_addr), as8Addr.data(), as8Addr.size() );
			u16Port = ntohs( (((struct sockaddr_in6 *)sa)->sin6_port) );
			break;

		default:
			strcpy(as8Addr.data(), "Unknown AF");
            return 0;
	}

    retStr = as8Addr.data();
	return u16Port;
}

//============================================================================
bool InetAddress::isLittleEndian( void )
{
	return ( 4L == ntohl(4L));
}

//============================================================================
uint64_t InetAddress::swap64Bit( uint64_t src )
{
	uint64_t dest;
	uint16_t * pu8Src = (uint16_t *)&src;
	uint16_t * pu8Dest = (uint16_t *)&dest;
	pu8Dest[1] = htons(pu8Src[0]);
	pu8Dest[0] = htons(pu8Src[1]);
	pu8Dest[3] = htons(pu8Src[2]);
	pu8Dest[2] = htons(pu8Src[3]);
	return dest;
}

//============================================================================
void InetAddress::litteEndianToNetIPv6( uint16_t * src, uint16_t * dest )
{
	if( isLittleEndian() )
	{
		uint64_t u64Hi = *((uint64_t *)src);
		uint64_t u64Lo = *((uint64_t *)(&src[4]));
		*((uint64_t *)dest) = swap64Bit(u64Hi);
		*((uint64_t *)(&dest[4])) = swap64Bit(u64Lo);
	}
	else
	{
		uint32_t u32Hi1 = *((uint32_t *)src);
		uint32_t u32Hi2 = *((uint32_t *)(&src[2]));
		uint32_t u32Lo1 = *((uint32_t *)(&src[4]));
		uint32_t u32Lo2 = *((uint32_t *)(&src[6]));
		*((uint32_t *)dest) = htonl(u32Hi2);
		*((uint32_t *)(&dest[2])) = htonl(u32Hi1);
		*((uint32_t *)(&dest[4])) = htonl(u32Lo2);
		*((uint32_t *)(&dest[6])) = htonl(u32Lo1);
	}
}

//============================================================================
// InetAddrAndPort
//============================================================================

//============================================================================
InetAddrAndPort::InetAddrAndPort( const char* ipAddr )
: InetAddress( ipAddr )
, m_u16Port(0)
{
}

//============================================================================
InetAddrAndPort::InetAddrAndPort( const InetAddrAndPort& rhs )
    : InetAddress( rhs )
    , m_u16Port( rhs.m_u16Port )
{
}

//============================================================================
InetAddrAndPort::InetAddrAndPort( const InetAddress& rhs )
    : InetAddress( rhs )
    , m_u16Port( 0 )
{

}

//============================================================================
bool InetAddrAndPort::addToBlob( PktBlobEntry& blob )
{
    bool result = InetAddress::addToBlob( blob );
    result &= blob.setValue( m_u16Port );
    return result;
}

//============================================================================
bool InetAddrAndPort::extractFromBlob( PktBlobEntry& blob )
{
    bool result = InetAddress::extractFromBlob( blob );
    result &= blob.getValue( m_u16Port );
    return result;
}

//============================================================================
bool InetAddrAndPort::fromString( const char* pIpAddress )
{
	setToInvalid();
	uint16_t port = InetAddress::fromString( pIpAddress );
	if( isValid() )
	{
		if( port )
		{
			m_u16Port = port;
		}

		return true;
	}

	return false;
}

//============================================================================
std::string	InetAddrAndPort::toString( bool includePort )
{
	std::string ipAddr;
	if( isValid() )
	{
		std::string ipAddrNoPort = InetAddress::toString();
		if( ipAddrNoPort.empty() )
		{
			LogMsg( LOG_ERROR, "InetAddrAndPort::%s empty addr" );
			return ipAddr;
		}

		if( includePort && m_u16Port )
		{
			if( isIPv6() )
			{
				ipAddr = "[";
				ipAddr += ipAddrNoPort;
				ipAddr = "]:";
			}
			else
			{
				ipAddr = ipAddrNoPort;
				ipAddr += ":";
			}

			ipAddr += std::to_string( m_u16Port );
		}
		else
		{
			ipAddr = ipAddrNoPort;
		}
	}
	
	return ipAddr;
}

//============================================================================
InetAddrAndPort::InetAddrAndPort( const char* ipAddr, uint16_t port )
: InetAddress( ipAddr )
, m_u16Port( port )
{
}

//============================================================================
InetAddrAndPort& InetAddrAndPort::operator=(const InetAddress& oAddr) 
{
	if( this != &oAddr )
	{
		m_u64AddrHi = oAddr.m_u64AddrHi;
		m_u64AddrLo = oAddr.m_u64AddrLo;
	}

	return *this;
}

//============================================================================
InetAddrAndPort& InetAddrAndPort::operator=(const InetAddrAndPort& oAddr) 
{
	if( this != &oAddr )
	{
		m_u64AddrHi = oAddr.m_u64AddrHi;
		m_u64AddrLo = oAddr.m_u64AddrLo;
		m_u16Port = oAddr.m_u16Port;
	}

	return *this;
}

//============================================================================
InetAddrAndPort& InetAddrAndPort::operator=(const InetAddrIPv4& inetAddr) 
{
	setIp( inetAddr.getIPv4AddressInHostOrder() );
	return *this;
}

//============================================================================
InetAddrAndPort& InetAddrAndPort::operator=(const InetAddrIPv4AndPort& inetAddr) 
{
	setIp( inetAddr.getIPv4AddressInHostOrder() );
	m_u16Port = inetAddr.getPort();
	return *this;
}

//============================================================================
void InetAddrAndPort::setIpAndPort( struct sockaddr_storage& inetAddr )
{
	m_u16Port = setIp( inetAddr );
}

//============================================================================
void InetAddrAndPort::setIpAndPort( struct sockaddr& oAddr )
{
	m_u16Port = setIp( oAddr );
}

//============================================================================
void InetAddrAndPort::setIpAndPort( const char* ipAddr, uint16_t port )
{
    setIp( ipAddr );
    m_u16Port = port;
}

//============================================================================
void inet_addr_testcase ( const char* pszTest )
{
    unsigned char abyAddr[16];
    bool bIsIPv6;
    uint16_t nPort;
    bool bSuccess;

    LogMsg( LOG_DEBUG, "Inet Test case '%s'", pszTest );
    const char* pszTextCursor = pszTest;
    bSuccess = ParseIPv4OrIPv6( pszTest, abyAddr, nPort, bIsIPv6 );
    if ( ! bSuccess )
    {
        LogMsg( LOG_DEBUG, "parse failed, at about index %d; rest: '%s'", pszTextCursor - pszTest, pszTextCursor );
    }
    
    LogMsg( LOG_DEBUG, "addr:  %s", BinaryToHexString( abyAddr, bIsIPv6 ? 16 : 4 ).c_str() );

    if ( 0 == nPort )
        LogMsg( LOG_DEBUG, "port absent" );
    else
        LogMsg( LOG_DEBUG, "port:  %d", htons ( nPort ) );
    LogMsg( LOG_DEBUG, "\n" );
    
}

//============================================================================
void TestInetAddress( void )
{  
    //The "localhost" IPv4 address
    inet_addr_testcase ( "127.0.0.1" );

	inet_addr_testcase ( "0000:0000:0000:0000:0000:0000:0000:0001/128" );
    
    //The "localhost" IPv4 address, with a specified port (80)
    inet_addr_testcase ( "127.0.0.1:80" );
    //The "localhost" IPv6 address
    inet_addr_testcase ( "::1" );
    //The "localhost" IPv6 address, with a specified port (80)
    inet_addr_testcase ( "[::1]:80" );
    //Rosetta Code's primary server's public IPv6 address
    inet_addr_testcase ( "2605:2700:0:3::4713:93e3" );
    //Rosetta Code's primary server's public IPv6 address, with a specified port (80)
    inet_addr_testcase ( "[2605:2700:0:3::4713:93e3]:80" );
    
    //ipv4 space
    inet_addr_testcase ( "::ffff:192.168.173.22" );
    //ipv4 space with port
    inet_addr_testcase ( "[::ffff:192.168.173.22]:80" );
    //trailing compression
    inet_addr_testcase ( "1::" );
    //trailing compression with port
    inet_addr_testcase ( "[1::]:80" );
    //'any' address compression
    inet_addr_testcase ( "::" );
    //'any' address compression with port
    inet_addr_testcase ( "[::]:80" );

    inet_addr_testcase( "2604:980:7003:e6:29e::1" );
    inet_addr_testcase( "[2604:980:7003:e6:29e::1]:45124" );
    

}