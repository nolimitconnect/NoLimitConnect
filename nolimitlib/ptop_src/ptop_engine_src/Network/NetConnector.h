#pragma once
//============================================================================
// Copyright (C) 2014 Brett R. Jones
// Issued to MIT style license by Brett R. Jones in 2017
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

#include "NetworkDefs.h"
#include "ConnectRequest.h"

#include <PktLib/VxFriendMatch.h>
#include <CoreLib/VxThread.h>
#include <CoreLib/VxMutex.h>
#include <CoreLib/VxSemaphore.h>

#include <vector>
#include <memory>

class P2PEngine;
class BigListMgr;
class VxSktBase;
class PktAnnounce;
class P2PConnectList;
class NetworkMgr;
class VxPktHdr;
class VxPeerMgr;
class VxGUID;
class HostList;
class BigListInfo;

class NetConnector
{
public:
	NetConnector( P2PEngine& engine );
	virtual ~NetConnector();

	void						netConnectorStartup( void );
	void						stayConnectedStartup( void );
	void						netConnectorShutdown( void );
	void						stayConnectedShutdown( void );

	bool						connectToContact(	VxConnectInfo&				connectInfo, 
													std::shared_ptr<VxSktBase>&	ppoRetSkt,
													bool&						retIsNewConnection,
													EConnectReason				connectReason );

	RCODE						directConnectTo(	VxConnectInfo&				connectInfo,		 
													std::shared_ptr<VxSktBase>&	ppoRetSkt,		 
													EConnectReason				connectReason );
												
	bool						sendMyPktAnnounce(  VxGUID&						destinationId,
													std::shared_ptr<VxSktBase>&	sktBase, 
													bool						requestAnnReply = false,
													bool						requestReverseConnection = false,
													bool						requestSTUN = false );

	void						doNetConnectionsThread( void );
	void						doStayConnectedThread( void );

	bool						doConnectRequest( ConnectRequest& connectRequest, bool ignoreToSoonToConnectAgain = false );

	void						closeConnection( ESktCloseReason closeReason, VxGUID& onlineId, std::shared_ptr<VxSktBase>& skt, BigListInfo * poInfo = NULL );
	void						addConnectRequestToQue( VxConnectInfo& connectInfo, EConnectReason connectReason = eConnectReasonStayConnected, bool addToHeadOfQue = false, bool replaceExisting = true );

	void						handleConnectSuccess(  BigListInfo * bigListInfo, std::shared_ptr<VxSktBase>& skt, bool isNewConnection, EConnectReason connectReason );

protected:
	bool						connectUsingTcp(	VxConnectInfo&				connectInfo, 
													std::shared_ptr<VxSktBase>&	ppoRetSkt,
													EConnectReason				connectReason );

	bool						tryIPv6Connect(	VxConnectInfo&				connectInfo, 
												std::shared_ptr<VxSktBase>&	ppoRetSkt );

	bool						txPacket(	VxGUID&						destinationId, 
											std::shared_ptr<VxSktBase>&	sktBase, 
											VxPktHdr*					poPkt );

	void						addConnectRequestToQue( ConnectRequest& connectRequest, bool addToHeadOfQue = false, bool replaceExisting = true );

	//=== vars ===//
	VxThread					m_NetConnectThread;
	VxThread					m_StayConnectedThread;
	P2PEngine&					m_Engine;
	BigListMgr&					m_BigListMgr;
	PktAnnounce&				m_PktAnn;
	P2PConnectList&				m_ConnectList;
	NetworkMgr&					m_NetworkMgr;
	VxPeerMgr&					m_PeerMgr;

	VxMutex						m_NetConnectorMutex;
	VxSemaphore					m_WaitForConnectWorkSemaphore;
	std::vector<ConnectRequest>	m_IdentsToConnectList;
};

