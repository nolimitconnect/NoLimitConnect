#pragma once
//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletHostClientBase.h"

class AppletChatRoomClient : public AppletHostClientBase
{
	Q_OBJECT
public:
    AppletChatRoomClient( AppCommon& app, QWidget* parent );
	~AppletChatRoomClient() = default;
};
