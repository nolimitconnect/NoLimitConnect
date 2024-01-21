#pragma once
//============================================================================
// Copyright (C) 2024 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PktTypes.h"
#include "GroupieId.h"

#include "PktsRandConnectDefs.h"

#pragma pack(push)
#pragma pack(1)

class PktRandConnectReq : public VxPktHdr
{
public:
    PktRandConnectReq();

    void                        setRandAction( ERandAction randAction )         { m_RandAction = (uint8_t)randAction; }
    ERandAction                 getRandAction( void )                           { return (ERandAction)m_RandAction; }

    void                        setAccessState( enum EPluginAccess accessState )     { m_AccessState = (uint8_t)accessState; }
    EPluginAccess               getAccessState( void )                          { return (EPluginAccess)m_AccessState; }
    void                        setSessionId( VxGUID& sessionId )               { m_SessionId = sessionId; }
    VxGUID                      getSessionId( void )                            { return m_SessionId; }

    void                        setUserOnlineId( VxGUID onlineId )              { m_UserOnlineId = onlineId; }
    VxGUID&                     getUserOnlineId( void )                         { return m_UserOnlineId; }
    void                        setHostOnlineId( VxGUID onlineId )              { m_HostOnlineId = onlineId; }
    VxGUID&                     getHostOnlineId( void )                         { return m_HostOnlineId; }

    void                        setGroupieId( GroupieId& groupieId )            { m_UserOnlineId = groupieId.getUserOnlineId(); m_HostOnlineId = groupieId.getHostOnlineId(); m_HostType = (uint8_t)groupieId.getHostType(); }
    GroupieId                   getGroupieId( void )                            { return GroupieId( m_UserOnlineId, m_HostOnlineId, (EHostType)m_HostType ); }

    void                        setToUserOnlineId( VxGUID onlineId )            { m_ToUserOnlineId = onlineId; }
    VxGUID&                     getToUserOnlineId( void )                       { return m_ToUserOnlineId; }

private:
    uint8_t					    m_HostType{ 0 };
    uint8_t					    m_PluginType{ 0 };	
    uint8_t					    m_AccessState{ 0 };	
    uint8_t					    m_RandAction{ 0 };
    uint32_t					m_Res3{ 0 };	
    uint64_t					m_TimeRequestedMs{ 0 };		
    uint64_t					m_Res4{ 0 };
    VxGUID                      m_SessionId;
    VxGUID                      m_UserOnlineId;
    VxGUID                      m_HostOnlineId;
    VxGUID                      m_ToUserOnlineId;
};

class PktRandConnectReply : public VxPktHdr
{
public:
    PktRandConnectReply();

    void                        setRandAction( ERandAction randAction )         { m_RandAction = (uint8_t)randAction; }
    ERandAction                 getRandAction( void )                           { return (ERandAction)m_RandAction; }

    void                        setAccessState( enum EPluginAccess accessState )     { m_AccessState = (uint8_t)accessState; }
    EPluginAccess               getAccessState( void )                          { return (EPluginAccess)m_AccessState; }
    void                        setSessionId( VxGUID& sessionId )               { m_SessionId = sessionId; }
    VxGUID                      getSessionId( void )                            { return m_SessionId; }

    void                        setUserOnlineId( VxGUID onlineId )              { m_UserOnlineId = onlineId; }
    VxGUID&                     getUserOnlineId( void )                         { return m_UserOnlineId; }
    void                        setHostOnlineId( VxGUID onlineId )              { m_HostOnlineId = onlineId; }
    VxGUID&                     getHostOnlineId( void )                         { return m_HostOnlineId; }

    void                        setGroupieId( GroupieId& groupieId )            { m_UserOnlineId = groupieId.getUserOnlineId(); m_HostOnlineId = groupieId.getHostOnlineId(); m_HostType = (uint8_t)groupieId.getHostType(); }
    GroupieId                   getGroupieId( void )                            { return GroupieId( m_UserOnlineId, m_HostOnlineId, (EHostType)m_HostType ); }

    void                        setToUserOnlineId( VxGUID onlineId )            { m_ToUserOnlineId = onlineId; }
    VxGUID&                     getToUserOnlineId( void )                       { return m_ToUserOnlineId; }

private:
    uint8_t					    m_HostType{ 0 };
    uint8_t					    m_PluginType{ 0 };	
    uint8_t					    m_AccessState{ 0 };	
    uint8_t					    m_RandAction{ 0 };
    uint32_t					m_Res3{ 0 };	
    uint64_t					m_TimeRequestedMs{ 0 };		
    uint64_t					m_Res4{ 0 };
    VxGUID                      m_SessionId;
    VxGUID                      m_UserOnlineId;
    VxGUID                      m_HostOnlineId;
    VxGUID                      m_ToUserOnlineId;
};



#pragma pack(pop)


