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

#include "ActivityBase.h"

#include <PktLib/VxCommon.h>

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
    class CreateAccountClass;
}
QT_END_NAMESPACE

class P2PEngine;

class ActivityCreateAccount : public ActivityBase
{
	Q_OBJECT
public:
    ActivityCreateAccount( AppCommon& app, QWidget* parent = nullptr );
	virtual ~ActivityCreateAccount() override = default;

    // overrides required for dialogs with there own title bar and bottom bar widgets
	TitleBarWidget*				getTitleBarWidget( void ) override;
	BottomBarWidget*			getBottomBarWidget( void ) override;

	void						setRootUserDataDirectory( const char* userDir ) { m_strRootUserDataDir = userDir; };
	//! validate user input
	bool						accountValidate( void );
    bool						wasLoginNameEntered( void );

	virtual void				onBackButtonClicked( void ) override;

private slots:
	//! login was clicked
	void						slotButtonLoginClicked( void );

protected:
	//=== vars ===//
	Ui::CreateAccountClass&		ui;
	VxNetIdent					m_UserAccount;
	std::string					m_strRootUserDataDir;
};


