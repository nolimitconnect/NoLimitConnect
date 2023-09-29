#pragma once
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
