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

#include "TodGameLogic.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletPeerTodGameUi;
}
QT_END_NAMESPACE

class GuiOfferSession;

class AppletPeerTodGame : public AppletPeerBase
{
	Q_OBJECT

public:
	AppletPeerTodGame( AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletPeerTodGame() override = default;

    bool                        setOfferSession( std::shared_ptr<GuiOfferSession> offerSession ) override;
	//! called by base class with in session state
    virtual void				onInSession( bool isInSession ) override;

protected:

    void						toGuiInstMsg( GuiUser* friendIdent, EPluginType pluginType, QString instMsg ) override;
	void				        callbackToGuiOfferMsg( GuiUser* guiUser, EPluginType pluginType, VxGUID& offerId, std::string& msg ) override;

	void						onOfferWasSet( void ) override;

	//=== vars ===//
	Ui::AppletPeerTodGameUi&	ui;
	TodGameLogic				m_TodGameLogic;
};
