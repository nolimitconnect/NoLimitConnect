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

#include "AppletBase.h"
#include "AppDefs.h"

#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletPermissionListtUi;
}
QT_END_NAMESPACE

class AppletPermissionList : public AppletBase
{
	Q_OBJECT
public:
	AppletPermissionList( AppCommon& app, QWidget* parent );
	virtual ~AppletPermissionList();

protected:
    void                        initializePermissionList( void );
    void                        createPermissionItem( EPluginType pluginType );

	Ui::AppletPermissionListtUi&	ui;
    QVector<EPluginType>        m_PluginList;
};


