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
#include <PluginSettings/PluginSettingMgr.h>

#include "ThumbnailChooseWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class PluginSettingsUi;
}
QT_END_NAMESPACE

class QComboBox;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class PermissionWidget;
class InformationWidget;
class ThumbnailChooseWidget;

class PluginSettingsWidget : public QWidget
{
	Q_OBJECT
public:
    PluginSettingsWidget( QWidget* parent );
	virtual ~PluginSettingsWidget() = default;

    void						setupSettingsWidget( EApplet applet, EPluginType pluginType );

    void                        setPluginSetting( PluginSetting& pluginSetting )    { m_PluginSetting = pluginSetting; }
    PluginSetting&              getPluginSetting( void )                            { return m_PluginSetting; }

    virtual QLabel *            getServiceUrlLabel( bool ipv6 );
    virtual QLineEdit *         getServiceUrlEdit( bool ipv6 );

    virtual QLineEdit *         getServiceTitleEdit();

    virtual QPlainTextEdit *    getServiceDescriptionEdit();
    virtual QPlainTextEdit *    getGreetingEdit();
    virtual QPlainTextEdit *    getRejectEdit();

    virtual QPushButton *       getStartButton();
    virtual QPushButton *       getStopButton();
    virtual QPushButton *       getApplyButton();
    virtual QComboBox *         getAgeComboBox();
    virtual QComboBox *         getContentRatingComboBox();
    virtual QComboBox *         getGenderComboBox();
    virtual QComboBox *         getLanguageComboBox();
    virtual PermissionWidget *  getPermissionWidget();
    virtual InformationWidget * getInformationWidget();

    virtual ThumbnailChooseWidget * getThumbnailChooseWidget();

signals:
    void                        signalPluginSettingsApplied();
    void                        signalThumbnailAssetChanged( VxGUID thumbId, bool isCircular );

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

    Ui::PluginSettingsUi&       ui;
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


