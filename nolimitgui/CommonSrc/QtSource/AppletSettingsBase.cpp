//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletSettingsBase.h"

#include "AppCommon.h"
#include "GuiHelpers.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>

//============================================================================
AppletSettingsBase::AppletSettingsBase( const char* ObjName, AppCommon& app, QWidget* parent )
    : AppletBase( ObjName, app, parent )
{
}

//============================================================================
void AppletSettingsBase::connectServiceWidgets()
{
    connect( getPluginSettingsWidget()->getApplyButton(), SIGNAL(clicked()), this, SLOT( slotApplyServiceSettings() ) );
    connect( getPluginSettingsWidget(), SIGNAL(signalThumbnailAssetChanged(VxGUID,bool)), this, SLOT(slotThumbnailAssetChanged(VxGUID,bool)) );
}

//============================================================================
void AppletSettingsBase::loadPluginSetting()
{
    if( ePluginTypeInvalid != getPluginType() )
    {
        m_OrigPermissionLevel = m_MyApp.getAppGlobals().getMyNetIdent()->getPluginPermission( getPluginType() );
        getPluginSettingsWidget()->getPermissionWidget()->setPermissionLevel( m_OrigPermissionLevel );

        m_PluginSetting.setPluginType( getPluginType() );// must set before get settings so engine will know which
        m_MyApp.getEngine().getPluginSettingMgr().getPluginSetting( getPluginType(), m_PluginSetting );
        loadUiFromSetting();
    }
}

//============================================================================
void AppletSettingsBase::savePluginSetting()
{
    if( ( ePluginTypeInvalid != getPluginType() ) && ( ePluginTypeInvalid != m_PluginSetting.getPluginType() ) )
    {
        saveUiToSetting();
        m_MyApp.getEngine().getPluginSettingMgr().setPluginSetting( m_PluginSetting );
    }
}

//============================================================================
void AppletSettingsBase::loadUiFromSetting()
{
    if( ePluginTypeInvalid != getPluginType() )
    {
        GuiHelpers::pluginSettingsToWidget( getPluginType(), m_PluginSetting, getPluginSettingsWidget() );
    }
}

//============================================================================
void AppletSettingsBase::saveUiToSetting()
{
    if( ePluginTypeInvalid != getPluginType() )
    {
        GuiHelpers::widgetToPluginSettings( getPluginType(), getPluginSettingsWidget(), m_PluginSetting );
    }
}

//============================================================================
void AppletSettingsBase::slotApplyServiceSettings()
{
    saveUiToSetting();
    m_MyApp.getEngine().getPluginSettingMgr().setPluginSetting( m_PluginSetting );

    EFriendState newPermissionLevel = getPluginSettingsWidget()->getPermissionWidget()->getPermissionLevel();
    if( newPermissionLevel != m_OrigPermissionLevel )
    {
        m_MyApp.getEngine().setPluginPermission( getPluginSettingsWidget()->getPermissionWidget()->getPluginType(), newPermissionLevel );
    }

    savePluginSetting();
    QMessageBox::information( this, QObject::tr( "Service Settings" ), QObject::tr( "Service Settings Applied" ), QMessageBox::Ok );
}

//============================================================================
void AppletSettingsBase::slotThumbnailAssetChanged( VxGUID thumbId, bool isCircular )
{
    m_PluginSetting.setThumnailId( thumbId, isCircular );
    // keep settings in sync TODO there should only be one setting to FIXME
    getPluginSettingsWidget()->setPluginSetting( m_PluginSetting );
}