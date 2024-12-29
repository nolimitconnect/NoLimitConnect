//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletGetStarted.h"

#include "AppCommon.h"
#include "AppletMgr.h"
#include "BottomBarWidget.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include <QDesktopServices>

#include "ui_AppletGetStarted.h"

//============================================================================
AppletGetStarted::AppletGetStarted( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_GET_STARTED, app, parent )
, ui(*(new Ui::AppletGetStartedUi))
{
	ui.setupUi( getContentItemsFrame() );
    setAppletType( eAppletGetStarted );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

    ui.m_BackButton->setFixedSize( eButtonSizeSmall );
    ui.m_ExpandButton->setFixedSize( eButtonSizeSmall );
    ui.m_ShrinkButton->setFixedSize( eButtonSizeSmall );

    ui.m_JoinGroupButton->setFixedSize( eButtonSizeSmall );
    ui.m_JoinChatRoomButton->setFixedSize( eButtonSizeSmall );
    ui.m_JoinRandomConnectButton->setFixedSize( eButtonSizeSmall );

    ui.m_PersonalNotesButton->setFixedSize( eButtonSizeSmall );
    ui.m_LibraryButton->setFixedSize( eButtonSizeSmall );
    
    ui.m_IdentityButton->setFixedSize( eButtonSizeSmall );
    ui.m_AboutMeButton->setFixedSize( eButtonSizeSmall );
    ui.m_StoryboardButton->setFixedSize( eButtonSizeSmall );

    ui.m_WebCamButton->setFixedSize( eButtonSizeSmall );

    ui.m_BackButton->setIcon( eMyIconBack );
    ui.m_ExpandButton->setIcon( eMyIconWindowExpand );
    ui.m_ShrinkButton->setIcon( eMyIconWindowShrink );

    ui.m_JoinGroupButton->setIcon( eMyIconGroupClient );
    ui.m_JoinChatRoomButton->setIcon( eMyIconChatRoomClient );
    ui.m_JoinRandomConnectButton->setIcon( eMyIconRandomConnectClient );

    ui.m_PersonalNotesButton->setIcon( eMyIconNotepadNormal );
    ui.m_LibraryButton->setIcon( eMyIconLibraryNormal );
    
    ui.m_IdentityButton->setIcon( eMyIconUserIdentity );
    ui.m_AboutMeButton->setIcon( eMyIconAboutMeEdit );
    ui.m_StoryboardButton->setIcon( eMyIconStoryboardEdit );

    ui.m_WebCamButton->setIcon( eMyIconCamSetupNormal );


    connect( this, SIGNAL(signalBackButtonClicked()), this, SLOT(closeApplet()) );
    connect( ui.m_BackButton, SIGNAL(clicked()), this, SLOT(closeApplet()) );
    connect( ui.m_ExpandButton, SIGNAL(clicked()), this, SLOT(slotExpandButton()) );
    connect( ui.m_ShrinkButton, SIGNAL(clicked()), this, SLOT(slotShrinkButton()) );

    connect( ui.m_JoinGroupButton, SIGNAL(clicked()), this, SLOT(slotJoinGroup()) );
    connect( ui.m_JoinChatRoomButton, SIGNAL(clicked()), this, SLOT(slotJoinChatRoom()) );
    connect( ui.m_JoinRandomConnectButton, SIGNAL(clicked()), this, SLOT(slotJoinRandomConnect()) );

    connect( ui.m_PersonalNotesButton, SIGNAL(clicked()), this, SLOT(slotPersonalNotes()) );
    connect( ui.m_LibraryButton, SIGNAL(clicked()), this, SLOT(slotLibrary()) );

    connect( ui.m_IdentityButton, SIGNAL(clicked()), this, SLOT(slotIdentity()) );
    connect( ui.m_AboutMeButton, SIGNAL(clicked()), this, SLOT(slotAboutMe()) );
    connect( ui.m_StoryboardButton, SIGNAL(clicked()), this, SLOT(slotStoryboard()) );

    connect( ui.m_WebCamButton, SIGNAL(clicked()), this, SLOT(slotWebCam()) );

    connect( ui.gotoWebsiteButton, SIGNAL(clicked()), this, SLOT(gotoWebsite()) );

	m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletGetStarted::~AppletGetStarted()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletGetStarted::gotoWebsite( void )
{
    QDesktopServices::openUrl( QUrl( "https://nolimitconnect.com/" ) );
}

//============================================================================
void AppletGetStarted::slotExpandButton()
{
    getBottomBarWidget()->slotExpandWindowButtonClicked();
}

//============================================================================
void AppletGetStarted::slotShrinkButton()
{
    getBottomBarWidget()->slotExpandWindowButtonClicked();
}

//============================================================================
void AppletGetStarted::slotJoinGroup()
{
    m_MyApp.getAppletMgr().launchApplet( eAppletGroupJoin, this );
}

//============================================================================
void AppletGetStarted::slotJoinChatRoom()
{
    m_MyApp.getAppletMgr().launchApplet( eAppletChatRoomJoin, this );
}

//============================================================================
void AppletGetStarted::slotJoinRandomConnect()
{
    m_MyApp.getAppletMgr().launchApplet( eAppletRandomConnectJoin, this );
}

//============================================================================
void AppletGetStarted::slotPersonalNotes()
{
    m_MyApp.getAppletMgr().launchApplet( eAppletPersonalRecorder, this );
}

//============================================================================
void AppletGetStarted::slotLibrary()
{
    m_MyApp.getAppletMgr().launchApplet( eAppletLibrary, this );
}

//============================================================================
void AppletGetStarted::slotIdentity()
{
    m_MyApp.getAppletMgr().launchApplet( eAppletEditAvatarImage, this );
}

//============================================================================
void AppletGetStarted::slotAboutMe()
{
    m_MyApp.getAppletMgr().launchApplet( eAppletEditAboutMe, this );
}

//============================================================================
void AppletGetStarted::slotStoryboard()
{
    m_MyApp.getAppletMgr().launchApplet( eAppletEditStoryboard, this );
}

//============================================================================
void AppletGetStarted::slotWebCam()
{
    m_MyApp.getAppletMgr().launchApplet( eAppletServiceShareWebCam, this );
}

