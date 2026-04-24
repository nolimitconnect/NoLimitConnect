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
#include "AssetSendMgr.h"
#include "GuiMemberActiveMgr.h"
#include "GuiUserMultiListWidget.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include <GuiInterface/IFromGui.h>

#include <QFrame>
#include <QLabel>
#include <QTimer>

#include "ui_AppletHostClient.h"

//============================================================================
AppletHostAdminBase::AppletHostAdminBase( const char* ObjName, AppCommon& app, QWidget* parent )
: AppletBase( ObjName, app, parent )
, ui(*(new Ui::AppletHostClientUi ))
{
    // Connect to AssetSendMgr for multi-send progress updates
    connect( &m_MyApp.getAssetSendMgr(), &AssetSendMgr::signalSendingToMember, this, &AppletHostAdminBase::slotSendingToMember );
    connect( &m_MyApp.getAssetSendMgr(), &AssetSendMgr::signalMultiSendComplete, this, &AppletHostAdminBase::slotMultiSendComplete );
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

	return m_MyApp.getAssetSendMgr().handleGroupieAssetAction( this, groupieId, assetAction, assetInfo, true );
}

//============================================================================
void AppletHostAdminBase::slotUserSelected( GuiUser* guiUser )
{
    m_SelectedUser = guiUser;
}

//============================================================================
void AppletHostAdminBase::slotSendingToMember( VxGUID assetId, VxGUID memberId, QString memberName )
{
    ui.m_SessionWidget->sendingToMember( assetId, memberId, memberName );
    QLabel* statusLabel = ui.m_SessionWidget->getSessionStatusLabel();
    if( statusLabel )
    {
        statusLabel->setText( tr( "Sending to: %1" ).arg( memberName ) );
        statusLabel->setVisible( true );
    }
}

//============================================================================
void AppletHostAdminBase::slotMultiSendComplete( VxGUID assetId, bool allSucceeded, int successCount, int failCount )
{
    ui.m_SessionWidget->multiSendComplete( assetId, allSucceeded, successCount, failCount );
    QLabel* statusLabel = ui.m_SessionWidget->getSessionStatusLabel();
    if( statusLabel )
    {
        if( allSucceeded )
        {
            statusLabel->setText( tr( "Sent to %1 member(s)" ).arg( successCount ) );
        }
        else
        {
            statusLabel->setText( tr( "Sent: %1 success, %2 failed" ).arg( successCount ).arg( failCount ) );
        }
        // Clear status after a delay
        QTimer::singleShot( 3000, statusLabel, [statusLabel]() {
            statusLabel->setVisible( false );
        } );
    }
}
