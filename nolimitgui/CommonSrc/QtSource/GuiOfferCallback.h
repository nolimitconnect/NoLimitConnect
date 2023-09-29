#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
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