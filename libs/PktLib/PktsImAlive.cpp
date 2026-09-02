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

#include <CoreLib/VxDebug.h>

//============================================================================
PktImAliveReq::PktImAliveReq()
: m_Res(0)
{
	setPktType( PKT_TYPE_IM_ALIVE_REQ );
	setPktLength( sizeof( PktImAliveReq ) );
	vx_assert( 0 == ( getPktLength() & 0x0f ) );
}

//============================================================================
PktImAliveReply::PktImAliveReply()
: m_Res(0)
{
	setPktType( PKT_TYPE_IM_ALIVE_REPLY );
	setPktLength( sizeof( PktImAliveReply ) );
	vx_assert( 0 == ( getPktLength() & 0x0f ) );
}

