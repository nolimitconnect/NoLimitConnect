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
#include <PluginSettings/PluginSettingMgr.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletServiceBaseSettingsUi;
}
QT_END_NAMESPACE

class InformationWidget;
class PermissionWidget;
class LogWidget;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;

class AppletServiceBaseSettings : public AppletBase
{
	Q_OBJECT
public:
    AppletServiceBaseSettings( const char* objName, AppCommon& app, QWidget* parent );
	virtual ~AppletServiceBaseSettings() = default;

    // override so we do initialization after applet type is set
    virtual void                setupServiceBaseApplet( EApplet applet, EPluginType pluginType );

    virtual QLabel *            getServiceTitle();

    virtual QLabel *            getServiceUrlLabel();
    virtual QLineEdit *         getServiceUrlEdit();
    virtual QLabel *            getServiceNameLabel();
    virtual QLabel *            getServiceDescriptionLabel();

    virtual QLineEdit *         getServiceNameEdit();
    virtual QPlainTextEdit *    getServiceDescriptionEdit();

    virtual QPushButton *       getApplyButton();

    virtual PermissionWidget *  getPermissionWidget();
    virtual InformationWidget * getInformationWidget();
    virtual LogWidget *         getLogWidget();

protected slots:
    virtual void                slotApplyButtonClicked();

protected:
    virtual void                connectServiceWidgets();
    virtual void                loadPluginSetting();
    virtual void                savePluginSetting();
    virtual void                loadUiFromSetting();
    virtual void                saveUiToSetting();


    Ui::AppletServiceBaseSettingsUi& ui;

    QWidget*                   m_HostServiceWidget;
    PluginSetting               m_PluginSetting;
    EFriendState                m_OrigPermissionLevel = eFriendStateIgnore;
};


