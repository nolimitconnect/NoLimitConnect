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
#include "ui_ActivityPermissions.h"

class ActivityPermissions : public ActivityBase
{
	Q_OBJECT
public:
	ActivityPermissions( AppCommon& app, QWidget* parent = nullptr );
	virtual ~ActivityPermissions() override = default;

    // overrides required for dialogs with there own title bar and bottom bar widgets
    virtual TitleBarWidget *	getTitleBarWidget( void ) override { return ui.m_TitleBarWidget; }
    virtual BottomBarWidget *	getBottomBarWidget( void ) override { return ui.m_BottomBarWidget; }

	//=== helpers ===//
	//! Set plugin to set permissions on   
    void						setPluginType( EPluginType ePluginType ) override;
	//! Set plugin icon based on permission level
	void						setPluginIcon( EPluginType ePluginType, EFriendState ePluginPermission );
	//! get current permission selection
	EFriendState				getPermissionSelection( void );
	//! set which radio button is checked
	void						setCheckedPermission( EFriendState ePluginPermission );

public slots:

	void						onOkButClick( void );
	void						onCancelButClick( void ); 
	void						onPermissionClick( void );

protected:
	//=== vars ===//
	Ui::PermissionsDialog		ui;
	EPluginType					m_ePluginType;					

};
