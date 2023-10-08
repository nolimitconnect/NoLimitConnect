#pragma once
//============================================================================
// Copyright (C) 2023 Brett R. Jones 
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

#include "PktTypes.h"
#include "PktBlobEntry.h"

#include <GuiInterface/IDefs.h>

#pragma pack(push)
#pragma pack(1)
class PktTestConnTestReq : public VxPktHdr
{
public:
    PktTestConnTestReq();

    void                        calcPktLen( void );

    void						setSessionId( VxGUID& guid )        { m_SessionId = guid; }
    VxGUID&                     getSessionId( void )                { return m_SessionId; }

    void                        setHostType( EHostType hostType )   { m_HostType = ( uint8_t )hostType; }
    EHostType                   getHostType( void ) const           { return ( EHostType )m_HostType; }

    void                        setCommError( enum ECommErr commError )  { m_CommError = ( uint8_t )commError; }
    ECommErr                    getCommError( void ) const          { return ( ECommErr )m_CommError; }

    bool                        setNetCmd( std::string& netCmd );
    bool                        getNetCmd( std::string& netCmd );

    PktBlobEntry&               getBlobEntry( void )                { return m_BlobEntry; }

private:
    //=== vars ===//
    // VxPktHdr 40 bytes 
    VxGUID                      m_SessionId;        // 16 bytes
    uint8_t					    m_HostType{ 0 };    // 1 byte
    uint8_t					    m_CommError{ 0 };   // 1 byte
    uint16_t                    m_Res1{ 0 };        // fill to 16 byte boundry
    uint32_t                    m_Res2{ 0 };        // fill to 16 byte boundry

    PktBlobEntry                m_BlobEntry;	    // size 14352
};

class PktTestConnTestReply : public VxPktHdr
{
public:
    PktTestConnTestReply();

    void                        calcPktLen( void );

    void						setSessionId( VxGUID& guid )    { m_SessionId = guid; }
    VxGUID&                     getSessionId( void )            { return m_SessionId; }

    void                        setHostType( enum EHostType hostType ) { m_HostType = ( uint8_t )hostType; }
    EHostType                   getHostType( void ) const       { return ( EHostType )m_HostType; }

    void                        setCommError( enum ECommErr commError ) { m_CommError = ( uint8_t )commError; }
    ECommErr                    getCommError( void ) const      { return ( ECommErr )m_CommError; }

    bool                        setNetCmd( std::string& netCmd );
    bool                        getNetCmd( std::string& netCmd );

    PktBlobEntry&               getBlobEntry( void )            { return m_BlobEntry; }

private:
    //=== vars ===//
    // VxPktHdr 40 bytes 
    VxGUID                      m_SessionId;        // 16 bytes
    uint8_t					    m_HostType{ 0 };    // 1 byte
    uint8_t					    m_CommError{ 0 };   // 1 byte
    uint16_t                    m_Res1{ 0 };        // fill to 16 byte boundry
    uint32_t                    m_Res2{ 0 };        // fill to 16 byte boundry

    PktBlobEntry                m_BlobEntry;	    // size 14352
};


class PktTestConnPingReq : public VxPktHdr
{
public:
    PktTestConnPingReq();

    void                        calcPktLen( void );

    void                        setHostType( EHostType hostType )   { m_HostType = ( uint8_t )hostType; }
    EHostType                   getHostType( void ) const           { return ( EHostType )m_HostType; }

    void                        setCommError( enum ECommErr commError ) { m_CommError = ( uint8_t )commError; }
    ECommErr                    getCommError( void ) const          { return ( ECommErr )m_CommError; }

    void						setSessionId( VxGUID& guid )        { m_SessionId = guid; }
    VxGUID&                     getSessionId( void )                { return m_SessionId; }

    bool                        setNetCmd( std::string& netCmd );
    bool                        getNetCmd( std::string& netCmd );

    PktBlobEntry&               getBlobEntry( void )                { return m_BlobEntry; }

protected:
    //=== vars ===//
    // VxPktHdr 40 bytes 
    VxGUID                      m_SessionId;        // 16 bytes
    uint8_t					    m_HostType{ 0 };    // 1 byte
    uint8_t					    m_CommError{ 0 };   // 1 byte
    uint16_t                    m_Res1{ 0 };        // fill to 16 byte boundry
    uint32_t                    m_Res2{ 0 };        // fill to 16 byte boundry

    PktBlobEntry                m_BlobEntry;	    // size 14352
};

class PktTestConnPingReply : public VxPktHdr
{
public:
    PktTestConnPingReply();

    void                        calcPktLen( void );

    void                        setHostType( EHostType hostType )   { m_HostType = ( uint8_t )hostType; }
    EHostType                   getHostType( void ) const           { return ( EHostType )m_HostType; }

    void                        setCommError( ECommErr commError )  { m_CommError = ( uint8_t )commError; }
    ECommErr                    getCommError( void ) const          { return ( ECommErr )m_CommError; }

    void						setSessionId( VxGUID& guid )        { m_SessionId = guid; }
    VxGUID&                     getSessionId( void )                { return m_SessionId; }

    bool                        setNetCmd( std::string& netCmd );
    bool                        getNetCmd( std::string& netCmd );

    PktBlobEntry&               getBlobEntry( void )                { return m_BlobEntry; }

protected:
    //=== vars ===//
    // VxPktHdr 40 bytes 
    VxGUID                      m_SessionId;        // 16 bytes
    uint8_t					    m_HostType{ 0 };    // 1 byte
    uint8_t					    m_CommError{ 0 };   // 1 byte
    uint16_t                    m_Res1{ 0 };        // fill to 16 byte boundry
    uint32_t                    m_Res2{ 0 };        // fill to 16 byte boundry

    PktBlobEntry                m_BlobEntry;	    // size 14352
};

#pragma pack(pop)

