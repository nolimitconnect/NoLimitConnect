//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletIgnoredHosts.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "GuiHostedListSession.h"
#include "GuiHostedListItem.h"

#include <ptop_src/ptop_engine_src/UserJoinMgr/UserJoinMgr.h>

#include <CoreLib/VxDebug.h>

//============================================================================
AppletIgnoredHosts::AppletIgnoredHosts( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_IGNORED_HOSTS, app, parent )
{
	ui.setupUi( getContentItemsFrame() );
    setAppletType( eAppletIgnoredHosts );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

	ui.m_GuiHostedListWidget->setIsHostView( true );
	ui.m_GuiHostedListWidget->setIsIgnoredHostsView( true );

	// so is actually destroyed
	connect( this, SIGNAL( signalBackButtonClicked() ), this, SLOT( closeApplet() ) );

	connect( ui.m_GuiHostedListWidget, SIGNAL( signalIconButtonClicked(GuiHostedListSession*,GuiHostedListItem*) ),		this, SLOT( slotIconButtonClicked(GuiHostedListSession*,GuiHostedListItem*) ) );
	connect( ui.m_GuiHostedListWidget, SIGNAL( signalMenuButtonClicked(GuiHostedListSession*,GuiHostedListItem*) ),		this, SLOT( slotMenuButtonClicked(GuiHostedListSession*,GuiHostedListItem*) ) );
	connect( ui.m_GuiHostedListWidget, SIGNAL( signalJoinButtonClicked(GuiHostedListSession*,GuiHostedListItem*) ),		this, SLOT( slotJoinButtonClicked(GuiHostedListSession*,GuiHostedListItem*) ) );
	connect( ui.m_GuiHostedListWidget, SIGNAL( signalConnectButtonClicked(GuiHostedListSession*,GuiHostedListItem*) ),	this, SLOT( slotConnectButtonClicked(GuiHostedListSession*,GuiHostedListItem*) ) );
	connect( ui.m_GuiHostedListWidget, SIGNAL( signalKickButtonClicked(GuiHostedListSession*,GuiHostedListItem*) ),		this, SLOT( slotKickButtonClicked(GuiHostedListSession*,GuiHostedListItem*) ) );
	connect( ui.m_GuiHostedListWidget, SIGNAL( signalIgnoreButtonClicked(GuiHostedListSession*,GuiHostedListItem*) ), this, SLOT( slotIgnoreButtonClicked(GuiHostedListSession*,GuiHostedListItem*) ) );


	m_MyApp.activityStateChange( this, true );

	queryIgnoredHostsList();
}

//============================================================================
AppletIgnoredHosts::~AppletIgnoredHosts()
{
	m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletIgnoredHosts::queryIgnoredHostsList( void )
{
	m_MyApp.getEngine().getIgnoreListMgr().getIgnoredHostsList( m_IgnoredHostList );
	for( auto ignoredHost : m_IgnoredHostList )
	{
		insertIntoHostList( ignoredHost.second );
	}
}

//============================================================================
void AppletIgnoredHosts::insertIntoHostList( IgnoredHostInfo& hostInfo )
{
	VxGUID sessionId;
	sessionId.initializeWithNewVxGUID();
	VxPtopUrl ptopUrlIpv4( hostInfo.getHostUrl(false) );
	VxPtopUrl ptopUrlIpv6( hostInfo.getHostUrl(true) );
	EHostType hostType = ptopUrlIpv4.isValid() ? ptopUrlIpv4.getHostType() : ptopUrlIpv6.getHostType();
	HostedId hostedId( hostInfo.getOnlineId(), hostType );

	GuiHosted* guiHosted = new GuiHosted( m_MyApp );
	guiHosted->setParent( this );
	guiHosted->setHostedId( hostedId );
	guiHosted->setThumbId( hostInfo.getThumbId() );
	guiHosted->setHostInfoTimestamp( hostInfo.getTimestampMs() );
	guiHosted->setHostInviteUrl( false, hostInfo.getHostUrl( false ) );
	guiHosted->setHostInviteUrl( true, hostInfo.getHostUrl( true ) );
	guiHosted->setHostTitle( hostInfo.getHostTitle() );
	guiHosted->setHostDescription( hostInfo.getHostDescription() );
	guiHosted->setIsIgnored( true );

	ui.m_GuiHostedListWidget->updateHostedList( hostedId, guiHosted, sessionId );
}

//============================================================================
void AppletIgnoredHosts::setStatusMsg( QString statusMsg )
{
	ui.m_StatusLabel->setText( statusMsg );
}

//============================================================================
void AppletIgnoredHosts::setListLabel( QString labelText )
{
	ui.m_HostOrListViewLabel->setText( labelText );
}

//============================================================================
void AppletIgnoredHosts::slotIconButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem )
{
	LogMsg( LOG_VERBOSE, "AppletIgnoredHosts::slotIconButtonClicked" );
}

//============================================================================
void AppletIgnoredHosts::slotMenuButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem )
{
	LogMsg( LOG_VERBOSE, "AppletIgnoredHosts::slotMenuButtonClicked" );
}

//============================================================================
void AppletIgnoredHosts::slotJoinButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem )
{
	LogMsg( LOG_VERBOSE, "AppletIgnoredHosts::slotJoinButtonClicked" );
}

//============================================================================
void AppletIgnoredHosts::slotConnectButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem )
{
	LogMsg( LOG_VERBOSE, "AppletIgnoredHosts::slotConnectButtonClicked" );
}

//============================================================================
void AppletIgnoredHosts::slotKickButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem )
{
	LogMsg( LOG_VERBOSE, "AppletIgnoredHosts::slotKickButtonClicked" );
}

//============================================================================
void AppletIgnoredHosts::slotIgnoreButtonClicked( GuiHostedListSession* hostSession, GuiHostedListItem* hostItem )
{
	LogMsg( LOG_VERBOSE, "AppletIgnoredHosts::slotIgnoreButtonClicked" );
	if( m_MyApp.getEngine().getIgnoreListMgr().removeHostIgnore( hostSession->getHostOnlineId() ) )
	{
		ui.m_GuiHostedListWidget->removeItemWidget( hostItem );
	}
}
