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

	bool						setOfferSession( std::shared_ptr<GuiOfferSession>& offerSession ) override;

protected:
    void						toGuiInstMsg( GuiUser* friendIdent, EPluginType pluginType, QString instMsg ) override;

	void				        callbackToGuiOfferMsg( GuiUser* guiUser, EPluginType pluginType, VxGUID& offerId, std::string& msg ) override;
	void				        callbackToGuiPluginSessionStarted( std::shared_ptr<GuiOfferSession>& offer ) override;
	void				        callbackToGuiPluginSessionEnded( std::shared_ptr<GuiOfferSession>& offer ) override;

	void						onOfferWasSet( void ) override;
	void						onInSession( bool isInSession ) override;

	void						onStateTextChanged( QString& stateText ) override;

	//=== vars ===//
	Ui::AppletPeerVoicePhoneUi&	ui;
};
