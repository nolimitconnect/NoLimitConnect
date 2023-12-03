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

#include "AppletClientBase.h"

class AppletChatRoomClient : public AppletClientBase
{
	Q_OBJECT
public:
    AppletChatRoomClient( AppCommon& app, QWidget* parent );
	virtual ~AppletChatRoomClient() override;

protected slots:
    void                        slotSetSessionVisible( bool makeVisible );

protected:
    void                        showEvent( QShowEvent* ev ) override;

    //=== vars ===//
    Ui::AppletChatRoomClientUi	ui;
};


