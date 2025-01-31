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

#include "ActivityMessageBox.h"
#include "AppCommon.h"
#include "AppletMgr.h"
#include "AppletMultiMessenger.h"
#include "AppletOfferView.h"
#include "AppletPeerVoicePhone.h"
#include "AppletPeerVideoPhone.h"
#include "AppletPeerTodGame.h"
#include "GuiHelpers.h"
#include "GuiOfferCallback.h"
#include "GuiOfferSession.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>
#include <CoreLib/VxTime.h>

#include <QFrame>
#include <QTimer>

//========================================================================
GuiOfferMgrBase::GuiOfferMgrBase( AppCommon& myApp )
: QWidget()
, m_MyApp( myApp )
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
std::shared_ptr<GuiOfferSession> GuiOfferMgrBase::createOfferSession( GuiUser* guiUser, OfferBaseInfo& offerBaseInfo )
{
	GuiOfferInfo offerInfo( offerBaseInfo );
	offerInfo.setUser( guiUser );
	offerInfo.updateLastActivityTime();
    return std::make_shared<GuiOfferSession>( GuiOfferSession( offerInfo ));
}

//========================================================================
void GuiOfferMgrBase::toGuiRxedPluginOffer( VxGUID& onlineId, OfferBaseInfo& offerInfo )
{
	GuiUser* guiUser = m_MyApp.getUserMgr().getUser( onlineId );
	if( !guiUser )
	{
		LogMsg( LOG_ERROR, "GuiOfferMgrBase::toGuiRxedOfferReply unknown user" );
		return;
	}

	bool newOfferSession{ false };
	EOfferState offerState{ eOfferStateNone };
	std::shared_ptr<GuiOfferSession> offerSession(findOfferSession( offerInfo.getPluginType(), offerInfo.getOfferId(), guiUser ));
	if( !offerSession.get() )
	{
		offerInfo.setOfferMgr( eOfferMgrClient );
		offerSession = createOfferSession( guiUser, offerInfo );
		offerSession->setOfferState( eOfferStateNeedResponse );
		m_OfferList.emplace_back( offerSession );
		onNewOfferSession( offerSession );
		newOfferSession = true;
		offerState = offerSession->getOfferState();
	}
	else
	{
		EOfferState offerState = offerSession->getOfferState();
		offerSession->updateOfferInfo( offerInfo );
		if( offerState != offerSession->getOfferState() )
		{
			offerState = offerSession->getOfferState();
		}
	}

	switch( offerInfo.getPluginType() )
	{
	case ePluginTypePersonFileXfer:
		offerSession->setRequiresReply( true );
		break;
	default:
		break;
	}

	if( newOfferSession )
	{
		for( auto client : m_OfferCallbackList )
		{
			client->callbackToGuiRxedPluginOffer( offerSession );
		}
	}

	if( offerState != eOfferStateNone )
	{
		for( auto client : m_OfferCallbackList )
		{
			client->callbackToGuiRxedOfferStateChange( offerSession, offerState );
		}
	}
}

//========================================================================
void GuiOfferMgrBase::toGuiRxedOfferReply( VxGUID& onlineId, OfferBaseInfo& offerInfo )
{
	GuiUser* guiUser = m_MyApp.getUserMgr().getUser( onlineId );
	if( !guiUser )
	{
        LogMsg( LOG_ERROR, "GuiOfferMgrBase::%s unknown user", __func__ );
		return;
	}

	std::shared_ptr<GuiOfferSession> offerSession = findOfferSession( offerInfo.getPluginType(), offerInfo.getOfferId(), guiUser );
    if( !offerSession.get() )
	{
        LogMsg( LOG_WARNING, "GuiOfferMgrBase::%s offer of %s from %s not found", __func__,
               GuiParams::describePluginOffer( offerInfo.getPluginType() ).toUtf8().constData(),
               guiUser->getOnlineName().c_str() );
		return;
	}

	EOfferState oldOfferState = offerSession->getOfferState();
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
		LogMsg( LOG_ERROR, "GuiOfferMgrBase::%s Response was not set", __func__ );
		newOfferState = eOfferStateNone;
		break;

	default:
		LogMsg( LOG_ERROR, "GuiOfferMgrBase::%s Unknown Response %d", __func__, offerSession->getOfferResponse() );
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
		int64_t timeExpire = GetGmtTimeMs();
		if( offerSession->getExpiresTime() <= timeExpire + 5000 ) // +5000 in case internet is really really slow
		{
			if( ePluginTypePersonFileXfer == offerSession->getPluginType() )
			{
				// need special handling for file offers
				// TODO BRJ
			}
			else if( !offerInfo.isPhoneTypePlugin() ) // phone types are launched on send offer and offer accepted
			{
				bool result{ false };
				if( guiUser->isMyself() )
				{
					result = launchOfferResponseAccept( offerSession, GuiHelpers::pluginTypeToOppositeDefaultContentFrame( offerSession->getPluginType() ) );
				}
				else
				{
					result = launchOfferResponseAccept( offerSession, GuiHelpers::pluginTypeToDefaultContentFrame( offerSession->getPluginType() ) );
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

	std::shared_ptr<GuiOfferSession> offerSession = findOfferSession( pluginType, lclSessionId, guiUser );
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
			std::shared_ptr<GuiOfferSession> offerSession = (*iter);
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
void GuiOfferMgrBase::changeOfferState( std::shared_ptr<GuiOfferSession> offerSession, EOfferState newOfferState )
{
	if( !offerSession.get() )
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
		std::shared_ptr<GuiOfferSession> sessionStateTop = m_OfferList[0];
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
void GuiOfferMgrBase::forceToNotInSession( std::shared_ptr<GuiOfferSession> offerSession )
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
void GuiOfferMgrBase::updateSndsAndMessages( std::shared_ptr<GuiOfferSession> offerSession )
{
	if( 0 == offerSession )
	{
		return;
	}
}

//========================================================================
std::shared_ptr<GuiOfferSession> GuiOfferMgrBase::getTopGuiOfferSession( void ) // returns null if no session offers
{
	if( m_OfferList.size() )
	{
		auto iter = m_OfferList.begin(); 
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
	std::shared_ptr<GuiOfferSession> offerSession = findOfferSession( pluginType, offerSessionId, guiUser );
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
        launchOfferResponseAccept( offerSession, GuiHelpers::pluginTypeToDefaultContentFrame( offerSession->getPluginType() ) );
	}

	removePluginSessionOffer( offerSessionId );
}

//========================================================================
void GuiOfferMgrBase::rejectOfferButtonClicked( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser )
{
	std::shared_ptr<GuiOfferSession> offerSession = findOfferSession( pluginType, offerSessionId, guiUser );
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
		std::shared_ptr<GuiOfferSession> offerSession = (*iter);
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
	bool offerFound{ false };
    auto iter = m_OfferList.begin();
	while( iter != m_OfferList.end() )
	{
		std::shared_ptr<GuiOfferSession> offerSession = (*iter);
		if( offerSession->getOfferId() == offerSessionId )
		{
			m_OfferHistory.emplace_back( offerSession->getOfferInfo() );
			iter = m_OfferList.erase( iter );
			for( auto client : m_OfferCallbackList )
			{
				client->callbackGuiOfferRemoved( offerSession );
			}

			offerSession->deleteLater();
			offerFound = true;
		}
		else
		{
			++iter;
		}
	}

	if( offerFound )
	{
		updateActiveOfferCount();
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
std::shared_ptr<GuiOfferSession> GuiOfferMgrBase::findOfferSession( EPluginType pluginType, VxGUID sessionId, GuiUser* guiUser )
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
std::shared_ptr<GuiOfferSession> GuiOfferMgrBase::findActiveAndAvailableOffer( GuiUser* guiUser, EPluginType pluginType )
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
	offerInfo.setCreationTime( GetGmtTimeMs() );
	offerInfo.setOfferMgr( eOfferMgrHost );

	bool result = m_MyApp.getFromGuiInterface().fromGuiMakePluginOffer( guiUser->getMyOnlineId(), offerInfo );
	if( result )
	{
		if( offerInfo.isPhoneTypePlugin() )
		{
			EApplet appletType{ eAppletUnknown };
			AppletPeerBase* appletPeerBase{ nullptr };
			switch( offerInfo.getPluginType() )
			{
			case ePluginTypeVoicePhone:
				appletType = eAppletPeerVoicePhone;
				appletPeerBase = dynamic_cast<AppletPeerBase*>( m_MyApp.getAppletMgr().launchApplet( appletType, parent ) );
				break;
			case ePluginTypeVideoPhone:
				appletType = eAppletPeerVideoPhone;
				appletPeerBase = dynamic_cast<AppletPeerBase*>( m_MyApp.getAppletMgr().launchApplet( appletType, parent ) );
				break;
			case ePluginTypeTruthOrDare:
				appletType = eAppletPeerTruthOrDare;
				appletPeerBase = dynamic_cast<AppletPeerBase*>( m_MyApp.getAppletMgr().launchApplet( appletType, parent ) );
				break;
			default:
				LogModule( eLogOffer, LOG_ERROR, "GuiOfferMgrBase::%s unknown phone type plugin", __func__ );
			}
			
			if( appletPeerBase )
			{
				std::shared_ptr<GuiOfferSession> offerSession = createOfferSession( guiUser, offerInfo );
				appletPeerBase->setOfferSession( offerSession );
			}
		}
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
	offerInfo.setCreationTime( GetGmtTimeMs() );
	offerInfo.setOfferResponse( offerResponse );
	offerInfo.setOfferMgr( eOfferMgrClient );

	bool result = m_MyApp.getFromGuiInterface().fromGuiToPluginOfferReply( guiUser->getMyOnlineId(), offerInfo );
	if( result )
	{
		//m_ResponseSentList.emplace_back( offerInfo );
	}

	return result;
}

//========================================================================
void GuiOfferMgrBase::slotUpdateOffersTimer( void )
{
	//updateActiveOfferCount();
}

//========================================================================
void GuiOfferMgrBase::updateActiveOfferCount( void )
{
	int activeCnt = 0;
	int historyCnt = 0;
	for( auto offerSession : m_OfferList )
	{
		if( offerSession->isAvailableAndActiveOffer() )
		{
			activeCnt++;
		}
		else
		{
			historyCnt++;
		}
	}

    if( activeCnt != m_LastActiveOfferCount )
	{
        m_LastActiveOfferCount = activeCnt;
        int historyOfferCount = getHistoryOfferCount() + historyCnt;
		for( auto client : m_OfferCallbackList )
		{
            client->callbackActiveOfferCount( activeCnt, historyOfferCount );
		}
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
			launchOfferResponseAccept( offerSession, contentFrame );
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

//========================================================================
void GuiOfferMgrBase::onNewOfferSession( std::shared_ptr<GuiOfferSession> offerSession )
{
    offerSession->setOfferTimestamp( GetGmtTimeMs() );
	updateActiveOfferCount();
}

//============================================================================
bool GuiOfferMgrBase::launchOfferResponseAccept( GuiOfferSession* offerSessionIn, QWidget* contentFrame )
{
	// find the offer session in our list
	std::shared_ptr<GuiOfferSession> offerSession = findOfferSession( offerSessionIn->getPluginType(), offerSessionIn->getOfferId(), offerSessionIn->getUser() );
	if( !offerSession.get() )
	{
		LogMsg( LOG_ERROR, "GuiOfferMgrBase::%s session not found", __func__ );
		GuiHelpers::errorMsgBox( eErrMsgSessionNotFound, contentFrame );
		return false;
	}

	return launchOfferResponseAccept( offerSession, contentFrame );
}

//============================================================================
bool GuiOfferMgrBase::launchOfferResponseAccept( std::shared_ptr<GuiOfferSession>& offerSession, QWidget* contentFrame )
{
	GuiUser* guiUser = offerSession->getUser();
	if( !guiUser )
	{
		LogMsg( LOG_ERROR, "GuiOfferMgrBase::%s null guiUser", __func__ );
		return false;
	}

	bool result{ false };
	switch( offerSession->getPluginType() )
	{
	case ePluginTypeMessenger:
	{
		AppletMultiMessenger* appletMessenger = m_MyApp.getAppletMultiMessenger();
		if( appletMessenger )
		{
			appletMessenger->setSelectedUser( guiUser );
			result = true;
		}
	}

	break;

	case ePluginTypeVoicePhone:
	{
		if( !m_MyApp.getAppletMgr().findAppletDialog( eAppletPeerVoicePhone ) || guiUser->isMyself() ) // allow multiple sessions if is myself for testing
		{
			AppletPeerVoicePhone* applet = dynamic_cast<AppletPeerVoicePhone*>( m_MyApp.getAppletMgr().launchApplet( eAppletPeerVoicePhone, contentFrame ) );
			if( applet )
			{
				result = applet->setOfferSession( offerSession );
			}
		}
		else
		{
			GuiHelpers::errorMsgBox( eErrMsgAlreadyInSession, contentFrame );
		}
	}

	break;

	case ePluginTypeVideoPhone:
	{
		if( !m_MyApp.getAppletMgr().findAppletDialog( eAppletPeerVideoPhone ) || guiUser->isMyself() ) // allow multiple sessions if is myself for testing
		{
			AppletPeerVideoPhone* applet = dynamic_cast<AppletPeerVideoPhone*>(m_MyApp.getAppletMgr().launchApplet( eAppletPeerVideoPhone, contentFrame ));
			if( applet )
			{
				result = applet->setOfferSession( offerSession );
			}
		}
		else
		{
			GuiHelpers::errorMsgBox( eErrMsgAlreadyInSession, contentFrame );
		}
	}

	break;

	case ePluginTypeTruthOrDare:
	{
		if( !m_MyApp.getAppletMgr().findAppletDialog( eAppletPeerTruthOrDare ) || guiUser->isMyself() ) // allow multiple sessions if is myself for testing
		{
			AppletPeerTodGame* applet = dynamic_cast<AppletPeerTodGame*>(m_MyApp.getAppletMgr().launchApplet( eAppletPeerTruthOrDare, contentFrame ));
			if( applet )
			{
				result = applet->setOfferSession( offerSession );
			}
		}
		else
		{
			GuiHelpers::errorMsgBox( eErrMsgAlreadyInSession, contentFrame );
		}
	}

	break;

	default:
		ActivityMessageBox errMsgBox( m_MyApp, contentFrame, LOG_ERROR, "GuiOfferMgrBase::%s UNKNOWN plugin type %d", 
									  __func__, offerSession->getPluginType() );
		errMsgBox.exec();
	}

	return result;
}
