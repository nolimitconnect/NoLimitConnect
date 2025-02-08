//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PhoneRinger.h"

#include "AppCommon.h"
#include "GuiOfferMgrBase.h"
#include "SoundDefs.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>

#include <QTimer>

namespace {	
    const int 			        RING_COUNT 				= 4; 
	const int 			        RING_ELAPSE_SEC 		= 4; 
	const int 			        RING_TIMEOUT_MS 		= 17000; 
}

//============================================================================
PhoneRinger::PhoneRinger( AppCommon& myApp, GuiOfferMgrBase& offerMgr, QObject *parent )
    : QObject( parent )
    , m_MyApp( myApp )
	, m_OfferMgr( offerMgr )
    , m_RingTimer( new QTimer( this ) )
{
    m_RingTimer->setInterval( 1000 );

	connect( m_RingTimer, SIGNAL(timeout()), this, SLOT(slotOncePerSecondRingTimer()) );
}

//========================================================================
void PhoneRinger::slotOncePerSecondRingTimer()
{
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "PhoneRinger::%s ", __func__ );
	if( 0 != m_RingTimerCycleCnt )
	{
		if( 0 != m_RingTimerSecondCnt )
		{
			m_RingTimerSecondCnt--;
		}

		if( 0 == m_RingTimerSecondCnt )
		{
			m_RingTimerSecondCnt = RING_ELAPSE_SEC;
			if( m_RingTimerCycleCnt )
			{
				m_RingTimerCycleCnt--;
				m_MyApp.playSound( eSndDefPhoneRing1 );
			}
			else
			{
				if( checkForRingTimeout() )
				{
					m_RingTimer->stop();
				}
			}		
		}			
	}	
	else
	{
		if( checkForRingTimeout() )
		{
			m_RingTimer->stop();
		}
	}	
}

//========================================================================
void PhoneRinger::startRinging( std::shared_ptr<GuiOfferSession>& offerSession )
{
	if( doesSessionExist( offerSession->getOfferId() ) )
	{
		LogMsg( LOG_ERROR, "PhoneRinger::%s offer already exists ", __func__ );
		return;
	}

	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );

	m_OfferSessions.emplace_back( offerSession );
	m_RingTimerSecondCnt	= RING_ELAPSE_SEC;	
	m_RingTimerCycleCnt		= RING_COUNT;
	m_RingTimer->start();
}

//========================================================================
void PhoneRinger::stopRinging( std::shared_ptr<GuiOfferSession>& offerSession )
{
	if(LogEnabled(eLogOffer))LogModule( eLogOffer, LOG_VERBOSE, "GuiOfferMgrBase::%s ", __func__ );
	removeSession( offerSession->getOfferId() );
	if( m_OfferSessions.empty() )
	{
		m_RingTimer->stop();
		m_RingTimerCycleCnt					= 0;
		m_RingTimerSecondCnt				= 0;
	}		
}

//========================================================================
bool PhoneRinger::doesSessionExist( VxGUID& offerId )
{
	for( auto session : m_OfferSessions )
	{
		if( offerId == session->getOfferId() )
		{
			return true;
		}
	}

	return false;
}

//========================================================================
bool PhoneRinger::removeSession( VxGUID& offerId )
{
	for( auto iter = m_OfferSessions.begin(); iter != m_OfferSessions.end(); ++iter )
	{
		if( (*iter)->getOfferId() == offerId )
		{
			m_OfferSessions.erase( iter );
			return true;
		}
	}

	return false;
}

//========================================================================
bool PhoneRinger::checkForRingTimeout( void )
{
	auto timeNow = GetGmtTimeMs();
	for( auto iter = m_OfferSessions.begin(); iter != m_OfferSessions.end(); )
	{
        std::shared_ptr<GuiOfferSession> offerSession = *iter;
        if( offerSession->getOfferTimestamp() + RING_TIMEOUT_MS < timeNow )
		{
			iter = m_OfferSessions.erase( iter );		
			m_OfferMgr.phoneRingTimeout( offerSession );				
		}
		else
		{
			iter++;
		}
	}

	if( !m_OfferSessions.empty() )
	{
		// keep ringing till all expire
		m_RingTimerSecondCnt	= RING_ELAPSE_SEC;	
		m_RingTimerCycleCnt		= 1;
	}

	return m_OfferSessions.empty();
}
