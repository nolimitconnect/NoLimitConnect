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

#include "GroupieId.h"
#include "PktBlobEntry.h"

//============================================================================
GroupieId::GroupieId( VxGUID& userOnlineId, VxGUID& hostOnlineId, EHostType hostType )
    : m_UserOnlineId( userOnlineId )
    , m_HostedId( hostOnlineId, hostType )
{
}

//============================================================================
GroupieId::GroupieId( VxGUID& userOnlineId, HostedId& hostedId )
    : m_UserOnlineId( userOnlineId )
    , m_HostedId( hostedId )
{
}

//============================================================================
GroupieId::GroupieId( const GroupieId& rhs )
    : m_UserOnlineId( rhs.m_UserOnlineId )
    , m_HostedId( rhs.m_HostedId )
{
}

//============================================================================
GroupieId& GroupieId::operator =( const GroupieId& rhs )
{
	if( this != &rhs )
	{
        m_UserOnlineId            = rhs.m_UserOnlineId;
        m_HostedId                = rhs.m_HostedId;
	}

	return *this;
}

//============================================================================
bool GroupieId::operator == ( const GroupieId& rhs ) const
{
    return m_UserOnlineId == rhs.m_UserOnlineId &&  m_HostedId == rhs.m_HostedId;
}

//============================================================================
bool GroupieId::operator != ( const GroupieId& rhs ) const
{
    return !(*this == rhs);
}

//============================================================================
bool GroupieId::operator < ( const GroupieId& rhs ) const
{
    return m_UserOnlineId < rhs.m_UserOnlineId || (m_UserOnlineId == rhs.m_UserOnlineId && m_HostedId < rhs.m_HostedId );
}

//============================================================================
bool GroupieId::operator <= ( const GroupieId& rhs ) const
{
    if( *this == rhs )
    {
        return true;
    }

    if( *this < rhs )
    {
        return true;
    }

    return false;
}

//============================================================================
bool GroupieId::operator > ( const GroupieId& rhs ) const
{
    return m_UserOnlineId > rhs.m_UserOnlineId || (m_UserOnlineId == rhs.m_UserOnlineId && m_HostedId > rhs.m_HostedId );
}

//============================================================================
bool GroupieId::operator >= ( const GroupieId& rhs ) const
{
    if( *this == rhs )
    {
        return true;
    }

    if( *this > rhs )
    {
        return true;
    }

    return false;
}

//============================================================================
bool GroupieId::addToBlob( PktBlobEntry& blob )
{
    bool result = blob.setValue( m_UserOnlineId );
    result &= m_HostedId.addToBlob( blob );
    return result;
}

//============================================================================
bool GroupieId::extractFromBlob( PktBlobEntry& blob )
{
    bool result = blob.getValue( m_UserOnlineId );
    result &= m_HostedId.extractFromBlob( blob );
    return result;
}

// returns 0 if equal else -1 if less or 1 if greater
//============================================================================
int GroupieId::compareTo( GroupieId& groupieId )
{
    int result = 0;
    if( *this > groupieId )
    {
        result = 1;
    }
    else if( *this < groupieId )
    {
        result = -1;
    }

    if( 0 == result )
    {
        result = m_UserOnlineId.compareTo( groupieId.getUserOnlineId() );
    }

    return result;
}

// returns true if guids are same value
//============================================================================
bool GroupieId::isEqualTo( const GroupieId& groupieId )
{
    return *this == groupieId;
}

// get a description of the plugin id
//============================================================================
std::string GroupieId::describeGroupieId( void ) const
{
    std::string desc = m_HostedId.describeHostedId();
    desc += " user ";
    desc += m_UserOnlineId.toOnlineIdString();

    return desc;
}

//============================================================================
EPluginType GroupieId::getHostPluginType( void )
{
    return HostTypeToHostPlugin( getHostType() );
}

//============================================================================
EPluginType GroupieId::getClientPluginType( void )
{
    return HostTypeToClientPlugin( getHostType() );
}