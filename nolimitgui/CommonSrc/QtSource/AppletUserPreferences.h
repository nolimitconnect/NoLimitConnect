#pragma once
//============================================================================
// Copyright (C) 2020 Brett R. Jones
// Issued to MIT style license by Brett R. Jones in 2017
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


