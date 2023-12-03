#pragma once
//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityBase.h"
#include <QDialog>
#include "ui_NetworkTestWidget.h"

#include <CoreLib/VxDefs.h>
#include <GuiInterface/IToGui.h>

class AppCommon;
class P2PEngine;

class NetworkTestWidget : public QWidget
{
	Q_OBJECT
public:
	NetworkTestWidget( QWidget* parent = nullptr );
    virtual ~NetworkTestWidget() override = default;

protected slots:
	//void						gotoWebsite( void );
	//void						slotNetworkStateChanged( ENetworkStateType eNetworkState );
    void                        slotRunFullTestButClick( void );
    void                        slotIsPortOpenButClick( void );

protected:
    LogWidget*                  getLogWidget( void ) { return ui.m_LogWidget; }
	//void						setupNetworkTestWidget( void );

	//void						showNoInternetHelp( void );
	//void						showDiscoverNetworkHelp( void );
	//void						showRelaySearchHelp( void );
	//void						showDirectConnectMessage( void );
	//void						showRelayConnectedHelp( void );
	//void						showHostError( void );

	//void						clearHelpLines( void );
	//void						setHelpLine( int lineIdx, QString helpText );

	//=== vars ===//
	Ui::NetworkTestWidget	    ui;
    AppCommon&                  m_MyApp;
    QTimer *                    m_Timer;

};
