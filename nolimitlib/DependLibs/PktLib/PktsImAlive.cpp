//============================================================================
// Copyright (C) 2003 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PktsImAlive.h"

#include <CoreLib/IsBigEndianCpu.h>

//============================================================================
PktImAliveReq::PktImAliveReq()
: m_Res(0)
{
	setPktType( PKT_TYPE_IM_ALIVE_REQ );
	setPktLength( sizeof( PktImAliveReq ) );
}

////============================================================================
//void PktImAliveReq::setTimestamp( uint64_t timeStamp )
//{
//	m_Timestamp = htonU64( timeStamp );
//}
//
////============================================================================
//uint64_t PktImAliveReq::getTimestamp( void )
//{
//	return ntohU64( m_Timestamp );
//}

//============================================================================
PktImAliveReply::PktImAliveReply()
: m_Res(0)
{
	setPktType( PKT_TYPE_IM_ALIVE_REPLY );
	setPktLength( sizeof( PktImAliveReq ) );
}

////============================================================================
//void PktImAliveReply::setTimestamp( uint64_t timeStamp )
//{
//	m_Timestamp = htonU64( timeStamp );
//}
//
////============================================================================
//uint64_t PktImAliveReply::getTimestamp( void )
//{
//	return ntohU64( m_Timestamp );
//}
