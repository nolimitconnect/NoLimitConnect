//============================================================================
// Copyright (C) 2017 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletLaunchPage.h"

#include "AppCommon.h"
#include "AppletLaunchWidget.h"
#include "AppSettings.h"
#include "TitleBarWidget.h"
#include "VxTilePositioner.h"
#include "WaitingSpinnerWidget.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletLaunchPage::AppletLaunchPage( AppCommon& app, QWidget* parent, EApplet eAppletType, const char* objName )
: ActivityBase( objName, app, parent, eAppletType )
, m_IsInitialized( false )
{
    if( !( ( eAppletNetHostingPage == eAppletType ) 
           || ( eAppletSearchPage == eAppletType )
           || ( eAppletShareServicesPage == eAppletType )
           || ( eAppletSettingsPage == eAppletType ) ) )
    {
        setTitleBarText( QObject::tr( "Home" ) );
        setupAppletLaunchPage();
        setBackButtonVisibility( false );
        setPowerButtonVisibility( true );
        setExpandWindowVisibility( true );
        connect( this, SIGNAL(signalPowerButtonClicked()), this, SLOT(slotPowerButtonClicked()) );
        getTitleBarWidget()->updateTitleBar();
    }
}

//============================================================================
void AppletLaunchPage::slotPowerButtonClicked( void )
{
	if( m_MyApp.confirmAppShutdown( this ) )
	{
		m_MyApp.shutdownAppCommon();
	}
}

//============================================================================
void AppletLaunchPage::setupAppletLaunchPage( void )
{
	if( !m_IsInitialized )
    {
        // create launchers for the basic applets
        for( int i = int( eAppletHomePage + 1 ); i < eMaxBasicApplets; i++ )
        {
            AppletLaunchWidget* applet = new AppletLaunchWidget( m_MyApp, (EApplet)i, this );
            m_AppletList.emplace_back( applet );
        }

        m_IsInitialized = true;
    }
}

//============================================================================
void AppletLaunchPage::resizeEvent( QResizeEvent* ev )
{
    ActivityBase::resizeEvent( ev );
	VxTilePositioner::repositionTiles( m_AppletList, getContentItemsFrame(), 2 );
}

//============================================================================
void AppletLaunchPage::showEvent( QShowEvent* showEvent )
{
    ActivityBase::showEvent( showEvent );
    static bool firstShowHomePage{ true };
    if( firstShowHomePage )
    {    
        if( eAppletHomePage == getAppletType() )
        {
            firstShowHomePage = false;
            startSpinner();
        }        
    }
}

//============================================================================
void AppletLaunchPage::startSpinner( void )
{
    if( !m_BusySpinner )
    {
        m_BusySpinner = new WaitingSpinnerWidget( this );
        m_BusySpinner->startWaiting(  m_MyApp.getAppTheme().getNotifyColor( eNotifyOnline ) );
    }
}

//============================================================================
void AppletLaunchPage::stopSpinner( void )
{
    if( m_BusySpinner )
    {
        m_BusySpinner->stopWaiting();
	    m_BusySpinner->close();
	    m_BusySpinner = nullptr;
    }
}