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

#include "AppletCamClient.h"

class AppletCamServerViewMine : public AppletCamClient
{
	Q_OBJECT
public:
	AppletCamServerViewMine( AppCommon& app, QWidget* parent );
	virtual ~AppletCamServerViewMine();

    virtual void				setAppletType( EApplet applet ) override;
};


