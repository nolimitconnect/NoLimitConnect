#pragma once
//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
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

protected:

};


