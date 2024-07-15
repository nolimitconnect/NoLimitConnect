//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PermissionWidget.h"

#include "AppCommon.h"
#include "AppGlobals.h"
#include "ActivityInformation.h"
#include "AppletBase.h"
#include "GuiHelpers.h"
#include "GuiParams.h"
#include "MyIcons.h"

#include <P2PEngine/P2PEngine.h>

#include "ui_PermissionWidget.h"

VxPushButton *              PermissionWidget::getPluginRunButton()            { return ui.m_PluginRunButton; }
VxPushButton *              PermissionWidget::getPluginSettingsButton()       { return ui.m_PluginSettingsButton; }

//============================================================================
PermissionWidget::PermissionWidget( QWidget* parent )
	: QWidget( parent )
    , ui(*(new Ui::PermissionWidgetUi))
    , m_MyApp( GetAppInstance() )
{
    m_ParentApplet = GuiHelpers::findParentApplet(parent);
	initPermissionWidget();
}

//============================================================================
void PermissionWidget::initPermissionWidget()
{
	ui.setupUi( this );
    ui.m_PluginInfoButton->setIcon( eMyIconInformation );
    fillPermissionComboBox();
    if( m_PluginType != ePluginTypeInvalid )
    {
        updateUi();
    }

    // for now jump to service is not enabled so just hide the button
    ui.m_PluginRunButton->setVisible( false );
}

//============================================================================
void PermissionWidget::fillPermissionComboBox( void )
{
    GuiHelpers::fillPermissionComboBox( ui.m_PermissionComboBox );
    ui.m_PermissionComboBox->setCurrentIndex( 4 );
}

//============================================================================
void PermissionWidget::updateUi( void )
{
    if( m_PluginType == ePluginTypeInvalid )
    {
        return;
    }

    ui.m_PluginLabel->setText( GuiParams::describePlugin( m_PluginType, false ).c_str() );
    EFriendState pluginPermission = m_MyApp.getMyNetIdent()->getPluginPermission( m_PluginType );
    if( !m_OrigPermissionIsSet )
    {
        m_OrigPermissionIsSet = true;
        m_OrigPermissionLevel = pluginPermission;
    }

    ui.m_PermissionComboBox->setCurrentIndex( GuiHelpers::friendStateToComboIdx( pluginPermission ) );
    if( !m_PermissionsConnected )
    {
        m_PermissionsConnected = true;
        connect( ui.m_PermissionButton, SIGNAL(clicked()), this, SLOT( slotShowPermissionInformation() ) );
        connect( ui.m_PermissionComboBox, SIGNAL( currentIndexChanged( int ) ), this, SLOT( slotHandleSelectionChanged( int ) ) );
        connect( ui.m_PluginRunButton, SIGNAL(clicked()), this, SLOT( slotRunPlugin() ) );
        connect( ui.m_PluginSettingsButton, SIGNAL(clicked()), this, SLOT( slotSetupPlugin() ) );
        connect( ui.m_PluginInfoButton, SIGNAL(clicked()), this, SLOT( slotShowPluginInformation() ) );
    }

    updatePermissionIcon();
}

//============================================================================
void PermissionWidget::setPluginType( EPluginType pluginType, int subType ) 
{ 
    if( pluginType == ePluginTypeInvalid )
    {
        return;
    }

    m_PluginType = pluginType;
    m_SubPluginType = subType;

    if( !m_OrigPermissionIsSet )
    {
        m_OrigPermissionIsSet = true;
        m_OrigPermissionLevel = m_MyApp.getMyNetIdent()->getPluginPermission( m_PluginType );;
        setPermissionLevel( m_OrigPermissionLevel );
    }

    ui.m_PluginRunButton->setIcon( m_MyApp.getMyIcons().getPluginIcon( m_PluginType ) );
    ui.m_PluginSettingsButton->setIcon( m_MyApp.getMyIcons().getPluginSettingsIcon( m_PluginType ) );

    updateUi();  
}

//============================================================================
void PermissionWidget::updatePermissionIcon( void )
{
    if( m_PluginType == ePluginTypeInvalid )
    {
        return;
    }

    ui.m_PermissionButton->setIcon( m_MyApp.getMyIcons().getFriendshipIcon( GuiHelpers::comboIdxToFriendState( ui.m_PermissionComboBox->currentIndex() ) ) );
}

//============================================================================
void PermissionWidget::slotHandleSelectionChanged( int )
{
    if( m_PluginType == ePluginTypeInvalid )
    {
        return;
    }

    EFriendState pluginPermission = GuiHelpers::comboIdxToFriendState( ui.m_PermissionComboBox->currentIndex() );
    m_MyApp.getAppGlobals().updatePluginPermission( m_PluginType, pluginPermission );
    updatePermissionIcon();
}

//============================================================================
void PermissionWidget::setPermissionLevel( EFriendState permLevel )
{
    if( m_PluginType == ePluginTypeInvalid )
    {
        return;
    }

    ui.m_PermissionComboBox->setCurrentIndex( GuiHelpers::friendStateToComboIdx( permLevel ) );
}

//============================================================================
EFriendState PermissionWidget::getPermissionLevel( void )
{
    if( m_PluginType == ePluginTypeInvalid )
    {
        return eFriendStateIgnore;
    }

    return  GuiHelpers::comboIdxToFriendState( ui.m_PermissionComboBox->currentIndex() );
}

//============================================================================
void PermissionWidget::slotShowPermissionInformation()
{
    ActivityInformation * activityInfo = new ActivityInformation( m_MyApp, this, eInfoTypePermission );
    if( activityInfo )
    {
        activityInfo->show();
    }
}

//============================================================================
void PermissionWidget::slotShowPluginInformation()
{
    ActivityInformation * activityInfo = new ActivityInformation( m_MyApp, this, m_PluginType );
    if( activityInfo )
    {
        activityInfo->show();
    }
}

//============================================================================
void PermissionWidget::slotRunPlugin()
{
    if( m_ParentApplet )
    {
        m_MyApp.launchApplet( GuiHelpers::pluginTypeToUserApplet( m_PluginType ), m_ParentApplet->getParentPageFrame() );
    }
}

//============================================================================
void PermissionWidget::slotSetupPlugin()
{
    if( m_ParentApplet )
    {
        m_MyApp.launchApplet( GuiHelpers::pluginTypeToSettingsApplet( m_PluginType ), m_ParentApplet->getParentPageFrame() );
    }
}
