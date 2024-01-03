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

#include <OfferBase/OfferBaseMgr.h>

#include <CoreLib/VxThread.h>
#include <CoreLib/VxSemaphore.h>
#include <CoreLib/VxMutex.h>

class OfferCallbackInterface;

class OfferMgr : public OfferBaseMgr
{
public:
	OfferMgr( P2PEngine& engine, const char* dbName, const char* stateDbName );
	virtual ~OfferMgr() = default;

    void                        fromGuiUserLoggedOn( void ) override;

    bool				        fromGuiOfferCreated( OfferBaseInfo& offerInfo );
    bool				        fromGuiOfferUpdated( OfferBaseInfo& offerInfo );

    virtual void				announceOfferAdded( OfferBaseInfo* offerInfo ) override;
    virtual void				announceOfferUpdated( OfferBaseInfo* offerInfo ) override;
    virtual void				announceOfferRemoved( OfferBaseInfo* offerInfo ) override;
    virtual void				announceOfferXferState( VxGUID& assetOfferId, EOfferSendState assetSendState, int param ) override;

protected:

};

