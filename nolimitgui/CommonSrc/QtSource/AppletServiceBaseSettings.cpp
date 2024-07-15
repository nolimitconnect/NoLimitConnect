//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletServiceBaseSettings.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletServiceBaseSettings.h"

QLabel *            AppletServiceBaseSettings::getServiceTitle() { return ui.m_ServiceTitleLabel; }

QLabel *            AppletServiceBaseSettings::getServiceUrlLabel() { return ui.m_UrlDescriptionLabel; }
QLineEdit *         AppletServiceBaseSettings::getServiceUrlEdit() { return ui.m_UrlEdit; }

QLabel *            AppletServiceBaseSettings::getServiceNameLabel() { return ui.m_UserDisplayedNameLabel; }
QLabel *            AppletServiceBaseSettings::getServiceDescriptionLabel() { return ui.m_DescriptionLabel; }

QLineEdit *         AppletServiceBaseSettings::getServiceNameEdit() { return ui.m_NameEdit; }
QPlainTextEdit *    AppletServiceBaseSettings::getServiceDescriptionEdit() { return ui.m_DescriptionEdit; }

QPushButton *       AppletServiceBaseSettings::getApplyButton()            { return ui.m_ApplyButton; }

PermissionWidget *  AppletServiceBaseSettings::getPermissionWidget()       { return ui.m_PermissionWidget; }
InformationWidget * AppletServiceBaseSettings::getInformationWidget()      { return ui.m_InfoWidget; }
LogWidget *         AppletServiceBaseSettings::getLogWidget()              { return ui.m_LogWidget; }

//============================================================================
AppletServiceBaseSettings::AppletServiceBaseSettings( const char* objName, AppCommon& app, QWidget* parent )
    : AppletBase( objName, app, parent )
    , ui(*(new Ui::AppletServiceBaseSettingsUi))
{
}

//============================================================================
void AppletServiceBaseSettings::setupServiceBaseApplet( EApplet applet, EPluginType pluginType )
{
    setAppletType( applet );
    setPluginType( pluginType );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    getServiceUrlLabel()->setVisible( false );
    getServiceUrlEdit()->setVisible( false );
    getInformationWidget()->setVisible( false );

    if( ePluginTypeInvalid != getPluginType() )
    {
        getPermissionWidget()->setPluginType( getPluginType() );
        getServiceTitle()->setText( GuiParams::describePlugin( getPluginType(), false ).c_str() );
        loadPluginSetting();
    }

    connectServiceWidgets();

    ui.m_DescriptionEdit->setFixedHeight( GuiParams::getButtonSize( eButtonSizeMedium ).height() );
}

//============================================================================
void AppletServiceBaseSettings::connectServiceWidgets()
{
    connect( getApplyButton(), SIGNAL(clicked()), this, SLOT( slotApplyButtonClicked() ) );
}

//============================================================================
void AppletServiceBaseSettings::loadPluginSetting()
{
    if( ePluginTypeInvalid != getPluginType() )
    {
        m_OrigPermissionLevel = m_MyApp.getAppGlobals().getMyNetIdent()->getPluginPermission( getPluginType() );
        getPermissionWidget()->setPermissionLevel( m_OrigPermissionLevel );
        m_MyApp.getEngine().getPluginSettingMgr().getPluginSetting( getPluginType(), m_PluginSetting );
        loadUiFromSetting();
    }
}

//============================================================================
void AppletServiceBaseSettings::savePluginSetting()
{
    if( ePluginTypeInvalid != getPluginType() )
    {
        saveUiToSetting();
        m_MyApp.getEngine().getPluginSettingMgr().setPluginSetting( m_PluginSetting );
    }
}

//============================================================================
void AppletServiceBaseSettings::loadUiFromSetting()
{
    if( ePluginTypeInvalid != getPluginType() )
    {

    }
}

//============================================================================
void AppletServiceBaseSettings::saveUiToSetting()
{
    if( ePluginTypeInvalid != getPluginType() )
    {
    }
}

//============================================================================
void AppletServiceBaseSettings::slotApplyButtonClicked()
{
    saveUiToSetting();
    m_MyApp.getEngine().getPluginSettingMgr().setPluginSetting( m_PluginSetting );
}