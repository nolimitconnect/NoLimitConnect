//============================================================================
// Copyright (C) 2014 Brett R. Jones
// Issued to MIT style license by Brett R. Jones in 2017
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

#include "PluginSessionBase.h"

#include <MediaToolsLib/OpusAudioDecoder.h>

namespace
{
	const int AUDIO_JITTER_QUEUE_DEPTH = 20; // 20 * 60ms = 1.2 seconds of audio buffering
}

//============================================================================
PluginSessionBase::PluginSessionBase()
: m_JitterBuffer( AUDIO_JITTER_QUEUE_DEPTH )
{
}

//============================================================================
PluginSessionBase::PluginSessionBase( std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, EPluginType pluginType )
: m_ePluginType( pluginType )
, m_Ident( netIdent )
, m_Skt( sktBase )
, m_JitterBuffer( AUDIO_JITTER_QUEUE_DEPTH )
{
	m_LclSessionId.initializeWithNewVxGUID();
}

//============================================================================
PluginSessionBase::PluginSessionBase( VxGUID& lclSessionId, std::shared_ptr<VxSktBase>& sktBase, VxNetIdent* netIdent, EPluginType pluginType )
: m_ePluginType( pluginType )
, m_Ident( netIdent )
, m_Skt( sktBase )
, m_LclSessionId( lclSessionId )
, m_eOfferResponse( eOfferResponseNotSet )
, m_JitterBuffer( AUDIO_JITTER_QUEUE_DEPTH )
{
}

//============================================================================
PluginSessionBase::~PluginSessionBase()
{
	delete m_AudioDecoder;
}

//============================================================================
OpusAudioDecoder *	PluginSessionBase::getAudioDecoder( void )		
{ 
	if( nullptr == m_AudioDecoder )
	{
		m_AudioDecoder = new OpusAudioDecoder();
	}

	return m_AudioDecoder;
}

//============================================================================
void PluginSessionBase::setPluginType( EPluginType pluginType  )		
{ 
	m_ePluginType = pluginType; 
}

//============================================================================
EPluginType PluginSessionBase::getPluginType( void )
{ 
	return m_ePluginType; 
}

//============================================================================
void PluginSessionBase::setIdent( VxNetIdent* ident )	
{ 
	m_Ident = ident; 
}

//============================================================================
VxNetIdent* PluginSessionBase::getIdent( void )
{ 
	return m_Ident; 
}

//============================================================================
const char* PluginSessionBase::getOnlineName( void )
{ 
	return m_Ident->getOnlineName(); 
}

//============================================================================
VxGUID& PluginSessionBase::getOnlineId( void )
{ 
	return m_Ident->getMyOnlineId(); 
}

//============================================================================
void PluginSessionBase::setSkt( std::shared_ptr<VxSktBase>& sktBase )		
{ 
	m_Skt = sktBase; 
}

//============================================================================
std::shared_ptr<VxSktBase>& PluginSessionBase::getSkt( void )
{ 
	return m_Skt; 
}

//============================================================================
void PluginSessionBase::setSessionType( EPluginSessionType sessionType )
{
	m_ePluginSessionType = sessionType;
}

//============================================================================
EPluginSessionType PluginSessionBase::getSessionType( void )
{
	return m_ePluginSessionType;
}

//============================================================================
bool  PluginSessionBase::isP2PSession( void )
{
	return m_ePluginSessionType == ePluginSessionTypeP2P ? true : false;
}

//============================================================================
bool  PluginSessionBase::isTxSession( void )
{
	return m_ePluginSessionType == ePluginSessionTypeTx ? true : false;
}

//============================================================================
bool  PluginSessionBase::isRxSession( void )
{
	return m_ePluginSessionType == ePluginSessionTypeRx ? true : false;
}

//============================================================================
void PluginSessionBase::setIsSessionStarted( bool isStarted )	
{ 
	m_bSessionStarted = isStarted; 
}

//============================================================================
bool PluginSessionBase::getIsSessionStarted( void )
{ 
	return m_bSessionStarted; 
}

//============================================================================
void  PluginSessionBase::setIsRmtInitiated( bool isRmtInitiated )
{ 
	m_bRmtInitiatedSession = isRmtInitiated; 
}

//============================================================================
bool  PluginSessionBase::isRmtInitiated( void )					
{ 
	return m_bRmtInitiatedSession; 
}
