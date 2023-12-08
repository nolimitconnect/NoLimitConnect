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
void PluginBaseService::broadcastToClients( VxPktHdr* pktHdr, VxGUID& requestorOnlineId, std::shared_ptr<VxSktBase>& sktBaseRequester )
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
                m_Engine.getPeerMgr().lockSktList();
                std::shared_ptr<VxSktBase> sktBase = m_Engine.getPeerMgr().findSktBase( const_cast<ConnectId&>(connectId).getSocketId(), true );
                if( sktBase && sktBase->isConnected() )
                {
                    if( txPacket( const_cast<ConnectId&>(connectId).getUserOnlineId(), sktBase, pktHdr, false, getClientPluginType() ) )
                    {
                        if( requestorSktConnectionId == sktBase->getSocketId() )
                        {
                            sentToRequestor = true;
                        }
                    }
                }

                m_Engine.getPeerMgr().unlockSktList();
            }
        }

        if( !sentToRequestor && sktBaseRequester && requestorOnlineId.isVxGUIDValid() )
        {
            // allways send to requester even if not still joined
            txPacket( requestorOnlineId, sktBaseRequester, pktHdr );
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "PluginBaseService::broadcastToHostClients invalid pkt host %s", DescribeHostType( getHostType() ) );
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
