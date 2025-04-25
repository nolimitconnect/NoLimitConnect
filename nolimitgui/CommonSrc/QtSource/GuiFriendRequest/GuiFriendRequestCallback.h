#pragma once
//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <CoreLib/GroupieId.h>

#include <memory>

class GuiFriendRequest;

class GuiFriendRequestListCallback
{
public:
    virtual void				callbackGuiFriendRequestListUpdated( GuiFriendRequest* friendRequest ) {};
    virtual void				callbackGuiFriendRequestListRemoved( VxGUID requestId ) {};
};

