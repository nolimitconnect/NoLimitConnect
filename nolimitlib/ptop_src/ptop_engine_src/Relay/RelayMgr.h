#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
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

#include <memory>

class GroupieInfo;
class P2PEngine;
class PktAnnounce;
class PluginBaseRelay;
class VxSktBase;
class VxPktHdr;
class BigListInfo;
class VxGUID;

class RelayMgr
{
public:
	RelayMgr( P2PEngine& engine );
	virtual ~RelayMgr() = default;

	bool						handleRelayPkt( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr );

	bool						onRelayPktAnnounce( std::shared_ptr<VxSktBase>& sktBase, PktAnnounce* pktAnn, BigListInfo* srcBigInfo, BigListInfo* destBigInfo );

	bool						isJoinedToRelayHost( VxGUID& onlineId );

	bool						sendRelayError( VxPktHdr* pktHdr, VxGUID& srcOnlineId, VxGUID& destOnlineId, std::shared_ptr<VxSktBase>& sktBase, ERelayErr relayErr );

	//=== vars ====//
	P2PEngine&					m_Engine;
};
