//============================================================================
// Copyright (C) 2013 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppCommon.h"

#include "AppGlobals.h"
#include "ActivityNetworkState.h"

#include "GuiFileXferSession.h"
#include "GuiOfferSession.h"
#include "GuiPluginMgr.h"
#include "HomeWindow.h"
#include "ToGuiActivityInterface.h"

#include <CoreLib/VxGlobals.h>

#include <QMessageBox>

//============================================================================
//! received offer from friend
void AppCommon::toGuiRxedPluginOffer( VxGUID onlineId, OfferBaseInfo& offerInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiRxedPluginOffer( onlineId, offerInfo );
}

//============================================================================
void AppCommon::slotInternalToGuiRxedPluginOffer( VxGUID onlineId, OfferBaseInfo offerInfo )
{
	getOfferMgr().toGuiRxedPluginOffer( onlineId, offerInfo );
}

//============================================================================
//! response to offer
void AppCommon::toGuiRxedOfferReply( VxGUID onlineId, OfferBaseInfo& offerInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiRxedOfferReply( onlineId, offerInfo );
}

//============================================================================
void AppCommon::slotInternalToGuiRxedOfferReply( VxGUID onlineId, OfferBaseInfo offerInfo )
{
	getOfferMgr().toGuiRxedOfferReply( onlineId, offerInfo );
}

//============================================================================
void AppCommon::toGuiPluginSessionEnded( VxNetIdent* netIdent, EPluginType pluginType, VxGUID& lclSessionId )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiPluginSessionEnded( netIdent->getMyOnlineId(), pluginType, lclSessionId );
}

//============================================================================
void AppCommon::slotInternalToGuiPluginSessionEnded( VxGUID onlineId, EPluginType pluginType, VxGUID lclSessionId )
{
	getOfferMgr().toGuiPluginSessionEnded( onlineId, pluginType, lclSessionId );
}

//============================================================================
void AppCommon::toGuiPluginStatus( EPluginType pluginType, int statusType, int statusValue )
{
	getPluginMgr().toGuiPluginStatus( pluginType, statusType, statusValue );
}

//============================================================================
void AppCommon::toGuiInstMsg( VxNetIdent* netIdent, EPluginType	pluginType, const char*	pMsg )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalToGuiInstMsg( m_UserMgr.getUser( netIdent->getMyOnlineId() ), pluginType, pMsg );
}

//============================================================================
void AppCommon::slotToGuiInstMsg( GuiUser* guiUser, EPluginType pluginType, QString pMsg )
{
	m_ToGuiActivityInterfaceBusy = true;
	for( auto client : m_ToGuiActivityInterfaceList )
	{
		client->toGuiInstMsg( guiUser, pluginType, pMsg );
	}

	m_ToGuiActivityInterfaceBusy = false;
}
