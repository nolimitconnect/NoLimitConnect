//============================================================================
// Copyright (C) 2016 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppCommon.h"
#include "AppletPeerBase.h"
#include "OfferSessionLogic.h"
#include "GuiOfferSession.h"
#include "GuiOfferCallback.h"
#include "GuiOfferMgr.h"
#include "GuiOfferSession.h"
#include "GuiHelpers.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxGlobals.h>

#include <stdio.h>

//======================================================================== 
OfferSessionLogic::OfferSessionLogic( AppletBase* appletBase, GuiOfferCallback* callbackInterface, AppCommon& myApp )
: m_MyApp( myApp )
, m_OfferMgr( myApp.getOfferMgr() )
, m_AppletBase( appletBase )
, m_OfferCallback( callbackInterface )
, m_MyIdent( myApp.getMyNetIdent() )
{
}

//========================================================================
void OfferSessionLogic::setGuiOfferSession( std::shared_ptr<GuiOfferSession>& offerSession )
{
    m_GuiOfferSession = offerSession;
    if( m_GuiOfferSession.get() )
    {
        m_OfferId = m_GuiOfferSession->getOfferId();
    }
    else
    {
        LogMsg( LOG_ERROR, "OfferSessionLogic::%s null offerSession", __func__ );
    }
}

//========================================================================
OfferBaseInfo& OfferSessionLogic::getOfferInfo( void )
{
    return m_GuiOfferSession->getOfferInfo();
}

//======================================================================== 
bool OfferSessionLogic::isHost( void )									
{ 
	return m_GuiOfferSession->getOfferMgr() == eOfferMgrHost; 
}

//======================================================================== 
bool OfferSessionLogic::isOurSessionType( std::shared_ptr<GuiOfferSession>& offerSession )
{
	if( ( offerSession->getPluginType() == m_ePluginType )
		&& ( 0 != m_HisIdent ) )
	{
		return true;
	}

	return false;
}

//======================================================================== 
bool OfferSessionLogic::isOurSessionInstance( std::shared_ptr<GuiOfferSession>& offerSession )
{
	if( isOurSessionType( offerSession ) )
	{
		if( GuiHelpers::isPluginSingleSession( offerSession->getPluginType() ) )
		{
			if(  offerSession->getUserIdent()->getMyOnlineId() ==  m_HisIdent->getMyOnlineId() )
			{
				return true;
			}				
		}
		else if(  offerSession->getOfferId() == getOfferInfo().getOfferId() ) 
		{
			return true;
		}
	}

	return false;
}

//======================================================================== 
void OfferSessionLogic::onInSession( bool isInSession )
{
	std::string sessionMsg = isInSession ? "In " : "Ended ";
	sessionMsg += GuiParams::describePlugin( m_ePluginType, m_IsOffer );
	sessionMsg += " Session";
	m_MyApp.toGuiStatusMessage( sessionMsg.c_str() );

	m_OfferCallback->onInSession( isInSession );
	m_OfferMgr.onIsInSession( m_ePluginType, getOfferInfo().getOfferId(), m_HisIdent, isInSession );
}

//======================================================================== 
void OfferSessionLogic::callbackToGuiRxedPluginOffer( std::shared_ptr<GuiOfferSession>& offerSession )
{
	if( isOurSessionType( offerSession ) )
	{			
		if( GuiHelpers::isPluginSingleSession( m_ePluginType ) )
		{
			// special case for multisession and other single session use online id instead of session id and auto accept
			if( offerSession->getUserIdent()->getMyOnlineId() == m_HisIdent->getMyOnlineId() )
			{
				if( offerSession->getOfferId().isVxGUIDValid() )
				{
					getOfferInfo().getOfferId() = offerSession->getOfferId();
				}

				m_OfferMgr.removePluginSessionOffer( m_ePluginType, m_HisIdent );
				// already in session with this user
				offerSession->getOfferInfo().setOfferResponse( eOfferResponseAccept );
				m_MyApp.getEngine().fromGuiToPluginOfferReply( m_HisIdent->getMyOnlineId(), offerSession->getOfferInfo() );
				if( !m_IsInSession )
				{
					m_IsInSession = true;
					m_MyApp.getEngine().fromGuiStartPluginSession( 	m_ePluginType, 
						m_HisIdent->getMyOnlineId(), 
						getOfferInfo().getOfferId() );
					onInSession( true );
				}
			}
			else
			{
				// already in session with someone else
				offerSession->getOfferInfo().setOfferResponse( eOfferResponseBusy );
				m_MyApp.getEngine().fromGuiToPluginOfferReply(	offerSession->getUserIdent()->getMyOnlineId(), offerSession->getOfferInfo() );
								
				m_OfferMgr.sentOfferReply( m_ePluginType, offerSession->getOfferId(), offerSession->getUserIdent(), eOfferResponseBusy );
			}
		}
		else
		{
			if( offerSession->getOfferId() == getOfferInfo().getOfferId() )
			{
				// already in session with this user
				offerSession->getOfferInfo().setOfferResponse( eOfferResponseAccept );
				m_MyApp.getEngine().fromGuiToPluginOfferReply( m_HisIdent->getMyOnlineId(), offerSession->getOfferInfo() );

				if( !m_IsInSession )
				{
					m_IsInSession = true;
					m_MyApp.getEngine().fromGuiStartPluginSession( 	m_ePluginType, 
																	m_HisIdent->getMyOnlineId(), 
																	getOfferInfo().getOfferId() );
					onInSession( true );
				}
			}
			else
			{
				// already in session with someone else
				offerSession->getOfferInfo().setOfferResponse( eOfferResponseBusy );
				m_MyApp.getEngine().fromGuiToPluginOfferReply( offerSession->getUserIdent()->getMyOnlineId(), offerSession->getOfferInfo() );
							
				m_OfferMgr.sentOfferReply( m_ePluginType, offerSession->getOfferId(), offerSession->getUserIdent(), eOfferResponseBusy );
			}
		}
	}
}

//======================================================================== 
void OfferSessionLogic::callbackToGuiRxedOfferReply( std::shared_ptr<GuiOfferSession>& offerSession )
{
	if( isOurSessionType( offerSession ) )
	{			
		if( isOurSessionInstance( offerSession ) )
		{
			EOfferResponse offerResponse		= offerSession->getOfferResponse();			
			VxGUID offerSessionId				= offerSession->getOfferId();			
			if( eOfferResponseAccept == offerResponse )
			{
				m_MyApp.playSound( eSndDefOfferAccepted );
				postStatusMsg( (getHisOnlineName() + " Accepted Offer " + describePlugin()).c_str() );

				if( !m_IsInSession )
				{
					m_IsInSession = true;
					m_MyApp.getEngine().fromGuiStartPluginSession( m_ePluginType, m_HisIdent->getMyOnlineId(),
						offerSessionId );
					m_OfferMgr.startedSessionInReply( m_ePluginType, offerSessionId, m_HisIdent );
					onInSession( true );
				}
			}
			else 
			{
				handleSessionEnded( offerResponse );
			}

			if( GuiHelpers::isPluginSingleSession( m_ePluginType ) )
			{
				m_OfferMgr.removePluginSessionOffer( m_ePluginType, m_HisIdent );
			}
		}
	}
}

//======================================================================== 
void OfferSessionLogic::toGuiPluginSessionEnded( std::shared_ptr<GuiOfferSession>& offerSession )
{
	/*
	if( isOurSessionInstance( offerSession ) )
	{
		EOfferResponse offerResponse = offerSession->getOfferResponse();	

        m_OfferMgr.recievedSessionEnd( m_ePluginType, m_OfferId, m_HisIdent, offerResponse );
		handleSessionEnded( offerResponse );
	}
	*/
}

//======================================================================== 
void OfferSessionLogic::toGuiContactOffline( GuiUser* friendIdent ) 
{
	if( m_HisIdent && ( friendIdent->getMyOnlineId() == m_HisIdent->getMyOnlineId() ) )
	{
		//m_OfferMgr.contactWentOffline( m_HisIdent );
		handleSessionEnded( eOfferResponseUserOffline );
	}
}

//========================================================================
bool OfferSessionLogic::sendOfferOrResponse()
{
	if( getIsServerSession() )
	{
		return true;
	}

    if( m_ePluginType < eMaxNetUsePluginType )
	{
		bool sentOk = true;
		if( m_IsOffer )
		{
			getOfferInfo().setOfferResponse( eOfferResponseAccept );
			if( !m_MyApp.getEngine().fromGuiToPluginOfferReply(	m_HisIdent->getMyOnlineId(), getOfferInfo() ) )
			{
				if( ePluginTypeMessenger != m_ePluginType )
				{
					showOfflineMsg();
				}
				else
				{
                    postStatusMsg( "%s is offline-chat disabled", m_HisIdent->getOnlineName().c_str() );
				}

				sentOk = false;
			}
			else
			{
				startPluginSessionIfIsSessionOffer();
			}
		}
		else
		{
			if( !m_MyApp.getEngine().fromGuiMakePluginOffer( m_HisIdent->getMyOnlineId(), getOfferInfo() ) )
			{
				if( ePluginTypeMessenger != m_ePluginType )
				{
					showOfflineMsg();
				}
				else
				{
                    postStatusMsg( "%s is offline-chat disabled", m_HisIdent->getOnlineName().c_str() );
				}

				sentOk = false;
			}
			else
			{
				m_OfferMgr.sentOffer( m_ePluginType, getOfferInfo().getOfferId(), m_HisIdent );
				postStatusMsg( "Waiting Reply Offer %s From %s", describePlugin().c_str(),  getHisOnlineName().c_str() );
			}
		}

		return sentOk;
	}
    else
    {
        LogMsg( LOG_ERROR, "OfferSessionLogic::%s invalid plugin %d", __func__, m_ePluginType );
        vx_assert( false );
    }

	return true;
}

//============================================================================
bool OfferSessionLogic::sendOfferReply( EOfferResponse offerResponse )
{
    if( m_ePluginType < eMaxNetUsePluginType )
	{	
		m_GuiOfferSession->setOfferResponse( offerResponse );
		bool bSentMsg = m_MyApp.getEngine().fromGuiToPluginOfferReply( getHisIdent()->getMyOnlineId(), getOfferInfo() );
		if( false == bSentMsg )
		{
			handleUserWentOffline();
		}
		else
		{
			m_AppletBase->setStatusText( QObject::tr( "Sent Offer Reply" ) );
		}

		return bSentMsg;
	}
    else
    {
        LogMsg( LOG_ERROR, "OfferSessionLogic::%s invalid plugin %d", __func__, m_ePluginType );
        vx_assert( false );
        return false;
    }
}

//============================================================================  
void OfferSessionLogic::onStop()
{
	if( false == m_IsOnStopCalled && m_HisIdent )
	{
		m_IsOnStopCalled = true;
        m_OfferMgr.onSessionExit( m_ePluginType, m_OfferId, m_HisIdent );
		if( false == getIsServerSession() )
		{
			setCallState( eCallStateHangedUp );
			if( false == m_SessionEndIsHandled )
			{
				m_SessionEndIsHandled = true;
				setIsInSession( false );
				if( 0 != m_HisIdent )
				{
                    if( m_ePluginType < eMaxNetUsePluginType )
					{
                        m_OfferMgr.fromGuiToPluginOfferReply( m_ePluginType, m_HisIdent, getOfferInfo(), m_OfferId, eOfferResponseEndSession );
                        m_OfferMgr.sentOfferReply( m_ePluginType, m_OfferId, m_HisIdent, eOfferResponseEndSession );
					}
                    else
                    {
                        LogMsg( LOG_ERROR, "OfferSessionLogic::%s invalid plugin %d", __func__, m_ePluginType );
                        vx_assert( false );
                    }
				}
			}

			if( 0 != m_HisIdent )
			{
                if( m_ePluginType < eMaxNetUsePluginType )
				{
					m_MyApp.getEngine().fromGuiStopPluginSession( 	m_ePluginType, 
																	m_HisIdent->getMyOnlineId(),
                                                                    m_OfferId
																	);		   
				}
                else
                {
                    LogMsg( LOG_ERROR, "OfferSessionLogic::%s invalid plugin %d", __func__, m_ePluginType );
                    vx_assert( false );
                }
			}
		}

		postStatusMsg( (describePlugin() + " with " + getHisOnlineName() + " Has Ended ").c_str() );
	}
	else
	{
        LogMsg( LOG_INFO, "OfferSessionLogic::%s was already called", __func__ );
	}
}

//============================================================================  
bool OfferSessionLogic::startPluginSessionIfIsSessionOffer()
{
	if( isSessionOffer() )
	{
		setIsInSession( true );
		m_MyApp.getEngine().fromGuiStartPluginSession( 	m_ePluginType, 
														m_HisIdent->getMyOnlineId(),
                                                        m_OfferId );
        m_OfferMgr.startedSessionInReply( m_ePluginType, m_OfferId, m_HisIdent );
		onInSession( true );
		return true;
	}

	return false;
}

//============================================================================  
void OfferSessionLogic::setCallState( int eCallState )
{
	switch( eCallState )
	{
	case eCallStateCalling:
		startPhoneRinging();
		break;

	case eCallStateInCall:
		break;

	case eCallStateHangedUp:
		stopPhoneRinging();
		break;

	default:
		break;
	}
}

//========================================================================
bool OfferSessionLogic::handleOfferResponse( GuiOfferSession  poSession )
{
	bool bResponesOk = false;
	if( ( poSession.getPluginType() == m_ePluginType ) &&
		( poSession.getUserIdent()->getMyOnlineId() == m_HisIdent->getMyOnlineId() )) 
	{
		stopPhoneRinging();
		bResponesOk = handleOfferResponseCode( poSession.getOfferResponse()  );
	}

	return bResponesOk;
}

//========================================================================
bool OfferSessionLogic::handleOfferResponseCode( EOfferResponse responseCode )
{
	bool responesOk = false;
	if( responseCode != eOfferResponseAccept )
	{
		std::string strExitReason =  this->describeResponse( responseCode );
		if( GuiHelpers::isPluginSingleSession( m_ePluginType ) )
		{
			m_OfferCallback->onSessionActivityShouldExit( strExitReason.c_str() );
		}
	}
	else
	{
		responesOk = true;
	}

	return responesOk;
}

//============================================================================   
void OfferSessionLogic::handleSessionEnded( std::shared_ptr<GuiOfferSession> offerSession )
{
	if( isOurSessionInstance( offerSession ) ) 
	{
		handleSessionEnded( offerSession->getOfferResponse() );
	}
}

//============================================================================
std::string OfferSessionLogic::getHisOnlineName( void )
{
	return m_HisIdent->getOnlineName();
}

//============================================================================
std::string OfferSessionLogic::describePlugin( void )
{
	return GuiParams::describePlugin( m_ePluginType, m_IsOffer );
}

//============================================================================
void OfferSessionLogic::postStatusMsg( const char* statusMsg, ... )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	char szBuffer[2048];
	szBuffer[0] = 0;
	va_list arg_ptr;
	va_start(arg_ptr, statusMsg);
#ifdef TARGET_OS_WINDOWS
    vsnprintf(szBuffer, 2048, statusMsg,(char *) arg_ptr);
#else
    vsnprintf(szBuffer, 2048, statusMsg, arg_ptr);
#endif // TARGET_OS_WINDOWS
	szBuffer[2047] = 0;
	va_end(arg_ptr);

	if( 0 != szBuffer[0] )
	{
		m_MyApp.toGuiStatusMessage( szBuffer );
	}
}

//============================================================================
void OfferSessionLogic::postUserMsg( const char* userMsg, ... )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	char szBuffer[2048];
	szBuffer[0] = 0;
	va_list arg_ptr;
	va_start(arg_ptr, userMsg);
#ifdef TARGET_OS_WINDOWS
    vsnprintf(szBuffer, 2048, userMsg,(char *) arg_ptr);
#else
    vsnprintf(szBuffer, 2048, userMsg, arg_ptr);
#endif // TARGET_OS_WINDOWS
	szBuffer[2047] = 0;
	va_end(arg_ptr);

	if( 0 != szBuffer[0] )
	{
		m_MyApp.toGuiUserMessage( szBuffer );
	}
}

//============================================================================
std::string OfferSessionLogic::describeResponse( EOfferResponse offerResponse )
{
	std::string responseText = m_HisIdent->getOnlineName();
	responseText += " ";

	switch( offerResponse )
	{
	case eOfferResponseAccept:
		responseText += "Accepted Offer ";
		break;

	case eOfferResponseReject:
		responseText += "Rejected Offer ";
		break;

	case eOfferResponseBusy:
		responseText += "Is Unavailable for ";
		break;

	case eOfferResponseCancelSession:
		responseText += "Canceled Session ";
		break;

	case eOfferResponseEndSession:
		responseText += "Ended Session ";
		break;
	case eOfferResponseUserOffline:
		responseText += "Is Offline And Cannot ";
		break;
	default:
		responseText += "Ended Session ";
	}

	responseText += GuiParams::describePlugin( m_ePluginType, m_IsOffer );
	return responseText;
}

//========================================================================
void OfferSessionLogic::handleSessionEnded( EOfferResponse responseCode )
{
	if( false == m_SessionEndIsHandled )
	{
		m_SessionEndIsHandled = true;
		std::string responseDesc = this->describeResponse( responseCode );

		setCallState( eCallStateHangedUp );
		if( eOfferResponseBusy == responseCode )
		{
			if( ePluginTypeCamServer != m_ePluginType )
			{
				m_MyApp.playSound( eSndDefBusy );
			}
		}

		if( m_IsInSession )
		{
			m_IsInSession = false;
			onInSession( false );
            m_MyApp.getEngine().fromGuiStopPluginSession( m_ePluginType, m_HisIdent->getMyOnlineId(), m_OfferId );
		}
		
		postStatusMsg( responseDesc.c_str() );
		if( ( ePluginTypeMessenger != m_ePluginType )
			&& ( ePluginTypeCamServer != m_ePluginType ) )
		{
			if( GuiHelpers::isPluginSingleSession( m_ePluginType ) )
			{
				m_OfferCallback->onSessionActivityShouldExit( responseDesc.c_str() );
			}
		}
	}
} 

//========================================================================
void OfferSessionLogic::handleUserWentOffline()
{
	setCallState( eCallStateHangedUp );
	showOfflineMsg( true );
}

//========================================================================
void OfferSessionLogic::startPhoneRinging()
{

}

//========================================================================
void OfferSessionLogic::stopPhoneRinging()
{

}

//========================================================================
void OfferSessionLogic::showOfflineMsg( bool bExitWhenClicked )
{
	std::string offlineMsg = m_HisIdent->getOnlineName();
	offlineMsg += " Is Offline.";
	m_MyApp.toGuiStatusMessage( offlineMsg.c_str() );
	//QMessageBox::information( m_Activity, "User Offline", offlineMsg.c_str(), QMessageBox::Ok );
	if( bExitWhenClicked )
	{
		m_OfferCallback->onSessionActivityShouldExit( offlineMsg.c_str() );
	}
}

