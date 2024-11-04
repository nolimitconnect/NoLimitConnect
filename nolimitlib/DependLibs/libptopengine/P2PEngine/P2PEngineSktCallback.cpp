//============================================================================
// Copyright (C) 2009 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "P2PEngine.h"

#include "NetServices/NetServicesMgr.h"
#include "P2PConnectList.h"

#include "Network/NetworkDefs.h" 
#include "Network/NetworkMgr.h"

#include "BigListLib/BigListInfo.h"
#include <Plugins/PluginMgr.h>

#include <NetLib/VxSktBase.h>
#include <PktLib/PktsRelay.h>
#include <NetLib/VxSktCrypto.h>

#include <string.h>

//============================================================================
void P2PEngine::handleTcpData( std::shared_ptr<VxSktBase>& sktBase )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

    if( sktBase->isUdpSocket() )
    {
        LogMsg( LOG_ERROR, "P2PEngine::handleTcpData: attempted handle UDP data" );
        return;
    }

    if( !sktBase->isConnected() )
    {
        LogModule( eLogSktData, LOG_INFO, "P2PEngine::handleTcpData: callback was for data but socket is not connected" );
        return;
    }

	int	iDataLen =	sktBase->getSktBufDataLen();
	if( iDataLen < sizeof(VxPktHdr) || iDataLen & 0x0f )
	{
		return;
	}

	VxGUID firstPktSignature;
	uint32_t u32UsedLen = 0;
	// app should get the buffer ( this also locks it from being modified by threads )
	// then read the data then call sktBufAmountRead ( releases mutex )
    uint8_t * pSktBuf = (uint8_t *)sktBase->getSktReadBuf();
	VxPktHdr* pktHdr = (VxPktHdr*)pSktBuf;
	if( sktBase->isFirstRxPacket() )
	{
		if( false == sktBase->isRxEncryptionKeySet() )
		{
			// first packet can be PKT_ANNOUNCE or a NetService req/reply
			// check for NetService first
			bool wasNetServiceRequest{ false };
            bool netServiceRequestRejected{ false };
            bool permissionError{ false };
            uint16_t pktType{0};
            std::string pktTypeDesc;
            std::string cryptoPwd;
            if( getNetServicesMgr().getNetPktRxCryptoPassword( cryptoPwd, sktBase ) )
            {
                std::unique_ptr<VxCrypto> netServCrypto = std::make_unique<VxCrypto>();

                netServCrypto->setPassword( cryptoPwd.c_str(), cryptoPwd.size() );

                // use copy of data because encyption key is different for net services
                uint8_t* bufCopy = new uint8_t[iDataLen];
                memcpy( bufCopy, pSktBuf, iDataLen );
                if( 0 == netServCrypto->decrypt( bufCopy, iDataLen ) )
                {
                    VxPktHdr* pktHdrNetServ = (VxPktHdr*)bufCopy;
                    if( pktHdrNetServ->isValidPkt() && pktHdrNetServ->isNetServicePkt() && iDataLen >= pktHdrNetServ->getPktLength() )
                    {
                        wasNetServiceRequest = true;
                        pktType = pktHdrNetServ->getPktType();
                        if( getNetServicesMgr().shouldHandleNetServicePacket() )
                        {
                            sktBase->setIsNetServiceConnection( true );
                            wasNetServiceRequest = getNetServicesMgr().handlePktNetService( sktBase, pktHdrNetServ, permissionError );
                            if( !wasNetServiceRequest )
                            {
                                // probably a permission error
                                // should this be considered a hack?
                                sktBase->setIsNetServiceConnection( false );
                            }
                        }
                        else
                        {
                            netServiceRequestRejected = true;
                            pktTypeDesc = pktHdrNetServ->describePktType( pktType );
                        }
                    }
                    else
                    {
                        LogMsg( LOG_ERROR, "P2PEngine::handleTcpData Bad Encryption for Rx net service packet or is really PKT_ANNOUNCE from ip %s", sktBase->getRemoteIp().c_str() );
                    }
                }

                delete[] bufCopy;
            }
            else
            {
                // make a signature in case it needs to be recorded as hacker
                firstPktSignature.fromRawData( pSktBuf );
            }

            if( permissionError )
            {
                firstPktSignature.fromRawData( pSktBuf );
                hackerOffense( eHackerLevelMedium, eHackerReasonAccessDenied, sktBase->getRemoteIpBinary(), firstPktSignature, "Hacker attempted disabled net service ip %s", sktBase->getRemoteIp().c_str() );
                sktBase->closeSkt( eSktCloseNetServiceHandled );
                return;
            }
            else if( wasNetServiceRequest )
            {
                sktBase->sktBufAmountRead( iDataLen );  // release mutex
                sktBase->setIsFirstRxPacket( false );
                if( netServiceRequestRejected )
                {
                    // this can happen if is port open test times out before net service ping request is received
                    LogMsg( LOG_DEBUG, "Rejected net service pkt type %s from ip %s", pktTypeDesc.c_str(), sktBase->getRemoteIp().c_str() );
                }

                return;
            }
            else
            {
                // make a signature in case it needs to be recorded as hacker
                firstPktSignature.fromRawData( pSktBuf );
            }
			
			if( wasNetServiceRequest )
			{
				return;
			}
			else
			{	
				// make a signature in case it needs to be recorded as hacker
				firstPktSignature.fromRawData( pSktBuf );
			}
		}
		else
		{
			sktBase->decryptReceiveData();
			iDataLen = sktBase->getRxDecryptedLen();
		}

		// not ptop.. better be a announce packet
		if( (false == sktBase->isAcceptSocket()) 
			&& (false == sktBase->isRxEncryptionKeySet() ) )
		{
			// we connected out but never set our key
			vx_assert( false );
		}

        if( false == sktBase->isRxEncryptionKeySet() )
		{
			// this data has not been decrypted.. set encryption key and decrypt it
			GenerateRxConnectionKey( sktBase, &m_PktAnn.m_DirectConnectId, m_NetworkMgr.getNetworkKey() );
			sktBase->decryptReceiveData();
			iDataLen = sktBase->getRxDecryptedLen();
		}

		if( ( PKT_TYPE_ANNOUNCE != pktHdr->getPktType() ) ||
			 ( pktHdr->getPktLength() < sizeof( PktAnnounce ) - 100 ) ||  
			 ( pktHdr->getPktLength() > sizeof( PktAnnounce ) + 100 ) ) // leave room for expanding pkt announce in the future
		{
			if( pktHdr->isValidPkt() && pktHdr->getPktType() == PKT_TYPE_PING_REQ )
			{
				// ping request can happen depending on timing.. not really a hack attack so do not block the ip address
				LogMsg( LOG_ERROR, "First packet is ping request pkt skt %d type %d length %d ip %s:%d id %s signature %s", sktBase->getSktNumber(),
					pktHdr->getPktType(),
					pktHdr->getPktLength(),
					sktBase->getRemoteIp().c_str(),
					sktBase->getRemotePort(),
					firstPktSignature.toOnlineIdString().c_str(),
					firstPktSignature.toAsci().c_str() );

				if( iDataLen != pktHdr->getPktLength() )
				{
					// do not try to recover just close it
					sktBase->closeSkt( eSktClosePktPingRequestIsFirstPkt );
					return;
				}
				else
				{
					// TODO check if we are testing connection
					PktHandlerBase::handlePkt( sktBase, pktHdr );
					return;
				}
			}

			if( pktHdr->isValidPkt() && pktHdr->getPktType() == PKT_TYPE_IM_ALIVE_REQ )
			{
				// ping request can happen depending on timing.. not really a hack attack so do not block the ip address
				LogMsg( LOG_ERROR, "First packet is im alive request pkt skt %d type %d length %d ip %s:%d id %s signature %s", sktBase->getSktNumber(),
					pktHdr->getPktType(),
					pktHdr->getPktLength(),
					sktBase->getRemoteIp().c_str(),
					sktBase->getRemotePort(),
					firstPktSignature.toOnlineIdString().c_str(),
					firstPktSignature.toAsci().c_str() );

				if( iDataLen != pktHdr->getPktLength() )
				{
					// do not try to recover just close it
					sktBase->closeSkt( eSktClosePktPingRequestIsFirstPkt );
					return;
				}
				else
				{
					// just ignore it
					sktBase->sktBufAmountRead( iDataLen );
					return;
				}
			}

			if( pktHdr->isValidPkt() && pktHdr->getPktType() == PKT_TYPE_HOST_INVITE_ANN_REQ )
			{
				// ping request can happen depending on timing.. not really a hack attack so do not block the ip address
				LogMsg( LOG_ERROR, "First packet is host invite request pkt skt %d type %d length %d ip %s:%d id %s signature %s", sktBase->getSktNumber(),
					pktHdr->getPktType(),
					pktHdr->getPktLength(),
					sktBase->getRemoteIp().c_str(),
					sktBase->getRemotePort(),
					firstPktSignature.toOnlineIdString().c_str(),
					firstPktSignature.toAsci().c_str() );

				if( iDataLen != pktHdr->getPktLength() )
				{
					// do not try to recover just close it
					sktBase->closeSkt( eSktClosePktPingRequestIsFirstPkt );
					return;
				}
				else
				{
					sktBase->sktBufAmountRead( iDataLen );
					return;
				}
			}

			// somebody tried to send crap .. this may be a hack attack or it may be that our ip and port is same as someone else or network key has changed
			LogMsg( LOG_SEVERE, "First packet is not Announce pkt skt %d type %d length %d ip %s:%d id %s signature %s", sktBase->getSktNumber(),
																							pktHdr->getPktType(),  
																							pktHdr->getPktLength(),
                                                                                            sktBase->getRemoteIp().c_str(),
																							sktBase->getRemotePort(),
																							firstPktSignature.toOnlineIdString().c_str(),
																							firstPktSignature.toAsci().c_str() );
			// release the skt buffer mutex
			sktBase->sktBufAmountRead( 0 );
            hackerOffense( eHackerLevelMedium, eHackerReasonPktAnnNotFirstPacket, sktBase->getRemoteIpBinary(), firstPktSignature, "Hacker no announcement attack from ip %s", sktBase->getRemoteIp().c_str() );
			sktBase->closeSkt( eSktClosePktAnnNotFirstPacket );
			return;
		}

		if( false == pktHdr->isPktAllHere(iDataLen) )
		{
			// not all of packet arrived
			return;
		}

		// pkt announce has arrived
		PktAnnounce* pktAnn = ( PktAnnounce* )pktHdr;
		if( !pktHdr->isValidPktHdr() || !validateIdent( (VxNetIdent*)pktAnn ) )
		{
			// invalid announcement packet
			sktBase->setIsFirstRxPacket( false ); 
			// release the mutex
			sktBase->sktBufAmountRead( 0 );
            LogMsg( LOG_ERROR, "Invalid Packet announce from ip %s", sktBase->getRemoteIp().c_str() );
			hackerOffense( eHackerLevelMedium, eHackerReasonPktAnnNotFirstPacket, sktBase->getRemoteIpBinary(), firstPktSignature, "Hacker invalid announcement attack from ip %s", sktBase->getRemoteIp().c_str() );
			// disconnect
			sktBase->closeSkt( eSktClosePktAnnInvalid );
		}

		// NOTE: TODO check if is in our Ident ignore list

		//LogMsg( LOG_INFO, "Got Ann on Skt %d\n", sktBase->m_SktNumber );

		u32UsedLen = pktHdr->getPktLength();

		sktBase->setIsFirstRxPacket( false );
		onPktAnnounce( sktBase, pktHdr );
		// success
		// fall thru in case there are more packets
	}

    while( !VxIsAppShuttingDown() )
	{
		//LogMsg( LOG_INFO, "AppRcpCallback.. 3 Skt %d num %d Total Len %d Used Len %d decrypted Len %d\n", 
		//	sktBase->GetSocketId(),
		//	sktBase->m_SktNumber,
		//	u32Len,
		//	u32UsedLen,
		//	sktBase->m_u32DecryptedLen );
		if( false == sktBase->isConnected() )
		{
			//socket has been closed
            LogModule( eLogSktData, LOG_INFO, "P2PEngine::handleTcpData: callback was for data but socket is not connected" );
			return;
		}

		if( sktBase->getRxDecryptedLen() <= u32UsedLen )
		{
			//all decrypted data used up
			break;
		}

		if( sizeof( VxPktHdr ) > ( sktBase->getRxDecryptedLen() - u32UsedLen ) )
		{
			//not enough for a valid packet
			break;
		}

		pktHdr = (VxPktHdr*)&pSktBuf[ u32UsedLen ];
		if( false == pktHdr->isValidPkt() )
		{
			// invalid data
			hackerOffense( eHackerLevelMedium, eHackerReasonPktHdrInvalid, nullptr, sktBase->getRemoteIpBinary(), "Invalid VxPktHdr" );
			// release the mutex
			sktBase->sktBufAmountRead( 0 );
			sktBase->closeSkt( eSktClosePktHdrInvalid );
			return;
		}

		uint16_t u16PktLen = pktHdr->getPktLength();

		if( u32UsedLen + u16PktLen > sktBase->getRxDecryptedLen() )
		{
			//not all of packet is here
			//LogMsg( LOG_VERBOSE,  "AppRcpCallback.. Skt %d num %d Not all of packet arrived\n", 
			//		sktBase->GetSocketId(),
			//		sktBase->m_SktNumber );
			break;
		}

		uint16_t pktType = pktHdr->getPktType();
		if( PKT_TYPE_IM_ALIVE_REQ != pktType && PKT_TYPE_IM_ALIVE_REPLY != pktType && PKT_TYPE_PING_REQ != pktType && PKT_TYPE_PING_REPLY != pktType )
		{
			sktBase->setLastSessionTimeMs( GetGmtTimeMs() );
		}

		if( pktHdr->getDestOnlineId() == getMyOnlineId() )
		{
			PktHandlerBase::handlePkt( sktBase, pktHdr );
		}
		else
		{
			handleIncommingRelayData( sktBase, pktHdr );
		}

		//we used up this packet
		u32UsedLen += u16PktLen;
	}			

	sktBase->sktBufAmountRead( u32UsedLen );
}

//============================================================================
void P2PEngine::handleIncommingRelayData( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr )
{
	getRelayMgr().handleRelayPkt( sktBase, pktHdr );
	/*
	// Relay now handled by a Relay Manager. TODO remove relay plugin if no longer needed
	bool dataWasRelayed = false;
	bool relayAvailable = false;
	uint16_t pktType = pktHdr->getPktType();
	std::string strSrcId;
	pktHdr->getSrcOnlineId().getVxGUID( strSrcId );
	std::string strDestId;
	pktHdr->getDestOnlineId().getVxGUID( strDestId );
	uint16_t pktLen = pktHdr->getPktLength();
	LogMsg( LOG_ERROR, "handleIncommingRelayData pkt type %d len %d src id %s dest id %s", 
									pktType, pktLen, strSrcId.c_str(), strDestId.c_str() );
	if( false == pktHdr->getDestOnlineId().isVxGUIDValid() )
	{
		LogMsg( LOG_ERROR, "handleIncommingRelayData invalid dest id pkt type %d len %d", pktHdr->getPktType(), pktHdr->getPktLength() );
		return;
	}

	RCODE rc = 0;
	if( getPluginServiceRelay().isUserRelayOk( pktHdr->getSrcOnlineId(), pktHdr->getDestOnlineId() ) )
	{
		relayAvailable = true;
		m_ConnectionList.connectListLock();
		RcConnectInfo * connectInfo = m_ConnectionList.findConnection( pktHdr->getDestOnlineId(), true );
		if( 0 != connectInfo )
		{
			rc = connectInfo->getSkt()->txPacket( pktHdr->getDestOnlineId(), pktHdr );
			if( 0 == rc )
			{
				dataWasRelayed = true;
			}
		}

		m_ConnectionList.connectListUnlock();
	}

	if( false == dataWasRelayed )
	{
		LogMsg( LOG_INFO, "Not Relayed data pkt type %d len %d relayAvail %d", pktHdr->getPktType(), pktHdr->getPktLength(), relayAvailable );
		PktRelayUserDisconnect		relayReply;
		relayReply.m_UserId			= pktHdr->getDestOnlineId();
		relayReply.setSrcOnlineId( m_PktAnn.getMyOnlineId() );
		sktBase->txPacket( pktHdr->getSrcOnlineId(), &relayReply );	
	}
	*/
}

//============================================================================
void P2PEngine::handleMulticastData( std::shared_ptr<VxSktBase>& sktBase )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	int	iDataLen		= sktBase->getSktBufDataLen();
	char * pSktBuf		= (char *)sktBase->getSktReadBuf();
	if( false == VxIsEncryptable( iDataLen ) )
	{
		//LogMsg( LOG_INFO, "RcSysSktMgr::HandleUdpData: Skt %d not encryptable len %d\n", sktBase->m_SktNumber, iDataLen );
		sktBase->sktBufAmountRead( iDataLen );
		return;
	}

	// decrypt
	VxPktHdr* poPkt = (VxPktHdr*)pSktBuf;
	VxSymDecrypt( &sktBase->m_RxKey, pSktBuf, iDataLen );
	if( poPkt->isValidPkt() )
	{
		//LogMsg( LOG_INFO, "RcSysSktMgr::HandleUdpData: Skt %d valid packet\n", sktBase->m_SktNumber );
		// valid pkt
		if( PKT_TYPE_ANNOUNCE == poPkt->getPktType() )
		{
			//LogMsg( LOG_INFO, "RcSysSktMgr::HandleUdpData: Skt %d from %s port %d PktAnnounce\n", sktBase->m_SktNumber, sktBase->m_strRmtIp.c_str(), sktBase->m_u16RmtPort );
			//LogMsg( LOG_INFO, "RcSysSktMgr::HandleUdpData: ProcessingSktEvent\n" );
			onPktAnnounce( sktBase, poPkt );
		}
	}
	else
	{
		LogMsg( LOG_ERROR, "P2PEngine::handleMulticastData: Invalid Packet" );
	}

	sktBase->sktBufAmountRead( iDataLen );
}
