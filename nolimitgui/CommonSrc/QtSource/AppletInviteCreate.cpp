//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletInviteCreate.h"

#include "AppletMgr.h"
#include "AppletInformation.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/Invite.h>
#include <NetLib/NetHostSetting.h>
#include <CoreLib/IsBigEndianCpu.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxUrl.h>

#include <QDesktopServices>
#include <QUrl>
#include <QClipboard>
#include <QRegularExpression>
#include <QScrollBar>
#include <QApplication>

#include "ui_AppletInviteCreate.h"

namespace
{
    const int MAX_INFO_MSG_SIZE = 2048;
}

QPlainTextEdit *            AppletInviteCreate::getInviteMessageEdit( void )    { return ui.m_InviteMessageTextEdit; }

//============================================================================
AppletInviteCreate::AppletInviteCreate( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_INVITE_CREATE, app, parent )
, ui(*(new Ui::AppletInviteCreateUi))
{
    setAppletType( eAppletInviteCreate );
    ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

    ui.m_InviteInfoButton->setFixedSize( eButtonSizeSmall );
    ui.m_InviteInfoButton->setIcon( eMyIconInformation );
    connect( ui.m_InviteInfoButton, SIGNAL(clicked()), this, SLOT(slotInviteInfoButtonClicked()) );

    ui.m_InviteUrlWidget->setupInvite( true );
    ui.m_ClipboardCopyWidget->setActionText( QObject::tr( "Copy invite to clipboard" ) );

    connect( ui.m_ClipboardCopyWidget, SIGNAL(clicked()), this, SLOT(slotCopyInviteButtonClicked()) );
    connect( ui.m_InviteMessageTextEdit, SIGNAL(textChanged()), this, SLOT(slotUpdateInviteUserMsg()) );

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletInviteCreate::~AppletInviteCreate()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletInviteCreate::slotCopyInviteButtonClicked( void )
{
    std::string inviteText = ui.m_InviteUrlWidget->getInviteText();
    if( !inviteText.empty() )
    {
        ui.m_ClipboardCopyWidget->copyToClipboard( inviteText.c_str() );
        okMessageBox( QObject::tr( "Invite Was Copied To Clipboard" ), QObject::tr( "Invite Was Copied To Clipboard" ) );
    }
    else
    {
        okMessageBox( QObject::tr( "Invite Is Empty" ), QObject::tr( "Cannot create an Invite if Invite is empty" ) );
    }
}

//============================================================================
void AppletInviteCreate::slotUpdateInviteUserMsg( void )
{
    ui.m_InviteUrlWidget->setInviteUserMsg( ui.m_InviteMessageTextEdit->toPlainText().toUtf8().constData() );
}

//============================================================================
bool AppletInviteCreate::setInviteType( EHostType hostType )
{
    return ui.m_InviteUrlWidget->setHostInviteType( hostType );
}

//============================================================================
void AppletInviteCreate::slotInviteInfoButtonClicked( void )
{
    AppletInformation * activityInfo = new AppletInformation( m_MyApp, this, eInfoTypeWhatIsAInvite );
    activityInfo->show();
}
