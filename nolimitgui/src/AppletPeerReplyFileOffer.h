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

#include "GuiUser.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletPeerReplyFileOfferUi;
}
QT_END_NAMESPACE

class AppCommon;
class P2PEngine;
class GuiOfferSession;

class AppletPeerReplyFileOffer : public AppletPeerBase
{
	Q_OBJECT
public:
	AppletPeerReplyFileOffer( AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletPeerReplyFileOffer() override = default;

public slots:
	void						onReceiveFileButClick();
	void						onCancelButClick(); 

protected:
	//! Set plugin icon based on permission level
	void						setPluginIcon( EPluginType pluginType, EFriendState ePluginPermission );

	//=== vars ===//
	Ui::AppletPeerReplyFileOfferUi&	ui;
    std::shared_ptr<GuiOfferSession>			m_Offer{ nullptr };
};
