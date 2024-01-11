//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ServiceSettingsWidget.h"

#include "ActivityInformation.h"
#include "AppletBase.h"
#include "AppCommon.h"
#include "AppGlobals.h"
#include "AppletMgr.h"
#include "GuiHelpers.h"
#include "GuiParams.h"
#include "MyIcons.h"

#include <P2PEngine/P2PEngine.h>

//============================================================================
ServiceSettingsWidget::ServiceSettingsWidget( QWidget* parent )
	: QWidget( parent )
    , m_MyApp( GetAppInstance() )
{
	initServiceSettingsWidget();
}

//============================================================================
void ServiceSettingsWidget::initServiceSettingsWidget( void )
{
	ui.setupUi( this );
    ui.m_SettingsButton->setFixedSize( eButtonSizeSmall );
    ui.m_ViewServiceButton->setFixedSize( eButtonSizeSmall );

    connect( ui.m_SettingsButton, SIGNAL(clicked()), this, SLOT(slotServiceSettingsClicked()) );
    connect( ui.m_ViewServiceButton, SIGNAL(clicked()), this, SLOT(slotViewServiceClicked()) );
}

//============================================================================
void ServiceSettingsWidget::updateUi( void )
{
    if( m_PluginType == ePluginTypeInvalid )
    {
        return;
    }

    updateIcons();
}

//============================================================================
void ServiceSettingsWidget::updateIcons( void )
{
    if( m_PluginType == ePluginTypeInvalid )
    {
        return;
    }

    ui.m_SettingsButton->setIcon( m_MyApp.getMyIcons().getPluginSettingsIcon( m_PluginType ) );
    QString settingsText = GuiParams::describePlugin( m_PluginType, false ).c_str();
    settingsText += QObject::tr( "Settings" );
    ui.m_SettingsLabel->setText( settingsText );


    ui.m_ViewServiceButton->setIcon( m_MyApp.getMyIcons().getPluginIcon( m_PluginType ) );
    QString viewText = QObject::tr( "View " ) + GuiParams::describePlugin( m_PluginType, false ).c_str();
    ui.m_ViewServiceLabel->setText( viewText );

    EApplet viewAppletType = GuiHelpers::pluginTypeToViewApplet( m_PluginType );
    if( eAppletUnknown == viewAppletType )
    {
        ui.m_ViewServiceFrame->setVisible( false );
    }
}

//============================================================================
void ServiceSettingsWidget::slotServiceSettingsClicked()
{
    launchApplet( GuiHelpers::pluginTypeToSettingsApplet( m_PluginType ) );
}

//============================================================================
void ServiceSettingsWidget::slotViewServiceClicked()
{
    launchApplet( GuiHelpers::pluginTypeToViewApplet( m_PluginType ) );
}

//============================================================================
void ServiceSettingsWidget::launchApplet( EApplet appletType )
{
    if( eAppletUnknown != appletType )
    {
        AppletBase * parentApplet = GuiHelpers::findParentApplet( this );
        if( parentApplet )
        {
            m_MyApp.getAppletMgr().launchApplet( appletType, parentApplet );
        }
        else
        {
            QString msgText = QObject::tr( "Unable to determine parent" );
            QMessageBox::information( this, msgText, msgText );
        }
    }
    else
    {
        QString msgText = QObject::tr( "Unknown Applet " );
        QMessageBox::information( this, msgText, msgText );
    }
}

