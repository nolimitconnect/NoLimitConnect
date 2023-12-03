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

#include "AppletBase.h"
#include <GuiInterface/IToGui.h>
#include <ptop_src/ptop_engine_src/PluginSettings/PluginSettingMgr.h>

#include "ui_AppletServiceBaseSettings.h"

class AppletServiceBaseSettings : public AppletBase
{
	Q_OBJECT
public:
    AppletServiceBaseSettings( const char* objName, AppCommon& app, QWidget* parent );
	virtual ~AppletServiceBaseSettings() = default;

    // override so we do initialization after applet type is set
    virtual void                setupServiceBaseApplet( EApplet applet, EPluginType pluginType );

    virtual QLabel *            getServiceTitle() { return ui.m_ServiceTitleLabel; }
    //virtual QLabel *            getStatusLabel() { return ui.m_StatusLabel; }
    //virtual QLabel *            getServiceStatusLabel() { return ui.m_ServiceStatusLabel; }

    virtual QLabel *            getServiceUrlLabel() { return ui.m_UrlDescriptionLabel; }
    virtual QLineEdit *         getServiceUrlEdit() { return ui.m_UrlEdit; }

    virtual QLabel *            getServiceNameLabel() { return ui.m_UserDisplayedNameLabel; }
    virtual QLabel *            getServiceDescriptionLabel() { return ui.m_DescriptionLabel; }

    virtual QLineEdit *         getServiceNameEdit() { return ui.m_NameEdit; }
    virtual QPlainTextEdit *    getServiceDescriptionEdit() { return ui.m_DescriptionEdit; }

    virtual QPushButton *       getApplyButton()            { return ui.m_ApplyButton; }

    virtual PermissionWidget *  getPermissionWidget()       { return ui.m_PermissionWidget; }
    virtual InformationWidget * getInformationWidget()      { return ui.m_InfoWidget; }
    virtual LogWidget *         getLogWidget()              { return ui.m_LogWidget; }

protected slots:
    virtual void                slotApplyButtonClicked();

protected:
    virtual void                connectServiceWidgets();
    virtual void                loadPluginSetting();
    virtual void                savePluginSetting();
    virtual void                loadUiFromSetting();
    virtual void                saveUiToSetting();


    Ui::AppletServiceBaseSettingsUi ui;

    QWidget*                   m_HostServiceWidget;
    PluginSetting               m_PluginSetting;
    EFriendState                m_OrigPermissionLevel = eFriendStateIgnore;
};


