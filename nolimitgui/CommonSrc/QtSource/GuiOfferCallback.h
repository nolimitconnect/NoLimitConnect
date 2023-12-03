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

class GuiOfferCallback
{
public:
	virtual void				callbackActiveOfferCount( int activeCnt ) {};

	virtual void				callbackToGuiRxedPluginOffer( GuiOfferSession* offerSession ) {};
	virtual void				callbackToGuiRxedOfferReply( GuiOfferSession* offerSession ) {};
	virtual void				callbackToGuiPluginSessionEnded( GuiOfferSession* offerSession ) {};

	virtual void				callbackGuiUpdatePluginOffer( GuiOfferSession* offerSession ) {};
	virtual void				callbackGuiOfferRemoved( GuiOfferSession* offerSession ) {};

	virtual void				callbackGuiAllOffersRemoved( void ) {};

	virtual void 				onSessionStateChange( ESessionState eSessionState ) {};
	virtual void 				onInSession( bool isInSession ) {};
	virtual void 				onSessionActivityShouldExit( QString shouldExitReason ) {};
};