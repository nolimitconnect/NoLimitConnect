//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletHelpNetSignalBars.h"

#include "AppCommon.h"
#include "GuiParams.h"

#include <GuiInterface/IAudioDefs.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

#include <QClipboard>

#include "ui_AppletHelpNetSignalBars.h"

//============================================================================
AppletHelpNetSignalBars::AppletHelpNetSignalBars( AppCommon& app, QWidget* parent )
    : AppletBase( OBJNAME_APPLET_HELP_NET_SIGNAL_BARS, app, parent )
    , ui(*(new Ui::AppletHelpNetSignalBarsUi))
{
    ui.setupUi( getContentItemsFrame() );
    setAppletType( eAppletHelpNetSignalBars );
    setTitleBarText( DescribeApplet( m_EAppletType ) );
    connect( this, SIGNAL(signalBackButtonClicked()), this, SLOT( closeApplet() ) );

    ui.m_Pic_1->setFixedSize( GuiParams::getButtonSize( eButtonSizeMedium ) );
    ui.m_Pic_1->setResourceImage( ":/AppRes/Resources/bars_1_internet.png", true );
    ui.m_Pic_2->setFixedSize( GuiParams::getButtonSize( eButtonSizeMedium ) );
    ui.m_Pic_2->setResourceImage( ":/AppRes/Resources/bars_2_test_avail.png", true );
    ui.m_Pic_3->setFixedSize( GuiParams::getButtonSize( eButtonSizeMedium ) );
    ui.m_Pic_3->setResourceImage( ":/AppRes/Resources/bars_3_net_avail.png", true );
    ui.m_Pic_4->setFixedSize( GuiParams::getButtonSize( eButtonSizeMedium ) );
    ui.m_Pic_4->setResourceImage( ":/AppRes/Resources/bars_4_relay_search.png", true );
    ui.m_Pic_5->setFixedSize( GuiParams::getButtonSize( eButtonSizeMedium ) );
    ui.m_Pic_5->setResourceImage( ":/AppRes/Resources/bars_5_onlie_with_relay.png", true );
    ui.m_Pic_6->setFixedSize( GuiParams::getButtonSize( eButtonSizeMedium ) );
    ui.m_Pic_6->setResourceImage( ":/AppRes/Resources/bars_6_direct.png", true );
    ui.m_Pic_7->setFixedSize( GuiParams::getButtonSize( eButtonSizeMedium ) );
    ui.m_Pic_7->setResourceImage( ":/AppRes/Resources/bars_7_hosted_direct.png", true );
    ui.m_Pic_8->setFixedSize( GuiParams::getButtonSize( eButtonSizeMedium ) );
    ui.m_Pic_8->setResourceImage( ":/AppRes/Resources/bars_7_hosted_with_relay.png", true );

    connectBarWidgets();

    m_MyApp.activityStateChange( this, true );

}

//============================================================================
AppletHelpNetSignalBars::~AppletHelpNetSignalBars()
{
    m_MyApp.activityStateChange( this, false );
}
