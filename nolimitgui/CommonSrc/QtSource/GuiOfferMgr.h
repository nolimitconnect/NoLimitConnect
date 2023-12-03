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

#include "GuiOfferMgrBase.h"

class GuiOfferCallback;

class GuiOfferMgr : public GuiOfferMgrBase
{
    Q_OBJECT
public:
    GuiOfferMgr() = delete;
    GuiOfferMgr( AppCommon& myApp )
        : GuiOfferMgrBase( myApp )
    {
    }

    virtual ~GuiOfferMgr() override = default;

    virtual void                onAppCommonCreated( void ) override;
    virtual void                onMessengerReady( bool ready ) override {}
    virtual bool                isMessengerReady( void ) override;
    virtual void                onSystemReady( bool ready ) override {}

    void                        wantGuiOfferCallbacks( GuiOfferCallback* clientInterface, bool wantCallbacks );    
};
