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
GuiOfferMgrBase::GuiOfferMgrBase( AppCommon&  myApp )
: m_MyApp( myApp )
, m_OfferUpdateTimer( new QTimer( this ) )
, m_RingTimer( new QTimer( this ) )
{
	m_OfferUpdateTimer->setInterval( 2000 );
	m_RingTimer->setInterval( 1000 );
	connect( m_OfferUpdateTimer, SIGNAL(timeout()), this, SLOT(slotUpdateOffersTimer()) );
	connect( m_RingTimer, SIGNAL(timeout()), this, SLOT(slotOncePerSecondRingTimer()) );

	connectCallbackSignalsAndSlots();

	m_OfferUpdateTimer->start();
}

//========================================================================
bool GuiOfferMgrBase::isMessengerReady( void )
{ 
	return m_MyApp.isMessengerReady(); 
}

//========================================================================
GuiOfferSession* GuiOfferMgrBase::createOfferSession( GuiUser* guiUser, OfferBaseInfo& offerBaseInfo )
{
	GuiOfferInfo offerInfo( offerBaseInfo );
	offerInfo.setUser( guiUser );
	offerInfo.updateLastActivityTime();
	return new GuiOfferSession( offerInfo, this );
}

//========================================================================
void GuiOfferMgrBase::toGuiRxedPluginOffer( VxGUID onlineId, OfferBaseInfo& offerInfo )
{
	GuiUser* guiUser = m_MyApp.getUserMgr().getUser( onlineId );
	if( !guiUser )
	{
		LogMsg( LOG_ERROR, "GuiOfferMgrBase::toGuiRxedOfferReply unknown user" );
return;
	}

	GuiOfferSession* offerSession = findOfferSession( offerInfo.getPluginType(), offerInfo.getOfferId(), guiUser );
	if( !offerSession )
	{
		offerSession = createOfferSession( guiUser, offerInfo );
		offerSession->setIsRemoteInitiated( true );
		offerSession->setOfferState( eOfferStateNeedResponse );
		m_OfferList.push_back( offerSession );
		updateActiveOfferCount();
	}
	else
	{
		offerSession->updateOfferInfo( offerInfo );
	}

	switch( offerInfo.getPluginType() )
	{
	case ePluginTypePersonFileXfer:
		offerSession->setRequiresReply( true );
		break;
	default:
		break;
	}

	//GuiOfferSession* offerSession = findOrAddOfferSession( offerSession );
	//changeOfferState( offerSession, eOfferStateRxedByUser );
	//updateSndsAndMessages( offerSession );
	for( auto client : m_OfferCallbackList )
	{
		client->callbackToGuiRxedPluginOffer( offerSession );
	}
}

//========================================================================
void GuiOfferMgrBase::toGuiRxedOfferReply( VxGUID onlineId, OfferBaseInfo& offerInfo )
{
	GuiUser* guiUser = m_MyApp.getUserMgr().getUser( onlineId );
	if( !guiUser )
	{
		LogMsg( LOG_ERROR, "GuiOfferMgrBase::toGuiRxedOfferReply unknown user" );
		return;
	}

	GuiOfferSession* offerSession = findOfferSession( offerInfo.getPluginType(), offerInfo.getOfferId(), guiUser );
	if( !offerSession )
	{
		LogMsg( LOG_WARNING, "GuiOfferMgrBase::toGuiRxedOfferReply offer of %s from %s not found", guiUser->getOnlineName().c_str() );
		return;
	}

	offerSession->updateOfferInfo( offerInfo );
	EOfferState newOfferState = eOfferStateBusy;
	switch( offerSession->getOfferResponse() )
	{
	case eOfferResponseAccept:
		newOfferState = eOfferStateAccepted;
		break;

	case eOfferResponseReject:
		newOfferState = eOfferStateRejected;
		break;

	case eOfferResponseBusy:
		newOfferState = eOfferStateBusy;
		break;

	case eOfferResponseCancelSession:
	case eOfferResponseEndSession:
		newOfferState = eOfferStateCanceled;
		break;

	case eOfferResponseUserOffline:
		newOfferState = eOfferStateUserOffline;
		break;

	case eOfferResponseNotSet:
		LogMsg( LOG_INFO, "toGuiRxedOfferReply Response was not set" );
		newOfferState = eOfferStateNone;
		break;

	default:
		LogMsg( LOG_INFO, "toGuiRxedOfferReply Unknown Response %d", offerSession->getOfferResponse() );
		newOfferState = eOfferStateNone;
		break;
	}

	changeOfferState( offerSession, newOfferState );
	updateSndsAndMessages( offerSession );

	for( auto client : m_OfferCallbackList )
	{
		client->callbackToGuiRxedOfferReply( offerSession );
	}

	if( newOfferState == eOfferStateAccepted )
	{
		int64_t timeExpire = GetHighResolutionTimeMs();
		if( offerSession->getExpiresTime() <= timeExpire + 5000 ) // +5000 in case internet is really really slow
		{
			if( ePluginTypePersonFileXfer == offerSession->getPluginType() )
			{
				// need special handling for file offers
				// TODO BRJ
			}
			else
			{
				bool result{ false };
				if( guiUser->isMyself() )
				{
					result = m_MyApp.launchOfferResponseAccept( offerSession, GuiHelpers::pluginTypeToOppositeDefaultContentFrame( offerSession->getPluginType() ) );
				}
				else
				{
					result = m_MyApp.launchOfferResponseAccept( offerSession, GuiHelpers::pluginTypeToDefaultContentFrame( offerSession->getPluginType() ) );
				}
				
				if( !result )
				{
					LogMsg( LOG_VERBOSE, "GuiOfferMgrBase::toGuiRxedOfferReply failed to launch applet for plugin %s",
						GuiParams::describePlugin( offerSession->getPluginType(), false ).c_str() );
				}
			}
		}
		else
		{
			LogMsg( LOG_VERBOSE, "GuiOfferMgrBase::toGuiRxedOfferReply expired offer" );
		}
	}
}

//========================================================================
void GuiOfferMgrBase::toGuiPluginSessionEnded( VxGUID& onlineId, EPluginType pluginType, VxGUID& lclSessionId )
{
	GuiUser* guiUser = m_MyApp.getUserMgr().getUser( onlineId );
	if( !guiUser )
	{
		LogMsg( LOG_ERROR, "GuiOfferMgrBase::toGuiPluginSessionEnded unknown user" );
		return;
	}

	GuiOfferSession* offerSession = findOfferSession( pluginType, lclSessionId, guiUser );
	if( !offerSession )
	{
		LogMsg( LOG_WARNING, "GuiOfferMgrBase::toGuiPluginSessionEnded offer of %s from %s not found", guiUser->getOnlineName().c_str() );
		return;
	}

	offerSession->setOfferState( eOfferStateSessionComplete );
	updateSndsAndMessages( offerSession );
	for( auto client : m_OfferCallbackList )
	{
		client->callbackToGuiPluginSessionEnded( offerSession );
	}
}

//========================================================================
void GuiOfferMgrBase::callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline )
{
	if( !isOnline )
	{
		for( auto iter = m_OfferList.begin(); iter != m_OfferList.end(); )
		{
			GuiOfferSession* offerSession = (*iter);
			if( offerSession->getUser()->getMyOnlineId() == guiUser->getMyOnlineId() )
			{
				changeOfferState( offerSession, eOfferStateUserOffline );
				iter = m_OfferList.erase( iter );
				for( auto client : m_OfferCallbackList )
				{
					client->callbackGuiOfferRemoved( offerSession );
				}

				offerSession->deleteLater();
			}
			else
			{
				iter++;
			}
		}
	}
}

//========================================================================
void GuiOfferMgrBase::changeOfferState( GuiOfferSession* offerSession, EOfferState newOfferState )
{
	if( 0 == offerSession )
	{
		return;
	}

	switch( newOfferState )
	{
	case eOfferStateRxedByUser:
		// always play notify sound
		m_MyApp.playSound( eSndDefNotify1 );
		m_MyApp.toGuiStatusMessage( offerSession->describeOffer().c_str() );
		// just in case thinks in session
		forceToNotInSession( offerSession );
		offerSession->setOfferState( newOfferState );
		startRingTimerIfNotInSession();	
		break;

	case eOfferStateInSession:			
		offerSession->setOfferState( newOfferState );
		stopRingTimer();	
		break;

	case eOfferStateCanceled:
		offerSession->setOfferState( newOfferState );
		m_MyApp.toGuiStatusMessage( (offerSession->getOnlineName() + " Canceled Offer " + offerSession->describePlugin()).c_str() );
		m_MyApp.playSound( eSndDefOfferRejected );
		break;

	case eOfferStateBusy:
		offerSession->setOfferState( newOfferState );
		m_MyApp.toGuiStatusMessage( (offerSession->getOnlineName() + " Is Too Busy For " + offerSession->describePlugin()).c_str() );
		m_MyApp.playSound( eSndDefBusy );
		break;

	case eOfferStateAccepted:
		offerSession->setOfferState( newOfferState );
		break;

	case eOfferStateRejected:		
		offerSession->setOfferState( newOfferState );
		m_MyApp.playSound( eSndDefOfferRejected );
		m_MyApp.toGuiStatusMessage( (offerSession->getOnlineName() + " Rejected Offer " + offerSession->describePlugin()).c_str() );
		break;

	case eOfferStateSessionComplete:
		//m_MyApp.playSound( eSndDefOfferRejected );
		m_MyApp.toGuiStatusMessage( (offerSession->getOnlineName() + " Ended Session " + offerSession->describePlugin()).c_str() );
		break;

	case eOfferStateUserOffline:
		m_MyApp.toGuiStatusMessage( (offerSession->getOnlineName() + " cannot " + offerSession->describePlugin() + " because is offline").c_str() );
		break;

	default:
        LogMsg( LOG_INFO, "changeOfferState Unknown Offer State %d", newOfferState );
		break;
	}	

	if( m_OfferList.size() )
	{
		GuiOfferSession* sessionStateTop = m_OfferList[0];
		if( sessionStateTop == offerSession )
		{
			for( auto client : m_OfferCallbackList )
			{
				client->callbackGuiUpdatePluginOffer( offerSession );
			}
		}
	}
}

//========================================================================
void GuiOfferMgrBase::forceToNotInSession( GuiOfferSession* offerSession )
{
	EOfferState oldOfferState = offerSession->getOfferState();
	if( ( eOfferStateRxedByUser == oldOfferState )
		||	( eOfferStateInSession == oldOfferState ) )	
	{
        LogMsg( LOG_INFO, "forceToNotInSession %s %s", offerSession->getOnlineName().c_str(), offerSession->describePlugin().c_str() );
		offerSession->setOfferState( eOfferStateNone );
	}
}

//========================================================================
void GuiOfferMgrBase::startRingTimerIfNotInSession()
{
	if( false == m_UserIsInSession )
	{
		m_RingTimerSecondCnt				= RING_ELAPSE_SEC;	
		m_RingTimerCycleCnt					= RING_COUNT;
	}	
}

//========================================================================
void GuiOfferMgrBase::stopRingTimer()
{
	m_RingTimerCycleCnt					= 0;
	m_RingTimerSecondCnt				= 0;		
}

//========================================================================
void GuiOfferMgrBase::slotOncePerSecondRingTimer()
{
	if( 0 != m_RingTimerCycleCnt )
	{
		if( 0 != m_RingTimerSecondCnt )
		{
			m_RingTimerSecondCnt--;
		}

		if( 0 == m_RingTimerSecondCnt )
		{
			m_RingTimerSecondCnt = RING_ELAPSE_SEC;
			m_RingTimerCycleCnt--;
			m_MyApp.playSound( eSndDefOfferStillWaiting );
		}			
	}		
}

//========================================================================
void GuiOfferMgrBase::updateSndsAndMessages( GuiOfferSession* offerSession )
{
	if( 0 == offerSession )
	{
		return;
	}
}

//========================================================================
GuiOfferSession* GuiOfferMgrBase::getTopGuiOfferSession( void ) // returns null if no session offers
{
	if( m_OfferList.size() )
	{
		std::vector<GuiOfferSession*>::iterator iter;
		iter = m_OfferList.begin(); 
		if( iter != m_OfferList.end() )
		{
			return *iter;
		}
	}

	return nullptr;
}

//========================================================================
void GuiOfferMgrBase::acceptOfferButtonClicked( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser )
{
	GuiOfferSession* offerSession = findOfferSession( pluginType, offerSessionId, guiUser );
	if( !offerSession )
	{
		checkAndUpdateIfEmptyOfferList();
		return;
	}

	if( ePluginTypePersonFileXfer == offerSession->getPluginType() )
	{
		offerSession->assuredValidOfferId();

		//if( false == m_MyApp.getEngine().fromGuiToPluginOfferReply( offerSession->getPluginType(), offerSession->getUserIdent()->getMyOnlineId(), lclSessionId, eOfferResponseAccept );
		//{
  //          QString strErrMsg = offerSession->getUser()->getOnlineName().c_str();
  //          strErrMsg += QObject::tr( " Is Offline " );
		//	m_MyApp.toGuiStatusMessage( strErrMsg.toUtf8().constData() );
		//}
	}
	else
	{
		m_MyApp.launchOfferResponseAccept( offerSession, GuiHelpers::pluginTypeToDefaultContentFrame( offerSession->getPluginType() ) );
	}

	removePluginSessionOffer( offerSessionId );
}

//========================================================================
void GuiOfferMgrBase::rejectOfferButtonClicked( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser )
{
	GuiOfferSession* offerSession = findOfferSession( pluginType, offerSessionId, guiUser );
	if( !offerSession )
	{
		checkAndUpdateIfEmptyOfferList();
		return;
	}

	if( offerSession->getUser()->isOnline() 
		&& offerSession->isAvailableAndActiveOffer() )
	{
		offerSession->getOfferInfo().setOfferResponse( eOfferResponseReject );
		bool sentMsg = m_MyApp.getEngine().fromGuiToPluginOfferReply( offerSession->getUserIdent()->getMyOnlineId(), offerSession->getOfferInfo() );

		if( false == sentMsg )
		{
			LogMsg( LOG_INFO, "ActivityOfferListDlg::slotRejectOfferClicked user went offline" );
		}
	}

	removePluginSessionOffer( offerSessionId );
}

//========================================================================
void GuiOfferMgrBase::removePluginSessionOffer( EPluginType pluginType, GuiUser* guiUser )
{	
	for( auto iter = m_OfferList.begin(); iter != m_OfferList.end();)
	{
		GuiOfferSession* offerSession = (*iter);
		if( pluginType == offerSession->getPluginType() && offerSession->getUser() == guiUser )
		{
			iter = m_OfferList.erase( iter );
			for( auto client : m_OfferCallbackList )
			{
				client->callbackGuiOfferRemoved( offerSession );
			}

			offerSession->deleteLater();
		}
		else
		{
			++iter;
		}
	}

	checkAndUpdateIfEmptyOfferList();
}

//========================================================================
void GuiOfferMgrBase::checkAndUpdateIfEmptyOfferList( void )
{
	if( m_OfferList.empty() )
	{
		for( auto client : m_OfferCallbackList )
		{
			client->callbackGuiAllOffersRemoved();
		}
	}
}

//========================================================================
void GuiOfferMgrBase::removePluginSessionOffer( VxGUID&  offerSessionId )
{
	std::vector<GuiOfferSession*>::iterator iter;
	iter = m_OfferList.begin();
	while( iter != m_OfferList.end() )
	{
		GuiOfferSession* offerSession = (*iter);
		if( offerSession->getOfferSessionId() == offerSessionId )
		{
			iter = m_OfferList.erase( iter );
			for( auto client : m_OfferCallbackList )
			{
				client->callbackGuiOfferRemoved( offerSession );
			}

			offerSession->deleteLater();
		}
		else
		{
			++iter;
		}
	}
}

//========================================================================
void GuiOfferMgrBase::recievedOffer( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser )
{

}

//========================================================================
void GuiOfferMgrBase::sentOffer( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser )
{

}

//========================================================================
void GuiOfferMgrBase::sentOfferReply( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser, EOfferResponse offerResponse )
{
	//m_MyApp.getOfferListDialog()->sentOfferReply( pluginType, offerSessionId, guiUser, offerResponse );
}

//========================================================================
void GuiOfferMgrBase::recievedOfferReply( EPluginType  pluginType, VxGUID offerSessionId, GuiUser* guiUser, EOfferResponse offerResponse )
{

}

//========================================================================
void GuiOfferMgrBase::recievedSessionEnd( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser, EOfferResponse offerResponse )
{
}

//========================================================================
void GuiOfferMgrBase::startedSessionInReply( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser )
{
	if( GuiHelpers::isPluginSingleSession( pluginType ) )
	{
		removePluginSessionOffer( pluginType, guiUser );			
	}
	else
	{
		removePluginSessionOffer( offerSessionId );	
	}	
}

//========================================================================
void GuiOfferMgrBase::onIsInSession( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser, bool isInSession )
{
	if( GuiHelpers::isPluginSingleSession( pluginType ) )
	{
		removePluginSessionOffer( pluginType, guiUser );			
	}
	else
	{
		removePluginSessionOffer( offerSessionId );	
	}	
}

//========================================================================
void GuiOfferMgrBase::onSessionExit( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser )
{
	if( GuiHelpers::isPluginSingleSession( pluginType ) )
	{
		removePluginSessionOffer( pluginType, guiUser );			
	}
	else
	{
		removePluginSessionOffer( offerSessionId );	
	}	
}

//========================================================================
GuiOfferSession* GuiOfferMgrBase::findOfferSession( EPluginType pluginType, VxGUID sessionId, GuiUser* guiUser )
{
	for( auto offerSession : m_OfferList )
	{
		if( offerSession->getPluginType() == pluginType && offerSession->getUser() == guiUser && offerSession->getOfferId() == sessionId )
		{
			return offerSession;
		}
	}

	return nullptr;
}

//============================================================================
GuiOfferSession* GuiOfferMgrBase::findActiveAndAvailableOffer( GuiUser* guiUser, EPluginType pluginType )
{
	for( auto offerSession : m_OfferList )
	{
		if( offerSession->getPluginType() == pluginType && offerSession->getUser() == guiUser )
		{
			if( offerSession->isAvailableAndActiveOffer() )
			{
				return offerSession;
			}
		}
	}

	return nullptr;
}


//========================================================================
bool GuiOfferMgrBase::fromGuiMakePluginOffer( QWidget* parent, EPluginType pluginType, GuiUser* guiUser, FileInfo& fileInfo ) 
{ 
	OfferBaseInfo offerInfo( fileInfo );

	return fromGuiMakePluginOffer( parent, pluginType, guiUser, offerInfo );
}

//========================================================================
bool GuiOfferMgrBase::fromGuiMakePluginOffer( QWidget* parent, EPluginType pluginType, GuiUser* guiUser, OfferBaseInfo& offerInfo )
{ 
	offerInfo.fillOfferSend( pluginType, guiUser->getNetIdent() );

	offerInfo.getAssetUniqueId().assureIsValidGUID();
	offerInfo.getOfferId().assureIsValidGUID();
	offerInfo.setCreationTime( GetHighResolutionTimeMs() );
	offerInfo.setOfferMgr( eOfferMgrHost );

	if( IsPluginSingleSession( pluginType ) )
	{
		for( auto iter = m_OffersSentList.begin(); iter != m_OffersSentList.end(); ++iter )
		{
			if( iter->isSessionMatch( offerInfo ) )
			{
				m_OffersSentList.erase( iter );
				break;
			}
		}
	}

	bool result = m_MyApp.getFromGuiInterface().fromGuiMakePluginOffer( guiUser->getMyOnlineId(), offerInfo );
	if( result )
	{
		m_OffersSentList.push_back( offerInfo );
	}
	else
	{
		GuiHelpers::errorMsgBox( eErrMsgOfferSendFailed );
	}

	return result;
}

//========================================================================
bool GuiOfferMgrBase::fromGuiToPluginOfferReply( EPluginType pluginType, GuiUser* guiUser, OfferBaseInfo& offerInfo, VxGUID& offerSessionId, EOfferResponse offerResponse ) 
{ 
	// do not initialize offer id.. it should already be set
	offerInfo.setPluginType( pluginType );
	offerInfo.setCreationTime( GetHighResolutionTimeMs() );
	offerInfo.setOfferResponse( offerResponse );
	offerInfo.setOfferMgr( eOfferMgrClient );

	bool result = m_MyApp.getFromGuiInterface().fromGuiToPluginOfferReply( guiUser->getMyOnlineId(), offerInfo );
	if( result )
	{
		m_ResponseSentList.push_back( offerInfo );
	}

	return result;
}

//========================================================================
void GuiOfferMgrBase::slotUpdateOffersTimer( void )
{
	updateActiveOfferCount();
}

//========================================================================
void GuiOfferMgrBase::updateActiveOfferCount( void )
{
	int activeCnt = 0;
	for( auto offerSession : m_OfferList )
	{
		if( offerSession->isAvailableAndActiveOffer() )
		{
			activeCnt++;
		}
	}

	if( activeCnt != m_ActiveOfferCount )
	{
		m_ActiveOfferCount = activeCnt;
		for( auto client : m_OfferCallbackList )
		{
			client->callbackActiveOfferCount( m_ActiveOfferCount );
		}
	}
	else
	{
		m_ActiveOfferCount = activeCnt;
	}
}

//========================================================================
void GuiOfferMgrBase::viewOffer( GuiOfferSession* offerSession, QWidget* contentFrame )
{
	if( validateOffer( offerSession, contentFrame ) )
	{
		AppletOfferView* applet = (AppletOfferView*)m_MyApp.getAppletMgr().launchApplet( eAppletOfferView, contentFrame );
		if( applet )
		{
			applet->setPluginType( offerSession->getPluginType() );
			applet->setUser( offerSession->getUser() );
			applet->setOfferInfo( offerSession->getOfferInfo() );
			applet->setOfferMessage( offerSession->getMessagesText().c_str() );
		}
	}
}

//========================================================================
bool GuiOfferMgrBase::acceptOffer( GuiOfferSession* offerSession, QWidget* contentFrame )
{
	if( !offerSession->getUser() || !offerSession->getUser()->isOnline() )
	{
		GuiHelpers::errorMsgBox( eErrMsgUserUnavailable, contentFrame, offerSession->getUser() );
		return false;
	}

	bool offerSent = false;
	if( validateOffer( offerSession, contentFrame ) )
	{
		GuiOfferInfo& offerInfo = offerSession->getOfferInfo();
		offerInfo.setOfferResponse( eOfferResponseAccept );
		offerSent = m_MyApp.getEngine().fromGuiToPluginOfferReply( offerSession->getUserIdent()->getMyOnlineId(), offerSession->getOfferInfo() );
		if( !offerSent )
		{
			GuiHelpers::errorMsgBox( eErrMsgUserUnavailable, contentFrame, offerSession->getUser() );
		}
		else
		{
			m_MyApp.launchOfferResponseAccept( offerSession, contentFrame );
		}
	}

	return offerSent;
}

//========================================================================
bool GuiOfferMgrBase::rejectOffer( GuiOfferSession* offerSession, QWidget* contentFrame )
{
	if( !offerSession->getUser() || !offerSession->getUser()->isOnline() )
	{
		GuiHelpers::errorMsgBox( eErrMsgUserUnavailable, contentFrame, offerSession->getUser() );
		return false;
	}

	bool offerSent = false;
	if( validateOffer( offerSession, contentFrame ) )
	{
		GuiOfferInfo& offerInfo = offerSession->getOfferInfo();
		offerInfo.setOfferResponse( eOfferResponseReject );
		offerSent = m_MyApp.getEngine().fromGuiToPluginOfferReply( offerSession->getUserIdent()->getMyOnlineId(), offerSession->getOfferInfo() );
		if( !offerSent )
		{
			GuiHelpers::errorMsgBox( eErrMsgUserUnavailable, contentFrame, offerSession->getUser() );
		}
	}

	return offerSent;
}


//========================================================================
bool GuiOfferMgrBase::validateOffer( GuiOfferSession* offerSession, QWidget* contentFrame, bool showErrorMsg )
{

	if( ePluginTypeInvalid == offerSession->getPluginType() )
	{
		if( showErrorMsg )
		{
			QMessageBox::information( contentFrame, QObject::tr( "Plugin Type Not Set" ), QObject::tr( "Plugin Type Not Set" ) );
		}

		return false;
	}

	if( !offerSession->getUser() )
	{
		if( showErrorMsg )
		{
			QMessageBox::information( contentFrame, QObject::tr( "User Not Set" ), QObject::tr( "User Not Set" ) );
		}

		return false;
	}

	GuiOfferInfo& offerInfo = offerSession->getOfferInfo();
	if( !offerInfo.isValid() )
	{
		if( showErrorMsg )
		{
			QMessageBox::information( contentFrame, QObject::tr( "Offer Not Set" ), QObject::tr( "Offer Not Set" ) );
		}

		return false;
	}

	if( offerInfo.isFileAsset() && !offerInfo.isFileHashValid() )
	{
		if( showErrorMsg )
		{
			QMessageBox::information( contentFrame, QObject::tr( "File Hash Is Not Valid" ), QObject::tr( "File Hash Is Not Valid" ) );
		}

		return false;
	}

	return true;
}
