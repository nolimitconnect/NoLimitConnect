//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginBaseService.h"
#include "PluginMgr.h"
#include "P2PSession.h"
#include "RxSession.h"
#include "TxSession.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

#include <NetLib/VxPeerMgr.h>
#include <NetLib/VxSktBase.h>
#include <CoreLib/VxFileUtil.h>

//============================================================================
PluginBaseService::PluginBaseService( P2PEngine& engine, PluginMgr& pluginMgr, VxNetIdent* myIdent, EPluginType pluginType )
: PluginBase( engine, pluginMgr, myIdent, pluginType )
, m_HostType( PluginTypeToHostType( pluginType ) )
, m_HostedId( myIdent->getMyOnlineId(), m_HostType )
{
}

//============================================================================
void PluginBaseService::broadcastToClients( VxPktHdr* pktHdr, VxGUID& requesterOnlineId, std::shared_ptr<VxSktBase>& sktBaseRequester, bool includeRequester )
{
    if( pktHdr && pktHdr->isValidPkt() )
    {
        bool sentToRequestor{ false };
        VxGUID requestorSktConnectionId;
        if( sktBaseRequester )
        {
            requestorSktConnectionId = sktBaseRequester->getSocketId();
        }

        std::set<ConnectId> connectIdSet;
        std::set<ConnectId> relayedIdSet;
        if( m_Engine.getConnectIdListMgr().getConnections( getHostedId(), connectIdSet, relayedIdSet ) )
        {
            for( auto& connectId : connectIdSet )
            {
                VxGUID memberOnlineId = const_cast<ConnectId&>(connectId).getUserOnlineId();

                m_Engine.getPeerMgr().lockSktList();
                std::shared_ptr<VxSktBase> sktBase = m_Engine.getPeerMgr().findSktBase( const_cast<ConnectId&>(connectId).getSocketId(), true );
                if( sktBase && sktBase->isConnected() )
                {
                    if( sktBase->getPeerOnlineId() != memberOnlineId )
                    {
                        LogMsg( LOG_VERBOSE, "PluginBaseService::broadcastToClients peer %s id %s does NOT match user id %s",
                                sktBase->getPeerOnlineName().c_str(), sktBase->getPeerOnlineId().toOnlineIdString().c_str(), memberOnlineId.toOnlineIdString().c_str() );
                    }
                    else
                    {
                        LogMsg( LOG_VERBOSE, "PluginBaseService::broadcastToClients peer %s id %s does match user id %s",
                                sktBase->getPeerOnlineName().c_str(), sktBase->getPeerOnlineId().toOnlineIdString().c_str(), memberOnlineId.toOnlineIdString().c_str() );
                    }

                    bool isRequester = requestorSktConnectionId == sktBase->getSocketId();
                    if( isRequester && !includeRequester )
                    {
                        m_Engine.getPeerMgr().unlockSktList();
                        continue;
                    }
                    else if( !includeRequester && memberOnlineId == sktBase->getPeerOnlineId() )
                    {
                        LogMsg( LOG_ERROR, "PluginBaseService::broadcastToClients ERROR peer %s id %s does match user id %s but should not",
                                sktBase->getPeerOnlineName().c_str(), sktBase->getPeerOnlineId().toOnlineIdString().c_str(), memberOnlineId.toOnlineIdString().c_str() );
                        continue;
                    }

                    if( txPacket( memberOnlineId, sktBase, pktHdr, false, getClientPluginType() ) )
                    {
                        if( isRequester )
                        {
                            sentToRequestor = true;
                        }
                    }
                }

                m_Engine.getPeerMgr().unlockSktList();
            }
        }

        if( !sentToRequestor && includeRequester && sktBaseRequester && requesterOnlineId.isVxGUIDValid() )
        {
            // allways send to requester even if not still joined
            txPacket( requesterOnlineId, sktBaseRequester, pktHdr, false, getClientPluginType() );
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "PluginBaseService::broadcastToHostClients invalid pkt %s host %s", pktHdr->describePktHdr().c_str(), DescribeHostType( getHostType() ) );
    }
}

//============================================================================
void PluginBaseService::broadcastToClients( VxPktHdr* pktHdr, VxGUID& excludedOnlineId )
{
    if( pktHdr && pktHdr->isValidPkt() )
    {
        std::set<ConnectId> connectIdSet;
        std::set<ConnectId> relayedIdSet;
        if( m_Engine.getConnectIdListMgr().getConnections( getHostedId(), connectIdSet, relayedIdSet ) )
        {
            for( auto& connectId : connectIdSet )
            {
                VxGUID memberOnlineId = const_cast<ConnectId&>(connectId).getUserOnlineId();

                m_Engine.getPeerMgr().lockSktList();
                std::shared_ptr<VxSktBase> sktBase = m_Engine.getPeerMgr().findSktBase( const_cast<ConnectId&>(connectId).getSocketId(), true );
                if( sktBase && sktBase->isConnected() )
                {
                    if( sktBase->getPeerOnlineId() != memberOnlineId )
                    {
                        LogMsg( LOG_VERBOSE, "PluginBaseService::broadcastToClients peer id %s does NOT match user id %s",
                                sktBase->getPeerOnlineId().toOnlineIdString().c_str(), memberOnlineId.toOnlineIdString().c_str() );
                    }
                    else
                    {
                        LogMsg( LOG_VERBOSE, "PluginBaseService::broadcastToClients peer id %s does match user id %s",
                                sktBase->getPeerOnlineId().toOnlineIdString().c_str(), memberOnlineId.toOnlineIdString().c_str() );
                    }

                    bool isExcludeId = excludedOnlineId == sktBase->getPeerOnlineId();
                    if( isExcludeId )
                    {
                        m_Engine.getPeerMgr().unlockSktList();
                        continue;
                    }

                    if( !txPacket( memberOnlineId, sktBase, pktHdr, false, getClientPluginType() ) )
                    {
                        // logging ?
                    }
                }

                m_Engine.getPeerMgr().unlockSktList();
            }
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "PluginBaseService::broadcastToHostClients invalid pkt %s host %s", pktHdr->describePktHdr().c_str(),  DescribeHostType( getHostType() ) );
    }
}

//============================================================================
EConnectReason PluginBaseService::getHostAnnounceConnectReason( void )
{
    EConnectReason connectReason = eConnectReasonUnknown;
    switch( getPluginType() )
    {
    case ePluginTypeClientChatRoom:
    case ePluginTypeHostChatRoom:
        connectReason = eConnectReasonChatRoomAnnounce;
        break;
    case ePluginTypeClientGroup:
    case ePluginTypeHostGroup:
        connectReason = eConnectReasonGroupAnnounce;
        break;
    case ePluginTypeHostRandomConnect:
    case ePluginTypeClientRandomConnect:
        connectReason = eConnectReasonRandomConnectAnnounce;
        break;
    default:
        break;
    }

    return connectReason;
}

//============================================================================
EConnectReason PluginBaseService::getHostJoinConnectReason( void )
{
    EConnectReason connectReason = eConnectReasonUnknown;
    switch( getPluginType() )
    {
    case ePluginTypeClientChatRoom:
    case ePluginTypeHostChatRoom:
        connectReason = eConnectReasonChatRoomJoin;
        break;
    case ePluginTypeClientGroup:
    case ePluginTypeHostGroup:
        connectReason = eConnectReasonGroupJoin;
        break;
    case ePluginTypeHostRandomConnect:
    case ePluginTypeClientRandomConnect:
        connectReason = eConnectReasonRandomConnectJoin;
        break;
    default:
        break;
    }

    return connectReason;
}

//============================================================================
EConnectReason PluginBaseService::getHostSearchConnectReason( void )
{
    EConnectReason connectReason = eConnectReasonUnknown;
    switch( getPluginType() )
    {
    case ePluginTypeClientChatRoom:
    case ePluginTypeHostChatRoom:
        connectReason = eConnectReasonChatRoomSearch;
        break;
    case ePluginTypeClientGroup:
    case ePluginTypeHostGroup:
        connectReason = eConnectReasonGroupSearch;
        break;
    case ePluginTypeHostRandomConnect:
    case ePluginTypeClientRandomConnect:
        connectReason = eConnectReasonRandomConnectSearch;
        break;
    default:
        break;
    }

    return connectReason;
}
