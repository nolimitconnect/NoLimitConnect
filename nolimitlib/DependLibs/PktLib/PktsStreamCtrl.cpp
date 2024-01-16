//============================================================================
// Copyright (C) 2003 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PktTypes.h"
#include "PktsStreamCtrl.h"
#include "PktsFileShare.h"

#include <CoreLib/VxDebug.h>

#include <string.h>

//============================================================================
PktStreamCtrlReq::PktStreamCtrlReq()
{
   setPktType( PKT_TYPE_STREAM_CTRL_REQ );
}

//============================================================================
void PktStreamCtrlReq::setFileName( std::string &csName )
{
	if( csName.size() )
	{
		strcpy( (char *)m_FileName, csName.c_str() );
	}
	else
	{
		m_FileName[ 0 ] = 0; 
	}

	uint16_t u16PktLen = ( uint16_t )((sizeof( PktStreamCtrlReq ) - sizeof( m_FileName )) + strlen( m_FileName ) + 1);
	setPktLength( ROUND_TO_16BYTE_BOUNDRY( u16PktLen ) );
}

//============================================================================
void PktStreamCtrlReq::getFileName( std::string &csName )
{
	csName = m_FileName;
}

//============================================================================
PktStreamCtrlReply::PktStreamCtrlReply()
{
	setPktType( PKT_TYPE_STREAM_CTRL_REPLY );
}

//============================================================================
void PktStreamCtrlReply::getFileName( std::string &csName )
{
	csName = m_FileName;
}

//============================================================================
void PktStreamCtrlReply::setFileName( std::string &csName )
{
    if( csName.size() )
    {
		strcpy( (char *)m_FileName, csName.c_str() );
    }
    else
    {
        m_FileName[ 0 ] = 0;
    }

	uint16_t u16PktLen = ( uint16_t)(( sizeof( PktStreamCtrlReply ) - sizeof( m_FileName )) + strlen( m_FileName ) + 1);
	setPktLength( ROUND_TO_16BYTE_BOUNDRY( u16PktLen ) );
}

//============================================================================
uint16_t PktStreamCtrlReply::getemptyLen( void )
{
    return (uint16_t)(sizeof( PktStreamCtrlReq )-VX_MAX_PATH);
}

//============================================================================
const char* DescribeStreamCtrlError( uint16_t streamError )
{
	switch( streamError )
	{
	case PKT_REQ_STATUS_OK:
		return "200 Ok";
	case PKT_REQ_STATUS_CREATED:
		return "201 Created";
	case PKT_REQ_STATUS_ACCEPTED:
		return "202 Accepted";
	case PKT_REQ_ERR_NO_CONTENT:
		return "204 No Content";
	case PKT_REQ_ERR_MOVED_PERM:
		return "301 Moved Permanently";
	case PKT_REQ_ERR_MOVED_TEMP:
		return "301 Moved Temporarily";
	case PKT_REQ_ERR_NOT_MODIFIED:
		return "304 Not Modified";
	case PKT_REQ_ERR_BAD_REQUEST:
		return "400 Bad Request";
	case PKT_REQ_ERR_UNAUTHORIZED:
		return "401 Unauthorized";
	case PKT_REQ_ERR_FORBIDDEN:
		return "403 Forbidden";
	case PKT_REQ_ERR_NOT_FOUND:
		return "404 Not Found";
	case PKT_REQ_ERR_INTERNAL_SERVER_ERR:
		return "500 Internal Server Error";
	case PKT_REQ_ERR_NOT_IMPLEMENTED:
		return "501 Not Implemented";
	case PKT_REQ_ERR_BAD_GATEWAY:
		return "502 Bad Gateway";
	case PKT_REQ_ERR_SERVICE_UNAVAIL:
		return "503 Service Unavailable";
		//custom statuses
	case PKT_REQ_ERR_BANDWITH_LIMIT:
		return "600 Refused because of Bandwidth limit";
	case PKT_REQ_ERR_CONNECT_LIMIT:
		return "601 Refused because of Connection limit";
	case PKT_REQ_ERR_SERVICE_DISABLED:
		return "602	Refused because Service was disabled";
	case PKT_REQ_ERR_ALL_THREADS_BUSY:
		return "602	Refused because all threads are busy";
	default:
		return "Unknown Err";
	}
}


