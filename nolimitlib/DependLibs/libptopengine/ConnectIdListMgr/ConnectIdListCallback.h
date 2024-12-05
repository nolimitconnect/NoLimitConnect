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
#include <stdint.h>

class VxGUID;
class ConnectId;

class ConnectIdListCallback
{
public:
    virtual void				callbackOnlineStatusChange( VxGUID& onlineId, bool isOnline ) {};

    virtual void				callbackRelayStatusChange( ConnectId& connectId, bool isRelayed ) {};
    virtual void				callbackConnectionStatusChange( ConnectId& connectId, bool isConnected ) {};

    virtual void				callbackConnectionReason( VxGUID& socketId, enum EConnectReason connectReason, bool enableReason ) {};
    virtual void				callbackConnectionLost( VxGUID& socketId ) {};
};

