//============================================================================
// Copyright (C) 2020 Brett R. Jones
// Issued to MIT style license by Brett R. Jones in 2017
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletUserPreferences.h"

#include "ActivityInformation.h"
#include "AppGlobals.h"
#include "AppCommon.h"
#include "AppSettings.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/ObjectCommon.h>

//============================================================================
AppletUserPreferences::AppletUserPreferences( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_USER_PREFERENCES, app, parent )
{
	setAppletType( eAppletUserPreferences );
	ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    ui.m_SavePushButton->setVisible( false );
    ui.m_CancelPushButton->setVisible( false );

    ui.m_MaxMessageHistoryInfoButton->setFixedSize( eButtonSizeTiny );
    ui.m_MaxMessageHistoryInfoButton->setIcon( eMyIconInformation );

    ui.m_MaxMessageHistorySpinBox->setValue( VxGetMaxMessageHistory() );

    connect( ui.m_MaxMessageHistoryInfoButton, SIGNAL(clicked()), this, SIGNAL(slotMaxMsgHistoryInfoButtonClicked()) );
    connect( ui.m_MaxMessageHistorySpinBox, SIGNAL(valueChanged(int)), this, SIGNAL(slotMaxMsgHistoryValueChanged(int)) );

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletUserPreferences::~AppletUserPreferences()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletUserPreferences::showEvent( QShowEvent* ev )
{
    AppletBase::showEvent( ev );
    updateDlgFromSettings();
}

//============================================================================
void AppletUserPreferences::hideEvent( QHideEvent* ev )
{
    AppletBase::hideEvent( ev );
    updateSettingsFromDlg();
}

//============================================================================
void AppletUserPreferences::updateDlgFromSettings()
{
    ui.m_MilitaryTimeCheckBox->setChecked( m_MyApp.getAppSettings().getUseMilitaryTime() );
}

//============================================================================
void AppletUserPreferences::updateSettingsFromDlg()
{
    m_MyApp.getAppSettings().setUseMilitaryTime( ui.m_MilitaryTimeCheckBox->isChecked() );
    SetUseMilitaryTime( ui.m_MilitaryTimeCheckBox->isChecked() );
}

//============================================================================
void AppletUserPreferences::slotSaveSettings( void )
{
    updateSettingsFromDlg();
    m_MyApp.activityStateChange( this, false );
    closeApplet();
}

//============================================================================
void AppletUserPreferences::slotCancel( void )
{
    m_MyApp.activityStateChange( this, false );
    closeApplet();
}

//============================================================================
void AppletUserPreferences::slotMaxMsgHistoryInfoButtonClicked( void )
{
    ActivityInformation* activityInfo = new ActivityInformation( m_MyApp, this, eInfoTypeMaxMessageHistory );
    if( activityInfo )
    {
        activityInfo->show();
    }
}

//============================================================================
void AppletUserPreferences::slotMaxMsgHistoryValueChanged( int maxHistory )
{
    m_MyApp.getAppSettings().setMaxMessageHistory( maxHistory );
    VxSetMaxMessageHistory( maxHistory );
}