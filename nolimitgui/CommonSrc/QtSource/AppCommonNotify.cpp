//============================================================================
// Copyright (C) 2013 Brett R. Jones 
// Issued to MIT style license by Brett R. Jones in 2017
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

#include "GuiOfferSession.h"
#include "AppCommon.h"
#include "AppGlobals.h"

#include "ToGuiActivityInterface.h"
#include "ActivityNetworkState.h"

#include <CoreLib/VxGlobals.h>

#include <QMessageBox>

//============================================================================
//! received offer from friend
void AppCommon::toGuiRxedPluginOffer( VxNetIdent* netIdent, OfferBaseInfo& offerInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiRxedPluginOffer( netIdent->getMyOnlineId(), offerInfo );
}

//============================================================================
void AppCommon::slotInternalToGuiRxedPluginOffer( VxGUID onlineId, OfferBaseInfo offerInfo )
{
	getOfferMgr().toGuiRxedPluginOffer( onlineId, offerInfo );
}

//============================================================================
//! response to offer
void AppCommon::toGuiRxedOfferReply( VxNetIdent* netIdent, OfferBaseInfo& offerInfo )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalToGuiRxedOfferReply( netIdent->getMyOnlineId(), offerInfo );
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
void AppCommon::slotInternalMediaAction( EAppModule appModule, EMediaPlayerAction playerAction, int actionVal, QString fileName )
{
	LogMsg( LOG_VERBOSE, "Media Action %d val %d fileName %s", playerAction, actionVal, fileName.toUtf8().constData() );
}

//============================================================================
void AppCommon::slotInternalMediaError( EAppModule appModule, EMediaError mediaError, QString msg )
{
    static bool isBusy{false};
	LogMsg( LOG_ERROR, "Media Error %d %s", mediaError, msg.toUtf8().constData() );

	if( isBusy )
	{
		// just log instead of show message box if user has not acked the previous message	
		return;
	}

	isBusy = true;
	QMessageBox::warning(&getHomePage(), QObject::tr("Media Error"), msg);
	isBusy = false;
}

//============================================================================
void AppCommon::toGuiPluginStatus( EPluginType pluginType, int statusType, int statusValue )
{
	emit signalInternalToGuiPluginStatus( pluginType, statusType, statusValue );
}

//============================================================================
void AppCommon::slotInternalToGuiPluginStatus( EPluginType pluginType, int statusType, int statusValue )
{
	if( pluginType == ePluginTypeCamServer )
	{
		getPluginMgr().setIsCamServerEnabled( statusType ? true : false );
		getPluginMgr().setCamServerClientCount( statusValue );
	}

	m_ToGuiActivityInterfaceBusy = true;
	for( auto client : m_ToGuiActivityInterfaceList )
	{
		client->toGuiPluginStatus( pluginType, statusType, statusValue );
	}

	m_ToGuiActivityInterfaceBusy = false;
}

//============================================================================
void AppCommon::toGuiInstMsg(	VxNetIdent*	netIdent, 
								EPluginType		pluginType,
								const char*	pMsg )
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
