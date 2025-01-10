//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletSearchPage.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "VxTilePositioner.h"
#include "AppletLaunchWidget.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include <QFrame>

//============================================================================
AppletSearchPage::AppletSearchPage( AppCommon& app, QWidget* parent )
: AppletLaunchPage( app, parent, eAppletSearchPage, OBJNAME_APPLET_SEARCH_PAGE )
{
    setAppletType( eAppletSearchPage );
    setTitleBarText( DescribeApplet( m_EAppletType ) );
	setupAppletSearchPage();
	setHomeButtonVisibility( false );
	setBackButtonVisibility( true );
	setPowerButtonVisibility( false );
	setExpandWindowVisibility( true );

    slotRepositionToParent();
}

//============================================================================
void AppletSearchPage::slotPowerButtonClicked( void )
{
	if( m_MyApp.confirmAppShutdown( this ) )
	{
		m_MyApp.shutdownAppCommon();
	}
}

//============================================================================
void AppletSearchPage::setupAppletSearchPage( void )
{
	if( ! m_IsInitialized )
    {
        m_AppletList.clear();
        // create launchers for the basic applets
        for( int i = int( eMaxSettingsApplets + 1 ); i < eMaxSearchApplets; i++ )
        {
            AppletLaunchWidget* applet = new AppletLaunchWidget( m_MyApp, (EApplet)i, this );
            m_AppletList.emplace_back( applet );
        }

        m_IsInitialized = true;
    }
}

//============================================================================
void AppletSearchPage::resizeEvent( QResizeEvent* ev )
{
	ActivityBase::resizeEvent( ev );
	//LogMsg( LOG_DEBUG, "AppletSearchPage::resizeEvent total height %d contentsFrame height %d\n", this->height(), getContentItemsFrame()->height() );
	VxTilePositioner::repositionTiles( m_AppletList, getContentItemsFrame(), 2 );
}

//============================================================================
void AppletSearchPage::showEvent( QShowEvent* showEvent )
{
    AppletLaunchPage::showEvent( showEvent );
    VxTilePositioner::repositionTiles( m_AppletList, getContentItemsFrame(), 2 );
}
