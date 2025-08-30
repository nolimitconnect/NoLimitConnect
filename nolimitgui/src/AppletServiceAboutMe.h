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

#include "AppletServiceBase.h"

class AppletServiceAboutMe : public AppletServiceBase
{
	Q_OBJECT
public:
	AppletServiceAboutMe( AppCommon& app, QWidget* parent );
	virtual ~AppletServiceAboutMe();



protected:
};


