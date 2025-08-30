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

#include "AppletBase.h"

class AppletTestBase : public AppletBase
{
	Q_OBJECT
public:
	AppletTestBase( const char* ObjName, AppCommon& app, QWidget* parent );
	virtual ~AppletTestBase();

protected:
	EHostType                   m_HostType{ eHostTypeUnknown };
	VxGUID						m_HostOnlineId;
};


