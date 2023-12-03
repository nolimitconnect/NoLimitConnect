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

#include "IdentListMgrBase.h"

#include <CoreLib/VxGUID.h>

#include <vector>

// maintains a list of nearby sorted by timestamp for faster lookup

class NearbyListMgr : public IdentListMgrBase
{
public:
    NearbyListMgr() = delete;
    NearbyListMgr( P2PEngine& engine );
    virtual ~NearbyListMgr() = default;

    bool                        isNearby( VxGUID& onlineId );
    virtual void                updateIdent( VxGUID& onlineId, int64_t timestamp ) override;
    virtual void                removeIdent( VxGUID& onlineId ) override;
    virtual void                removeAll( void ) override;

    std::vector<std::pair<VxGUID, int64_t>>& getIdentList()         { return m_NearbyIdentList; };

protected:
    std::vector<std::pair<VxGUID, int64_t>> m_NearbyIdentList;
};

