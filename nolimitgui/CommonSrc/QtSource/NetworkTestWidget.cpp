//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "NetworkTestWidget.h"
#include "AppCommon.h"
#include "ActivityChooseTestWebsiteUrl.h"

#include <P2PEngine/EngineSettings.h>
#include <CoreLib/VxSktUtil.h>
#include <P2PEngine/P2PEngine.h>
#include <CoreLib/VxFileUtil.h>
#include <CoreLib/VxParse.h>
#include <CoreLib/VxGlobals.h>

#include <QMessageBox>
#include <QTimer>

#include <stdarg.h>
#include <stdio.h>

#include "ui_NetworkTestWidget.h"

LogWidget*			NetworkTestWidget::getLogWidget( void ) { return ui.m_LogWidget; }

//============================================================================
NetworkTestWidget::NetworkTestWidget( QWidget* parent )
    : QWidget( parent )
    , ui(*(new Ui::NetworkTestWidget))
    , m_MyApp( GetAppInstance() )
    , m_Timer(new QTimer(this))
{
	m_Timer->setSingleShot(true);

	ui.setupUi(this);

    connect( &m_MyApp,
             SIGNAL( signalIsPortOpenStatus( EIsPortOpenStatus, QString ) ),
             this,
             SLOT( slotIsPortOpenStatus( EIsPortOpenStatus, QString ) ) );

    connect( ui.m_RunFullTestButton, SIGNAL(clicked()), this, SLOT( slotRunFullTestButClick() ) );
    connect( ui.m_IsMyPortOpenButton, SIGNAL(clicked()), this, SLOT( slotIsPortOpenButClick() ) );
}

//============================================================================
void NetworkTestWidget::slotRunFullTestButClick( void )
{
	bool isBusyWithTest = false;
	if( false == isBusyWithTest )
	{
		isBusyWithTest = true;
		getLogWidget()->clear();
		//m_MyApp.getEngine().getFromGuiInterface().fromGuiVerifyNetHostSettings();
		isBusyWithTest = false;
	}
}

//============================================================================
void NetworkTestWidget::slotIsPortOpenButClick( void )
{
    uint16_t tcpPort = m_MyApp.getEngine().getEngineSettings().getTcpIpPort();

    if( !tcpPort )
    {
        QMessageBox::information( this, QObject::tr( "Network Settings Error" ), QObject::tr( "TCP Port cannot be zero." ) );
    }
    else
    {
        getLogWidget()->clear();
        m_MyApp.getEngine().getFromGuiInterface().fromGuiRunIsPortOpenTest( tcpPort );
    }
}


//
////============================================================================
//void NetworkTestWidget::slotHostStatus( EHostTestStatus eHostStatus, QString strMsg )
//{
//	//strMsg.remove(QRegExp("[\\n\\r]"));
//	ui.m_LogEdit->append(strMsg);
//}
