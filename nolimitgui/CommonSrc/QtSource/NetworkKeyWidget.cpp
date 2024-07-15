//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "NetworkKeyWidget.h"
#include "AppCommon.h"
#include "AppGlobals.h"
#include "ActivityInformation.h"

#include <P2PEngine/P2PEngine.h>

#include "ui_NetworkKeyWidget.h"

QString						NetworkKeyWidget::getNetworkKey( void ) { return ui.m_NetworkKeyEdit->text();  }

//============================================================================
NetworkKeyWidget::NetworkKeyWidget( QWidget* parent )
	: QWidget( parent )
    , ui(*(new Ui::NetworkKeyWidgetUi))
    , m_MyApp( GetAppInstance() )
{
	initNetworkKeyWidget();
}

//============================================================================
void NetworkKeyWidget::initNetworkKeyWidget( void )
{
	ui.setupUi( this );
    ui.m_NetworkKeyInfoButton->setIcon( eMyIconInformation );
    ui.m_NetworkKeyButton->setIcon( eMyIconNetworkKey );
    fillNetworkKeyEditField();

    connect( ui.m_NetworkKeyInfoButton, SIGNAL(clicked()), this, SLOT( slotShowNetworkKeyInformation() ) );
    connect( ui.m_NetworkKeyButton, SIGNAL(clicked()), this, SLOT( slotShowNetworkKeyInformation() ) );
    connect( ui.m_ApplyKeyButton, SIGNAL(clicked()), this, SLOT( slotApplyNetworkKey() ) );
}
//============================================================================
void NetworkKeyWidget::fillNetworkKeyEditField( void )
{
    std::string networkKey;
    m_MyApp.getEngine().getEngineSettings().getNetworkKey( networkKey );
    ui.m_NetworkKeyEdit->setText( networkKey.c_str() );
}

//============================================================================
void NetworkKeyWidget::slotShowNetworkKeyInformation()
{
    ActivityInformation * activityInfo = new ActivityInformation( m_MyApp, this, eInfoTypeNetworkKey );
    activityInfo->show();
}

//============================================================================
void NetworkKeyWidget::slotApplyNetworkKey()
{
    QString keyVal = getNetworkKey();
    if( verifyNetworkKey( keyVal ) )
    {
        std::string keyString = keyVal.toUtf8().constData();
        m_MyApp.getEngine().getEngineSettings().setNetworkKey( keyString );
        QMessageBox::warning( this, QObject::tr( "Network Key" ), QObject::tr( "You may need to restart application to avoid connection problems." ) );
    }
}

//============================================================================
bool NetworkKeyWidget::verifyNetworkKey( QString& keyVal )
{
    bool isValid = true;
    if( keyVal.size() < 6 )
    {
        isValid = false;
        QMessageBox::warning( this, QObject::tr( "Network Key" ), QObject::tr( "Network Key must be at least 6 characters ( 8 or more characters recommended )." ) );
    }

    return isValid;
}