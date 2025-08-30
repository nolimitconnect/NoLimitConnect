//============================================================================
// Copyright (C) 2016 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiOfferMgrBase.h"

#include "AppCommon.h"
#include "AppletMgr.h"
#include "AppletOfferView.h"
#include "GuiOfferCallback.h"
#include "GuiOfferSession.h"
#include "GuiHelpers.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>

#include <QTimer>


//========================================================================
void GuiOfferMgrBase::connectCallbackSignalsAndSlots( void )
{
    connect( this, SIGNAL(signalCallbackFileWasShredded(QString)), this, SLOT(slotCallbackFileWasShredded(QString)), Qt::QueuedConnection );
    connect( this, SIGNAL(signalCallbackOfferSendState(VxGUID,EOfferSendState,int)), this, SLOT(slotCallbackOfferSendState(VxGUID,EOfferSendState,int)), Qt::QueuedConnection );
    connect( this, SIGNAL(signalCallbackOfferAction(VxGUID,EOfferAction,int)), this, SLOT(slotCallbackOfferAction(VxGUID,EOfferAction,int)), Qt::QueuedConnection );

    connect( this, SIGNAL(signalCallbackOfferAdded(OfferBaseInfo*)), this, SLOT(slotCallbackOfferAdded(OfferBaseInfo*)), Qt::QueuedConnection );
    connect( this, SIGNAL(signalCallbackOfferUpdated(OfferBaseInfo*)), this, SLOT(slotCallbackOfferUpdated(OfferBaseInfo*)), Qt::QueuedConnection );
    connect( this, SIGNAL(signalCallbackOfferRemoved(VxGUID)), this, SLOT(slotCallbackOfferRemoved(VxGUID)), Qt::QueuedConnection );
}

//========================================================================
void GuiOfferMgrBase::callbackFileWasShredded( std::string& fileName ) 
{
    if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s", __func__ );
    emit signalCallbackFileWasShredded( fileName.c_str() );
}

//========================================================================
void GuiOfferMgrBase::slotCallbackFileWasShredded( QString fileName )
{

}

//========================================================================
void GuiOfferMgrBase::callbackOfferSendState( VxGUID& assetOfferId, EOfferSendState offerSendState, int param )
{
    if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s %s", __func__, DescribeOfferSendState( offerSendState ) );
    emit signalCallbackOfferSendState( assetOfferId, offerSendState, param );
}

//========================================================================
void GuiOfferMgrBase::slotCallbackOfferSendState( VxGUID assetOfferId, EOfferSendState assetSendState, int param )
{

}

//========================================================================
void GuiOfferMgrBase::callbackOfferAction( VxGUID& assetOfferId, EOfferAction offerAction, int param )
{
    if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s offerAction %s", __func__, DescribeOfferAction( offerAction ) );
    emit slotCallbackOfferAction( assetOfferId, offerAction, param );
}

//========================================================================
void GuiOfferMgrBase::slotCallbackOfferAction( VxGUID assetOfferId, EOfferAction assetSendState, int param )
{

}

//========================================================================
void GuiOfferMgrBase::callbackOfferAdded( OfferBaseInfo* offerInfo )
{
    if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s", __func__ );
    emit signalCallbackOfferAdded( new OfferBaseInfo( *offerInfo ) );
}

//========================================================================
void GuiOfferMgrBase::slotCallbackOfferAdded( OfferBaseInfo* offerInfo )
{
    if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s", __func__ );
    toGuiRxedPluginOffer( offerInfo->getCreatorId(), *offerInfo );
    delete offerInfo;
}

//========================================================================
void GuiOfferMgrBase::callbackOfferUpdated( OfferBaseInfo* offerInfo )
{
    if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s", __func__ );
    emit signalCallbackOfferUpdated( new OfferBaseInfo( *offerInfo ) );
}

//========================================================================
void GuiOfferMgrBase::slotCallbackOfferUpdated( OfferBaseInfo* offerInfo )
{
    toGuiRxedOfferUpdated( offerInfo );
    delete offerInfo;
}

//========================================================================
void GuiOfferMgrBase::callbackOfferRemoved( VxGUID& offerId )
{
    if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s", __func__ );
    emit signalCallbackOfferRemoved( offerId );
}

//========================================================================
void GuiOfferMgrBase::slotCallbackOfferRemoved( VxGUID offerId )
{


}
