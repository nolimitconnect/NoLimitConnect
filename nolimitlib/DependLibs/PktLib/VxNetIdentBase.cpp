//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxConnectInfo.h"
#include "VxNetIdentBase.h"
#include <CoreLib/PktBlobEntry.h>

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxParse.h>

#include <memory.h>
#include <stdio.h>

#define IS_JOINED_CHAT_ROOM_FLAG				0x01	
#define IS_JOINED_GROUP_FLAG					0x02	
#define IS_JOINED_RANDOM_CONNECT_FLAG			0x04	

//============================================================================
VxNetIdentBase::VxNetIdentBase( const VxNetIdentBase &rhs )
    : VxConnectInfo( rhs )
    , VxOnlineStatusFlags( rhs )
    , m_JoinedFlags( rhs.m_JoinedFlags )
    , m_u8OfferCnt( rhs.m_u8OfferCnt )
    , m_u8ReplyCnt( rhs.m_u8ReplyCnt )
    , m_u32TruthCnt( rhs.m_u32TruthCnt )
    , m_u32DareCnt( rhs.m_u32DareCnt )
    , m_u16RejectedTruthsCnt( rhs.m_u16RejectedTruthsCnt )
    , m_u16RejectedDaresCnt( rhs.m_u16RejectedDaresCnt )
{
}

//============================================================================
VxNetIdentBase&  VxNetIdentBase::operator = ( const VxNetIdentBase& rhs )
{
    if( this != &rhs )
    {
        *((VxConnectInfo*)this) = *((VxConnectInfo*)&rhs);
        *((VxOnlineStatusFlags*)this) = *((VxOnlineStatusFlags*)&rhs);
        m_JoinedFlags = rhs.m_JoinedFlags;
        m_u8OfferCnt = rhs.m_u8OfferCnt;
        m_u8ReplyCnt = rhs.m_u8ReplyCnt;
        m_u32TruthCnt = rhs.m_u32TruthCnt;
        m_u32DareCnt = rhs.m_u32DareCnt;
        m_u16RejectedTruthsCnt = rhs.m_u16RejectedTruthsCnt;
        m_u16RejectedDaresCnt = rhs.m_u16RejectedDaresCnt;
    }

    return *this;
}

//============================================================================
bool VxNetIdentBase::addToBlob( PktBlobEntry& blob )
{
    bool result = VxConnectInfo::addToBlob( blob );
    result &= VxOnlineStatusFlags::addToBlob( blob );
    result &= blob.setValue( m_JoinedFlags );
    result &= blob.setValue( m_u8OfferCnt );
    result &= blob.setValue( m_u8ReplyCnt );
    result &= blob.setValue( m_u32TruthCnt );
    result &= blob.setValue( m_u32DareCnt );
    result &= blob.setValue( m_u16RejectedTruthsCnt );
    result &= blob.setValue( m_u16RejectedDaresCnt );
    return result;
}

//============================================================================
bool VxNetIdentBase::extractFromBlob( PktBlobEntry& blob )
{
    bool result = VxConnectInfo::extractFromBlob( blob );
    result &= VxOnlineStatusFlags::extractFromBlob( blob );
    result &= blob.getValue( m_JoinedFlags );
    result &= blob.getValue( m_u8OfferCnt );
    result &= blob.getValue( m_u8ReplyCnt );
    result &= blob.getValue( m_u32TruthCnt );
    result &= blob.getValue( m_u32DareCnt );
    result &= blob.getValue( m_u16RejectedTruthsCnt );
    result &= blob.getValue( m_u16RejectedDaresCnt );
    return result;
}

//============================================================================
bool VxNetIdentBase::isMyself( void )
{
    return GetPtoPEngine().getMyOnlineId() == getMyOnlineId();
}

//============================================================================
bool VxNetIdentBase::isDirectConnected( void )
{
    return GetPtoPEngine().getConnectIdListMgr().isDirectConnected( getMyOnlineId() );
}

//============================================================================
bool VxNetIdentBase::isRelayed( void )
{
    return GetPtoPEngine().getConnectIdListMgr().isRelayed( getMyOnlineId() );
}

//============================================================================
bool VxNetIdentBase::canDirectConnectToUser( void )
{
    return !requiresRelay() || isDirectConnected();
}

//============================================================================
//! return true if identity matches
bool VxNetIdentBase::isVxNetIdentMatch( const VxNetIdentBase& oOtherIdent ) const
{
	return ( *((VxGUID *)&oOtherIdent.m_DirectConnectId) == *((VxGUID *)&m_DirectConnectId));
}

//============================================================================
bool VxNetIdentBase::operator ==( const VxNetIdentBase &a ) const
{
	return this->isVxNetIdentMatch( a );
}

//============================================================================
//! not equal operator
bool VxNetIdentBase::operator != ( const VxNetIdentBase &a ) const
{
	return !(this->isVxNetIdentMatch( a ));
}

//============================================================================
void VxNetIdentBase::setDareCount( uint32_t dareCnt )
{
	m_u32DareCnt = htonl( dareCnt );
}

//============================================================================
uint32_t	VxNetIdentBase::getDareCount()
{
	return ntohl( m_u32DareCnt );
}

//============================================================================
void VxNetIdentBase::setRejectedTruthCount( uint16_t rejectedCnt )
{
	m_u16RejectedTruthsCnt = htons( rejectedCnt );
}

//============================================================================
uint16_t VxNetIdentBase::getRejectedTruthCount()
{
	return ntohs( m_u16RejectedTruthsCnt );
}

//============================================================================
void VxNetIdentBase::setRejectedDareCount( uint16_t rejectedCnt )
{
	m_u16RejectedDaresCnt = htons( rejectedCnt );
}

//============================================================================
uint16_t VxNetIdentBase::getRejectedDareCount()
{
	return ntohs( m_u16RejectedDaresCnt );
}

//============================================================================
void VxNetIdentBase::setTruthCount( uint32_t truthCnt )
{
	m_u32TruthCnt = htonl( truthCnt );
}

//============================================================================
uint32_t VxNetIdentBase::getTruthCount( void )
{
	return ntohl( m_u32TruthCnt );
}

//============================================================================
void VxNetIdentBase::clearIsJoined( void )
{
    m_JoinedFlags = 0;
}

//============================================================================
void VxNetIdentBase::setIsJoined( EHostType hostType, bool isJoined )
{
    uint8_t hostFlag{ 0 };
    switch( hostType )
    {
    case eHostTypeChatRoom:
        hostFlag = IS_JOINED_CHAT_ROOM_FLAG;
        break;
    case eHostTypeGroup:
        hostFlag = IS_JOINED_GROUP_FLAG;
        break;
    case eHostTypeRandomConnect:
        hostFlag = IS_JOINED_RANDOM_CONNECT_FLAG;
        break;
    default:
        LogMsg( LOG_ERROR, "VxNetIdentBase::setIsJoined invalid host type %d", hostType );
        return;
    }

    if( hostFlag )
    {
        if( isJoined )
        {
            if( !( m_JoinedFlags & hostFlag ) )
            {
                m_JoinedFlags |= hostFlag;
                LogMsg( LOG_VERBOSE, "VxNetIdentBase::setIsJoined joined host type %s", DescribeHostType( hostType ) );
            }
        }
        else
        {
            if( m_JoinedFlags & hostFlag )
            {
                m_JoinedFlags &= ~hostFlag;
                LogMsg( LOG_VERBOSE, "VxNetIdentBase::setIsJoined NOT joined host type %s", DescribeHostType( hostType ) );
            }
        }
    }
}

//============================================================================
bool VxNetIdentBase::getIsJoined( EHostType hostType )
{
    uint8_t hostFlag{ 0 };
    switch( hostType )
    {
    case eHostTypeChatRoom:
        hostFlag = IS_JOINED_CHAT_ROOM_FLAG;
        break;
    case eHostTypeGroup:
        hostFlag = IS_JOINED_GROUP_FLAG;
        break;
    case eHostTypeRandomConnect:
        hostFlag = IS_JOINED_RANDOM_CONNECT_FLAG;
        break;
    default:
        LogMsg( LOG_ERROR, "VxNetIdentBase::getIsJoined invalid host type %d", hostType );
    }

    return hostFlag & m_JoinedFlags;
}

//============================================================================
bool VxNetIdentBase::isJoinedAny( void )
{
    return m_JoinedFlags != 0;
}