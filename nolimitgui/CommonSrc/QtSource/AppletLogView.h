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

#include "ui_AppletLogView.h"

#include "AppletBase.h"
#include "AppDefs.h"
#include "AppCommon.h"

class LogMgr;

class AppletLogView : public AppletBase
{
	Q_OBJECT
public:
    AppletLogView( AppCommon& app, QWidget* parent );
	virtual ~AppletLogView();

protected slots:
    void						slotExitButtonClick( void );
    void						slotLaunchLogSettings( void );
    void                        slotLogMsg( uint32_t u32LogFlags, const QString& logMsg );

protected:
    void						closeEvent( QCloseEvent *event );

    void						connectSignals( void );

    void						updateDlgFromSettings( bool initialSettings );
    void						updateSettingsFromDlg( void );

	Ui::AppletLogViewWidget		ui;
    LogMgr&                     m_LogMgr;
};


