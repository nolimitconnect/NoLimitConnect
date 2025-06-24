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
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>

#include <QFrame>
#include <QTimer>

//========================================================================
GuiOfferMgrBase::GuiOfferMgrBase( AppCommon& myApp )
: QWidget()
, m_MyApp( myApp )
, m_PhoneRinger( myApp, *this, this )
{
	connectCallbackSignalsAndSlots();
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
		LogMsg( LOG_ERROR, "GuiOfferMgrBase::%s unknown user", __func__ );
		return;
	}

	bool newOfferSession{ false };
	EOfferState offerState{ eOfferStateNone };
	std::shared_ptr<GuiOfferSession> offerSession(findOfferSession( offerInfo.getPluginType(), offerInfo.getOfferId(), guiUser ));
	if( !offerSession.get() )
	{
		offerInfo.setOfferMgr( eOfferMgrClient );
		if( offerInfo.isPhoneTypePlugin() )
		{
            if( m_MyApp.getEngine().fromGuiIsPluginInSession( offerInfo.getPluginType() ) )
            {
                sendResponse( guiUser, offerInfo, eOfferStateBusy );
                return;
            }
		}

		offerSession = createOfferSession( guiUser, offerInfo );
		m_OfferList.emplace_back( offerSession );
		onNewOfferSession( offerSession );
		newOfferSession = true;
		changeOfferState( offerSession, eOfferStateRxedByUser );
		offerState = offerSession->getOfferState();
		updateActiveOfferCount();
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

	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s offer state %s", 
										__func__, DescribeOfferState( offerSession->getOfferState() ) );

	if( newOfferSession )
	{
		for( auto& client : m_OfferCallbackList )
		{
			client->callbackToGuiRxedPluginOffer( offerSession );
		}
	}

	if( offerState != eOfferStateNone )
	{
		for( auto& client : m_OfferCallbackList )
		{
			client->callbackToGuiRxedOfferStateChange( offerSession, eOfferStateNone, offerState );
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

	updateRxedOffer( guiUser, offerSession, offerInfo );
}

//========================================================================
void GuiOfferMgrBase::updateRxedOffer( GuiUser* guiUser, std::shared_ptr<GuiOfferSession>& offerSession, OfferBaseInfo& offerInfo )
{
	if( offerInfo.hasOfferMsg() && offerInfo.getOfferMsg() != offerSession->getOfferMsg() )
	{
		announceOfferMsg( guiUser, offerInfo.getPluginType(), offerInfo.getOfferId(), offerInfo.getOfferMsg() );
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

	for( auto& client : m_OfferCallbackList )
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
void GuiOfferMgrBase::toGuiRxedOfferUpdated( OfferBaseInfo* offerInfo )
{
	if( !offerInfo )
	{
        LogMsg( LOG_ERROR, "GuiOfferMgrBase::%s null offerInfo", __func__ );
		return;
	}

	GuiUser* guiUser = m_MyApp.getUserMgr().getUser( offerInfo->getFromOnlineId() );
	if( !guiUser )
	{
        LogMsg( LOG_ERROR, "GuiOfferMgrBase::%s unknown user", __func__ );
		return;
	}

	std::shared_ptr<GuiOfferSession> offerSession = findOfferSession( offerInfo->getPluginType(), offerInfo->getOfferId(), guiUser );
    if( !offerSession.get() )
	{
        LogMsg( LOG_WARNING, "GuiOfferMgrBase::%s offer of %s from %s not found", __func__,
               GuiParams::describePluginOffer( offerInfo->getPluginType() ).toUtf8().constData(),
               guiUser->getOnlineName().c_str() );
		return;
	}

	updateRxedOffer( guiUser, offerSession, *offerInfo );
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
    if( !offerSession.get() )
	{
        LogMsg( LOG_WARNING, "GuiOfferMgrBase::%s offer from %s not found", __func__, guiUser->getOnlineName().c_str() );
		return;
	}

    changeOfferState( offerSession, eOfferStateSessionComplete );

	for( auto& client : m_OfferCallbackList )
	{
		client->callbackToGuiPluginSessionEnded( offerSession );
	}

	moveToHistory( offerSession->getOfferId() );
}

//========================================================================
void GuiOfferMgrBase::callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline )
{
	if( !isOnline )
	{
		std::vector<std::shared_ptr<GuiOfferSession>> offlineList;
		for( auto iter = m_OfferList.begin(); iter != m_OfferList.end(); ++iter )
		{
			std::shared_ptr<GuiOfferSession> offerSession = (*iter);
			if( offerSession->getUser()->getMyOnlineId() == guiUser->getMyOnlineId() )
			{
				changeOfferState( offerSession, eOfferStateUserOffline );
				for( auto& client : m_OfferCallbackList )
				{
					client->callbackGuiOfferRemoved( offerSession );
				}

				offlineList.emplace_back( offerSession );
			}
		}

		for( auto offer : offlineList )
		{
			moveToHistory( offer->getOfferId() );
		}
	}
}

//========================================================================
void GuiOfferMgrBase::changeOfferState( std::shared_ptr<GuiOfferSession>& offerSession, EOfferState newOfferState )
{
	if( !offerSession.get() )
	{
		return;
	}

	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s new state %s old state %s", 
										__func__, DescribeOfferState( newOfferState ), DescribeOfferState( offerSession->getOfferState() ) );
	EOfferState oldOfferState = offerSession->getOfferState();
	offerSession->setOfferState( newOfferState );
	switch( newOfferState )
	{
	case eOfferStateRxedByUser:
		// always play notify sound
		m_MyApp.playSound( eSndDefNotify1 );
		m_MyApp.toGuiStatusMessage( offerSession->describeOffer().c_str() );
		if( offerSession->isPhoneCall() )
		{
			m_PhoneRinger.startRinging( offerSession );
		}
		
		break;

	case eOfferStateInSession:			
		m_PhoneRinger.stopRinging( offerSession );	
		break;

	case eOfferStateCanceled:
		m_MyApp.toGuiStatusMessage( (offerSession->getOnlineName() + " Canceled Offer " + offerSession->describePlugin()).c_str() );
		m_MyApp.playSound( eSndDefOfferRejected );
		break;

	case eOfferStateBusy:
		m_MyApp.toGuiStatusMessage( (offerSession->getOnlineName() + " Is Too Busy For " + offerSession->describePlugin()).c_str() );
		m_MyApp.playSound( eSndDefBusy );
		break;

	case eOfferStateAccepted:		
		m_PhoneRinger.stopRinging( offerSession );	
		break;

	case eOfferStateRejected:		
		m_MyApp.playSound( eSndDefOfferRejected );
		m_MyApp.toGuiStatusMessage( (offerSession->getOnlineName() + " Rejected Offer " + offerSession->describePlugin()).c_str() );
		m_PhoneRinger.stopRinging( offerSession );	
		break;

	case eOfferStateSessionComplete:
		//m_MyApp.playSound( eSndDefOfferRejected );
		m_MyApp.toGuiStatusMessage( (offerSession->getOnlineName() + " Ended Session " + offerSession->describePlugin()).c_str() );
		break;

	case eOfferStateUserOffline:
		m_MyApp.toGuiStatusMessage( (offerSession->getOnlineName() + " cannot " + offerSession->describePlugin() + " because is offline").c_str() );
		m_PhoneRinger.stopRinging( offerSession );	
		break;

	case eOfferStateNoResponse:
		m_MyApp.toGuiStatusMessage( (offerSession->getOnlineName() + " no response " + offerSession->describePlugin()).c_str() );
		m_PhoneRinger.stopRinging( offerSession );	
		m_MyApp.playSound( eSndDefNotify2 );		
		break;

	case eOfferStateMissedCall:
		m_MyApp.toGuiStatusMessage( (offerSession->getOnlineName() + " missed call " + offerSession->describePlugin()).c_str() );
		m_PhoneRinger.stopRinging( offerSession );	
		m_MyApp.playSound( eSndDefNotify2 );		
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
			for( auto& client : m_OfferCallbackList )
			{
				client->callbackGuiUpdatePluginOffer( offerSession );
			}
		}
	}

	for( auto& client : m_OfferCallbackList )
	{
		client->callbackToGuiRxedOfferStateChange( offerSession, oldOfferState, newOfferState );
	}
}

//========================================================================
std::shared_ptr<GuiOfferSession> GuiOfferMgrBase::getTopGuiOfferSession( void ) // returns null if no session offers
{
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
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
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
	std::shared_ptr<GuiOfferSession> offerSession = findOfferSession( pluginType, offerSessionId, guiUser );
	if( !offerSession )
	{
		checkAndUpdateIfEmptyOfferList();
		return;
	}

	offerSession->assuredValidOfferId();
	offerSession->setOfferResponse( eOfferResponseAccept );
	offerSession->setOfferState( eOfferStateAccepted );

	if( false == m_MyApp.getEngine().fromGuiToPluginOfferReply( offerSession->getUserIdent()->getMyOnlineId(), offerSession->getOfferInfo() ) )
	{
        QString strErrMsg = offerSession->getUser()->getOnlineName().c_str();
        strErrMsg += QObject::tr( " Is Offline " );
		m_MyApp.toGuiStatusMessage( strErrMsg.toUtf8().constData() );
	}

	if( ePluginTypePersonFileXfer == offerSession->getPluginType() )
	{
		removePluginSessionOffer( offerSessionId );
	}
	else
	{
		m_OfferList.emplace_back( offerSession );
        launchOfferResponseAccept( offerSession, GuiHelpers::pluginTypeToDefaultContentFrame( offerSession->getPluginType() ) );
	}

	updateActiveOfferCount();
}

//========================================================================
void GuiOfferMgrBase::rejectOfferButtonClicked( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser )
{
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
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
		offerSession->setOfferState( eOfferStateRejected );
		bool sentMsg = m_MyApp.getEngine().fromGuiToPluginOfferReply( offerSession->getUserIdent()->getMyOnlineId(), offerSession->getOfferInfo() );

		if( false == sentMsg )
		{
			LogMsg( LOG_INFO, "ActivityOfferListDlg::%s user went offline", __func__ );
		}
	}
	
	if( ePluginTypePersonFileXfer == offerSession->getPluginType() )
	{
		removePluginSessionOffer( offerSessionId );
	}
	else
	{
		moveToHistory( offerSessionId );
	}
	
	updateActiveOfferCount();
}

//========================================================================
void GuiOfferMgrBase::removePluginSessionOffer( EPluginType pluginType, GuiUser* guiUser )
{	
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
	for( auto iter = m_OfferList.begin(); iter != m_OfferList.end();)
	{
		std::shared_ptr<GuiOfferSession> offerSession = (*iter);
		if( pluginType == offerSession->getPluginType() && offerSession->getUser() == guiUser )
		{
			for( auto& client : m_OfferCallbackList )
			{
				client->callbackGuiOfferRemoved( offerSession );
			}

            iter = m_OfferList.erase( iter );
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
		for( auto& client : m_OfferCallbackList )
		{
			client->callbackGuiAllOffersRemoved();
		}
	}
}

//========================================================================
void GuiOfferMgrBase::removePluginSessionOffer( VxGUID&  offerSessionId )
{
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
	bool offerFound{ false };
    auto iter = m_OfferList.begin();
	while( iter != m_OfferList.end() )
	{
		std::shared_ptr<GuiOfferSession> offerSession = (*iter);
		if( offerSession->getOfferId() == offerSessionId )
		{
            m_OfferHistory.emplace_back( offerSession );
			iter = m_OfferList.erase( iter );
			for( auto& client : m_OfferCallbackList )
			{
				client->callbackGuiOfferRemoved( offerSession );
			}

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
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
}

//========================================================================
void GuiOfferMgrBase::sentOffer( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser )
{
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
}

//========================================================================
void GuiOfferMgrBase::sentOfferReply( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser, EOfferResponse offerResponse )
{
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
}

//========================================================================
void GuiOfferMgrBase::recievedOfferReply( EPluginType  pluginType, VxGUID offerSessionId, GuiUser* guiUser, EOfferResponse offerResponse )
{
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
}

//========================================================================
void GuiOfferMgrBase::recievedSessionEnd( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser, EOfferResponse offerResponse )
{
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
}

//========================================================================
void GuiOfferMgrBase::startedSessionInReply( EPluginType pluginType, VxGUID offerSessionId, GuiUser* guiUser )
{
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
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
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
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
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
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
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
	for( auto offerSession : m_OfferList )
	{
		if( offerSession->getPluginType() == pluginType && offerSession->getUser() == guiUser && offerSession->getOfferId() == sessionId )
		{
			return offerSession;
		}
	}

	for( auto offerSession : m_OfferHistory )
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
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
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
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
	OfferBaseInfo offerInfo( fileInfo );

	return fromGuiMakePluginOffer( parent, pluginType, guiUser, offerInfo );
}

//========================================================================
bool GuiOfferMgrBase::fromGuiMakePluginOffer( QWidget* parent, EPluginType pluginType, GuiUser* guiUser, OfferBaseInfo& offerInfo )
{ 
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
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
				offerSession->setOfferState( eOfferStateSent );
				m_OfferList.emplace_back( offerSession );
				appletPeerBase->setOfferSession( offerSession );
				updateActiveOfferCount();
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
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
	// do not initialize offer id.. it should already be set
	offerInfo.setPluginType( pluginType );
	offerInfo.setCreationTime( GetGmtTimeMs() );
	offerInfo.setOfferResponse( offerResponse );
	offerInfo.setOfferMgr( eOfferMgrClient );
    if( offerResponse == eOfferResponseEndSession )
    {
        toGuiPluginSessionEnded( guiUser->getMyOnlineId(), pluginType, offerInfo.getOfferId() );
    }

	bool result = m_MyApp.getFromGuiInterface().fromGuiToPluginOfferReply( guiUser->getMyOnlineId(), offerInfo );
	if( result )
	{
		//m_ResponseSentList.emplace_back( offerInfo );
	}

	return result;
}

//========================================================================
void GuiOfferMgrBase::updateActiveOfferCount( void )
{
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
	int activeCnt = 0;
	for( auto offerSession : m_OfferList )
	{
		if( offerSession->getCreatorOnlineId() != m_MyApp.getMyOnlineId() && offerSession->isAvailableAndActiveOffer() )
		{
			activeCnt++;
		}
	}

    if( activeCnt != m_LastActiveOfferCount )
	{
        m_LastActiveOfferCount = activeCnt;
        int historyOfferCount = getHistoryOfferCount();
		for( auto& client : m_OfferCallbackList )
		{
            client->callbackActiveOfferCount( activeCnt, historyOfferCount );
		}
	}
}

//========================================================================
void GuiOfferMgrBase::viewOffer( GuiOfferSession* offerSessionIn, QWidget* contentFrame )
{
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
	std::shared_ptr<GuiOfferSession> offerSession = findOffer( offerSessionIn );
	if( !offerSession.get() )
	{
		LogMsg( LOG_ERROR, "GuiOfferMgrBase::%s ", __func__, "failed to find offer" );
		return;
	}

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
bool GuiOfferMgrBase::acceptOffer( GuiOfferSession* offerSessionIn, QWidget* contentFrame )
{
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
	std::shared_ptr<GuiOfferSession> offerSession = findOffer( offerSessionIn );
	if( !offerSession.get() )
	{
		LogMsg( LOG_ERROR, "GuiOfferMgrBase::%s failed to find offer", __func__ );
		return false;
	}

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
			changeOfferState( offerSession, eOfferStateAccepted );
			launchOfferResponseAccept( offerSession, contentFrame );
		}
	}

	return offerSent;
}

//========================================================================
bool GuiOfferMgrBase::rejectOffer( GuiOfferSession* offerSessionIn, QWidget* contentFrame )
{
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
	std::shared_ptr<GuiOfferSession> offerSession = findOffer( offerSessionIn );
	if( !offerSession.get() )
	{
		LogMsg( LOG_ERROR, "GuiOfferMgrBase::%s ", __func__, "failed to find offer" );
		return false;
	}

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
		changeOfferState( offerSession, eOfferStateRejected );
		offerSent = m_MyApp.getEngine().fromGuiToPluginOfferReply( offerSession->getUserIdent()->getMyOnlineId(), offerSession->getOfferInfo() );
		if( !offerSent )
		{
			GuiHelpers::errorMsgBox( eErrMsgUserUnavailable, contentFrame, offerSession->getUser() );
		}
	}

	return offerSent;
}

//========================================================================
std::shared_ptr<GuiOfferSession> GuiOfferMgrBase::findOffer( GuiOfferSession* offerSession )
{
	if( !offerSession )
	{
		LogMsg( LOG_ERROR, "GuiOfferMgrBase::%s ", __func__, "null offerSession" );
		return nullptr;
	}

	for( auto offer : m_OfferList )
	{
		if( offer->getOfferId() == offerSession->getOfferId() )
		{
			return offer;
		}
	}

	LogMsg( LOG_ERROR, "GuiOfferMgrBase::%s ", __func__, "offerSession not found" );
	return nullptr;
}

//========================================================================
bool GuiOfferMgrBase::validateOffer( std::shared_ptr<GuiOfferSession>& offerSession, QWidget* contentFrame, bool showErrorMsg )
{
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
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
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
    offerSession->setOfferTimestamp( GetGmtTimeMs() );
	updateActiveOfferCount();
}

//============================================================================
bool GuiOfferMgrBase::launchOfferResponseAccept( GuiOfferSession* offerSessionIn, QWidget* contentFrame )
{
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
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
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
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

//============================================================================
bool GuiOfferMgrBase::sendResponse( GuiUser* guiUser, OfferBaseInfo& offerInfo, EOfferState offerState )
{
    bool offerSent = m_MyApp.getEngine().fromGuiToPluginOfferReply( guiUser->getMyOnlineId(), offerInfo );
    if( !offerSent )
    {
        //GuiHelpers::errorMsgBox( eErrMsgUserUnavailable, contentFrame, offerSession->getUser() );
    }

    return offerSent;
}

//============================================================================
void GuiOfferMgrBase::phoneRingTimeout( std::shared_ptr<GuiOfferSession>& offerSession )
{
	changeOfferState( offerSession, offerSession->getOfferMgr() == eOfferMgrClient ? eOfferStateMissedCall : eOfferStateNoResponse );
	moveToHistory( offerSession->getOfferId() );
	updateActiveOfferCount();
}

//============================================================================
void GuiOfferMgrBase::moveToHistory( VxGUID& offerId )
{
	if( m_OfferHistory.size() >= MAX_OFFER_HISTORY_ENTRIES )
	{
		m_OfferHistory.erase( m_OfferHistory.begin() );
	}

	std::shared_ptr<GuiOfferSession> offerSession;
	bool movedToHistory{ false };
	for( auto iter = m_OfferList.begin(); iter != m_OfferList.end(); ++iter )
	{
		if( (*iter)->getOfferId() == offerId )
		{
			offerSession = *iter;
			movedToHistory = true;
			m_OfferHistory.emplace_back( *iter );
			m_OfferList.erase( iter );	
			break;
		}
	}

	if( movedToHistory )
	{
		for( auto offerClient : m_OfferCallbackList )
		{
			offerClient->callbackToGuiOfferMovedToHistory( offerSession );
		}
	}

	updateActiveOfferCount();
}

//============================================================================
void GuiOfferMgrBase::announceOfferMsg( GuiUser* guiUser, EPluginType pluginType, VxGUID& offerId, std::string& offerMsg )
{
	for( auto offerClient : m_OfferCallbackList )
	{
		offerClient->callbackToGuiOfferMsg( guiUser, pluginType, offerId, offerMsg );
	}
}
