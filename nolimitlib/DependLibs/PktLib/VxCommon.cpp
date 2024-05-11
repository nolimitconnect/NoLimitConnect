//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxCommon.h"
#include "PktBlobEntry.h"

#include <NetLib/VxSktBase.h>
#include <NetLib/VxSktUtil.h>
#include <CoreLib/VxParse.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#include <memory.h>
#include <stdio.h>
#include <stdarg.h>

namespace
{
	IHackReportCallbackInterface* g_IHackReportCallback = nullptr;
}

//============================================================================
void VxSetHackReportCallback( IHackReportCallbackInterface* hackReportCallback )
{
	g_IHackReportCallback = hackReportCallback;
}

//============================================================================
//! generate connection key from network identity
bool GenerateConnectionKey(  bool					ipv6,	
							 VxKey *				poRetKey,		// set this key
                             VxConnectId *			poConnectId,	// network identity
                             uint16_t				cryptoPort,
                             const char*			networkName )
{
    std::string strNetName = networkName;
    std::string strIP;
    poConnectId->getIpAddress( ipv6, strIP );
    uint16_t u16Port = poConnectId->getPort();

    return GenerateConnectionKey( ipv6, poRetKey, strIP, u16Port, poConnectId->getOnlineId(), cryptoPort, strNetName );
}

//============================================================================
bool GenerateConnectionKey(  bool						ipv6,	
							 VxKey *					poRetKey,		// set this key
                             std::string&               ipAddr,
                             uint16_t                   port,
                             VxGUID&                    onlineId,
                             uint16_t					cryptoPort,
                             std::string&				networkName )
{
    uint64_t u64IdLowPart = onlineId.getVxGUIDLoPart();
    //vx_assert( u64IdLowPart );
    uint64_t u64IdHiPart = onlineId.getVxGUIDHiPart();
    //vx_assert( u64IdHiPart );
    std::string strIP = ipAddr;
    vx_assert( port );

    std::string strPwd;
    StdStringFormat( strPwd, "%d%llx%llx%s%s%d",
        port,
        u64IdLowPart,
        u64IdHiPart,
        networkName.c_str(),
        strIP.c_str(),
        cryptoPort
    );

    // LogModule( eLogSktData, LOG_VERBOSE, "GenerateConnectionKey: setting Key %s for %s:%d %d", strPwd.c_str(), strIPv4.c_str(), port, cryptoPort );
    poRetKey->setKeyFromPassword( strPwd.c_str(), (int)strPwd.size() );
    return !strIP.empty() && cryptoPort;
}

//============================================================================
PluginPermission::PluginPermission()
{
	memset( m_au8Permissions, 0, sizeof( m_au8Permissions ) );
}

//============================================================================
bool PluginPermission::addToBlob( PktBlobEntry& blob )
{
    return blob.setValue( m_au8Permissions, (int)sizeof( m_au8Permissions ) );
}

//============================================================================
bool PluginPermission::extractFromBlob( PktBlobEntry& blob )
{
    int iBufLen = sizeof( m_au8Permissions );
    return blob.getValue(  (void *)m_au8Permissions, iBufLen );
}

//============================================================================
PluginPermission& PluginPermission::operator = ( const PluginPermission& rhs )
{
    if( this != &rhs )
    {
        memcpy( m_au8Permissions, rhs.m_au8Permissions, sizeof( m_au8Permissions ) );
    }

    return *this;
}

//============================================================================
bool PluginPermission::isPluginEnabled( EPluginType ePlugin )		
{ 
	return (eFriendStateIgnore == getPluginPermission(ePlugin))?0:1; 
}

//============================================================================
//! get type of permission user has set for givin plugin
EFriendState PluginPermission::getPluginPermission( EPluginType pluginType, bool inGroup )
{ 
	if(( pluginType > 0 ) && ( pluginType < eMaxPluginType ) )
	{
		int pluginNum = (int)(pluginType - 1);
		int byteIdx = pluginNum >> 1;
		int byteShift = pluginNum & 0x01 ? 4 : 0;
		uint8_t byteWithPerm = m_au8Permissions[ byteIdx ];

		EFriendState friendState = (EFriendState)( ( byteWithPerm >> byteShift ) & 0xf );
		if( eFriendStateAnonymous == friendState && inGroup )
		{
			friendState = eFriendStateGuest;
		}

		return friendState;
	}

	return eFriendStateIgnore;
}

//============================================================================
//! set type of permission user has set for givin plugin
void PluginPermission::setPluginPermission( EPluginType pluginType, EFriendState eFriendState ) 
{ 
	if(( pluginType > 0 ) && ( pluginType < eMaxUserPluginType ) )
	{
		int pluginNum = (int)(pluginType - 1);
		int byteIdx = pluginNum >> 1;
		int byteShift = pluginNum & 0x01 ? 4 : 0;
		uint8_t byteWithPerm = m_au8Permissions[ byteIdx ];
		if( byteShift )
		{
			byteWithPerm &= 0x0f;
			byteWithPerm |= (eFriendState << byteShift);
		}
		else
		{
			byteWithPerm &= 0xf0;
			byteWithPerm |= (eFriendState);
		}

		if( ( byteIdx < PERMISSION_ARRAY_SIZE ) 
			&& ( 0 <= byteIdx ) )
		{
			m_au8Permissions[ byteIdx ] = byteWithPerm;
		}
		else
		{
			LogMsg( LOG_ERROR, "setPluginPermission index out of range %d", byteIdx );
		}
	}
}

//============================================================================
void PluginPermission::setPluginPermissionsToDefaultValues( void )	
{ 
	memset( m_au8Permissions, 0, sizeof( m_au8Permissions ) );

	setPluginPermission( ePluginTypeAdmin, eFriendStateAdmin );	
    setPluginPermission( ePluginTypeAboutMePageServer, eFriendStateGuest );
	setPluginPermission( ePluginTypeStoryboardServer, eFriendStateGuest );
    setPluginPermission( ePluginTypeCamServer, eFriendStateIgnore );
    setPluginPermission( ePluginTypeFileShareServer, eFriendStateIgnore );

    setPluginPermission( ePluginTypePersonFileXfer, eFriendStateFriend );
    setPluginPermission( ePluginTypeMessenger, eFriendStateGuest );
    setPluginPermission( ePluginTypeTruthOrDare, eFriendStateFriend );
    setPluginPermission( ePluginTypeVideoPhone, eFriendStateFriend );
    setPluginPermission( ePluginTypeVoicePhone, eFriendStateFriend );

    setPluginPermission( ePluginTypePushToTalk, eFriendStateIgnore );

	setPluginPermission( ePluginTypeClientPeerUser, eFriendStateGuest );
	setPluginPermission( ePluginTypeHostPeerUser, eFriendStateGuest );

	setPluginPermission( ePluginTypeClientConnectTest, eFriendStateIgnore );
	setPluginPermission( ePluginTypeHostConnectTest, eFriendStateIgnore );

	setPluginPermission( ePluginTypeClientChatRoom, eFriendStateGuest );
	setPluginPermission( ePluginTypeHostChatRoom, eFriendStateIgnore );

    setPluginPermission( ePluginTypeClientGroup, eFriendStateGuest );
	setPluginPermission( ePluginTypeHostGroup, eFriendStateIgnore );

    setPluginPermission( ePluginTypeClientNetwork, eFriendStateIgnore );
	setPluginPermission( ePluginTypeHostNetwork, eFriendStateIgnore );

    setPluginPermission( ePluginTypeClientRandomConnect, eFriendStateGuest );
	setPluginPermission( ePluginTypeHostRandomConnect, eFriendStateIgnore );

    setPluginPermission( ePluginTypeNetworkSearchList, eFriendStateIgnore );
} 

//============================================================================
//============================================================================
//============================================================================
VxNetIdent::VxNetIdent()
: m_u16AppVersion( htons( VxGetAppVersionShort() ) )	
{
}

//============================================================================
VxNetIdent::VxNetIdent(const VxNetIdent &rhs )
: VxNetIdentBase( rhs )
, PluginPermission( rhs )
, m_u16AppVersion( rhs.m_u16AppVersion )
, m_u16PingTimeMs( rhs.m_u16PingTimeMs )
, m_NetIdentRes1( rhs.m_NetIdentRes1 )
, m_NetIdentRes2( rhs.m_NetIdentRes2 )  
, m_NetIdentRes3( rhs.m_NetIdentRes3 )
, m_LastSessionTimeGmtMs( rhs.m_LastSessionTimeGmtMs )
{
}

//============================================================================
bool VxNetIdent::addToBlob( PktBlobEntry& blob )
{
    uint8_t startMagicNum = 98;
    bool result = blob.setValue( startMagicNum );
    result &= VxNetIdentBase::addToBlob( blob );
    result &= PluginPermission::addToBlob( blob );
    result &= blob.setValue( m_u16AppVersion );
    result &= blob.setValue( m_u16PingTimeMs );
    result &= blob.setValue( m_NetIdentRes1 );
    result &= blob.setValue( m_NetIdentRes2 );
    result &= blob.setValue( m_NetIdentRes3 );
    result &= blob.setValue( m_LastSessionTimeGmtMs );
    uint8_t stopMagicNum = 99;
    result &= blob.setValue( stopMagicNum );
    return result;
}

//============================================================================
bool VxNetIdent::extractFromBlob( PktBlobEntry& blob )
{
    uint8_t startMagicNum;
    bool result = blob.getValue( startMagicNum );
    if( !result || startMagicNum != 98 )
    {
        LogMsg( LOG_ERROR, "VxNetIdent::%s startMagicNum not valid", __func__ );
        return false;
    }

    result &= VxNetIdentBase::extractFromBlob( blob );
    result &= PluginPermission::extractFromBlob( blob );
    result &= blob.getValue( m_u16AppVersion );
    result &= blob.getValue( m_u16PingTimeMs );
    result &= blob.getValue( m_NetIdentRes1 );
    result &= blob.getValue( m_NetIdentRes2 );
    result &= blob.getValue( m_NetIdentRes3 );
    result &= blob.getValue( m_LastSessionTimeGmtMs );

    uint8_t stopMagicNum;
    result &= blob.getValue( stopMagicNum );
    if( !result || stopMagicNum != 99 )
    {
        LogMsg( LOG_ERROR, "VxNetIdent::%s stopMagicNum not valid", __func__ );
        return false;
    }

    return result;
}

//============================================================================
//! copy operator
VxNetIdent& VxNetIdent::operator =( const VxNetIdent& rhs  )
{
    if( this != &rhs )
    {
        *( (VxNetIdentBase*)this ) = *( (VxNetIdentBase*)&rhs );
        *( (PluginPermission*)this ) = *( (PluginPermission*)&rhs );
        m_u16AppVersion = rhs.m_u16AppVersion;
        m_u16PingTimeMs = rhs.m_u16PingTimeMs;
        m_NetIdentRes1 = rhs.m_NetIdentRes1;
        m_NetIdentRes2 = rhs.m_NetIdentRes2;
        m_NetIdentRes3 = rhs.m_NetIdentRes3;
        m_LastSessionTimeGmtMs = rhs.m_LastSessionTimeGmtMs;
    }

    return *this;
}

//============================================================================
bool VxNetIdent::isValidNetIdent()
{
	bool result = getMyOnlineId().isVxGUIDValid();

	result &= isOnlineNameValid();

	return result;
} 

//============================================================================
bool VxNetIdent::isOnlineNameValid( void )
{
    bool result = !(m_OnlineName[0] == 0);
	if( result )
	{
		int asciiCnt = 0;
		bool foundTerminator{ false };
		bool invalidChar{ false };
		for( int i = 0; i < MAX_ONLINE_NAME_LEN; i++ )
		{
			if( 0 == m_OnlineName[i] )
			{
				foundTerminator = true;
				break;
			}
			else if( isascii( m_OnlineName[i] ) )
			{
				asciiCnt++;
			}
			else
			{
				// invalid char
				invalidChar = true;
				break;
			}
		}

		result &= !invalidChar && foundTerminator && asciiCnt >= 3;
	}

	return result;
}

//============================================================================
void VxNetIdent::setPingTimeMs( uint16_t pingTime )
{
	m_u16PingTimeMs = htons( pingTime );
}

//============================================================================
uint16_t VxNetIdent::getPingTimeMs( void )
{
	return ntohs( m_u16PingTimeMs );
}

//============================================================================
EPluginAccess	VxNetIdent::getHisAccessPermissionFromMe( EPluginType pluginType, bool inGroup )
{
	EFriendState friendState = getMyFriendshipToHim();
	if( inGroup && friendState == eFriendStateAnonymous )
	{
		friendState = eFriendStateGuest;
	}

	return getPluginAccessState( pluginType, friendState );
}

//============================================================================
EPluginAccess VxNetIdent::getMyAccessPermissionFromHim( EPluginType pluginType, bool inGroup )
{
	EFriendState friendState = getHisFriendshipToMe();
	if( inGroup && friendState == eFriendStateAnonymous )
	{
		friendState = eFriendStateGuest;
	}

	EPluginAccess accessState = getPluginAccessState( pluginType, friendState );
	if( ePluginAccessOk == accessState )
	{
		if( ( ePluginTypeFileShareServer == pluginType ) 
			&& ( false == hasSharedFiles() ) )
		{
			// no files shared
			return ePluginAccessInactive;
		}

		if( ( ePluginTypeCamServer == pluginType ) 
			&& ( false == hasSharedWebCam() ) )
		{
			// no shared web cam
			return ePluginAccessInactive;
		}

		if( ( ePluginTypeAboutMePageServer == pluginType )
			|| ( ePluginTypeStoryboardServer == pluginType ) )
		{
			if( false == isOnline() )
			{
				accessState = ePluginAccessRequiresOnline;
			}
		}
		else if( ePluginTypeMessenger != pluginType )
		{
			if( false == isOnline() )
			{
				accessState = ePluginAccessRequiresOnline;
			}
		}		
	}

	return accessState;
}

//============================================================================
bool VxNetIdent::isHisAccessAllowedFromMe( EPluginType pluginType, bool inGroup )
{
	EFriendState friendState = this->getMyFriendshipToHim();
	if( eFriendStateAnonymous == friendState && inGroup )
	{
		friendState = eFriendStateGuest;
	}

	return ( ePluginAccessOk == getPluginAccessState( pluginType, friendState ) );
}

//============================================================================
bool VxNetIdent::isMyAccessAllowedFromHim( EPluginType pluginType, bool inGroup )
{
	EFriendState friendState = this->getHisFriendshipToMe();
	if( eFriendStateAnonymous == friendState && inGroup )
	{
		friendState = eFriendStateGuest;
	}

	return ( ePluginAccessOk == getPluginAccessState( pluginType, friendState ) );
}

//============================================================================
EPluginAccess VxNetIdent::getPluginAccessState( EPluginType pluginType, EFriendState eHisPermissionToMe )
{
	if( eFriendStateIgnore == eHisPermissionToMe )
	{
		return ePluginAccessIgnored;
	}

	EFriendState ePermissionLevel = this->getPluginPermission( pluginType );
	if( eFriendStateIgnore == ePermissionLevel )
	{
		return ePluginAccessDisabled;
	}

	if( ePermissionLevel > eHisPermissionToMe )
	{
		return ePluginAccessLocked;
	}

	if( (ePluginTypeFileShareServer == pluginType) && 
		(false == hasSharedFiles()) )
	{
		// no files shared
		return ePluginAccessInactive;
	}

	if( (ePluginTypeCamServer == pluginType) && 
		(false == hasSharedWebCam()) )
	{
		// no files shared
		return ePluginAccessInactive;
	}

	return ePluginAccessOk;
}

//============================================================================
bool VxNetIdent::canRequestJoin( EHostType hostType )
{
	if( requiresRelay() )
	{
		return false;
	}

	EPluginType pluginType = HostTypeToHostPlugin( hostType );
	EFriendState pluginPermissionLevel = getPluginPermission( pluginType );
	return pluginPermissionLevel != eFriendStateIgnore && getHisFriendshipToMe() != eFriendStateIgnore &&
		getMyFriendshipToHim() != eFriendStateIgnore;
}

//============================================================================
bool VxNetIdent::canJoinImmediate( EHostType hostType ) // request to join will be granted immediate because have sufficient permission
{
	if( requiresRelay() )
	{
		return false;
	}

	EPluginType pluginType = HostTypeToHostPlugin( hostType );
	EFriendState pluginPermissionLevel = getPluginPermission( pluginType );
	return pluginPermissionLevel != eFriendStateIgnore && getHisFriendshipToMe() != eFriendStateIgnore &&
		getMyFriendshipToHim() != eFriendStateIgnore && getHisFriendshipToMe() >= pluginPermissionLevel;
}

//============================================================================
//! dump identity
void VxNetIdent::debugDumpIdent( void )
{
	std::string strIPv4; 
	m_DirectConnectId.getIpAddress( false, strIPv4 );
	std::string strIPv6; 
	m_DirectConnectId.getIpAddress( true, strIPv6 );

	LogMsg( LOG_INFO, "Ident %s id %s my friendship %s his friendship %s search 0x%x ipv4 %s ipv6 %s port %d proxy flags 0x%x ",
		getOnlineName(),
		getMyOnlineId().describeVxGUID().c_str(),
		DescribeFriendState(getMyFriendshipToHim()),
		DescribeFriendState(getHisFriendshipToMe()),
		getSearchFlags(),
		strIPv4.c_str(),
		strIPv6.c_str(),
		m_DirectConnectId.getPort(),
		m_u8RelayFlags
		);
}

//============================================================================
std::string VxNetIdent::describeUser( void )
{
	std::string userDesc = getOnlineName();
	userDesc += " id ";
	userDesc += getMyOnlineId().toOnlineIdString();
	return userDesc;
}

//============================================================================
bool VxNetIdent::userIsHosting( enum EHostType hostType )
{
	switch( hostType )
	{
	case eHostTypeConnectTest:
		return getHisAccessPermissionFromMe( ePluginTypeHostConnectTest ) != ePluginAccessDisabled;
	case eHostTypeNetwork:
		return getHisAccessPermissionFromMe( ePluginTypeHostNetwork ) != ePluginAccessDisabled;
    case eHostTypeGroup:
		return getHisAccessPermissionFromMe( ePluginTypeHostGroup ) != ePluginAccessDisabled;
    case eHostTypeChatRoom:
		return getHisAccessPermissionFromMe( ePluginTypeHostChatRoom ) != ePluginAccessDisabled;
    case eHostTypeRandomConnect:
		return getHisAccessPermissionFromMe( ePluginTypeHostRandomConnect ) != ePluginAccessDisabled;

	default:
		return false;
	}
}

//============================================================================
bool VxNetIdent::requiresAnOpenPort( void )
{
	return userIsHosting( eHostTypeNetwork ) || 
		userIsHosting( eHostTypeGroup ) || 
		userIsHosting( eHostTypeChatRoom ) || 
		userIsHosting( eHostTypeRandomConnect ) || 
		userIsHosting( eHostTypeConnectTest );
}

//============================================================================
RCODE VxReportHack(	EHackerLevel hackerLevel, EHackerReason hackerReason, std::shared_ptr<VxSktBase>& sktBase, const char* pDescription, ... )	
{
	char as8Buf[ 2048 ];
	va_list argList;
	va_start( argList, pDescription );
	vsnprintf( as8Buf, sizeof( as8Buf ), pDescription, argList );
	as8Buf[sizeof( as8Buf ) - 1] = 0;
	va_end( argList );

	std::string ipAddr = sktBase ? sktBase->getRemoteIpAddress() : "";

	if( g_IHackReportCallback )
	{
		g_IHackReportCallback->reportHackOffense( hackerLevel, hackerReason, ipAddr, as8Buf );
	}
	else
	{
		LogModule( eLogHackers, LOG_ERROR, "%s %s %s %s", DescribeHackerLevel( hackerLevel ), DescribeHackerReason( hackerReason ),
			sktBase->describeSktConnection().c_str(),
			as8Buf );
	}

	return 0;
}

//============================================================================
RCODE VxReportHack(	EHackerLevel hackerLevel, EHackerReason hackerReason, SOCKET skt, const char* ipAddr, const char* pDescription, ... )
{
	char as8Buf[ 2048 ];
	va_list argList;
	va_start( argList, pDescription );
	vsnprintf( as8Buf, sizeof( as8Buf ), pDescription, argList );
	as8Buf[sizeof( as8Buf ) - 1] = 0;
	va_end( argList );

	if( g_IHackReportCallback )
	{
		g_IHackReportCallback->reportHackOffense( hackerLevel, hackerReason, ipAddr, as8Buf );
	}
	else
	{
		LogModule( eLogHackers, LOG_ERROR, "%s %s handle %d ip %s %s", DescribeHackerLevel( hackerLevel ), DescribeHackerReason( hackerReason ),
			skt,
			ipAddr,
			as8Buf );
	}

	return 0;
}

