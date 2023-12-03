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

class GuiUserJoin;

class GuiUserJoinCallback
{
public:
    virtual void				callbackGuiUserJoinRequested( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) {};
    virtual void				callbackGuiUserJoinWasGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) {};
    virtual void				callbackGuiUserJoinIsGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) {};
    virtual void				callbackGuiUserUnJoinGranted( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) {};
    virtual void				callbackGuiUserJoinDenied( GroupieId& groupieId, GuiUserJoin* guiUserJoin ) {};
    virtual void				callbackGuiUserJoinLeaveHost( GroupieId& groupieId ) {};
    virtual void				callbackGuiUserJoinRemoved( GroupieId& groupieId ) {};
};

