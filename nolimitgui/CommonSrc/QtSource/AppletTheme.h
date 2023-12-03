#pragma once
//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
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
	ENotifyType					m_ExampleButtonNotifyType{ eNotifyNone };
};


