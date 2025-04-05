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
    //ui.m_IdentWidget->setVisible( false );
    ui.m_AcceptInviteButton->setFixedSize( eButtonSizeMedium );
    ui.m_AcceptInviteButton->setIcon( eMyIconInviteAccept );
    ui.m_RejectInviteButton->setFixedSize( eButtonSizeMedium );
    ui.m_RejectInviteButton->setIcon( eMyIconCancelNormal );

    connect( ui.m_ClipboardPastWidget, SIGNAL(signalClipboardPaste(QString)), this, SLOT(slotPasteFromClipboard(QString)) );
    connect( ui.m_AcceptInviteButton, SIGNAL(clicked()), this, SLOT(slotAcceptInviteButtonClicked()) );
    connect( ui.m_RejectInviteButton, SIGNAL(clicked()), this, SLOT(slotRejectInviteButtonClicked()) );

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
void AppletInviteAccept::toGuiInfoMsg( char * infoMsg )
{
    QString infoStr( infoMsg );

    infoStr.remove(QRegularExpression("[\\n\\r]"));

    emit signalInfoMsg( infoStr );
}

//============================================================================
void AppletInviteAccept::slotPasteFromClipboard( QString clipboardText )
{
    if( clipboardText.isEmpty() )
    {
        okMessageBox( QObject::tr( "Clipboard Is Empty" ), QObject::tr( "Cannot Paste Empty Clipboard" ) );
        return;
    }
    
    Invite invite;
    if( !invite.setInviteText( clipboardText.toUtf8().constData() ) )
    {
        okMessageBox( QObject::tr( "Clipboard Contained Invalid Invite Text" ), QObject::tr( "Clipboard Has Text That Cannot Be Parsed Into An Invite" ) );
        return;
    }

    ui.m_InviteTextEdit->setPlainText( clipboardText.toUtf8().constData() );
    updateInvite();
}

//============================================================================
void AppletInviteAccept::slotAcceptInviteButtonClicked( void )
{

}

//============================================================================
void AppletInviteAccept::slotRejectInviteButtonClicked( void )
{
    closeApplet();
}

//============================================================================
void AppletInviteAccept::infoMsg( const char* errMsg, ... )
{
    char as8Buf[ MAX_INFO_MSG_SIZE ];
    va_list argList;
    va_start( argList, errMsg );
    vsnprintf( as8Buf, sizeof( as8Buf ), errMsg, argList );
    as8Buf[ sizeof( as8Buf ) - 1 ] = 0;
    va_end( argList );

    toGuiInfoMsg( as8Buf );
}

//============================================================================
void AppletInviteAccept::updateInvite( void )
{
    QString inviteText = getInviteEdit()->toPlainText();
    if( inviteText.isEmpty() )
    {
        return;
    }

    Invite invite;
    if( invite.setInviteText( inviteText.toUtf8().constData() ) )
    {
        ui.m_PersonUrlEdit->setText( invite.getInviteUrl( eHostTypePeerUser ).c_str() );
        ui.m_GroupUrlEdit->setText( invite.getInviteUrl( eHostTypeGroup ).c_str() );
    }
}
