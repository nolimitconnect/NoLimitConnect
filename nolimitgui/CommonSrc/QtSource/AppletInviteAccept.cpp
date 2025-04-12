//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletInviteAccept.h"

#include "AppCommon.h"
#include "AppletNetworkSettings.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/Invite.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxUrl.h>

#include <QDesktopServices>
#include <QUrl>
#include <QClipboard>
#include <QRegularExpression>
#include <QScrollBar>
#include <QApplication>

#include "ui_AppletInviteAccept.h"

namespace
{
    const int MAX_INFO_MSG_SIZE = 2048;
}

QPlainTextEdit *            AppletInviteAccept::getInviteEdit( void )     { return ui.m_InviteTextEdit; }

//============================================================================
AppletInviteAccept::AppletInviteAccept( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_INVITE_ACCEPT, app, parent )
, ui(*(new Ui::AppletInviteAcceptUi))
{
    setAppletType( eAppletInviteAccept );
    ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
    ui.m_IdentWidget->setIdentWidgetSize( eButtonSizeSmall );
    ui.m_IdentWidget->setVisible( false );
    ui.m_AcceptInviteButton->setFixedSize( eButtonSizeMedium );
    ui.m_AcceptInviteButton->setIcon( eMyIconInviteAccept );
    ui.m_RejectInviteButton->setFixedSize( eButtonSizeMedium );
    ui.m_RejectInviteButton->setIcon( eMyIconCancelNormal );
    ui.m_AcceptInviteButton->setIconOverrideColor( m_MyApp.getAppTheme().getAcceptColor() );
	ui.m_RejectInviteButton->setIconOverrideColor( m_MyApp.getAppTheme().getCancelColor() );

    ui.m_ClipboardPasteWidget->setActionText( QObject::tr( "Paste invite from clipboard" ) );
    ui.m_InviteUrlWidget->setupInvite( false );

    connect( ui.m_ClipboardPasteWidget, SIGNAL(signalClipboardPaste(QString)), this, SLOT(slotPasteFromClipboard(QString)) );
    connect( ui.m_AcceptInviteButton, SIGNAL(clicked()), this, SLOT(slotAcceptInviteButtonClicked()) );
    connect( ui.m_RejectInviteButton, SIGNAL(clicked()), this, SLOT(slotRejectInviteButtonClicked()) );

    connect( ui.m_InviteUrlWidget, SIGNAL(signalInviteChanged()), this, SLOT(slotInviteChanged()) );

    // Log is seperate now VxAddLogHandler( this );
    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletInviteAccept::~AppletInviteAccept()
{
    // Log is seperate now VxRemoveLogHandler( this );
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletInviteAccept::slotPasteFromClipboard( QString clipboardText )
{
    if( clipboardText.isEmpty() )
    {
        okMessageBox( QObject::tr( "Clipboard Is Empty" ), QObject::tr( "Cannot Paste Empty Clipboard" ) );
        return;
    }
    
    ui.m_InviteUrlWidget->setInviteText( clipboardText.toUtf8().constData() );
}

//============================================================================
void AppletInviteAccept::slotRejectInviteButtonClicked( void )
{
    closeApplet();
}

//============================================================================
void AppletInviteAccept::slotInviteChanged( void )
{
    getInviteEdit()->clear();
    getInviteEdit()->appendPlainText( ui.m_InviteUrlWidget->getInviteText().c_str() );
}

//============================================================================
VxGUID AppletInviteAccept::getFromOnlineId( void )
{
    VxGUID onlineId;
    onlineId.initializeWithNewVxGUID();
    for( auto& ptopUrl : m_HostUrls )
    {
        if( ptopUrl.getOnlineId().isVxGUIDValid() )
        {
            onlineId = ptopUrl.getOnlineId();
            break;
        }
    }

    return onlineId;
}

//============================================================================
void AppletInviteAccept::slotAcceptInviteButtonClicked( void )
{
    std::string inviteText = getInviteEdit()->toPlainText().toUtf8().constData();
    Invite invite;
    invite.setInviteText( inviteText );

    if( !invite.getInviteUrls( m_HostUrls, m_NetworkUrls, m_UserMsg ) )
    {
        GuiHelpers::showInviteInvalidError( this );
        return;
    }

    VxGUID onlineId = getFromOnlineId();
    if( onlineId == m_MyApp.getMyOnlineId() )
    {
        GuiHelpers::showInviteMyselfError( this );
        // return; 
    }

    bool acceptingNetwork{ false };
    if( !m_NetworkUrls.empty() )
    {       
        AppletNetworkSettings* applet = dynamic_cast<AppletNetworkSettings*>(m_MyApp.launchApplet( eAppletNetworkSettings, this ));
        if( applet )
        {
            acceptingNetwork = true;
            connect( applet, SIGNAL(signalBackButtonClicked()), this, SLOT(slotConnectToHosts()) );
            applet->acceptInvite( onlineId, m_NetworkUrls );          
        }
    }

    if( !acceptingNetwork )
    {
        slotConnectToHosts();
    }
}

//============================================================================
void AppletInviteAccept::slotConnectToHosts( void )
{

}