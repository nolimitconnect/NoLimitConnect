#pragma once
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

#include <GuiInterface/IDefs.h>

#include <CoreLib/VxGUID.h>

class PktBlobEntry;

#pragma pack(push)
#pragma pack(1)

class HostedId // size 16 + 1 = 17
{
public:
	HostedId() = default;
    HostedId( VxGUID& onlineId, EHostType hostType );
    HostedId( const HostedId& rhs );
    //do not use virtuals because this object is packed in packets
	HostedId&				    operator =( const HostedId& rhs );
    bool						operator == ( const HostedId& rhs ) const;
    bool						operator != ( const HostedId& rhs ) const;
    bool						operator < ( const HostedId& rhs ) const;
    bool						operator <= ( const HostedId& rhs ) const;
    bool						operator > ( const HostedId& rhs ) const;
    bool						operator >= ( const HostedId& rhs ) const;

    bool                        addToBlob( PktBlobEntry& blob );
    bool                        extractFromBlob( PktBlobEntry& blob );

    bool						setHostedId( VxGUID& onlineId, EHostType hostType ) { m_HostOnlineId = onlineId; setHostType( hostType ); return isValid(); }

    void						setHostOnlineId( VxGUID& onlineId )         { m_HostOnlineId = onlineId; }
    VxGUID&					    getHostOnlineId( void )                     { return m_HostOnlineId; }

    void						setHostType( EHostType hostType )           { m_HostType = (uint8_t)hostType; }
    EHostType  				    getHostType( void ) const                   
    { 
        return (EHostType)m_HostType; 
    }

    EPluginType                 getHostPluginType( void );
    EPluginType                 getClientPluginType( void );

    // returns 0 if equal else -1 if less or 1 if greater
    int							compareTo( HostedId& guid );
    // returns true if guids are same value
    bool						isEqualTo( const HostedId& guid );
    // get a description of the plugin id
    std::string                 describeHostedId( void ) const;

    bool                        isValid( void )                             { return eHostTypeUnknown != m_HostType && m_HostOnlineId.isVxGUIDValid(); }
    void                        clear( void )                               { m_HostOnlineId.clearVxGUID(); m_HostType = 0; }

protected:
	//=== vars ===//
    VxGUID					    m_HostOnlineId;
    uint8_t					    m_HostType{ 0 };
};

#pragma pack(pop)
