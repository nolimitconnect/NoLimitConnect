//============================================================================
// Copyright (C) 2017 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletLaunchWidget.h"
#include "AppletLaunchPage.h"
#include "AppCommon.h"

//============================================================================
AppletLaunchWidget::AppletLaunchWidget( AppCommon& myApp, EApplet appletType, AppletLaunchPage * parent )
: VxWidgetBase( parent->getContentItemsFrame() )
, m_MyApp( myApp )
, m_AppletType( appletType )
, m_AppletLaunchPage( parent )
{
	ui.setupUi( this );

	ui.m_LaunchButton->setIcon( GetAppletIcon( m_AppletType ) );
	ui.m_LauchLabel->setText( DescribeApplet( m_AppletType ) );

	connect( ui.m_LaunchButton, SIGNAL( clicked() ), this, SLOT( slotAppletIconPressed() ) );
    connect( ui.m_LauchLabel, SIGNAL( clicked() ), this, SLOT( slotAppletIconPressed() ) );
}

//============================================================================
VxPushButton * AppletLaunchWidget::getButton( void )
{
	return ui.m_LaunchButton;
}

//============================================================================
void AppletLaunchWidget::slotAppletIconPressed( void )
{
    if( m_MyApp.getIsAppInitialized()
        || ( ( eAppletPlayerNlc == m_AppletType ) && m_MyApp.getLoginCompleted() )
        || ( eAppletCreateAccount == m_AppletType )
        || ( eAppletSearchPage == m_AppletType )
        || ( eAppletNetHostingPage == m_AppletType )
        || ( eAppletShareServicesPage == m_AppletType )
        || ( eAppletTheme == m_AppletType )
        || ( eAppletPlayerCamClip == m_AppletType )
        || ( eAppletPlayerPhoto == m_AppletType )
        || ( eAppletPlayerVideo == m_AppletType )
        || ( eAppletPlayerNlc == m_AppletType )      
        )
    {
        if( eAppletUserConnections == m_AppletType )
        {
             m_MyApp.launchApplet( m_AppletType, m_AppletLaunchPage->getParentPageFrame() );
        }
        else
        {
            m_MyApp.launchApplet( m_AppletType, m_MyApp.getAppletFrame( m_AppletType ) );
        }
    }
    else
    {
        QMessageBox::information( this, QObject::tr( "Application Not Ready" ), QObject::tr( "Cannot Launch Applet Until Application Has Initialized" ) );
    }
}

