//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletPersonalRecorder.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletPersonalRecorder::AppletPersonalRecorder( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_PERSONAL_RECORDER, app, parent )
{
    setAppletType( eAppletPersonalRecorder );
    ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
    ui.m_SessionWidget->setAppModule( eAppModulePersonalNotes );

    setupMultiSessionActivity();
	m_MyApp.activityStateChange( this, true );
    m_MyApp.wantToGuiActivityCallbacks( this, true );
}

//============================================================================
AppletPersonalRecorder::~AppletPersonalRecorder()
{
    ui.m_SessionWidget->onActivityStop();
    m_MyApp.wantToGuiActivityCallbacks( this, false );
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletPersonalRecorder::setupMultiSessionActivity( void )
{
    ui.m_SessionWidget->setIsPersonalRecorder( true );
    ui.m_SessionWidget->setIdents( m_UserMgr.getMyIdent(), m_UserMgr.getMyIdent() );
    ui.m_SessionWidget->setEntryMode( eAssetTypeUnknown );
}

//============================================================================
void AppletPersonalRecorder::callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 )
{
    ui.m_SessionWidget->callbackGuiPlayMotionVideoFrame( feedOnlineId, vidFrame, motion0To100000 );
}

//============================================================================
void AppletPersonalRecorder::showEvent( QShowEvent* showEvent )
{
    AppletBase::showEvent( showEvent );
    //if( !m_HistoryQueried )
    //{
    //    m_HistoryQueried = true;
    //    ui.m_SessionWidget->initializeHistory();
    //}
}
