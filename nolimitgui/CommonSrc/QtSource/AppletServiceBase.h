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

#include "ui_AppletServiceBase.h"

class AppletServiceBase : public AppletBase
{
	Q_OBJECT
public:
    AppletServiceBase( const char* objName, AppCommon& app, QWidget* parent );
	virtual ~AppletServiceBase() = default;

    void						setupServiceBaseApplet( EApplet applet, EPluginType pluginType );

    virtual QLabel *            getServiceTitle() { return ui.m_ServiceTitleLabel; }

    virtual QLabel *            getServiceUrlLabel( bool ipv6 ) { return ipv6 ? ui.m_UrlDescriptionLabelIpv6 : ui.m_UrlDescriptionLabelIpv4; }
    virtual QLineEdit *         getServiceUrlEdit( bool ipv6 ) { return ipv6 ? ui.m_UrlEditIpv6 : ui.m_UrlEditIpv4; }

    virtual QLabel *            getServiceNameLabel() { return ui.m_UserDisplayedNameLabel; }
    virtual QLineEdit *         getServiceNameEdit() { return ui.m_NameEdit; }

    virtual QLabel *            getServiceKeyWordsLabel() { return ui.m_KeyWordsLabel; }
    virtual QLineEdit *         getServiceKeyWordsEdit() { return ui.m_KeyWordsEdit; }

    virtual QLabel *            getServiceDescriptionLabel() { return ui.m_DescriptionLabel; }
    virtual QPlainTextEdit *    getServiceDescriptionEdit() { return ui.m_DescriptionEdit; }

    virtual QPushButton *       getStartButton() { return ui.m_StartButton; }
    virtual QPushButton *       getStopButton() { return ui.m_StopButton; }

    virtual PermissionWidget *  getPermissionWidget() { return ui.m_PermissionWidget; }
    virtual InformationWidget * getInformationWidget() { return ui.m_InfoWidget; }

    virtual ThumbnailEditWidget *   getThumbEditWidget() { return ui.m_ThumbnailEditWidget; }

    virtual QCheckBox*          getRunOnAppStartCheckbox() { return ui.m_RunOnStartupCheckBox; }

protected slots:
    virtual void                slotApplyServiceSettings();

protected:
    virtual void                connectServiceWidgets();
    virtual void                loadPluginSetting();
    virtual void                savePluginSetting();

    virtual void                loadUiFromSetting();
    virtual void                saveUiToSetting();

    Ui::AppletServiceBaseUi     ui;
    QWidget*                   m_HostServiceWidget;
    PluginSetting               m_PluginSetting;
    EFriendState                m_OrigPermissionLevel = eFriendStateIgnore;
};


