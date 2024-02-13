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
#include "AppSettings.h"
#include "VxTilePositioner.h"
#include "MyIcons.h"
#include "AppletLaunchWidget.h"

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
        setHomeButtonVisibility( false );
        setBackButtonVisibility( false );
        setPowerButtonVisibility( true );
        setExpandWindowVisibility( true );
        connect( this, SIGNAL( signalPowerButtonClicked() ), this, SLOT( slotPowerButtonClicked() ) );
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
	if( ! m_IsInitialized )
    {
        // create launchers for the basic applets
        for( int i = int( eAppletHomePage + 1 ); i < eMaxBasicApplets; i++ )
        {
            bool shouldShow = true;
            if( eAppletChatRoomJoin == i && !m_MyApp.getAppSettings().getFeatureEnable( eAppFeatureChatRoom ) )
            {
                shouldShow = false;
            }
            else if( eAppletRandomConnectJoin == i && !m_MyApp.getAppSettings().getFeatureEnable( eAppFeatureRandomConnect ) )
            {
                shouldShow = false;
            }

            if( shouldShow )
            {
                AppletLaunchWidget* applet = new AppletLaunchWidget( m_MyApp, (EApplet)i, this );
                m_AppletList.push_back( applet );
            }
        }

        m_IsInitialized = true;
    }
}

//============================================================================
void AppletLaunchPage::resizeEvent( QResizeEvent* ev )
{
    ActivityBase::resizeEvent( ev );
	//LogMsg( LOG_DEBUG, "AppletLaunchPage::resizeEvent total height %d contentsFrame height %d\n", this->height(), getContentItemsFrame()->height() );
	VxTilePositioner::repositionTiles( m_AppletList, getContentItemsFrame(), 2 );
}

//============================================================================
void AppletLaunchPage::showEvent( QShowEvent* showEvent )
{
    ActivityBase::showEvent( showEvent );
}
