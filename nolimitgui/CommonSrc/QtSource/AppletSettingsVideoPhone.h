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

#include "AppletServiceBaseSettings.h"
#include "ui_AppletSettingsNoUserSettings.h"

class AppletSettingsVideoPhone : public AppletServiceBaseSettings
{
	Q_OBJECT
public:
    AppletSettingsVideoPhone( AppCommon& app, QWidget* parent );
	virtual ~AppletSettingsVideoPhone() override;

protected:
	Ui::AppletSettingsNoUserSettingsUi ui;
};


