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

#include "ui_IdentWidget.h"
#include "IdentLogicInterface.h"

class IdentWidget :  public IdentLogicInterface
{
	Q_OBJECT

public:
	IdentWidget(QWidget* parent=nullptr);

	VxPushButton*				getIdentAvatarButton( void ) override;
	VxPushButton*				getIdentFriendshipButton( void ) override;
	VxPushButton*				getIdentPushToTalkButton( void ) override;
	VxPushButton*				getIdentOfferViewButton( void ) override;
	VxPushButton*				getIdentOfferAcceptButton( void ) override;
	VxPushButton*				getIdentOfferRejectButton( void ) override;
	VxPushButton*				getIdentMenuButton( void ) override;

	virtual QLabel*				getIdentLine1( void ) override;
	virtual QLabel*				getIdentLine2( void ) override;
	virtual QLabel*				getIdentLine3( void ) override;

protected:
	Ui::IdentWidget				ui;
};
