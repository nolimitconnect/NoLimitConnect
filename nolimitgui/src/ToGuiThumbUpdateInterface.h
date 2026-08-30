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

#if defined(TARGET_OS_LINUX)
#endif // defined(TARGET_OS_LINUX)

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


