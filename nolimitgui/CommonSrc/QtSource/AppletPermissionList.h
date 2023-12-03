#pragma once
//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ui_AppletPermissionList.h"

#include "AppletBase.h"
#include "AppDefs.h"

#include <QTimer>

class AppletPermissionList : public AppletBase
{
	Q_OBJECT
public:
	AppletPermissionList( AppCommon& app, QWidget* parent );
	virtual ~AppletPermissionList();

protected:
    void                        initializePermissionList( void );
    void                        createPermissionItem( EPluginType pluginType );

    void						connectSignals( void );

	Ui::AppletPermissionListtUi	ui;
    QVector<EPluginType>        m_PluginList;
};


