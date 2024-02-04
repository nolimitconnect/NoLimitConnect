//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletEditStoryboard.h"

#include "AppletStoryboardClient.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "AppGlobals.h"
#include "AppCommon.h"
#include "AccountMgr.h"
#include "GuiHelpers.h"
#include "MyIcons.h"
#include "PermissionWidget.h"

#include <QMessageBox>
#include <QUuid>
#include <QFileDialog>

#include <GuiInterface/IFromGui.h>

#include <P2PEngine/EngineSettings.h>
#include <AssetMgr/AssetMgr.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#define SUPPORTED_IMAGE_FILES "Image files (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.xbm *.xpm)"

//============================================================================
AppletEditStoryboard::AppletEditStoryboard( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_EDIT_STORYBOARD, app, parent )
{
    setAppletType( eAppletEditStoryboard );
    ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
    ui.m_ViewStoryboardPageButton->setFixedSize( eButtonSizeMedium );
    ui.m_ViewStoryboardPageButton->setIcon( getMyIcons().getPluginIcon( ePluginTypeStoryboardServer ) );
    ui.m_PermissionWidget->setPluginType( ePluginTypeStoryboardServer );
    ui.m_PermissionWidget->getPluginSettingsButton()->setVisible( false );

    connect( ui.m_ViewStoryboardPageButton, SIGNAL(clicked()), this, SLOT( slotViewStoryboardButClick() ) );
    connect( ui.StoryEditWidget, SIGNAL( signalStoryBoardSavedModified() ), this, SLOT( slotStoryBoardSavedModified() ) );

    // get current working directory
    VxFileUtil::getCurrentWorkingDirectory( m_strSavedCwd );

    m_strStoryBoardDir = VxGetStoryBoardPageServerDirectory();
    VxFileUtil::setCurrentWorkingDirectory( m_strStoryBoardDir.c_str() );
    m_strStoryBoardFile = m_strStoryBoardDir + "story_board.htm";
    ui.StoryEditWidget->loadStoryBoardFile( m_strStoryBoardFile.c_str() );

	m_MyApp.activityStateChange( this, true );
}


//============================================================================
void AppletEditStoryboard::slotStoryBoardSavedModified( void )
{
    m_FromGui.fromGuiUserModifiedStoryboard();
}

//============================================================================
void AppletEditStoryboard::slotViewStoryboardButClick( void )
{
    AppletStoryboardClient* applet = dynamic_cast<AppletStoryboardClient*>(m_MyApp.launchApplet( eAppletStoryboardClient, getParentPageFrame() ));
    if( applet )
    {
        applet->setIdentity( m_MyApp.getUserMgr().getMyIdent() );
    }
}
