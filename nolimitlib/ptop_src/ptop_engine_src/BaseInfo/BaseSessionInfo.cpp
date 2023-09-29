//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software 
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <config_appcorelibs.h>
#include "BaseSessionInfo.h"

#include <CoreLib/VxDebug.h>
#include <PktLib/VxCommon.h>

//============================================================================
BaseSessionInfo::BaseSessionInfo()
{ 
}

//============================================================================
BaseSessionInfo::BaseSessionInfo( HostUserSessionId& hostUserSessionId )
    : HostUserSessionId( hostUserSessionId )
{ 
}

//============================================================================
BaseSessionInfo::BaseSessionInfo( const BaseSessionInfo& rhs )
    : HostUserSessionId( rhs )
    , m_OnlineState( rhs.m_OnlineState )
    , m_JoinState( rhs.m_JoinState )
{
}

//============================================================================
BaseSessionInfo& BaseSessionInfo::operator=( const BaseSessionInfo& rhs )
{
    if( this != &rhs )
    {
        getHostUserSessionId() = rhs;
        m_OnlineState = rhs.m_OnlineState;
        m_JoinState = rhs.m_JoinState;
    }

    return *this;
}

//============================================================================
bool BaseSessionInfo::operator==( const BaseSessionInfo& rhs )
{
    return getHostUserSessionId() == rhs;
}
