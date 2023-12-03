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

#include "AppletBase.h"
#include <GuiInterface/IToGui.h>
#include <ptop_src/ptop_engine_src/PluginSettings/PluginSettingMgr.h>

#include "ui_PluginSettingsWidget.h"

#include "ThumbnailChooseWidget.h"

class PluginSettingsWidget : public QWidget
{
	Q_OBJECT
public:
    PluginSettingsWidget( QWidget* parent );
	virtual ~PluginSettingsWidget() = default;

    void						setupSettingsWidget( EApplet applet, EPluginType pluginType );

    virtual QLabel *            getServiceUrlLabel( bool ipv6 )        { return ipv6 ? ui.m_UrlDescriptionLabelIpv4 : ui.m_UrlDescriptionLabelIpv6; }
    virtual QLineEdit *         getServiceUrlEdit( bool ipv6 )         { return ipv6 ? ui.m_UrlEditIpv4 : ui.m_UrlEditIpv6; }

    virtual QLineEdit *         getServiceTitleEdit()       { return ui.m_ServiceTitleEdit; }

    virtual QPlainTextEdit *    getServiceDescriptionEdit() { return ui.m_DescriptionEdit; }
    virtual QPlainTextEdit *    getGreetingEdit()           { return ui.m_GreetingEdit; }
    virtual QPlainTextEdit *    getRejectEdit()             { return ui.m_RejectEdit; }

    virtual QPushButton *       getStartButton()            { return ui.m_StartButton; }
    virtual QPushButton *       getStopButton()             { return ui.m_StopButton; }
    virtual QPushButton *       getApplyButton()            { return ui.m_ApplyButton; }

    virtual QComboBox *         getAgeComboBox()            { return ui.m_AgeComboBox; }
    virtual QComboBox *         getContentRatingComboBox()  { return ui.m_ContentRatingComboBox; }
    virtual QComboBox *         getGenderComboBox()         { return ui.m_GenderComboBox; }
    virtual QComboBox *         getLanguageComboBox()       { return ui.m_LanguageComboBox; }

    virtual PermissionWidget *  getPermissionWidget()       { return ui.m_PermissionWidget; }
    virtual InformationWidget * getInformationWidget()      { return ui.m_InfoWidget; }

    virtual ThumbnailChooseWidget * getThumbnailChooseWidget()  { return ui.m_ThumbnailChooseWidget; }

signals:
    void                        signalPluginSettingsApplied();

protected slots:
    virtual void                slotApplyServiceSettings();
    virtual void                slotThumbnailAssetChanged( ThumbInfo* thumbAsset );
    void                        slotRunOnStartupCheckBoxChange( int runOnStartup );

protected:
    EPluginType                 getPluginType() { return m_PluginType; }

    virtual void                connectServiceWidgets();
    virtual void                loadPluginSetting();
    virtual void                savePluginSetting();

    virtual void                loadUiFromSetting();
    virtual void                saveUiToSetting();

    Ui::PluginSettingsUi        ui;
    AppCommon&                  m_MyApp;
    EApplet                     m_EAppletType = eAppletUnknown;
    EPluginType                 m_PluginType = ePluginTypeInvalid;
    AppletBase*                 m_ParentApplet{ nullptr };
    PluginSetting               m_PluginSetting;
    int                         m_SubPluginType = 0;
    bool                        m_OrigPermissionIsSet = false;
    EFriendState                m_OrigPermissionLevel = eFriendStateIgnore;
    bool                        m_PermissionsConnected = false;

};


