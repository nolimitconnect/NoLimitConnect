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

#include "AppCommon.h"
#include "AppletInformation.h"
#include "VxPushButton.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>

#include <QMessageBox>

#include "ui_AppletSettingsHostBase.h"

PluginSettingsWidget*       AppletSettingsHostBase::getPluginSettingsWidget() { return ui.m_PluginSettingsWidget; }

//============================================================================
AppletSettingsHostBase::AppletSettingsHostBase( const char* ObjName, AppCommon& app, QWidget* parent )
    : AppletSettingsBase( ObjName, app, parent )
    , ui(*(new Ui::AppletSettingsHostBaseUi))
{
    ui.setupUi( getContentItemsFrame() );

    getPluginSettingsWidget()->getPermissionWidget()->getPluginRunButton()->setVisible( false );
    getPluginSettingsWidget()->getPermissionWidget()->getPluginSettingsButton()->setVisible( false );

    connect( getPluginSettingsWidget()->getApplyButton(), SIGNAL(clicked()), this, SLOT(slotApplyServiceSettings() ) );
    connect( ui.m_HostingRequirementsButton, SIGNAL(clicked()), this, SLOT(slotHostRequirementsButtonClicked() ) );
}

//============================================================================
void AppletSettingsHostBase::loadPluginSetting()
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
bool AppletSettingsHostBase::savePluginSetting()
{
    if( (ePluginTypeInvalid != getPluginType()) && (ePluginTypeInvalid != m_PluginSetting.getPluginType()) )
    {
        saveUiToSetting();
        m_PluginSetting.setUpdateTimestampToNow();
        return m_MyApp.getEngine().getPluginSettingMgr().setPluginSetting( m_PluginSetting );
    }
    else
    {
        return false;
    }
}

//============================================================================
void AppletSettingsHostBase::slotApplyServiceSettings()
{
    EFriendState newPermissionLevel = getPluginSettingsWidget()->getPermissionWidget()->getPermissionLevel();
    if( newPermissionLevel != m_OrigPermissionLevel )
    {
        m_MyApp.getEngine().setPluginPermission( getPluginSettingsWidget()->getPermissionWidget()->getPluginType(), newPermissionLevel );
    }

    if( savePluginSetting() )
    {
        QMessageBox::information( this, QObject::tr( "Host Settings" ), QObject::tr( "Host Settings Applied" ), QMessageBox::Ok );
        LogMsg( LOG_VERBOSE, "Host Settings %s Applied", DescribeApplet( getAppletType() ).toUtf8().constData() );
    }
    else
    {
        QMessageBox::information( this, QObject::tr( "Host Settings Failed" ), QObject::tr( "Host Settings Could Not Be Applied" ), QMessageBox::Ok );
        LogMsg( LOG_ERROR, "ERROR Host Settings %s could NOT be Applied", DescribeApplet( getAppletType() ).toUtf8().constData() );
    }
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
        AppletInformation* activityInfo = new AppletInformation( m_MyApp, this, infoType );
        if( activityInfo )
        {
            activityInfo->show();
        }
    }
}
