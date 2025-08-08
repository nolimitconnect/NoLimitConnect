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

	bool                        setOfferSession( std::shared_ptr<GuiOfferSession>& offerSession ) override;

protected slots:
	void						slotGameStatus( QString statusText );

protected:
	void						onResizeEvent( QSize& newSize ) override;
    void						toGuiInstMsg( GuiUser* friendIdent, EPluginType pluginType, QString instMsg ) override;

	void				        callbackToGuiOfferMsg( GuiUser* guiUser, EPluginType pluginType, VxGUID& offerId, std::string& msg ) override;

	void						onOfferWasSet( void ) override;
	void						onInSession( bool isInSession ) override;

	void						onStateTextChanged( QString& stateText ) override;

	//=== vars ===//
	Ui::AppletPeerTodGameUi&	ui;
	TodGameLogic				m_TodGameLogic;
};
