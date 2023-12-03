#pragma once
//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppCommon.h"
#include "AppDefs.h"

#include "ui_ServiceSettingsWidget.h"

class ServiceSettingsWidget : public QWidget
{
	Q_OBJECT

public:
    ServiceSettingsWidget( QWidget* parent = nullptr );

    void						setPluginType( EPluginType pluginType ) { m_PluginType = pluginType;  updateUi();  updateIcons(); }
    void						setViewServiceVisible( bool visible )   { ui.m_ViewServiceFrame->setVisible( visible ); }

protected slots:
    void                        slotServiceSettingsClicked();
    void                        slotViewServiceClicked();

protected:
	void						initServiceSettingsWidget( void );
    void						launchApplet( EApplet appletType );

    void						updateIcons( void );
    void                        updateUi( void );

    AppCommon&                  m_MyApp;
    EPluginType                 m_PluginType = ePluginTypeInvalid;
	Ui::ServiceSettingsWidgetUi	ui;
};
