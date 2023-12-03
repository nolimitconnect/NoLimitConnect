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

#include "AppletBase.h"
#include "ui_AppletGetStarted.h"

class AppletGetStarted : public AppletBase
{
	Q_OBJECT
public:
    AppletGetStarted( AppCommon& app, QWidget* parent );
	virtual ~AppletGetStarted();

protected:
    Ui::AppletGetStartedUi          ui;
};


