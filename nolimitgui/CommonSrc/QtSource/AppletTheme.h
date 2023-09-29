#pragma once
//============================================================================
// Copyright (C) 2010 Brett R. Jones
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

#include "ui_AppletTheme.h"

#include "AppletBase.h"
#include "AppDefs.h"


class AppletTheme : public AppletBase
{
	Q_OBJECT
public:
	AppletTheme( AppCommon& app, QWidget* parent );
	virtual ~AppletTheme(){};

    // override default behavior of closing dialog when back button is clicked
    virtual void				onBackButtonClicked( void ) override;

protected slots:
	void						slotThemeSelectionChanged( int idx );
	void						slotThemeAccepted( void );
	void						slotThemeCanceled( void );
    void				        slotCheckBoxClicked( int checkedState );
	void						slotSliderValueChanged( int sliderVal );
	void						slotExampleButtonClicked( void );

protected:
	virtual void				resizeEvent( QResizeEvent* ev ) override;

	void						fillThemeCombo( void );
    void                        fillExampleCombo( void );

	void						saveCurrentTheme( void );
	void						applyTheme( EThemeType themeType );
	void						restoreSavedTheme( void );
	int							getVertScrollBarWidth( void );

	Ui::ThemeWidgetClass		ui;
	EThemeType					m_SavedTheme;
	EThemeType					m_CurTheme;
	bool						m_ExampleButtonNotifyEnabled{ false };
};


