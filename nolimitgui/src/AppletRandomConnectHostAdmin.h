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
#include "InputClientBaseCallback.h"

class AppletRandomConnectHostAdmin : public AppletHostAdminBase
{
	Q_OBJECT
public:
    AppletRandomConnectHostAdmin( AppCommon& app, QWidget* parent );
	virtual ~AppletRandomConnectHostAdmin() override;

    EPluginType			        getInputClientPluginType( void ) override { return ePluginTypeClientRandomConnect; }

};


