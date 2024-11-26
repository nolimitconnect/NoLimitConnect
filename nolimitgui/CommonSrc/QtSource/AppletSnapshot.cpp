//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//



//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletSnapshot.h"

#include "AppCommon.h"	
#include "AppSettings.h"
#include "AppletPopupMenu.h"
#include "AppGlobals.h"
#include "FileItemInfo.h"
#include "FileActionMenu.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>
#include <AssetMgr/AssetMgr.h>

#include <PktLib/VxSearchDefs.h>
#include <NetLib/VxFileXferInfo.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxFileInfo.h>
#include <CoreLib/VxGlobals.h>
#include <VxVideoLib/VxVideoLib.h>

#include <QResizeEvent>
#include <QMessageBox>
#include <QTimer>

#include "ui_AppletSnapshot.h"

//============================================================================
AppletSnapshot::AppletSnapshot(	AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_SNAPSHOT, app, parent )
, ui(*(new Ui::AppletSnapshotUi))
, m_CloseDlgTimer( new QTimer( this ) )
{
    m_MediaSessionId.initializeWithNewVxGUID();

    setAppletType( eAppletSnapshot );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    ui.m_ImageScreen->setFixedSize( GuiParams::getSnapshotDesiredSize() );
    ui.m_SnapshotScreen->setFixedSize( GuiParams::getSnapshotDesiredSize() );

    ui.m_CamStopStartButton->setVisible( false );
    ui.m_CamStopStartButton->setFixedSize( eButtonSizeSmall );

    ui.m_CamFrontBackButton->setFixedSize( eButtonSizeSmall );
    ui.m_CamFrontBackButton->setIcon( eMyIconSelectCameraNormal );

    connect( ui.snapshotButton, SIGNAL(clicked()), this, SLOT( onSnapShotButClick() ) );
    connect( ui.m_DoneButton, SIGNAL(clicked()), this, SLOT( onDoneButClick() ) );
    connect( ui.m_CamFrontBackButton, SIGNAL(clicked()), this, SLOT( onCamFrontBackButClick() ) );
    connect( &m_MyApp.getCamLogic(), SIGNAL( signalCameraDescription(QString) ), this, SLOT( slotCameraDescription( QString ) ) );

    ui.m_CamNameLabel->setText( m_MyApp.getCamLogic().getCamDescription() );

    if( m_MyApp.getCamLogic().isCamAvailable() )
    {
        m_MyApp.getEngine().fromGuiWantMediaInput( m_MyApp.getMyOnlineId(), eMediaInputVideoJpgSmall, this, eAppModuleSnapshot, m_MediaSessionId, true );
    }
    else
    {
        QMessageBox::warning( this, QObject::tr( "Camera Capture" ), QObject::tr( "No Camera Source Available." ) );
        connect( m_CloseDlgTimer, SIGNAL( timeout() ), this, SLOT( onCancelButClick() ) );
        m_CloseDlgTimer->setSingleShot( true );
        m_CloseDlgTimer->start( 1000 );
    }

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletSnapshot::~AppletSnapshot()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
//! take picture for me
void AppletSnapshot::onSnapShotButClick( void )
{
    m_SnapShotPending = true;
}

//============================================================================
void AppletSnapshot::callbackVideoJpgSmall( VxGUID& vidFeedId, uint8_t * jpgData, uint32_t jpgDataLen, int motion0to100000 )
{
    if( jpgData && jpgDataLen && ( vidFeedId == m_MyApp.getMyOnlineId() ) )
    {
        QImage capBitmap;
        capBitmap.loadFromData( jpgData, jpgDataLen, "JPG" );

        VxLabel* camScreen = ui.m_ImageScreen;
        if( camScreen )
        {
            camScreen->playMotionVideoFrame( capBitmap, motion0to100000 );
        }

        if( m_SnapShotPending )
        {
            m_ImageBitmap = capBitmap;
            emit signalSnapshotImage( m_ImageBitmap );

            m_SnapShotPending = false;
        }
    }
}

//============================================================================
//! Implement the OnClickListener callback    
void AppletSnapshot::onCancelButClick( void )
{
    onBackButtonClicked();
}

//============================================================================ 
void AppletSnapshot::onDoneButClick( void )
{
    onBackButtonClicked();
}

//============================================================================ 
void AppletSnapshot::onCamFrontBackButClick( void )
{
    m_MyApp.getCamLogic().nextCamera();
}

//============================================================================
void AppletSnapshot::onCloseEvent( void )
{
    m_MyApp.getEngine().fromGuiWantMediaInput( m_MyApp.getMyOnlineId(), eMediaInputVideoJpgSmall, this, eAppModuleSnapshot, m_MediaSessionId, false );
    AppletBase::onCloseEvent();
}

//============================================================================
void AppletSnapshot::slotCameraDescription( QString camDescription )
{
    ui.m_CamNameLabel->setText( camDescription );
}
