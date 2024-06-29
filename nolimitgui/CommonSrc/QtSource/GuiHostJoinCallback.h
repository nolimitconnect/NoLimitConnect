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

#include <CoreLib/GroupieId.h>

class GuiHostJoin;

class GuiHostJoinCallback
{
public:
    virtual void				callbackJoinRequestCount( int requestCnt ) {};

    virtual void				callbackGuiHostJoinRequested( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) {};
    virtual void				callbackGuiHostJoinWasGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) {};
    virtual void				callbackGuiHostJoinIsGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) {};
    virtual void				callbackGuiHostUnJoinGranted( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) {};
    virtual void				callbackGuiHostJoinDenied( GroupieId& groupieId, GuiHostJoin* guiHostJoin ) {};
    virtual void				callbackGuiHostJoinLeaveHost( GroupieId& groupieId ) {};
    virtual void				callbackGuiHostUnJoin( GroupieId& groupieId ) {};
    virtual void				callbackGuiHostJoinRemoved( GroupieId& groupieId ) {};
};

