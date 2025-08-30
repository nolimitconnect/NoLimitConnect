//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletHostSelect.h"

#include "GuiHostedByMeJoinMgr.h"
#include "GuiHostJoin.h"
#include "GuiHostJoinSession.h"

#include "AppCommon.h"
#include "AppletMgr.h"
#include "AppSettings.h"
#include "GuiHelpers.h"	
#include "GuiParams.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include "ui_AppletHostSelect.h"

//============================================================================
AppletHostSelect::AppletHostSelect( AppCommon& app,  QWidget* parent )
: AppletBase( OBJNAME_APPLET_HOST_SELECT, app, parent )
, ui(*(new Ui::AppletHostSelectUi))
, m_HostJoinMgr( app.getHostJoinMgr() )
{
    setAppletType( eAppletHostSelect );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );
	GuiHelpers::fillJoinRequest( ui.m_RequestStateComboBox );
	ui.m_CreateInviteButton->setFixedSize( eButtonSizeSmall );
	ui.m_CreateInviteButton->setIcon( eMyIconInviteCreate );

	connect( &m_HostJoinMgr, SIGNAL(signalHostJoinOfferStateChange(GroupieId,EJoinState)), this, SLOT(slotHostJoinOfferStateChange(GroupieId,EJoinState)) );
	connect( &m_HostJoinMgr, SIGNAL(signalHostJoinOnlineStatus(GuiHostJoin*,bool)), this, SLOT(slotHostJoinOnlineStatus(GuiHostJoin*,bool)) );

	connect( ui.m_HostJoinRequestList, SIGNAL(signalAcceptButtonClicked(GuiHostJoinSession*,HostJoinRequestListItem*)), 
			 this, SLOT(slotAcceptButtonClicked(GuiHostJoinSession*,HostJoinRequestListItem*)) );
	connect( ui.m_HostJoinRequestList, SIGNAL(signalRejectButtonClicked(GuiHostJoinSession*,HostJoinRequestListItem*)), 
			 this, SLOT(slotRejectButtonClicked(GuiHostJoinSession*,HostJoinRequestListItem*)) );

	connect( ui.m_RequestStateComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotJoinComboBoxSelectionChange(int)) );
	connect( ui.m_CreateInviteButton, SIGNAL(clicked()), this, SLOT(slotCreateInviteButtonClicked()) );

	updateJoinList();

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletHostSelect::~AppletHostSelect()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletHostSelect::updateJoinList( void )
{
	ui.m_HostJoinRequestList->clear();
	std::map<GroupieId, GuiHostJoin*>& joinList = m_HostJoinMgr.getHostJoinList();
	for( auto& iter : joinList )
	{
		GuiHostJoin* user = iter.second;
		if( !user->getUser()->isIgnored() && m_JoinState == user->getJoinState() )
		{
			updateHostJoinRequest( user );
		}
	}
}

//============================================================================
void AppletHostSelect::slotHostJoinRequested( GuiHostJoin* guiHostJoin )
{
	updateHostJoinRequest( guiHostJoin );
}

//============================================================================
void AppletHostSelect::slotlHostJoinUpdated( GuiHostJoin* guiHostJoin )
{
	updateHostJoinRequest( guiHostJoin );
}

//============================================================================
void AppletHostSelect::slotHostJoinRemoved( GroupieId groupieId )
{
	ui.m_HostJoinRequestList->removeHostJoinRequest( groupieId );
}

//============================================================================
void AppletHostSelect::slotHostJoinOfferStateChange( GroupieId groupieId, EJoinState hostOfferState )
{
	updateJoinList();
}

//============================================================================
void AppletHostSelect::slotHostJoinOnlineStatus( GuiHostJoin* user, bool isOnline )
{
	updateJoinList();
}

//============================================================================
void AppletHostSelect::updateHostJoinRequest( GuiHostJoin* guiHostJoin )
{
	vx_assert( guiHostJoin );
	if( guiHostJoin->getJoinState() == eJoinStateJoinRequested )
	{
		ui.m_HostJoinRequestList->addOrUpdateHostRequest( guiHostJoin );
	}
	else
	{
		ui.m_HostJoinRequestList->removeHostJoinRequest( guiHostJoin->getGroupieId() );
	}
}

//============================================================================
void AppletHostSelect::slotAcceptButtonClicked( GuiHostJoinSession* joinSession, HostJoinRequestListItem* joinItem )
{
	if( joinSession && joinSession->getHostJoin() )
	{
		m_HostJoinMgr.joinAccepted( joinSession->getHostJoin() );
	}
}

//============================================================================
void AppletHostSelect::slotRejectButtonClicked( GuiHostJoinSession* joinSession, HostJoinRequestListItem* joinItem )
{
	m_HostJoinMgr.joinRejected( joinSession->getHostJoin() );
}

//============================================================================
void AppletHostSelect::slotJoinComboBoxSelectionChange( int comboIdx )
{
	m_JoinState = GuiHelpers::comboIdxToJoinState( comboIdx );
	updateJoinList();
}

//============================================================================
void AppletHostSelect::slotCreateInviteButtonClicked( void )
{
	m_MyApp.getAppletMgr().launchApplet( eAppletInviteCreate, getParentPageFrame() );
}
