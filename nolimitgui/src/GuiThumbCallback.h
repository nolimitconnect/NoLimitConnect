#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <GuiInterface/IDefs.h>

class VxGUID;
class GuiThumb;

class GuiThumbCallback
{
public:
    virtual void callbackThumbAdded( GuiThumb* guiThumb ) {};
    virtual void callbackThumbUpdated( GuiThumb* guiThumb ) {};
    virtual void callbackThumbRemoved( VxGUID& thumbId ) {};
};
