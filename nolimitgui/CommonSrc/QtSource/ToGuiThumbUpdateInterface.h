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

#include <QWidget> // must be declared first or linux Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include <GuiInterface/IDefs.h>

class ThumbInfo;
class VxGUID;

class ToGuiThumbUpdateInterface
{
public:
    virtual void				toGuiThumbAdded( ThumbInfo* thumb ) = 0; 
    virtual void				toGuiThumbUpdated( ThumbInfo* thumb ) = 0; 
    virtual void				toGuiThumbRemoved( VxGUID& thumbId ) = 0; 
};


