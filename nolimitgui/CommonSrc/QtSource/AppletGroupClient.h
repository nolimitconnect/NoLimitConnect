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

#include "ui_AppletGroupClient.h"

#include "AppletClientBase.h"
#include <GuiInterface/IDefs.h>
#include <GuiInterface/IToGui.h> 

class AppletGroupClient : public AppletClientBase
{
	Q_OBJECT
public:
	AppletGroupClient( AppCommon& app, QWidget* parent );
	virtual ~AppletGroupClient() override;

protected slots:
    void                        slotSetSessionVisible( bool makeVisible );

    void						slotViewChanged( EUserViewType viewType );

protected:
    void                        showEvent( QShowEvent* ev ) override;

    //=== vars ===//
    Ui::AppletGroupClientUi	ui;
};


