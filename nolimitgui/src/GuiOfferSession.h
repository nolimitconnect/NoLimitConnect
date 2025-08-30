#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#if defined(TARGET_OS_LINUX)
#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds
#endif // defined(TARGET_OS_LINUX)

#include "GuiUserSessionBase.h"
#include "GuiOfferInfo.h"

#include <CoreLib/VxGUID.h>
#include <CoreLib/VxSha1Hash.h>
#include <PktLib/VxCommon.h>


class AppCommon;
class GuiUser;
class GuiOfferSession;
class P2PEngine;

// offer from remote friend
class GuiOfferSession : public GuiUserSessionBase
{
	Q_OBJECT

public:
    GuiOfferSession(QObject* parent = nullptr);
    virtual ~GuiOfferSession() = default;
	GuiOfferSession( const GuiOfferInfo& offerInfo );
	GuiOfferSession( const GuiOfferSession& rhs );
    GuiOfferSession& operator=(const GuiOfferSession& rhs);

	VxGUID&						assuredValidOfferId( void );

	GuiUser*					getUser( void )								{ return m_OfferInfo.getUser(); }
	VxGUID&						getOnlineId( void );
	VxGUID&						getCreatorOnlineId( void )					{ return m_OfferInfo.getCreatorId(); }

	EPluginType					getPluginType( void )						{ return m_OfferInfo.getPluginType(); }

	EOfferMgrType				getOfferMgr( void )							{ return m_OfferInfo.getOfferMgr(); }

	// offer id might not be unique. Is online id for single session plugins
	VxGUID&					    getOfferId( void )							{ return m_OfferInfo.getOfferId(); }

	void						setUniqueId( VxGUID& uniqueId )				{ m_UniqueId = uniqueId; }
	VxGUID&						getUniqueId( void )							{ return m_UniqueId; }

	void						setOfferResponse( EOfferResponse offerResponse ) { m_OfferInfo.setOfferResponse( offerResponse ); emitOfferUpdated(); }
	EOfferResponse				getOfferResponse( void )					{ return m_OfferInfo.getOfferResponse(); }

	void						setOfferInfo( GuiOfferInfo& offerInfo )		{ m_OfferInfo = offerInfo; emitOfferUpdated(); }
	GuiOfferInfo&				getOfferInfo( void )						{ return m_OfferInfo; }

	void						setIsRemoteInitiated( bool rmtInitiated )	{ m_OfferInfo.setIsRemoteInitiated( rmtInitiated ); emitOfferUpdated(); }
	bool						getIsRemoteInitiated( void )				{ return m_OfferInfo.getIsRemoteInitiated(); }

	std::string&				getOfferMsg( void )							{ return m_OfferInfo.getOfferMsg(); }

	void						addMissedMessage( std::string& missedMsg )  { m_OfferInfo.addMissedMessage( missedMsg ); emitOfferUpdated(); }
	std::vector<std::string>&	getMissedMessages( void )					{ return m_OfferInfo.getMissedMessages(); }

	bool						hasMessages( void )							{ return m_OfferInfo.hasMessages(); }
	std::string					getMessagesText( void )						{ return m_OfferInfo.getMessagesText(); }

	void						setRequiresReply( bool requiresReply )		{ m_OfferInfo.setRequiresReply( requiresReply ); emitOfferUpdated(); }
	bool						getRequiresReply( void )					{ return m_OfferInfo.getRequiresReply(); }

	bool						isOfferAccepted( void )						{ return m_OfferInfo.isOfferAccepted(); }
	bool						isOfferRejected( void )						{ return m_OfferInfo.isOfferRejected(); }

	void						setIsMissedCall( void )						{ m_OfferInfo.setIsMissedCall(); emitOfferUpdated(); }
	void						setMissedCallsCnt( int missedCnt )			{ m_OfferInfo.setMissedCallsCnt( missedCnt ); emitOfferUpdated(); }
	int							getMissedCallsCnt( void )					{ return m_OfferInfo.getMissedCallsCnt(); }

	void 						setOfferState( EOfferState offerState )		{ m_OfferInfo.setOfferState( offerState ); emitOfferUpdated(); }
	EOfferState					getOfferState( void )						{ return m_OfferInfo.getOfferState(); }

	int64_t						getExpiresTime( void )						{ return m_OfferInfo.getExpiresTime(); }
	bool						isExpiredOffer( void );

	// return true if voice or video chat or truth or game
	bool						isPhoneCall( void )							{ return m_OfferInfo.isPhoneTypePlugin(); }

	void						updateOfferInfo( OfferBaseInfo& offerInfo );
	bool						isAvailableAndActiveOffer( void );

	std::string 				describePlugin( void );
	std::string 				describeOffer( void );
	std::string					getOnlineName( void );

	QString						getActiveDescription( void );
	QString						getExpireDate( void );

	void						emitOfferUpdated( void )					{ emit signalOfferUpdated(); };

    void						setOfferTimestamp( int64_t timems )			{ m_OfferInfo.setOfferTimestamp( timems ); }
	int64_t						getOfferTimestamp( void )					{ return m_OfferInfo.getOfferTimestamp(); }

	void						setOfferResponseTimestamp( int64_t timems ) { m_OfferInfo.setOfferResponseTimestamp( timems ); }
	int64_t						getOfferResponsTimestamp( void )			{ return m_OfferInfo.getOfferResponseTimestamp(); }

signals:
	void						signalOfferUpdated( void );

protected:
	//=== vars ===//
	GuiOfferInfo				m_OfferInfo;
	VxGUID						m_UniqueId;
	bool						m_OfferExpired{ false };

};
