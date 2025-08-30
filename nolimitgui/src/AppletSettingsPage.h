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

#include "AppletLaunchPage.h"

class AppletSettingsPage : public AppletLaunchPage
{
	Q_OBJECT
public:
	AppletSettingsPage( AppCommon& app, QWidget* parent );
	virtual ~AppletSettingsPage() = default;

protected:
    virtual void				showEvent( QShowEvent* );
    void						resizeEvent( QResizeEvent* );

private:
    void						setupAppletSettingsgPage( void );

    bool						m_IsInitialized{ false };
    QVector<VxWidgetBase*>		m_AppletList;
};


