//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletHostAdminBase.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiMemberActiveMgr.h"
#include "GuiUserMultiListWidget.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include <GuiInterface/IFromGui.h>

#include <QFrame>

#include "ui_AppletHostClient.h"

//============================================================================
AppletHostAdminBase::AppletHostAdminBase( const char* ObjName, AppCommon& app, QWidget* parent )
: AppletBase( ObjName, app, parent )
, ui(*(new Ui::AppletHostClientUi ))
{
}

//============================================================================
AppletHostAdminBase::~AppletHostAdminBase()
{
}

//============================================================================
bool AppletHostAdminBase::checkIfCanSend( void )
{
	return AppletBase::checkIfCanSend( ui.m_UserListWidget->getHostAdminId().getHostedId() );
}

//============================================================================
void AppletHostAdminBase::slotSetMembersVisible( bool visible )
{
    m_MyApp.getAppSettings().setAppletEyeUsersVisible( m_EAppletType, visible );

    if( visible )
    {
        ui.m_UserListWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
        ui.verticalLayout->setStretch( 0, 1 );
    }
    else
    {
        ui.m_UserListWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
        ui.verticalLayout->setStretch( 0, 0 );
    }

    ui.verticalLayout->setStretch( 1, 1 );
    ui.m_UserListWidget->updateGeometry();
    ui.m_SessionWidget->updateGeometry();
    ui.verticalLayout->invalidate();
}

//============================================================================
void AppletHostAdminBase::slotSetSessionVisible( bool visible )
{
    m_MyApp.getAppSettings().setAppletEyeSessionVisible( m_EAppletType, visible );
    ui.m_SessionWidget->setVisible( visible );
}

//============================================================================
bool AppletHostAdminBase::handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo )
{
	GroupieId groupieId = ui.m_UserListWidget->getHostAdminId();
    if( m_SelectedUser )
    {
        LogMsg(LOG_VERBOSE, "AppletHostAdminBase::%s assetAction: %d for selected user: %s", __func__, 
            assetAction, m_SelectedUser->getOnlineName().c_str());
        groupieId.setUserOnlineId( m_SelectedUser->getMyOnlineId() );
    }

	return handleGroupieAssetAction( groupieId, assetAction, assetInfo );
}

//============================================================================
void AppletHostAdminBase::slotUserSelected( GuiUser* guiUser )
{
    m_SelectedUser = guiUser;
}
