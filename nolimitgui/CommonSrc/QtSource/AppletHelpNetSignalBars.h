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
#include <GuiInterface/IDefs.h>

#include "ui_AppletHelpNetSignalBars.h"

class AppCommon;

class AppletHelpNetSignalBars : public AppletBase
{
	Q_OBJECT
public:
    AppletHelpNetSignalBars( AppCommon& app, QWidget* parent );
	virtual ~AppletHelpNetSignalBars() override;


protected:

	//=== vars ===//
	Ui::AppletHelpNetSignalBarsUi		    ui;

    static QString              m_NoInfoAvailable;
    static QString              m_NetworkDesign;
    static QString              m_PluginDefinitions;
    static QString              m_Permissions;
    static QString              m_NetworkKey;
    static QString              m_NetworkHost;
    static QString              m_ConnectTestUrl;
    static QString              m_ConnectTestSettings;
};
