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

#include <PktLib/GroupieId.h>

class GuiGroupie;

class GuiGroupieListCallback
{
public:
    virtual void				callbackGuiGroupieListUpdated( GroupieId& groupieId, GuiGroupie* guiGroupie ) {};
    virtual void				callbackGuiGroupieListRemoved( GroupieId& groupieId ) {};
    virtual void				callbackGuiGroupieListSearchResult( GroupieId& groupieId, GuiGroupie* guiGroupie, VxGUID& sessionId ) {};
    virtual void				callbackGuiGroupieListSearchComplete( EHostType hostType, VxGUID& sessionId ) {};
};

