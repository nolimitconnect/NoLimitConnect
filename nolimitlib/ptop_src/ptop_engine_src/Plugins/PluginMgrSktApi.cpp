//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PluginMgr.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

#include <NetLib/VxSktBase.h>
#include <NetLib/VxPeerMgr.h>

//============================================================================
bool PluginMgr::pluginApiSktConnectTo(		EPluginType			pluginType,	// plugin id
											VxNetIdentBase *	netIdent,		// identity of contact to connect to
											int					pvUserData,		// plugin defined data
											std::shared_ptr<VxSktBase>&		    ppoRetSkt, 		// returned Socket
											EConnectReason		connectReason )
{
	std::shared_ptr<VxSktBase> sktBase( nullptr );
	ppoRetSkt.reset();
	bool newConnection = false;
	if( true == m_Engine.connectToContact( netIdent->getConnectInfo(), sktBase, newConnection, connectReason ) )
	{
		ppoRetSkt = sktBase;
		return true;
	}

	return false;
}

//============================================================================
//! close socket connection
void PluginMgr::pluginApiSktClose( ESktCloseReason closeReason, std::shared_ptr<VxSktBase>& sktBase )
{
	sktBase->closeSkt(closeReason);
}

//============================================================================
//! close socket immediate.. don't bother to flush buffer
void PluginMgr::pluginApiSktCloseNow( ESktCloseReason closeReason, std::shared_ptr<VxSktBase>& sktBase )
{
	sktBase->closeSkt(closeReason,  false);
}

//============================================================================
bool PluginMgr::pluginApiTxPacket(  EPluginType			pluginType,
                                    const VxGUID&       onlineId,
                                    std::shared_ptr<VxSktBase>&          sktBase,
                                    VxPktHdr*           pktHdr,
                                    bool				bDisconnectAfterSend,
                                    EPluginType         overridePlugin )
{
    // when sending packets they are typically from plugin to the same remote plugin
    // for host/client we convert host to client and client to hot
    EPluginType hostClientType = ePluginTypeInvalid;
    switch( pluginType )
    {
    case ePluginTypeCamServer:
        hostClientType = ePluginTypeCamClient;
        break;
    case ePluginTypeCamClient:
        hostClientType = ePluginTypeCamServer;
        break;
    case ePluginTypeClientChatRoom:
        hostClientType = ePluginTypeHostChatRoom;
        break;
    case ePluginTypeFileShareServer:
        hostClientType = ePluginTypeFileShareClient;
        break;
    case ePluginTypeFileShareClient:
        hostClientType = ePluginTypeFileShareServer;
        break;
    case ePluginTypeHostChatRoom:
        hostClientType = ePluginTypeClientChatRoom;
        break;
    case ePluginTypeClientConnectTest:
        hostClientType = ePluginTypeHostConnectTest;
        break;
    case ePluginTypeHostConnectTest:
        hostClientType = ePluginTypeClientConnectTest;
        break;
    case ePluginTypeClientGroup:
        hostClientType = ePluginTypeHostGroup;
        break;
    case ePluginTypeHostGroup:
        hostClientType = ePluginTypeClientGroup;
        break;
    case ePluginTypeClientPeerUser:
        hostClientType = ePluginTypeHostPeerUser;
        break;
    case ePluginTypeHostPeerUser:
        hostClientType = ePluginTypeClientPeerUser;
        break;
    case ePluginTypeClientRandomConnect:
        hostClientType = ePluginTypeHostRandomConnect;
        break;
    case ePluginTypeHostRandomConnect:
        hostClientType = ePluginTypeClientRandomConnect;
        break;
    case ePluginTypeAboutMePageServer:
        hostClientType = ePluginTypeAboutMePageClient;
        break;
    case ePluginTypeAboutMePageClient:
        hostClientType = ePluginTypeAboutMePageServer;
        break;
    case ePluginTypeStoryboardServer:
        hostClientType = ePluginTypeStoryboardClient;
        break;
    case ePluginTypeStoryboardClient:
        hostClientType = ePluginTypeStoryboardServer;
        break;

    default:
        break;
    }

    if( overridePlugin != ePluginTypeInvalid )
    {
        pktHdr->setPluginNum( ( uint8_t )overridePlugin );
    }
    else if( hostClientType != ePluginTypeInvalid )
    {
        pktHdr->setPluginNum( ( uint8_t )hostClientType );
    }
    else
    {
        pktHdr->setPluginNum( ( uint8_t )pluginType );
    }

    pktHdr->setSrcOnlineId( m_Engine.getMyOnlineId() );

    if( onlineId == m_Engine.getMyOnlineId() && sktBase->isLoopbackSocket() )
    {
        // destination is ourself
        LogModule( eLogPkt, LOG_VERBOSE, "pluginApiTxPacket type %d len %d loopback %s", pktHdr->getPktType(), pktHdr->getPktLength(), DescribePluginType( pluginType ) );
        pktHdr->setDestOnlineId( onlineId );
        sktBase->txPacketWithDestId( pktHdr );
        return true;
    }

    LogModule( eLogPkt, LOG_VERBOSE, "pluginApiTxPacket type %d len %d plugin %s to %s", pktHdr->getPktType(), pktHdr->getPktLength(), 
        DescribePluginType( pluginType ), sktBase->getRemoteIpAddress() );
    return m_Engine.getPeerMgr().txPacket( sktBase, onlineId, pktHdr, bDisconnectAfterSend );
}

