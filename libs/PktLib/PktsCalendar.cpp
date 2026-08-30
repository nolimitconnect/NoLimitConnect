//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PktsCalendar.h"

#include <CoreLib/VxDebug.h>

//============================================================================
PktCalendarEventListReq::PktCalendarEventListReq()
{
    setPktType( PKT_TYPE_CALENDAR_EVENT_LIST_REQ );
    setPktLength( sizeof( PktCalendarEventListReq ) );
    vx_assert( 0 == ( getPktLength() & 0x0f ) );
}

//============================================================================
PktCalendarEventListReply::PktCalendarEventListReply()
{
    setPktType( PKT_TYPE_CALENDAR_EVENT_LIST_REPLY );
    setPktLength( ROUND_TO_16BYTE_BOUNDRY( sizeof( PktCalendarEventListReply ) ) );
    vx_assert( 0 == ( getPktLength() & 0x0f ) );
}

//============================================================================
void PktCalendarEventListReply::calcPktLen( void )
{
    uint16_t pktLen = ( uint16_t )sizeof( PktCalendarEventListReply ) - sizeof( PktBlobEntry );
    pktLen += getBlobEntry().getTotalBlobLen();
    setPktLength( ROUND_TO_16BYTE_BOUNDRY( pktLen ) );
    vx_assert( 0 == ( getPktLength() & 0x0f ) );
}

//============================================================================
PktCalendarEventUpdateReq::PktCalendarEventUpdateReq()
{
    setPktType( PKT_TYPE_CALENDAR_EVENT_UPDATE_REQ );
    setPktLength( ROUND_TO_16BYTE_BOUNDRY( sizeof( PktCalendarEventUpdateReq ) ) );
    vx_assert( 0 == ( getPktLength() & 0x0f ) );
}

//============================================================================
void PktCalendarEventUpdateReq::calcPktLen( void )
{
    uint16_t pktLen = ( uint16_t )sizeof( PktCalendarEventUpdateReq ) - sizeof( PktBlobEntry );
    pktLen += getBlobEntry().getTotalBlobLen();
    setPktLength( ROUND_TO_16BYTE_BOUNDRY( pktLen ) );
    vx_assert( 0 == ( getPktLength() & 0x0f ) );
}

//============================================================================
PktCalendarEventUpdateReply::PktCalendarEventUpdateReply()
{
    setPktType( PKT_TYPE_CALENDAR_EVENT_UPDATE_REPLY );
    setPktLength( sizeof( PktCalendarEventUpdateReply ) );
    vx_assert( 0 == ( getPktLength() & 0x0f ) );
}

//============================================================================
PktCalendarEventCancelReq::PktCalendarEventCancelReq()
{
    setPktType( PKT_TYPE_CALENDAR_EVENT_CANCEL_REQ );
    setPktLength( sizeof( PktCalendarEventCancelReq ) );
    vx_assert( 0 == ( getPktLength() & 0x0f ) );
}

//============================================================================
PktCalendarEventCancelReply::PktCalendarEventCancelReply()
{
    setPktType( PKT_TYPE_CALENDAR_EVENT_CANCEL_REPLY );
    setPktLength( sizeof( PktCalendarEventCancelReply ) );
    vx_assert( 0 == ( getPktLength() & 0x0f ) );
}
