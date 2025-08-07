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
    class AppletPeerVideoPhoneUi;
}
QT_END_NAMESPACE

class P2PEngine;
class GuiOfferSession;

class AppletPeerVideoPhone : public AppletPeerBase
{
	Q_OBJECT

public:
	AppletPeerVideoPhone( AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletPeerVideoPhone() override = default;

	//! called by base class with in session state
    void						onInSession( bool isInSession ) override;
	void				        callbackToGuiOfferMsg( GuiUser* guiUser, EPluginType pluginType, VxGUID& offerId, std::string& msg ) override;

	bool						setOfferSession( std::shared_ptr<GuiOfferSession>& offerSession ) override;

	void				        callbackToGuiPluginSessionStarted( std::shared_ptr<GuiOfferSession>& offer ) override;
	void				        callbackToGuiPluginSessionEnded( std::shared_ptr<GuiOfferSession>& offer ) override;

protected:
	void						onResizeEvent( QSize& newSize ) override;
    void						toGuiInstMsg( GuiUser* friendIdent, EPluginType pluginType, QString instMsg ) override;

	void						onOfferWasSet( void ) override;
	void						onStateTextChanged( QString& stateText ) override;

	//=== vars ===//
	Ui::AppletPeerVideoPhoneUi&	ui;
};

