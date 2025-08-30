//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "IdentWidget.h"
#include "AppletPeerChangeFriendship.h"
#include "GuiHelpers.h"
#include "GuiUser.h"

#include "AppCommon.h"

#include "ui_IdentWidget.h"

//============================================================================
IdentWidget::IdentWidget(QWidget* parent)
: IdentLogicInterface(parent)
, ui(*(new Ui::IdentWidget))
{
	ui.setupUi(this);
	setupIdentLogic( eButtonSizeMedium );
}

//============================================================================
VxPushButton* IdentWidget::getIdentAvatarButton( void ) { return ui.m_AvatarButton; }
//============================================================================
VxPushButton* IdentWidget::getIdentFriendshipButton( void ) { return ui.m_FriendshipButton; }
//============================================================================
VxPushButton* IdentWidget::getIdentPushToTalkButton( void ) { return ui.m_PushToTalkButton; }
//============================================================================
VxPushButton* IdentWidget::getIdentOfferInfoButton( void ) { return ui.m_OfferInfoButton; }
//============================================================================
VxPushButton* IdentWidget::getIdentOfferViewButton( void ) { return ui.m_OfferViewButton; }
//============================================================================
VxPushButton* IdentWidget::getIdentOfferAcceptButton( void ) { return ui.m_OfferAcceptButton; }
//============================================================================
VxPushButton* IdentWidget::getIdentOfferRejectButton( void ) { return ui.m_OfferRejectButton; }
//============================================================================
VxPushButton* IdentWidget::getIdentMenuButton( void ) { return ui.m_FriendMenuButton; }

//============================================================================
QLabel* IdentWidget::getIdentLine1( void ) { return ui.m_FriendNameLabel; }
//============================================================================
QLabel* IdentWidget::getIdentLine2( void ) { return ui.m_DescTextLabel; }
//============================================================================
QLabel* IdentWidget::getIdentLine3( void ) { return ui.m_TodLabel; }
