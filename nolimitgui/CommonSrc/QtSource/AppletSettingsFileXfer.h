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

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletSettingsNoUserSettingsUi;
}
QT_END_NAMESPACE

class AppletSettingsFileXfer : public AppletServiceBaseSettings
{
	Q_OBJECT
public:
    AppletSettingsFileXfer( AppCommon& app, QWidget* parent );
	virtual ~AppletSettingsFileXfer() override;

protected:
	Ui::AppletSettingsNoUserSettingsUi& ui;
};


