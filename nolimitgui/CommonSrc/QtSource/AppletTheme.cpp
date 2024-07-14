//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletTheme.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "HomeWindow.h"
#include "VxAppTheme.h"

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>

//============================================================================
AppletTheme::AppletTheme( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_THEME, app, parent )
, m_SavedTheme( eThemeTypeUnknown )
, m_CurTheme( eThemeTypeUnknown )
{
	setAppletType( eAppletTheme );
	ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	ui.m_ThemeExampleButton->setIcon( eMyIconPeople );
	fillThemeCombo();
    fillExampleCombo();
    ui.m_PlainTextEdit->appendPlainText( QObject::tr( "Some Text To Edit" ) );
	ui.horizontalSlider->setSingleStep( 1 );
	ui.horizontalSlider->setRange( 0, 100000 );
	ui.m_ExampleProgressBar->setRange( 0, 100000 );
	ui.m_VertProgressBar->setRange( 0, 100000 );

	connect( ui.m_AcceptCancelFrame, SIGNAL( signalAccepted() ), this, SLOT(slotThemeAccepted()) );
	connect( this, SIGNAL(signalBackButtonClicked()), this, SLOT(slotThemeCanceled()) );
	connect( ui.m_AcceptCancelFrame, SIGNAL( signalCanceled() ), this, SLOT(slotThemeCanceled()) );
    connect( ui.m_ExampleCheckBox, SIGNAL(stateChanged(int) ), this, SLOT(slotCheckBoxClicked(int)) );
	connect( ui.horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(slotSliderValueChanged(int)) );
	connect( ui.m_ThemeExampleButton, SIGNAL(clicked()), this, SLOT(slotExampleButtonClicked()) );
	
	m_MyApp.activityStateChange( this, true );
}

//============================================================================
void AppletTheme::resizeEvent( QResizeEvent* ev )
{
	ui.scrollAreaWidgetContents->setFixedWidth( ev->size().width() - ( 20 + getVertScrollBarWidth() ) );
	AppletBase::resizeEvent( ev );
}

//============================================================================
void AppletTheme::slotThemeAccepted( void )
{
	m_MyApp.getAppSettings().setLastSelectedTheme( m_CurTheme );
    getMyApp().getAppTheme().selectTheme( m_CurTheme, &getMyApp().getHomePage() );
    m_MyApp.activityStateChange( this, false );
	closeApplet();
}

//============================================================================
void AppletTheme::onBackButtonClicked( void )
{
    slotThemeCanceled();
}

//============================================================================
void AppletTheme::slotThemeCanceled( void )
{
	restoreSavedTheme();
    m_MyApp.activityStateChange( this, false );
	closeApplet();
}

//============================================================================
void AppletTheme::slotCheckBoxClicked( int checkedState )
{
    LogMsg( LOG_DEBUG,  "check box state %d", checkedState );
}

//============================================================================
void AppletTheme::slotSliderValueChanged( int sliderVal )
{
	ui.m_ExampleProgressBar->setValue( sliderVal );
	ui.m_VertProgressBar->setValue( sliderVal );
}

//============================================================================
void AppletTheme::fillThemeCombo( void )
{
	VxAppTheme& appTheme = getMyApp().getAppTheme();
	for( int i = (int)(eThemeTypeUnknown + 1); i < eMaxThemeType; ++i )
	{
		ui.m_ThemeComboBox->addItem( appTheme.describeTheme( ( EThemeType )i ) );
	}

	m_SavedTheme = getMyApp().getAppSettings().getLastSelectedTheme();
	m_CurTheme = m_SavedTheme;
	if( ( eThemeTypeUnknown < m_CurTheme )
		&& ( eMaxThemeType > m_CurTheme ) )
	{
		ui.m_ThemeComboBox->setCurrentIndex( (int)(m_CurTheme - 1) );
	}

	connect( ui.m_ThemeComboBox, SIGNAL( currentIndexChanged( int ) ), this, SLOT( slotThemeSelectionChanged( int ) ) );
}

//============================================================================
void AppletTheme::fillExampleCombo( void )
{
    ui.m_ExampleComboBox->addItem( QObject::tr( "Selection 1" ) );
    ui.m_ExampleComboBox->addItem( QObject::tr( "Selection 2" ) );
    ui.m_ExampleComboBox->addItem( QObject::tr( "Selection 3" ) );
    ui.m_ExampleComboBox->addItem( QObject::tr( "Selection 4" ) );
    ui.m_ExampleComboBox->addItem( QObject::tr( "Selection 5" ) );
    ui.m_ExampleComboBox->setCurrentIndex( 0 );
}

//============================================================================
void AppletTheme::slotThemeSelectionChanged( int idx )
{
	applyTheme( ( EThemeType )( idx + 1 ) );
}

//============================================================================
void AppletTheme::saveCurrentTheme( void )
{
	
}

//============================================================================
void AppletTheme::applyTheme( EThemeType themeType )
{
	m_CurTheme = themeType;
	getMyApp().getAppTheme().selectTheme( m_CurTheme, &getMyApp().getHomePage() );
}

//============================================================================
void AppletTheme::restoreSavedTheme( void )
{
	getMyApp().getAppTheme().selectTheme( m_SavedTheme, &getMyApp().getHomePage() );
}

//============================================================================
void AppletTheme::slotExampleButtonClicked( void )
{
	m_ExampleButtonNotifyType = (ENotifyType)(m_ExampleButtonNotifyType + 1);
	if( m_ExampleButtonNotifyType == eMaxNotifyType )
	{
		m_ExampleButtonNotifyType = eNotifyNone;
	}

	ui.m_ThemeExampleButton->setNotifyType( m_ExampleButtonNotifyType );
}

//============================================================================
int AppletTheme::getVertScrollBarWidth( void )
{
	if( ui.scrollArea->verticalScrollBar()->isVisible() )
	{
		return ui.scrollArea->verticalScrollBar()->width() + 5;
	}

	return 0;
}