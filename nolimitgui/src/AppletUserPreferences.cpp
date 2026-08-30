//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletUserPreferences.h"

#include "AppletInformation.h"
#include "AppGlobals.h"
#include "AppCommon.h"
#include "AppSettings.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>
#include <CoreLib/ObjectCommonDefs.h>

#include "ui_AppletUserPreferences.h"

#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>

//============================================================================
AppletUserPreferences::AppletUserPreferences( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_USER_PREFERENCES, app, parent )
, ui(*(new Ui::AppletUserPreferencesUi))
{
	setAppletType( eAppletUserPreferences );
	ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    ui.m_SavePushButton->setVisible( false );
    ui.m_CancelPushButton->setVisible( false );

    ui.m_MaxMessageHistoryInfoButton->setFixedSize( eButtonSizeTiny );
    ui.m_MaxMessageHistoryInfoButton->setIcon( eMyIconInformation );

    ui.m_MaxMessageHistorySpinBox->setValue( VxGetMaxMessageHistory() );

    connect( ui.m_MaxMessageHistoryInfoButton, SIGNAL(clicked()), this, SLOT(slotMaxMsgHistoryInfoButtonClicked()) );
    connect( ui.m_MaxMessageHistorySpinBox, SIGNAL(valueChanged(int)), this, SLOT(slotMaxMsgHistoryValueChanged(int)) );

    // not part of AppletUserPreferences.ui -- see header comment
    QHBoxLayout* purgeEventHistoryLayout = new QHBoxLayout();
    m_PurgeEventHistoryLabel = new QLabel( tr( "Purge event history:" ), getContentItemsFrame() );
    purgeEventHistoryLayout->addWidget( m_PurgeEventHistoryLabel );
    m_PurgeEventHistoryCombo = new QComboBox( getContentItemsFrame() );
    m_PurgeEventHistoryCombo->addItem( tr( DescribePurgeEventHistoryType( ePurgeEventHistoryUseHostDefault ) ), ePurgeEventHistoryUseHostDefault );
    m_PurgeEventHistoryCombo->addItem( tr( DescribePurgeEventHistoryType( ePurgeEventHistoryAfter2Min ) ), ePurgeEventHistoryAfter2Min );
    m_PurgeEventHistoryCombo->addItem( tr( DescribePurgeEventHistoryType( ePurgeEventHistoryAfter15Min ) ), ePurgeEventHistoryAfter15Min );
    m_PurgeEventHistoryCombo->addItem( tr( DescribePurgeEventHistoryType( ePurgeEventHistoryAfter1Hour ) ), ePurgeEventHistoryAfter1Hour );
    m_PurgeEventHistoryCombo->addItem( tr( DescribePurgeEventHistoryType( ePurgeEventHistoryAfter1Day ) ), ePurgeEventHistoryAfter1Day );
    m_PurgeEventHistoryCombo->addItem( tr( DescribePurgeEventHistoryType( ePurgeEventHistoryAfter1Week ) ), ePurgeEventHistoryAfter1Week );
    m_PurgeEventHistoryCombo->addItem( tr( DescribePurgeEventHistoryType( ePurgeEventHistoryAfter1Month ) ), ePurgeEventHistoryAfter1Month );
    m_PurgeEventHistoryCombo->addItem( tr( DescribePurgeEventHistoryType( ePurgeEventHistoryAfter1Year ) ), ePurgeEventHistoryAfter1Year );
    purgeEventHistoryLayout->addWidget( m_PurgeEventHistoryCombo );
    ui.verticalLayout->addLayout( purgeEventHistoryLayout );

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
    ui.m_UseSystemPlayerCheckBox->setChecked( m_MyApp.getAppSettings().getUseSystemMediaPlayer() );
    ui.m_UnattendedHostCheckBox->setChecked( m_MyApp.getAppSettings().getIsAutomatedHost() );
    ui.m_AllowJoinMultipleHostsCheckBox->setChecked( m_MyApp.getAppSettings().getAllowJoinMultipleHosts() );
    ui.m_MilitaryTimeCheckBox->setChecked( m_MyApp.getAppSettings().getUseMilitaryTime() );
    ui.m_DisableSoundEffectsCheckBox->setChecked( m_MyApp.getAppSettings().getDisableAllSoundEffects() );
    ui.m_SndDisableTrashCheckBox->setChecked( m_MyApp.getAppSettings().getDisableSndTrash() );
    ui.m_SndDisableButtonPressCheckBox->setChecked( m_MyApp.getAppSettings().getDisableSndKeyClick() );
    ui.m_SndDisableNotifyCheckBox->setChecked( m_MyApp.getAppSettings().getDisableSndNotify() );
    ui.m_SndDisableMessageRxCheckBox->setChecked( m_MyApp.getAppSettings().getDisableSndMsgRx() );

    EPurgeEventHistoryType purgeType = m_MyApp.getEngine().fromGuiGetPurgeEventHistoryType();
    int purgeComboIndex = m_PurgeEventHistoryCombo->findData( purgeType );
    m_PurgeEventHistoryCombo->setCurrentIndex( purgeComboIndex >= 0 ? purgeComboIndex : 0 );
}

//============================================================================
void AppletUserPreferences::updateSettingsFromDlg()
{
    bool unattendedHost = ui.m_UnattendedHostCheckBox->isChecked();

    m_MyApp.getAppSettings().setIsAutomatedHost( unattendedHost );
    m_MyApp.getAppSettings().setAllowJoinMultipleHosts( ui.m_AllowJoinMultipleHostsCheckBox->isChecked() );

    m_MyApp.getAppSettings().setUseSystemMediaPlayer( ui.m_UseSystemPlayerCheckBox->isChecked() );
    m_MyApp.getAppSettings().setUseMilitaryTime( ui.m_MilitaryTimeCheckBox->isChecked() );
    SetUseMilitaryTime( ui.m_MilitaryTimeCheckBox->isChecked() );

    m_MyApp.getAppSettings().setDisableAllSoundEffects( ui.m_DisableSoundEffectsCheckBox->isChecked() );
    m_MyApp.getAppSettings().setDisableSndTrash( ui.m_SndDisableTrashCheckBox->isChecked() );
    m_MyApp.getAppSettings().setDisableSndKeyClick( ui.m_SndDisableButtonPressCheckBox->isChecked() );
    m_MyApp.getAppSettings().setDisableSndNotify( ui.m_SndDisableNotifyCheckBox->isChecked() );
    m_MyApp.getAppSettings().setDisableSndMsgRx( ui.m_SndDisableMessageRxCheckBox->isChecked() );

    GuiUser* guiUser = m_MyApp.getUserMgr().getMyIdent();
    bool sendToEngine = guiUser && guiUser->isAutomatedHost() != unattendedHost;
    if( sendToEngine )
    {
        guiUser->setIsAutomatedHost( unattendedHost );
        m_MyApp.getEngine().fromGuiSetIsAutomatedHost( unattendedHost );
    }

    EPurgeEventHistoryType purgeType = ( EPurgeEventHistoryType )m_PurgeEventHistoryCombo->currentData().toInt();
    m_MyApp.getEngine().fromGuiSetPurgeEventHistoryType( purgeType );
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
    AppletInformation* activityInfo = new AppletInformation( m_MyApp, this, eInfoTypeMaxMessageHistory );
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