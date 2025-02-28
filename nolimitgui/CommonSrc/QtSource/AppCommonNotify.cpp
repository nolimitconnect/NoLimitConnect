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

	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "AppCommon::%s type %s response %s send state %s", 
										__func__, DescribeOfferType( offerInfo.getOfferType() ), DescribeOfferResponse( offerInfo.getOfferResponse() ),
										DescribeOfferSendState( offerInfo.getOfferSendState() ) );
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

	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "AppCommon::%s type %s response %s send state %s", 
										__func__, DescribeOfferType( offerInfo.getOfferType() ), DescribeOfferResponse( offerInfo.getOfferResponse() ), 
										DescribeOfferSendState( offerInfo.getOfferSendState() ) );
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

	LogMsg( LOG_VERBOSE, "AppCommon::%s plugin %s", __func__, DescribePluginType( pluginType ) );
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
	LogMsg( LOG_VERBOSE, "AppCommon::%s plugin %s statusType %d statusValue %d ", __func__, DescribePluginType( pluginType ),
			statusType, statusValue );
	getPluginMgr().toGuiPluginStatus( pluginType, statusType, statusValue );
}

//============================================================================
void AppCommon::toGuiInstMsg( VxNetIdent* netIdent, EPluginType	pluginType, const char*	pMsg )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalToGuiInstMsg( netIdent->getMyOnlineId(), pluginType, pMsg );
}

//============================================================================
void AppCommon::slotToGuiInstMsg( VxGUID onlineId, EPluginType pluginType, QString pMsg )
{
	GuiUser* guiUser = m_UserMgr.getUser( onlineId );
	if( !guiUser )
	{
        LogMsg( LOG_ERROR, "AppCommon::%s null guiUser", __func__ );
        vx_assert( false );
		return;
	}

	if( m_ToGuiActivityInterfaceBusy )
    {
        LogMsg( LOG_ERROR, "AppCommon::%s m_ToGuiActivityInterfaceBusy true", __func__ );
        vx_assert( false );
		return;
    }

	m_ToGuiActivityInterfaceBusy = true;
	for( auto& client : m_ToGuiActivityInterfaceList )
	{
		client->toGuiInstMsg( guiUser, pluginType, pMsg );
	}

	m_ToGuiActivityInterfaceBusy = false;
}
