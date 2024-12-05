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

class ConnectId;
class VxGUID;

class GuiConnectIdListCallback
{
public:
    virtual void				callbackRelayStatusChange( VxGUID& onlineId, bool isRelayed ) {};
    virtual void				callbackOnlineStatusChange( VxGUID& onlineId, bool isOnline ) {};
    virtual void				callbackConnectionStatusChange( ConnectId& connectId, bool isConnected ) {};
};

