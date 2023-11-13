#pragma once
//============================================================================
// Copyright (C) 2023 Brett R. Jones
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

#include "ui_AppletUserConnections.h"

#include "AppletClientBase.h"

class AppletUserConnections : public AppletClientBase
{
	Q_OBJECT
public:
    AppletUserConnections( AppCommon& app, QWidget* parent );
	virtual ~AppletUserConnections() override;

protected slots:
    void                        slotConnectTypeSelectionChange( int comboIdx );
    void                        slotHostTypeSelectionChange( int comboIdx );

protected:
    void                        showEvent( QShowEvent* ev ) override;

    //=== vars ===//
    Ui::AppletUserConnectionsUi	ui;
};


