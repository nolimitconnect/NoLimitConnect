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

#include "AppletHostBase.h"

#include <GuiInterface/IToGui.h>

class GuiHosted;
class GuiHostSession;

class AppletClientBase : public AppletHostBase
{
	Q_OBJECT
public:
    AppletClientBase( const char* objName, AppCommon& app, QWidget* parent );
	virtual ~AppletClientBase() = default;

    virtual void                onJointButtonClicked( GuiHostSession* hostSession );

	virtual void				setAdminGroupieId( GroupieId& adminGroupieId ) {};

protected:

};


