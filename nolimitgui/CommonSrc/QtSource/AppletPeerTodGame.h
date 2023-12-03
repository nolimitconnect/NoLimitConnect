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

#include "ui_AppletPeerTodGame.h"

class GuiOfferSession;

class AppletPeerTodGame : public AppletPeerBase
{
	Q_OBJECT

public:
	AppletPeerTodGame( AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletPeerTodGame() override = default;

	virtual bool				offerSession( GuiUser* guiUser, GuiOfferSession* offerSession ) override;
	//! called by base class with in session state
    virtual void				onInSession( bool isInSession ) override;
	//! called after session end or dialog exit
	virtual void				onEndSession( void );

protected:
	// override of ToGuiActivityInterface
    virtual void				callbackToGuiRxedPluginOffer( GuiOfferSession* offer ) override;
    virtual void				callbackToGuiRxedOfferReply( GuiOfferSession* offer ) override;

    void						toGuiInstMsg( GuiUser* friendIdent, EPluginType pluginType, QString instMsg ) override;

	//=== vars ===//
	Ui::AppletPeerTodGameUi	    ui;
	TodGameLogic				m_TodGameLogic;
	
};
