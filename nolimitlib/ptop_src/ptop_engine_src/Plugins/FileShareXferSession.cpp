//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "FileShareXferSession.h"

#include <stdio.h>

//============================================================================
FileShareXferSession::FileShareXferSession()
: m_FilesToXferList()
, m_FilesXferedList()
, m_FileXferInfo()
, m_iPercentComplete(0)
, m_Skt(nullptr)
, m_SendToId()
, m_Error( 0 )
{
	initLclSessionId();
}

//============================================================================
FileShareXferSession::FileShareXferSession( std::shared_ptr<VxSktBase>& sktBase, VxGUID& sendToId )
: m_FilesToXferList()
, m_FilesXferedList()
, m_FileXferInfo()
, m_iPercentComplete(0)
, m_Skt( sktBase )
, m_SendToId( sendToId )
, m_Error( 0 )
{
	initLclSessionId();
}

//============================================================================
FileShareXferSession::FileShareXferSession( VxGUID& lclSessionId, std::shared_ptr<VxSktBase>& sktBase, VxGUID& sendToId )
: m_FilesToXferList()
, m_FilesXferedList()
, m_FileXferInfo( lclSessionId )
, m_iPercentComplete(0)
, m_Skt( sktBase )
, m_SendToId( sendToId )
, m_Error( 0 )
{
	initLclSessionId();
}

//============================================================================
void FileShareXferSession::reset( void )
{
	m_iPercentComplete = 0;
}

//============================================================================
void FileShareXferSession::initLclSessionId( void )
{
	if( false == m_FileXferInfo.getLclSessionId().isVxGUIDValid() )
	{
		m_FileXferInfo.getLclSessionId().initializeWithNewVxGUID();
	}
}

//============================================================================
bool FileShareXferSession::isXferingFile( void )
{
	if( m_FileXferInfo.m_hFile )
	{
		return true;
	}
	return false;
}
