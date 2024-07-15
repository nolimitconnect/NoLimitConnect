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
    class AppletPeerVoicePhoneUi;
}
QT_END_NAMESPACE

class P2PEngine;
class GuiOfferSession;

class AppletPeerVoicePhone : public AppletPeerBase
{
	Q_OBJECT
public:
	AppletPeerVoicePhone( AppCommon& app, QWidget*	parent = nullptr );
	virtual ~AppletPeerVoicePhone() override = default;

	//! called by base class with in session state
    virtual void				onInSession( bool isInSession ) override;

protected:
	// override of ToGuiActivityInterface
    virtual void				callbackToGuiRxedPluginOffer( GuiOfferSession* offer ) override;
    virtual void				callbackToGuiRxedOfferReply( GuiOfferSession* offer ) override;

	void						setupActivityVoicePhone( void );
    void						toGuiInstMsg( GuiUser* friendIdent, EPluginType pluginType, QString instMsg ) override;

	//=== vars ===//
	Ui::AppletPeerVoicePhoneUi&	ui;
};
