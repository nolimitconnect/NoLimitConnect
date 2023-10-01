//============================================================================
// Copyright (C) 2021 Brett R. Jones 
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
#pragma once

#include "ConnectedListBase.h"
#include <GuiInterface/IDefs.h>

#include <memory>

class BigListInfo;
class IConnectRequestCallback;
class P2PEngine;

class ConnectedListAll : public ConnectedListBase
{
public:
    ConnectedListAll() = delete;
    ConnectedListAll( P2PEngine& engine );
    virtual ~ConnectedListAll() override = default;

    ConnectedInfo*              getOrAddConnectedInfo( const VxGUID& socketId, BigListInfo* bigListInfo );
    ConnectedInfo*              getConnectedInfo( const VxGUID& socketId, const VxGUID& onlineId );
    ConnectedInfo*              getAnyConnectedInfo( const VxGUID& onlineId );
    void                        removeConnectedInfo( const VxGUID& socketId, const VxGUID& onlineId, bool isLockedList = false );

    bool                        removeConnectedReason( VxGUID& sessionId, VxGUID& onlineId, IConnectRequestCallback* callback, EConnectReason connectReason, std::vector<std::shared_ptr<VxSktBase>>& retUnusedSkts );

    void                        onSktDisconnected( const VxGUID& socketId );

    //=== vars ===//
    P2PEngine&                  m_Engine;
};
