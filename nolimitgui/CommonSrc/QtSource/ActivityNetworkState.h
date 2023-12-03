#pragma once
//============================================================================
// Copyright (C) 2016 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityBase.h"
#include <QDialog>
#include "ui_ActivityNetworkStatus.h"

#include <CoreLib/VxDefs.h>
#include <GuiInterface/IToGui.h>

class P2PEngine;

class ActivityNetworkState : public ActivityBase
{
	Q_OBJECT
public:
	ActivityNetworkState( AppCommon& app, QWidget* parent = nullptr );
    virtual ~ActivityNetworkState() override = default;

    // overrides required for dialogs with there own title bar and bottom bar widgets
    virtual TitleBarWidget *	getTitleBarWidget( void ) override { return ui.m_TitleBarWidget; }
    virtual BottomBarWidget *	getBottomBarWidget( void ) override { return ui.m_BottomBarWidget; }

protected slots:
	void						gotoWebsite( void );
	void						slotNetworkStateChanged( ENetworkStateType eNetworkState );

protected:
	void						setupActivityNetworkState( void );

	void						showNoInternetHelp( void );
	void						showDiscoverNetworkHelp( void );
	void						showWaitForRelayHelp( void );
	void						showDirectConnectMessage( void );
	void						showRelayConnectedHelp( void );
    void						showHostNetworkError( void );
    void						showHostGroupError( void );
    void						showHostGroupListError( void );

	void						clearHelpLines( void );
	void						setHelpLine( int lineIdx, QString helpText );

	//=== vars ===//
	Ui::ActivityNetworkStatusClass	ui;

};
