//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletSettingsPage.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "VxTilePositioner.h"
#include "AppletLaunchWidget.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include <QFrame>

//============================================================================
AppletSettingsPage::AppletSettingsPage( AppCommon& app, QWidget* parent )
: AppletLaunchPage(  app, parent, eAppletSettingsPage, OBJNAME_APPLET_SETTINGS_PAGE )
{
    setAppletType( eAppletSettingsPage );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
    setupAppletSettingsgPage();

    setBackButtonVisibility( true );
    setPowerButtonVisibility( false );
    setExpandWindowVisibility( true );

    slotRepositionToParent();
}

//============================================================================
void AppletSettingsPage::setupAppletSettingsgPage( void )
{
    if( ! m_IsInitialized )
    {
        m_AppletList.clear();
        // create launchers for the basic applets
        for( int i = int( eMaxBasicApplets + 1 ); i < eMaxSettingsApplets; i++ )
        {
            AppletLaunchWidget* applet = new AppletLaunchWidget( m_MyApp, (EApplet)i, this );
            m_AppletList.emplace_back( applet );
        }

        m_IsInitialized = true;
    }
}

//============================================================================
void AppletSettingsPage::resizeEvent( QResizeEvent* ev )
{
    ActivityBase::resizeEvent( ev );
    VxTilePositioner::repositionTiles( m_AppletList, getContentItemsFrame(), 2 );
}

//============================================================================
void AppletSettingsPage::showEvent( QShowEvent* showEvent )
{
    AppletLaunchPage::showEvent( showEvent );
    VxTilePositioner::repositionTiles( m_AppletList, getContentItemsFrame(), 2 );
}
