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
    class AppletServiceBaseUi;
}
QT_END_NAMESPACE

class InformationWidget;
class PermissionWidget;
class QCheckBox;
class QLineEdit;
class QPlainTextEdit;
class ThumbnailEditWidget;

class AppletServiceBase : public AppletBase
{
	Q_OBJECT
public:
    AppletServiceBase( const char* objName, AppCommon& app, QWidget* parent );
	virtual ~AppletServiceBase() = default;

    void						setupServiceBaseApplet( EApplet applet, EPluginType pluginType );

    virtual QLabel*             getServiceTitle();

    virtual QLabel *            getServiceUrlLabel( bool ipv6 );
    virtual QLineEdit *         getServiceUrlEdit( bool ipv6 );

    virtual QLabel *            getServiceNameLabel();
    virtual QLineEdit *         getServiceNameEdit();

    virtual QLabel *            getServiceKeyWordsLabel();
    virtual QLineEdit *         getServiceKeyWordsEdit();

    virtual QLabel *            getServiceDescriptionLabel();
    virtual QPlainTextEdit *    getServiceDescriptionEdit();

    virtual QPushButton *       getStartButton();
    virtual QPushButton *       getStopButton();

    virtual PermissionWidget *  getPermissionWidget();
    virtual InformationWidget * getInformationWidget();

    virtual ThumbnailEditWidget*   getThumbEditWidget();

    virtual QCheckBox*          getRunOnAppStartCheckbox();

protected slots:
    virtual void                slotApplyServiceSettings();

protected:
    virtual void                connectServiceWidgets();
    virtual void                loadPluginSetting();
    virtual void                savePluginSetting();

    virtual void                loadUiFromSetting();
    virtual void                saveUiToSetting();

    Ui::AppletServiceBaseUi&    ui;
    QWidget*                    m_HostServiceWidget;
    PluginSetting               m_PluginSetting;
    EFriendState                m_OrigPermissionLevel = eFriendStateIgnore;
};


