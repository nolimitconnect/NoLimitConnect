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

#include <GuiInterface/IDefs.h>

class User;
class VxGUID;
class BaseSessionInfo;

class UserOnlineCallbackInterface
{
public:
    virtual void				callbackUserOnlineState( std::shared_ptr<User> user, bool online ){};
    virtual void				callbackUserOffline( VxGUID& onlineId ){};

    virtual void				callbackUserOnlineAdded( std::shared_ptr<User> user ){};
    virtual void				callbackUserOnlineUpdated( std::shared_ptr<User> user ){};
    virtual void				callbackUserOnlineRemoved( VxGUID& user ){};

    virtual void				callbackUserSessionAdded( std::shared_ptr<User> user, BaseSessionInfo& sessionInfo ){};
    virtual void				callbackUserSessionUpdated( std::shared_ptr<User> user, BaseSessionInfo& sessionInfo ){};
    virtual void				callbackUserSessionRemoved( VxGUID& user, BaseSessionInfo& sessionInfo ){};
};

