#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletPeerBase.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletChooseHostAdminOrSearchUi;
}
QT_END_NAMESPACE

class GuiUser;

class AppletChooseHostAdminOrSearch : public AppletBase
{
	Q_OBJECT
public:

	AppletChooseHostAdminOrSearch( AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletChooseHostAdminOrSearch() override = default;

	bool						getToAdminChoice( void ) { return m_ToAdmin; }

public slots:
	void						onOkButClick( void );
	void						onCancelButClick( void ); 
	void						onChooseButClick( void );

protected:
	void						updateChoiceSelection( void );

	//=== vars ===//
	Ui::AppletChooseHostAdminOrSearchUi&	ui;
	bool						m_ToAdmin{ true };
};