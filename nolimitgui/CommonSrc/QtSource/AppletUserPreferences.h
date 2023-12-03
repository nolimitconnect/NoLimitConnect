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

#include "ui_AppletUserPreferences.h"

#include "AppletBase.h"
#include "AppDefs.h"

class AppletUserPreferences : public AppletBase
{
	Q_OBJECT
public:
    AppletUserPreferences( AppCommon& app, QWidget* parent );
	virtual ~AppletUserPreferences();

protected slots:
	void						slotSaveSettings( void );
    void                        slotCancel( void );
	void                        slotMaxMsgHistoryInfoButtonClicked( void );
	void                        slotMaxMsgHistoryValueChanged( int maxHistory );

protected:
	virtual void				showEvent( QShowEvent* ev ) override;
	virtual void				hideEvent( QHideEvent* ev ) override;

    void						updateDlgFromSettings( void );
    void						updateSettingsFromDlg( void );

	Ui::AppletUserPreferencesUi	ui;
};


