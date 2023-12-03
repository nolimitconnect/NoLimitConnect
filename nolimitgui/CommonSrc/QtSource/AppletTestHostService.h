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

#include "AppletTestBase.h"
#include "ui_AppletTestHostService.h"

class AppletTestHostService : public AppletTestBase
{
	Q_OBJECT
public:
    AppletTestHostService( AppCommon& app, QWidget* parent );
	virtual ~AppletTestHostService();

protected:
    Ui::AppletTestHostServiceUi          ui;
};


