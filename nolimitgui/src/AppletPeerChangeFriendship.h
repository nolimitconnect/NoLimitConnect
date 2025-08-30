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
    class AppletPeerChangeFriendshipUi;
}
QT_END_NAMESPACE

class GuiUser;

class AppletPeerChangeFriendship : public AppletPeerBase
{
	Q_OBJECT
public:

	AppletPeerChangeFriendship( AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletPeerChangeFriendship() override = default;

	//=== helpers ===//
	//! set friend to change your permission to him   
	void						setFriend( GuiUser* poFriend );
	//! get current permission selection
	EFriendState				getPermissionSelection( void );
	//! set which radio button is checked
	void						setCheckedPermission( EFriendState eMyFriendshipToHim );

public slots:
	void						onOkButClick( void );
	void						onCancelButClick( void ); 
	void						onPermissionClick( void );
	void						onMakeFriendButClick( void );
	void						onIgnoreButClick( void );
	void						onPreferredButClick( void );

protected:
	void						setHisPermissionToMe( EFriendState hisFriendshipToMe );
	void						setMyPermissionToHim( EFriendState myFriendshipToHim );
	void						updatePreferredText( void );

	//=== vars ===//
	Ui::AppletPeerChangeFriendshipUi&	ui;
	GuiUser*					m_Friend{ nullptr };
	bool						m_PreferredUser{ false };
};