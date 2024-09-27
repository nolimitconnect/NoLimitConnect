//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <config_appcorelibs.h>
#include "HostJoinInfo.h"

#include <PktLib/VxSearchDefs.h>

#include <CoreLib/VxFileLists.h>
#include <CoreLib/VxFileIsTypeFunctions.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxPtopUrl.h>

#include <sys/types.h>
#include <sys/stat.h>

//============================================================================
HostJoinInfo::HostJoinInfo()
    : BaseJoinInfo()
{ 
}

//============================================================================
HostJoinInfo::HostJoinInfo( const HostJoinInfo& rhs )
    : BaseJoinInfo( rhs )
    , m_NetIdent( rhs.m_NetIdent )
    , m_FriendState( rhs.m_FriendState )
    , m_HostFlags( rhs.m_HostFlags )
    , m_GroupieId( rhs.m_GroupieId )
    , m_UserUrl( rhs.m_UserUrl )
    , m_ConnectionId( rhs.m_ConnectionId )
    , m_SessionId( rhs.m_SessionId )    
{
}

//============================================================================
HostJoinInfo& HostJoinInfo::operator=( const HostJoinInfo& rhs ) 
{	
	if( this != &rhs )
	{
        BaseJoinInfo::operator = ( rhs );
        m_NetIdent = rhs.m_NetIdent;
        m_FriendState = rhs.m_FriendState;
        m_HostFlags = rhs.m_HostFlags;
        m_GroupieId = rhs.m_GroupieId;
        m_UserUrl = rhs.m_UserUrl;
        m_ConnectionId = rhs.m_ConnectionId;
        m_SessionId = rhs.m_SessionId;
    }

	return *this;
}

//============================================================================
bool HostJoinInfo::isUrlValid( void )
{
    VxPtopUrl ptopUrl( m_UserUrl );
    return ptopUrl.isValid() && ptopUrl.getOnlineId() == getOnlineId();
}

//============================================================================
void HostJoinInfo::setHostType( enum EHostType hostType )
{
    BaseJoinInfo::setHostType( hostType );
}
