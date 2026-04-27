#pragma once
//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

enum class ECanSendState
{
    eCanSend,
    eInvalidHostId,
    eHostIsSelf,
    eNoMembersToSendTo,
    eAdminIsOffline
};

class CanSendInterface
{
public:
    virtual ~CanSendInterface() = default;

    virtual ECanSendState getCanSendState( void ) = 0;
};
