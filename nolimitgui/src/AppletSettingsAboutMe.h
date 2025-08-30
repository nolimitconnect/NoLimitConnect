#pragma once
//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletServiceBaseSettings.h"

class AppletSettingsAboutMe : public AppletServiceBaseSettings
{
	Q_OBJECT
public:
	AppletSettingsAboutMe( AppCommon& app, QWidget* parent );
	virtual ~AppletSettingsAboutMe();

};


