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
#include "GuiOfferSession.h"
#include "GuiHelpers.h"

#include <P2PEngine/P2PEngine.h>

#include <QTimer>


//========================================================================
void GuiOfferMgrBase::connectCallbackSignalsAndSlots( void )
{
    connect( this, SIGNAL(signalCallbackFileWasShredded(QString)), this, SLOT(slotCallbackFileWasShredded(QString)), Qt::QueuedConnection );
    connect( this, SIGNAL(signalCallbackOfferSendState(VxGUID,EOfferSendState,int)), this, SLOT(slotCallbackOfferSendState(VxGUID,EOfferSendState,int)), Qt::QueuedConnection );
    connect( this, SIGNAL(signalCallbackOfferAction(VxGUID,EOfferAction,int)), this, SLOT(slotCallbackOfferAction(VxGUID,EOfferAction,int)), Qt::QueuedConnection );

    connect( this, SIGNAL(signalCallbackOfferAdded(OfferBaseInfo*)), this, SLOT(slotCallbackOfferAdded(OfferBaseInfo*)), Qt::QueuedConnection );
    connect( this, SIGNAL(signalCallbackOfferUpdated(OfferBaseInfo*)), this, SLOT(slotCallbackOfferUpdated(OfferBaseInfo*)), Qt::QueuedConnection );
    connect( this, SIGNAL(signalCallbackOfferRemoved(OfferBaseInfo*)), this, SLOT(slotCallbackOfferRemoved(OfferBaseInfo*)), Qt::QueuedConnection );
}

//========================================================================
void GuiOfferMgrBase::callbackFileWasShredded( std::string& fileName ) 
{
    emit signalCallbackFileWasShredded( fileName.c_str() );
}

//========================================================================
void GuiOfferMgrBase::slotCallbackFileWasShredded( QString fileName )
{

}

//========================================================================
void GuiOfferMgrBase::callbackOfferSendState( VxGUID& assetOfferId, EOfferSendState assetSendState, int param )
{
    emit signalCallbackOfferSendState( assetOfferId, assetSendState, param );
}

//========================================================================
void GuiOfferMgrBase::slotCallbackOfferSendState( VxGUID assetOfferId, EOfferSendState assetSendState, int param )
{

}

//========================================================================
void GuiOfferMgrBase::callbackOfferAction( VxGUID& assetOfferId, EOfferAction offerAction, int param )
{
     emit slotCallbackOfferAction( assetOfferId, offerAction, param );
}

//========================================================================
void GuiOfferMgrBase::slotCallbackOfferAction( VxGUID assetOfferId, EOfferAction assetSendState, int param )
{

}

//========================================================================
void GuiOfferMgrBase::callbackOfferAdded( OfferBaseInfo* assetInfo )
{
    emit signalCallbackOfferAdded( new OfferBaseInfo( *assetInfo ) );
}

//========================================================================
void GuiOfferMgrBase::slotCallbackOfferAdded( OfferBaseInfo* assetInfo )
{

    delete assetInfo;
}

//========================================================================
void GuiOfferMgrBase::callbackOfferUpdated( OfferBaseInfo* assetInfo )
{
    emit signalCallbackOfferUpdated( new OfferBaseInfo( *assetInfo ) );
}

//========================================================================
void GuiOfferMgrBase::slotCallbackOfferUpdated( OfferBaseInfo* assetInfo )
{

    delete assetInfo;
}

//========================================================================
void GuiOfferMgrBase::callbackOfferRemoved( VxGUID& offerId )
{
    emit signalCallbackOfferRemoved( offerId );
}

//========================================================================
void GuiOfferMgrBase::slotCallbackOfferRemoved( VxGUID offerId )
{


}
