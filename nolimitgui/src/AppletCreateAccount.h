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

#include "AppletBase.h"

#include <PktLib/VxCommon.h>

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
    class CreateAccountUi;
}
QT_END_NAMESPACE

class P2PEngine;

class AppletCreateAccount : public AppletBase
{
	Q_OBJECT
public:
    AppletCreateAccount( AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletCreateAccount() override = default;

	void						setRootUserDataDirectory( std::string userDir ) { m_strRootUserDataDir = userDir; };
	//! validate user input
	bool						accountValidate( void );
    bool						wasLoginNameEntered( void );

signals:
	void						signalAccountCreated( bool wasCreated );

private slots:
	//! login was clicked
	void						slotButtonLoginClicked( void );

protected:
	virtual void				onBackButtonClicked( void ) override;

	//=== vars ===//
	Ui::CreateAccountUi&		ui;
	VxNetIdent					m_UserAccount;
	std::string					m_strRootUserDataDir;
};


