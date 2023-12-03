//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <app_precompiled_hdr.h>
#include "AppCommon.h"
#include "ActivityVideoOptions.h"
#include "AppGlobals.h"

#include <CoreLib/VxGlobals.h>

#include <QMessageBox>

//============================================================================
ActivityVideoOptions::ActivityVideoOptions( AppCommon&			app,
											VxNetIdent *			netIdent, 
											bool					bIsMyself,
											QWidget *				parent )
: AppletPeerBase( OBJNAME_ACTIVITY_VIDEO_OPTIONS, app, ePluginTypeCamServer, netIdent, 0 )
, m_bIsMyself( bIsMyself )
{
	ui.setupUi( this );
	ui.m_RotateButton->setIcons( eMyIconRefreshNormal );
    connectBarWidgets();

	//ui.m_VideoWidget->setRecordFileName( "F:/Junk.dat" );
	//ui.m_VideoWidget->setRecordFileName( "F:/Junk.avi" );

	connect( this, SIGNAL(finished(int)), this, SLOT(onStopButClick()));
	connect( ui.m_RotateButton, SIGNAL(clicked()), this, SLOT(onRotateButtonClick()));

	if( m_bIsMyself )
	{
		ui.m_VideoWidget->setVideoFeedId( m_MyApp.getAppGlobals().getUserIdent()->getMyOnlineId() );
	}
	else
	{
		ui.m_VideoWidget->setVideoFeedId( netIdent->getMyOnlineId() );
	}


    m_FromGui.fromGuiStartPluginSession( ePluginTypeCamServer, m_HisIdent->getMyOnlineId(), 0 );
}

//============================================================================
void ActivityVideoOptions::onStopButClick( void )
{
    m_FromGui.fromGuiStopPluginSession( ePluginTypeCamServer, m_HisIdent->getMyOnlineId(), 0 );
	if( m_bIsMyself )
	{
		m_Engine.setHasSharedWebCam(false);
	}

	emit signalWebCamClientDlgClosed( m_HisIdent );
	close();
}

//============================================================================
void ActivityVideoOptions::onRotateButtonClick( void )
{
	uint32_t rot = m_MyApp.getCamCaptureRotation() + 90;
	while( rot > 360 )
	{
		rot -= 360;
	}

	m_MyApp.setCamCaptureRotation( rot );
}


//============================================================================
void ActivityVideoOptions::playVideoFrame( VxGUID& feedId, unsigned char * pu8Jpg, unsigned long u32JpgLen, int motion0To100000 )
{
	ui.m_VideoWidget->playVideoFrame( feedId, pu8Jpg, u32JpgLen, motion0To100000 );
}

//============================================================================
void ActivityVideoOptions::webCamSourceOffline()
{
}
