//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================
#pragma once

#include <GuiInterface/IDefs.h>

#include <CoreLib/VxGUID.h>

#include <memory>

class P2PEngine;
class IConnectRequestCallback;
class VxSktBase;

class HandshakeInfo
{
public:
    HandshakeInfo() = default;
    HandshakeInfo( std::shared_ptr<VxSktBase>& sktBase, VxGUID& sessionId, VxGUID onlineId, IConnectRequestCallback* callback, EConnectReason connectReason, uint64_t timeStamp );
    HandshakeInfo( const HandshakeInfo& rhs );
    virtual ~HandshakeInfo() = default;

    HandshakeInfo&              operator =( HandshakeInfo && rhs ) = default;
    HandshakeInfo&				operator=( const HandshakeInfo& rhs );
    bool                        operator==( const HandshakeInfo& rhs );

    void                        setHanshakeComplete( bool complete ) { m_HanshakeComplete = complete; }

    uint64_t                    getTimeStamp( void ) const      { return m_TimeStamp; }
    VxGUID&                     getSocketId( void )             { return m_SocketId; }
    VxGUID&                     getSessionId( void )            { return m_SessionId; }
    std::shared_ptr<VxSktBase>& getSktBase( void )              { return m_SktBase; }
    IConnectRequestCallback*    getCallback( void )             { return m_Callback; }
    EConnectReason              getConnectReason( void )        { return m_ConnectReason; }

    void                        onContactConnected( void );
    void                        onHandshakeTimeout( void );
    void                        onContactSessionDone( void );
    void                        onSktDisconnected( void );

protected:
    std::shared_ptr<VxSktBase>  m_SktBase;
    VxGUID                      m_SocketId;
    VxGUID                      m_SessionId;
    VxGUID                      m_OnlineId;
    IConnectRequestCallback*    m_Callback{ nullptr };
    EConnectReason              m_ConnectReason{ eConnectReasonUnknown };
    uint64_t                    m_TimeStamp{ 0 };
    bool                        m_HanshakeComplete{ false };
};



