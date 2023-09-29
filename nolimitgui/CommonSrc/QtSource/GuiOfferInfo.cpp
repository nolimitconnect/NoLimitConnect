//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software 
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiOfferInfo.h"

#include <CoreLib/VxTimer.h>

//============================================================================
GuiOfferInfo::GuiOfferInfo( const GuiOfferInfo& rhs )
	: OfferBaseInfo( rhs )
	, m_HisIdent( rhs.m_HisIdent )
	, m_OfferState( rhs.m_OfferState )
	, m_LastActivityMs( rhs.m_LastActivityMs )
	, m_RmtInitiated( rhs.m_RmtInitiated )
	, m_HasBeenViewed( rhs.m_HasBeenViewed )
	, m_RequiresReply( rhs.m_RequiresReply )
	, m_HasNewResponse( rhs.m_HasNewResponse )
	, m_MissedCalls( rhs.m_MissedCalls )
	, m_MissedMessages( rhs.m_MissedMessages )
{
}

//============================================================================
GuiOfferInfo::GuiOfferInfo( const OfferBaseInfo& rhs )
	: OfferBaseInfo( rhs )
{
}

//============================================================================
GuiOfferInfo& GuiOfferInfo::operator=( const GuiOfferInfo& rhs )
{
	if( this != &rhs )
	{
		*((OfferBaseInfo*)this) = rhs;
		m_HisIdent = rhs.m_HisIdent;
		m_OfferState = rhs.m_OfferState;
		m_LastActivityMs = rhs.m_LastActivityMs;
		m_RmtInitiated = rhs.m_RmtInitiated;
		m_HasBeenViewed = rhs.m_HasBeenViewed;
		m_RequiresReply = rhs.m_RequiresReply;
		m_HasNewResponse = rhs.m_HasNewResponse;
		m_MissedCalls = rhs.m_MissedCalls;
		m_MissedMessages = rhs.m_MissedMessages;
	}

	return *this;
}

//============================================================================
void GuiOfferInfo::setOfferBaseInfo( OfferBaseInfo& offerBaseInfo )
{
	*((OfferBaseInfo*)this) = offerBaseInfo;
}

//============================================================================
void GuiOfferInfo::updateLastActivityTime( void )
{
	m_LastActivityMs = GetHighResolutionTimeMs();
}

//============================================================================
void GuiOfferInfo::addMissedMessage( std::string& missedMsg )
{
	if( !missedMsg.empty() )
	{
		m_MissedMessages.insert( m_MissedMessages.begin(), missedMsg );
	}
	
	m_LastActivityMs = GetHighResolutionTimeMs();
}

//============================================================================
bool GuiOfferInfo::hasMessages( void ) 
{ 
	if( !getOfferMsg().empty() )
	{
		return true;
	}

	if( m_MissedMessages.size() )
	{
		return true;
	}

	return false;
}

//============================================================================
std::string	GuiOfferInfo::getMessagesText( void ) 
{ 
	std::string allMessageText{ "" };
	if( !getOfferMsg().empty() )
	{
		allMessageText = getOfferMsg();
		allMessageText += "\n\n";
	}

	if( m_MissedMessages.size() )
	{
		for( auto& msg : m_MissedMessages )
		{
			allMessageText += msg;
			allMessageText += "\n\n";
		}
	}

	return allMessageText;
}