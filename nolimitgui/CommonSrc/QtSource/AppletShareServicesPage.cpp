//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletShareServicesPage.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "VxTilePositioner.h"
#include "AppletLaunchWidget.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include <QFrame>

//============================================================================
AppletShareServicesPage::AppletShareServicesPage( AppCommon& app, QWidget* parent )
: AppletLaunchPage( app, parent, eAppletShareServicesPage, OBJNAME_APPLET_SERVICE_SHARE_PAGE )
, m_IsInitialized( false )
{
    setAppletType( eAppletShareServicesPage );
    setTitleBarText( DescribeApplet( m_EAppletType ) );
	setupAppletShareServicesPage();
	setHomeButtonVisibility( true );
	setBackButtonVisibility( true );
	setPowerButtonVisibility( false );
	setExpandWindowVisibility( true );

    slotRepositionToParent();
 //   connectSignals();

    // save original values so can restore them if need be
 //   m_Engine.getEngineSettings().getNetworkHostUrl( m_OrigHostUrl );
    //m_Engine.getEngineSettings().getNetworkKey( m_OrigNetworkName );
    //m_Engine.getEngineSettings().getConnectTestUrl( m_OrigConnectionTestUrl );

    //updateDlgFromSettings();

}

//============================================================================
void AppletShareServicesPage::slotPowerButtonClicked( void )
{
	if( m_MyApp.confirmAppShutdown( this ) )
	{
		m_MyApp.shutdownAppCommon();
	}
}

//============================================================================
void AppletShareServicesPage::setupAppletShareServicesPage( void )
{
	if( ! m_IsInitialized )
    {
        m_AppletList.clear();
        // create launchers for the basic applets
        for( int i = int( eMaxHostApplets + 1 ); i < eMaxSharedServicesApplets; i++ )
        {
            AppletLaunchWidget* applet = new AppletLaunchWidget( m_MyApp, (EApplet)i, this );
            m_AppletList.emplace_back( applet );
        }

        m_IsInitialized = true;
    }
}

//============================================================================
void AppletShareServicesPage::resizeEvent( QResizeEvent* ev )
{
	ActivityBase::resizeEvent( ev );
	//LogMsg( LOG_DEBUG, "AppletShareServicesPage::resizeEvent total height %d contentsFrame height %d\n", this->height(), getContentItemsFrame()->height() );
	VxTilePositioner::repositionTiles( m_AppletList, getContentItemsFrame(), 2 );
}

//============================================================================
void AppletShareServicesPage::showEvent( QShowEvent* showEvent )
{
    AppletLaunchPage::showEvent( showEvent );
    VxTilePositioner::repositionTiles( m_AppletList, getContentItemsFrame(), 2 );
}
