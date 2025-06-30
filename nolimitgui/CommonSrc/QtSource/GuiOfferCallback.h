#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <GuiInterface/IDefs.h>

#include <QString>

class GuiOfferSession;
class GuiUser;
class VxGUID;

class GuiOfferCallback
{
public:
	virtual void				callbackActiveOfferCount( int activeCnt, int historyCnt ) {};

	virtual void				callbackToGuiRxedPluginOffer( std::shared_ptr<GuiOfferSession>& offerSession ) {};
	virtual void				callbackToGuiRxedOfferReply( std::shared_ptr<GuiOfferSession>& offerSession ) {};
	virtual void				callbackToGuiOfferMsg( GuiUser* guiUser, EPluginType pluginType, VxGUID& offerId, std::string& msg ) {};
	virtual void				callbackToGuiRxedOfferStateChange( std::shared_ptr<GuiOfferSession>& offerSession, EOfferState oldOfferState, EOfferState newOfferState ) {};
	virtual void				callbackToGuiPluginSessionEnded( std::shared_ptr<GuiOfferSession>& offerSession ) {};
	virtual void				callbackToGuiOfferMovedToHistory( std::shared_ptr<GuiOfferSession>& offerSession ) {};

	virtual void				callbackGuiUpdatePluginOffer( std::shared_ptr<GuiOfferSession>& offerSession ) {};
	virtual void				callbackGuiOfferRemoved( VxGUID& offerId ) {};

	virtual void				callbackGuiAllOffersRemoved( void ) {};

	virtual void 				onSessionStateChange( ESessionState eSessionState ) {};
	virtual void 				onInSession( bool isInSession ) {};
	virtual void 				onSessionActivityShouldExit( QString shouldExitReason ) {};
};
