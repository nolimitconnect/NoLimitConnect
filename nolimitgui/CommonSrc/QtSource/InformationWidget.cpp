//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "InformationWidget.h"
#include "AppCommon.h"
#include "MyIcons.h"
#include "ActivityInformation.h"

//============================================================================
InformationWidget::InformationWidget( QWidget* parent )
	: QWidget( parent )
    , m_MyApp( GetAppInstance() )
{
	initInformationWidget();
}

//============================================================================
void InformationWidget::initInformationWidget( void )
{
	ui.setupUi( this );

    ui.m_InfoButton->setIcon( eMyIconInformation );

    connect( ui.m_ContextButton, SIGNAL(clicked()), this, SLOT( slotShowInformation() ) );
    connect( ui.m_InfoButton, SIGNAL(clicked()), this, SLOT( slotShowInformation() ) );
}

//============================================================================
void InformationWidget::updateInformation( void )
{
    if( m_PluginType == ePluginTypeInvalid )
    {
        return;
    }

    ui.m_ContextButton->setIcon( m_MyApp.getMyIcons().getPluginIcon( m_PluginType ) );
}

//============================================================================
void InformationWidget::slotShowInformation()
{
   if( m_PluginType == ePluginTypeInvalid )
   {
       return;
   }

   m_ActivityInfo = new ActivityInformation( m_MyApp, this, m_PluginType );
   m_ActivityInfo->show();
}