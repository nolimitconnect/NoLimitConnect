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

#include "AppletHostAdminBase.h"

class AppletGroupHostAdmin : public AppletHostAdminBase
{
	Q_OBJECT
public:
    AppletGroupHostAdmin( AppCommon& app, QWidget* parent );
	virtual ~AppletGroupHostAdmin() override;

    EPluginType			        getInputClientPluginType( void ) override { return ePluginTypeClientGroup; }

};


