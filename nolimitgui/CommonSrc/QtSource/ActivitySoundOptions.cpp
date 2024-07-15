//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivitySoundOptions.h"
#include "AppGlobals.h"
#include "AppCommon.h"
#include "AppSettings.h"

#include <P2PEngine/EngineSettings.h>

#include <CoreLib/ObjectCommonDefs.h>

#include <QFileDialog>
#ifdef _MSC_VER
# pragma warning(disable: 4003) //not enough actual parameters for macro 'mkdir'
#endif

#include "ui_ActivitySoundOptions.h"

TitleBarWidget *	ActivitySoundOptions::getTitleBarWidget( void ) { return ui.m_TitleBarWidget; }
BottomBarWidget *	ActivitySoundOptions::getBottomBarWidget( void ) { return ui.m_BottomBarWidget; }

//============================================================================
ActivitySoundOptions::	ActivitySoundOptions(	AppCommon& app, 
												QWidget* parent )
: ActivityBase( OBJNAME_ACTIVITY_SOUND_OPTIONS, app, parent, eAppletMessengerFrame, true )
, ui(*(new Ui::SoundOptionsDialog))
{
	ui.setupUi(this);
	ui.m_TitleBarWidget->setTitleBarText( QObject::tr("Sound Options"));

    connectBarWidgets();

	connect( ui.AcceptButton, SIGNAL(clicked()), this, SLOT(onApplyButClick()) );
	connect( ui.CancelButton, SIGNAL(clicked()), this, SLOT(onCancelButClick()) );
	connect( ui.m_TitleBarWidget, SIGNAL(signalBackButtonClicked()), this, SLOT(onCancelButClick()) );

	bool bMutePhoneRing = m_MyApp.getAppSettings().getMutePhoneRing();
	bool bMuteNotifySound = m_MyApp.getAppSettings().getMuteNotifySound();

	ui.MutePhoneRingCheckBox->setCheckState( bMutePhoneRing ? Qt::Checked : Qt::Unchecked );
	ui.MuteNotifySouncCheckBox->setCheckState( bMuteNotifySound ? Qt::Checked : Qt::Unchecked );
}

//============================================================================  
void ActivitySoundOptions::onApplyButClick()
{
	bool bMutePhoneRing = ui.MutePhoneRingCheckBox->isChecked();
	bool bMuteNotifySound = ui.MuteNotifySouncCheckBox->isChecked();

	m_MyApp.getAppSettings().setMutePhoneRing( bMutePhoneRing );
	m_MyApp.getAppSettings().setMuteNotifySound( bMuteNotifySound );

	m_MyApp.applySoundSettings();

	accept();
}

//============================================================================   
void ActivitySoundOptions::onCancelButClick()
{
	reject();
}

