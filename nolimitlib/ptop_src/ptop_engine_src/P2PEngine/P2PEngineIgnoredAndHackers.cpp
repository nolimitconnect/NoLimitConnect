//============================================================================
// Copyright (C) 2009 Brett R. Jones 
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

#include "P2PEngine.h"

#include <NetLib/VxPeerMgr.h>
#include <NetLib/VxSktBase.h>

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <array>

//============================================================================
void P2PEngine::hackerOffense(  EHackerLevel	hackerLevel,
                                EHackerReason   hackerReason,
                                InetAddress		ipAddr,					// ip address if identity not known
                                VxGUID          signatureGuid,		// users identity info ( may be null if not known then use ipAddress )
                                const char*     pMsg, ... )			// message about the offense
{
    std::array< char, 4096 > szBuffer;
    szBuffer.data()[ 0 ] = 0;
    if( pMsg )
    {
        va_list argList;
        va_start( argList, pMsg );
        vsnprintf( szBuffer.data(), szBuffer.size(), pMsg, argList);
        szBuffer[ szBuffer.size() - 1 ] = 0;
        va_end( argList );
    }

    std::string strIp = ipAddr.toStdString();
    LogModule( eLogHackers, LOG_SEVERE, "%s %s: ip %s %s", DescribeHackerLevel( hackerLevel ), DescribeHackerReason( hackerReason ), strIp.c_str(), szBuffer.data() );
    if( hackerLevel >= eHackerLevelMedium )
    {
        getPeerMgr().addHackOffense( hackerLevel, hackerReason, strIp, signatureGuid );
    }
}

//============================================================================
//! called if hacker offense is detected
void P2PEngine::hackerOffense(	EHackerLevel	hackerLevel,			
                                EHackerReason	hackerReason,	
                                VxNetIdent*	    poContactIdent,			// users identity info ( may be null if not known then use ipAddress )                             
								InetAddress		IpAddr,					// ip address if identity not known
								const char*	    pMsg, ... )				// message about the offense
{
	std::array< char, 4096 > szBuffer;
    szBuffer.data()[0] = 0;
    if( pMsg )
    {
        va_list argList;
        va_start( argList, pMsg );
        vsnprintf( szBuffer.data(), szBuffer.size(), pMsg, argList);
        szBuffer[ szBuffer.size() - 1 ] = 0;
        va_end( argList );
    }

    InetAddress oIpAddr = IpAddr;
    if( poContactIdent )
    {
        oIpAddr = poContactIdent->getOnlineIpAddress( false );
    }

	std::string strIp = oIpAddr.toStdString();
    LogModule( eLogHackers, LOG_SEVERE, "%s %s: ip %s %s", DescribeHackerLevel( hackerLevel ), DescribeHackerReason( hackerReason ), strIp.c_str(), szBuffer.data() );
    if( hackerLevel >= eHackerLevelMedium )
    {
        VxGUID emptyGuid;
        getPeerMgr().addHackOffense( hackerLevel, hackerReason, strIp, poContactIdent ? poContactIdent->getMyOnlineId() : emptyGuid );
    }
}

//============================================================================
//! called if hacker offense is detected
void P2PEngine::hackerOffense(	EHackerLevel	hackerLevel,			    
                                EHackerReason   hackerReason,
                                VxPktHdr*       pktHdr,		
                                std::shared_ptr<VxSktBase>&      sktBase,
                                const char*	    pMsg, ... )				// message about the offense
{
	std::array< char, 4096 > szBuffer;
    szBuffer.data()[0] = 0;
    if( pMsg )
    {
        va_list argList;
        va_start( argList, pMsg );
        vsnprintf( szBuffer.data(), szBuffer.size(), pMsg, argList);
        szBuffer[ szBuffer.size() - 1 ] = 0;
        va_end( argList );
    }

    LogModule( eLogHackers, LOG_SEVERE, "%s %s: %s", DescribeHackerLevel( hackerLevel ),  DescribeHackerReason( hackerReason ), szBuffer.data() );

    if( hackerLevel >= eHackerLevelMedium )
    {
        std::string ipAddr = sktBase->getRemoteIpAddress();
        getPeerMgr().addHackOffense( hackerLevel, hackerReason, ipAddr, pktHdr->getSrcOnlineId() );
    }
}
