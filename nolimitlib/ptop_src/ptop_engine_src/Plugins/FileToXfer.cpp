//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "FileToXfer.h"

//============================================================================
FileToXfer::FileToXfer( FileInfo& fileInfo, VxGUID& lclSessionId, VxGUID& rmtSessionId )
: FileInfo( fileInfo )
, m_LclSessionId( lclSessionId )
, m_RmtSessionId( rmtSessionId )
{
}

//============================================================================
FileToXfer::FileToXfer( const FileToXfer& rhs )
    : FileInfo( rhs )
    , m_LclSessionId( rhs.m_LclSessionId )
    , m_RmtSessionId( rhs.m_RmtSessionId )
    , m_FileOffset( rhs.m_FileOffset )
{

}

//============================================================================
FileToXfer& FileToXfer::operator=(const FileToXfer& rhs) 
{	
	if( &rhs != this )
	{
		*((FileInfo*)this) = rhs;

		m_LclSessionId = rhs.m_LclSessionId;
		m_RmtSessionId = rhs.m_RmtSessionId;
		m_FileOffset = rhs.m_FileOffset;
	}

	return *this;
}

