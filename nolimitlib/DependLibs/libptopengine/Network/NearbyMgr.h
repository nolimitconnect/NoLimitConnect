#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "RcMulticast.h"
#include <NetworkMonitor/NetStatusAccum.h>

#include <CoreLib/VxGUID.h>

#include <vector>

class BigListInfo;

class NearbyMgr : public IMulticastListenCallback, public NetAvailStatusCallbackInterface, public RcMulticast
{
public:
    const int                   MIN_TIME_MULTICAST_CONNECT_ATTEMPT = 60000;

    NearbyMgr() = delete;
    NearbyMgr( P2PEngine& engine, NetworkMgr& networkMgr );
    virtual ~NearbyMgr() = default;

    void                        lockList( void ) { m_ListMutex.lock(); }
    void                        unlockList( void ) { m_ListMutex.unlock(); }

    void						networkMgrStartup( void );
    void						networkMgrShutdown( void );

    virtual bool                fromGuiNearbyBroadcastEnable( bool enable );
    virtual void				fromGuiNetworkAvailable( const char* lclIp, bool isCellularNetwork = false );
    virtual void				fromGuiNetworkLost( void );

    virtual void				callbackInternetStatusChanged( EInternetStatus internetStatus ) override;
    virtual void				callbackNetAvailStatusChanged( ENetAvailStatus netAvalilStatus ) override;

    bool                        isNearby( VxGUID& onlineId );
    virtual void                updateIdent( VxGUID& onlineId, int64_t timestamp );
    virtual void                removeIdent( VxGUID& onlineId );

    std::vector<std::pair<VxGUID, int64_t>>& getIdentList()         { return m_NearbyIdentList; };

    virtual	void				handleMulticastSktCallback( std::shared_ptr<VxSktBase>& sktBase );
    void						handleTcpLanConnectSuccess( BigListInfo* bigListInfo, std::shared_ptr<VxSktBase>& skt, bool isNewConnection, EConnectReason connectReason );

    virtual void                onNearbyUserUpdated( VxNetIdent* netIdent, int64_t timestamp = 0);
    void                        onConnectionLost( std::shared_ptr<VxSktBase>& sktBase, VxGUID& connectionId, VxGUID& peerOnlineId );

protected:
    void						setBroadcastPort( uint16_t u16Port );
    bool						setBroadcastEnable( bool enable );

    virtual void				multicastPktAnnounceAvail( std::shared_ptr<VxSktBase>& skt, PktAnnounce* pktAnnounce ) override;

    VxMutex                     m_ListMutex;
    std::vector<std::pair<VxGUID, int64_t>> m_NearbyIdentList;

    VxMutex                     m_MulticastIdentMutex;
    std::map<VxGUID, VxNetIdent> m_MulticastIdentList;
};

