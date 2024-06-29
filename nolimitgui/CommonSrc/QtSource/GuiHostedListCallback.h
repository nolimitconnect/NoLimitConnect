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

#include <CoreLib/HostedId.h>

class GuiHosted;

class GuiHostedListCallback
{
public:
    virtual void				callbackGuiHostedListAdded( HostedId& hostedId, GuiHosted* guiHosted ) {};
    virtual void				callbackGuiHostedListUpdated( HostedId& hostedId, GuiHosted* guiHosted ) {};
    virtual void				callbackGuiHostedListRemoved( HostedId& hostedId ) {};
    virtual void				callbackGuiHostedListSearchResult( HostedId& hostedId, GuiHosted* guiHosted, VxGUID& sessionId ) {};
    virtual void				callbackGuiHostedListSearchComplete( EHostType hostType, VxGUID& sessionId ) {};
};

