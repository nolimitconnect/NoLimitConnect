//============================================================================
// Copyright (C) 2023 Brett R. Jones 
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

#include "PktsQueryHostUrl.h"

#include <CoreLib/VxDebug.h>

#include <string.h>

//============================================================================
PktQueryHostUrlReq::PktQueryHostUrlReq()
{
    setPktType( PKT_TYPE_QUERY_HOST_URL_REQ );
    setPktLength( sizeof( PktQueryHostUrlReq ) );
    vx_assert( 0 == ( getPktLength() & 0x0f ) );
    m_SessionId.fillRandom(); 
}

//============================================================================
void PktQueryHostUrlReq::calcPktLen( void )
{
    uint16_t pktLen = ( uint16_t )sizeof( PktQueryHostUrlReq ) - sizeof( PktBlobEntry );
    pktLen += getBlobEntry().getTotalBlobLen();
    setPktLength( ROUND_TO_16BYTE_BOUNDRY( pktLen ) );
    vx_assert( 0 == ( getPktLength() & 0x0f ) );
}

//============================================================================
bool PktQueryHostUrlReq::setNetCmd( std::string& netCmd )
{
    m_BlobEntry.resetWrite();
    bool result = m_BlobEntry.setValue( netCmd );
    calcPktLen();

    return result;
}

//============================================================================
bool PktQueryHostUrlReq::getNetCmd( std::string& netCmd )
{
    m_BlobEntry.resetRead();
    bool result = m_BlobEntry.getValue( netCmd );

    return result;
}

//============================================================================
//============================================================================
PktQueryHostUrlReply::PktQueryHostUrlReply()
{
    setPktType( PKT_TYPE_QUERY_HOST_URL_REPLY );
    setPktLength( sizeof( PktQueryHostUrlReply ) );
    vx_assert( 0 == ( getPktLength() & 0x0f ) );
    m_SessionId.fillRandom(); 
}

//============================================================================
void PktQueryHostUrlReply::calcPktLen( void )
{
    uint16_t pktLen = ( uint16_t )sizeof( PktQueryHostUrlReply ) - sizeof( PktBlobEntry );
    pktLen += getBlobEntry().getTotalBlobLen();
    setPktLength( ROUND_TO_16BYTE_BOUNDRY( pktLen ) );
    vx_assert( 0 == ( getPktLength() & 0x0f ) );
}

//============================================================================
bool PktQueryHostUrlReply::setNetCmd( std::string& netCmd )
{
    m_BlobEntry.resetWrite();
    bool result = m_BlobEntry.setValue( netCmd );
    calcPktLen();

    return result;
}

//============================================================================
bool PktQueryHostUrlReply::getNetCmd( std::string& netCmd )
{
    m_BlobEntry.resetRead();
    bool result = m_BlobEntry.getValue( netCmd );

    return result;
}
