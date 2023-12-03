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

#include "ui_AppletLogSettings.h"

#include "AppletBase.h"
#include "AppDefs.h"

class LogMgr;

class AppletLogSettings : public AppletBase
{
	Q_OBJECT
public:
    AppletLogSettings( AppCommon& app, QWidget* parent );
	virtual ~AppletLogSettings();

protected slots:
	void						slotSaveSettings( void );
    void                        slotCancel( void );
    void						slotExitButtonClick( void );

    void						slotSelectAllLevels( void );
    void						slotClearAllLevels( void );
    void						slotSelectAllModules( void );
    void						slotClearAllModules( void );

protected:
    void                        showEvent( QShowEvent* ev ) override;

    void						connectSignals( void );
    void						updateDlgFromSettings( void );
    void						updateSettingsFromDlg( void );


	Ui::AppletLogSettingsWidget	ui;
    LogMgr&                     m_LogMgr;
    QVector<QCheckBox*>         m_LogLevelList;
    QVector<QCheckBox*>         m_LogModuleList;
};


