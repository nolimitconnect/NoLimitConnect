//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletNetHostingPage.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "VxTilePositioner.h"
#include "MyIcons.h"
#include "AppletLaunchWidget.h"

#include <CoreLib/VxDebug.h>

//============================================================================
AppletNetHostingPage::AppletNetHostingPage( AppCommon& app, QWidget* parent )
: AppletLaunchPage( app, parent, eAppletNetHostingPage, OBJNAME_APPLET_NET_HOSTING_PAGE )
, m_IsInitialized( false )
{
    setAppletType( eAppletNetHostingPage );
    setTitleBarText( DescribeApplet( m_EAppletType ) );
	setupAppletNetHostingPage();
	setHomeButtonVisibility( true );
	setBackButtonVisibility( true );
	setPowerButtonVisibility( false );
	setExpandWindowVisibility( true );
}

//============================================================================
void AppletNetHostingPage::setupAppletNetHostingPage( void )
{
	if( ! m_IsInitialized )
    {
        m_AppletList.clear();
        // create launchers for the hosting applets
        for( int i = int( eMaxSearchApplets + 1 ); i < eMaxHostApplets; i++ )
        {

            // do not include hidden or secondary service hosting plugins
            if( ( eAppletServiceHostNetwork != i ) &&
                ( eAppletServiceConnectionTest != i ) )
            {
                AppletLaunchWidget * applet = new AppletLaunchWidget( m_MyApp, ( EApplet )i, this );
                    m_AppletList.push_back( applet );
            }
        }

        m_IsInitialized = true;
    }
}

//============================================================================
void AppletNetHostingPage::resizeEvent( QResizeEvent* ev )
{
	ActivityBase::resizeEvent( ev );
	getMyApp().getTilePositioner().repositionTiles( m_AppletList, getContentItemsFrame(), 2 );
}

//============================================================================
void AppletNetHostingPage::showEvent( QShowEvent* showEvent )
{
    AppletLaunchPage::showEvent( showEvent );
    getMyApp().getTilePositioner().repositionTiles( m_AppletList, getContentItemsFrame(), 2 );
}
