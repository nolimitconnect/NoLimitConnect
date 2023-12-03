//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletSettingsHostBase.h"

#include "ActivityInformation.h"


//============================================================================
AppletSettingsHostBase::AppletSettingsHostBase( const char* ObjName, AppCommon& app, QWidget* parent )
    : AppletSettingsBase( ObjName, app, parent )
{
    ui.setupUi( getContentItemsFrame() );

    getPluginSettingsWidget()->getPermissionWidget()->getPluginRunButton()->setVisible( false );
    getPluginSettingsWidget()->getPermissionWidget()->getPluginSettingsButton()->setVisible( false );

    getConnectionTestWidget()->setPluginType( ePluginTypeHostConnectTest );

    connect( getPluginSettingsWidget()->getApplyButton(), SIGNAL( clicked() ), this, SLOT( slotApplyServiceSettings() ) );
    connect( ui.m_HostingRequirementsButton, SIGNAL( clicked() ), this, SLOT( slotHostRequirementsButtonClicked() ) );
}

//============================================================================
void AppletSettingsHostBase::loadPluginSetting()
{
    if( ePluginTypeInvalid != getPluginType() )
    {
        m_OrigPermissionLevel = m_MyApp.getAppGlobals().getMyNetIdent()->getPluginPermission( getPluginType() );
        m_OrigConnectTestPermission = m_MyApp.getAppGlobals().getMyNetIdent()->getPluginPermission( getConnectionTestWidget()->getPluginType() );
        getPluginSettingsWidget()->getPermissionWidget()->setPermissionLevel( m_OrigPermissionLevel );
        getConnectionTestWidget()->setPermissionLevel( m_OrigConnectTestPermission );

        m_PluginSetting.setPluginType( getPluginType() );// must set before get settings so engine will know which
        m_MyApp.getEngine().getPluginSettingMgr().getPluginSetting( getPluginType(), m_PluginSetting );
        loadUiFromSetting();
    }
}

//============================================================================
void AppletSettingsHostBase::savePluginSetting()
{
    if( (ePluginTypeInvalid != getPluginType()) && (ePluginTypeInvalid != m_PluginSetting.getPluginType()) )
    {
        saveUiToSetting();
        m_MyApp.getEngine().getPluginSettingMgr().setPluginSetting( m_PluginSetting, m_MyApp.elapsedMilliseconds() );
    }
}

//============================================================================
void AppletSettingsHostBase::slotApplyServiceSettings()
{
    saveUiToSetting();
    m_MyApp.getEngine().getPluginSettingMgr().setPluginSetting( m_PluginSetting );

    EFriendState newPermissionLevel = getPluginSettingsWidget()->getPermissionWidget()->getPermissionLevel();
    EFriendState newConnectionTestPermission = getConnectionTestWidget()->getPermissionLevel();
    if( newPermissionLevel != m_OrigPermissionLevel )
    {
        m_MyApp.getEngine().setPluginPermission( getPluginSettingsWidget()->getPermissionWidget()->getPluginType(), newPermissionLevel );
    }

    if( newConnectionTestPermission != m_OrigConnectTestPermission )
    {
        m_MyApp.getEngine().setPluginPermission( getConnectionTestWidget()->getPluginType(), newConnectionTestPermission );
    }

    savePluginSetting();
    QMessageBox::information( this, QObject::tr( "Host Settings" ), QObject::tr( "Host Settings Applied" ), QMessageBox::Ok );
    LogMsg( LOG_VERBOSE, "Host Settings Applied" );
}

//============================================================================
void AppletSettingsHostBase::slotHostRequirementsButtonClicked()
{
    EInfoType infoType{ eInfoTypeInvalid };
    switch( getPluginType() )
    {
    case ePluginTypeHostChatRoom:
        infoType = eInfoTypeHostChatRoom;
        break;

    case ePluginTypeHostGroup:
        infoType = eInfoTypeHostGroup;
        break;

    case ePluginTypeHostNetwork:
        infoType = eInfoTypeHostNetwork;
        break;

    case ePluginTypeHostRandomConnect:
        infoType = eInfoTypeHostRandomConnect;
        break;

    default:
        break;
    }

    if( eInfoTypeInvalid != infoType )
    {
        ActivityInformation* activityInfo = new ActivityInformation( m_MyApp, this, infoType );
        if( activityInfo )
        {
            activityInfo->show();
        }
    }
}
