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

#include "ui_AppletChatRoomClient.h"

#include "AppletBase.h"

class AppletGroupHostAdmin : public AppletBase
{
	Q_OBJECT
public:
    AppletGroupHostAdmin( AppCommon& app, QWidget* parent );
	virtual ~AppletGroupHostAdmin() override;

protected:
    //=== vars ===//
    Ui::AppletChatRoomClientUi ui;
};


