//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiOfferSession.h"
#include "GuiOfferInfo.h"

#include "GuiHelpers.h"
#include "GuiParams.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>

//============================================================================
GuiOfferSession::GuiOfferSession(QObject* parent)
: GuiUserSessionBase(parent)
{
}

//============================================================================
GuiOfferSession::GuiOfferSession( const GuiOfferInfo& offerInfo )
: GuiUserSessionBase()
, m_OfferInfo( offerInfo )
{
	m_UniqueId.assureIsValidGUID();
    GuiUserSessionBase::setUserSessionId( m_UniqueId );
	GuiUserSessionBase::setUserIdent( m_OfferInfo.getUser() );	
}

//============================================================================
GuiOfferSession::GuiOfferSession( const GuiOfferSession& rhs)
: GuiUserSessionBase(rhs)
, m_OfferInfo( rhs.m_OfferInfo )
, m_UniqueId( rhs.m_UniqueId )
, m_OfferExpired( rhs.m_OfferExpired )
{
}

//============================================================================
GuiOfferSession& GuiOfferSession::operator=(const GuiOfferSession& rhs)
{
	if( this != &rhs )
	{
		*((GuiUserSessionBase*)this) = *((GuiUserSessionBase*)&rhs);
		m_OfferInfo = rhs.m_OfferInfo;
		m_UniqueId = rhs.m_UniqueId;
		m_OfferExpired = rhs.m_OfferExpired;
	}

	return *this;
}

//======================================================================== 
VxGUID& GuiOfferSession::assuredValidOfferId() 
{
	if( GuiHelpers::isPluginSingleSession( getPluginType() ) )
	{
		if( !m_OfferInfo.getOfferId().isVxGUIDValid() )
		{
			m_OfferInfo.setOfferId( getUser()->getMyOnlineId() );
		}		
	}
	else
	{
		m_OfferInfo.getOfferId().assureIsValidGUID();
	}

	return m_OfferInfo.getOfferId();
}

//============================================================================
std::string GuiOfferSession::describePlugin()
{
	return GuiParams::describePlugin( getPluginType(), getIsRemoteInitiated() );
}

//============================================================================
std::string GuiOfferSession::describeOffer()
{
	if( getUser() )
	{
		std::string offerDesc = getUser()->getOnlineName();
		offerDesc += " Offers ";
		offerDesc += GuiParams::describePlugin( getPluginType(), getIsRemoteInitiated() );
		return offerDesc;
	}
	else
	{
		return QObject::tr( "Offer Uninitialized" ).toUtf8().constData();
	}
}

//============================================================================
std::string GuiOfferSession::getOnlineName()
{
	if( getUser() )
	{
		return getUser()->getOnlineName();
	}
	else
	{
		return QObject::tr( "User Uninitialized" ).toUtf8().constData();
	}
}

//============================================================================
void GuiOfferSession::updateOfferInfo( OfferBaseInfo& offerInfo )
{
	EOfferMgrType offerMgrType = m_OfferInfo.getOfferMgr();
	m_OfferInfo.setOfferBaseInfo( offerInfo );
	if( offerMgrType != eOfferMgrNotSet )
	{
		// maintain offer mgr
		m_OfferInfo.setOfferMgr( offerMgrType );
	}

	emitOfferUpdated();
}

//============================================================================
bool GuiOfferSession::isAvailableAndActiveOffer( void )
{
	if( getUser() )
	{
		if( false == getUser()->isOnline() )
		{
			return false;
		}

		bool avail = false;
		switch( getOfferState() )
		{
		case eOfferStateSending:
		case eOfferStateSent:
		case eOfferStateNeedResponse:
		case eOfferStateRxedByUser:
		case eOfferStateInSession:
			avail = true;
			break;
		default:
			break;
		}

		if( avail )
		{
			avail = !isExpiredOffer();
		}

		return avail;
	}
	else
	{
		LogModule( eLogOffer, LOG_ERROR, " GuiOfferSession::%s null user", __func__ );
		return false;
	}
}

//============================================================================
bool GuiOfferSession::isExpiredOffer( void )
{
	bool offerExpired = m_OfferInfo.isExpiredOffer();
	if( offerExpired != m_OfferExpired )
	{
		m_OfferExpired = offerExpired;
		emit signalOfferUpdated();
	}

	return offerExpired;
}

//============================================================================
VxGUID& GuiOfferSession::getOnlineId( void ) 
{ 
	return m_OfferInfo.getUser()->getMyOnlineId(); 
}

//============================================================================
QString GuiOfferSession::getActiveDescription( void )
{
	QString activeDesc;
	bool offerExpired = m_OfferInfo.isExpiredOffer();
	if( offerExpired )
	{
		activeDesc = QObject::tr( "Expired " );
		activeDesc += getExpireDate();
		activeDesc = QObject::tr( " " );
	}

	if( !getUser()->isOnline() )
	{
		activeDesc = QObject::tr( "User Offline " );
	}

	return activeDesc;
}

//============================================================================
QString GuiOfferSession::getExpireDate( void )
{
	return TimeWithZone::getChatHourMinTimeStamp( m_OfferInfo.getExpiresTime(), false ).c_str();
}
