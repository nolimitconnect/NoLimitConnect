#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
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

#include <OfferBase/OfferBaseInfo.h>

#include <vector>

class GuiUser;

class GuiOfferInfo : public OfferBaseInfo
{
public:
	GuiOfferInfo() = default;
	~GuiOfferInfo() override = default;
	GuiOfferInfo( const GuiOfferInfo& rhs );
    GuiOfferInfo( const OfferBaseInfo& rhs );

    GuiOfferInfo& operator=( const GuiOfferInfo& rhs );

	void						setOfferBaseInfo( OfferBaseInfo& offerBaseInfo );

	void 						setUser( GuiUser* guiUser )				{ m_HisIdent = guiUser; }
	GuiUser*					getUser()								{ return m_HisIdent; }

	void 						setOfferState( EOfferState offerState ) { m_OfferState = offerState; }
	EOfferState					getOfferState()							{ return m_OfferState; }

	void						setIsMissedCall( void )					{ m_MissedCalls++; }
	void						setMissedCallsCnt( int missedCnt )		{ m_MissedCalls = missedCnt; }
	int							getMissedCallsCnt( void )				{ return m_MissedCalls; }

	void						addMissedMessage( std::string& missedMsg );
	std::vector<std::string>&	getMissedMessages( void )				{ return m_MissedMessages; }

	bool						hasMessages( void );
	std::string					getMessagesText( void );

	void						setRequiresReply( bool requiresReply )	{ m_RequiresReply = requiresReply; }
	bool						getRequiresReply( void )				{ return m_RequiresReply; }

	bool						isOfferAccepted( void )					{ return (m_OfferResponse == eOfferResponseAccept); }
	bool						isOfferRejected( void )					{ return (m_OfferResponse == eOfferResponseReject) || (m_OfferResponse == eOfferResponseEndSession); }

	void						updateLastActivityTime( void );

protected:
    GuiUser*					m_HisIdent{ nullptr };
	EOfferState					m_OfferState{ eOfferStateNone };
	int64_t						m_LastActivityMs{ 0 };
	bool						m_RequiresReply{ false };
	bool						m_HasNewResponse{ false };
	int							m_MissedCalls{ 0 };

	std::vector<std::string>	m_MissedMessages;
};
