#pragma once
//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "PktTypes.h"
#include <CoreLib/PktBlobEntry.h>

#include <GuiInterface/IDefs.h>

// NOTE: PktLib has no dependency on libptopengine, so these packets expose only the raw
// PktBlobEntry -- CalendarEventInfo::serializeToBlob()/deserializeFromBlob() ( libptopengine,
// Calendar/CalendarEventInfo.h ) is what actually reads/writes an event to/from that blob.
// callers ( CalendarMgr ) are expected to check PktBlobEntry::haveRoom() before serializing
// another event into PktCalendarEventListReply so a packet is never left half-written.

#pragma pack(push)
#pragma pack(1)

//! client -> host: fetch the full calendar for a host just joined ( or being resynced ).
//! see event-calendar design notes: calendar is fetched on join, never visible before.
class PktCalendarEventListReq : public VxPktHdr
{
public:
    PktCalendarEventListReq();

    void                        setHostType( EHostType hostType )   { m_HostType = ( uint8_t )hostType; }
    EHostType                   getHostType( void ) const           { return ( EHostType )m_HostType; }

    void                        setSessionId( VxGUID& guid )        { m_SessionId = guid; }
    VxGUID&                     getSessionId( void )                { return m_SessionId; }

    void                        setHostOnlineId( VxGUID& guid )     { m_HostOnlineId = guid; }
    VxGUID&                     getHostOnlineId( void )             { return m_HostOnlineId; }

    //! number of ( visible-to-this-requester ) events to skip before filling this reply --
    //! 0 for the first request in a fetch cycle. lets a client resume where the previous
    //! reply's chunk left off instead of always re-reading from the start of the calendar --
    //! see CalendarMgr::onPktCalendarEventListReq / CalendarClientMgr's chunked-refresh loop.
    void                        setSkipCount( uint32_t skipCount )  { m_SkipCount = skipCount; }
    uint32_t                    getSkipCount( void ) const          { return m_SkipCount; }

private:
    uint8_t                     m_HostType{ 0 };
    uint8_t                     m_Res1{ 0 };
    uint16_t                    m_Res2{ 0 };
    uint32_t                    m_SkipCount{ 0 };
    VxGUID                      m_SessionId;
    VxGUID                      m_HostOnlineId;
};

//! host -> client: one packet's worth of events, serialized back-to-back into the blob via
//! CalendarEventInfo::serializeToBlob(). if getMoreEventsExist() is true, not everything fit
//! and the client should not assume the calendar is complete -- it should send another
//! PktCalendarEventListReq with setSkipCount() advanced by this reply's getEventCountThisPkt(),
//! up to CalendarClientMgr::MAX_LIST_REQUESTS_PER_HOST total requests for one fetch cycle ( a
//! defensive cap -- a host that keeps claiming MoreEventsExist forever, whether from an
//! unreasonably large calendar or deliberate abuse, cannot make a client hammer it indefinitely ).
class PktCalendarEventListReply : public VxPktHdr
{
public:
    PktCalendarEventListReply();

    void                        setHostType( EHostType hostType )   { m_HostType = ( uint8_t )hostType; }
    EHostType                   getHostType( void ) const           { return ( EHostType )m_HostType; }

    void                        setCommError( enum ECommErr commError ) { m_CommError = ( uint8_t )commError; }
    ECommErr                    getCommError( void ) const          { return ( ECommErr )m_CommError; }

    void                        setSessionId( VxGUID& guid )        { m_SessionId = guid; }
    VxGUID&                     getSessionId( void )                { return m_SessionId; }

    void                        setHostOnlineId( VxGUID& guid )     { m_HostOnlineId = guid; }
    VxGUID&                     getHostOnlineId( void )             { return m_HostOnlineId; }

    void                        setEventCountThisPkt( uint16_t count ) { m_EventThisPktCount = count; }
    uint16_t                    getEventCountThisPkt( void )        { return m_EventThisPktCount; }
    void                        incrementEventCount( void )         { m_EventThisPktCount++; }

    void                        setMoreEventsExist( bool moreExist ) { m_MoreEventsExist = moreExist ? 1 : 0; }
    bool                        getMoreEventsExist( void )          { return 0 != m_MoreEventsExist; }

    PktBlobEntry&               getBlobEntry( void )                { return m_BlobEntry; }

    void                        calcPktLen( void );

private:
    uint8_t                     m_HostType{ 0 };
    uint8_t                     m_CommError{ 0 };
    uint8_t                     m_MoreEventsExist{ 0 };
    uint8_t                     m_Res1{ 0 };
    uint16_t                    m_EventThisPktCount{ 0 };
    uint16_t                    m_Res2{ 0 };
    VxGUID                      m_SessionId;
    VxGUID                      m_HostOnlineId;
    PktBlobEntry                m_BlobEntry;
};

//! client ( host admin only ) -> host: create ( event id left invalid ) or edit ( existing
//! event id ) one event. host is the sole authority -- see event-calendar design notes.
class PktCalendarEventUpdateReq : public VxPktHdr
{
public:
    PktCalendarEventUpdateReq();

    void                        setHostType( EHostType hostType )   { m_HostType = ( uint8_t )hostType; }
    EHostType                   getHostType( void ) const           { return ( EHostType )m_HostType; }

    void                        setSessionId( VxGUID& guid )        { m_SessionId = guid; }
    VxGUID&                     getSessionId( void )                { return m_SessionId; }

    void                        setHostOnlineId( VxGUID& guid )     { m_HostOnlineId = guid; }
    VxGUID&                     getHostOnlineId( void )             { return m_HostOnlineId; }

    PktBlobEntry&               getBlobEntry( void )                { return m_BlobEntry; }

    void                        calcPktLen( void );

private:
    uint8_t                     m_HostType{ 0 };
    uint8_t                     m_Res1{ 0 };
    uint16_t                    m_Res2{ 0 };
    uint32_t                    m_Res3{ 0 };
    VxGUID                      m_SessionId;
    VxGUID                      m_HostOnlineId;
    PktBlobEntry                m_BlobEntry;
};

class PktCalendarEventUpdateReply : public VxPktHdr
{
public:
    PktCalendarEventUpdateReply();

    void                        setHostType( EHostType hostType )   { m_HostType = ( uint8_t )hostType; }
    EHostType                   getHostType( void ) const           { return ( EHostType )m_HostType; }

    void                        setCommError( enum ECommErr commError ) { m_CommError = ( uint8_t )commError; }
    ECommErr                    getCommError( void ) const          { return ( ECommErr )m_CommError; }

    void                        setSessionId( VxGUID& guid )        { m_SessionId = guid; }
    VxGUID&                     getSessionId( void )                { return m_SessionId; }

    void                        setHostOnlineId( VxGUID& guid )     { m_HostOnlineId = guid; }
    VxGUID&                     getHostOnlineId( void )             { return m_HostOnlineId; }

    //! the created/updated event's id ( so the client learns the id assigned to a new event )
    void                        setEventId( VxGUID& guid )          { m_EventId = guid; }
    VxGUID&                     getEventId( void )                  { return m_EventId; }

private:
    uint8_t                     m_HostType{ 0 };
    uint8_t                     m_CommError{ 0 };
    uint16_t                    m_Res1{ 0 };
    uint32_t                    m_Res2{ 0 };
    VxGUID                      m_SessionId;
    VxGUID                      m_HostOnlineId;
    VxGUID                      m_EventId;
};

//! client ( host admin only ) -> host: cancel/delete the whole event series by id.
class PktCalendarEventCancelReq : public VxPktHdr
{
public:
    PktCalendarEventCancelReq();

    void                        setHostType( EHostType hostType )   { m_HostType = ( uint8_t )hostType; }
    EHostType                   getHostType( void ) const           { return ( EHostType )m_HostType; }

    void                        setSessionId( VxGUID& guid )        { m_SessionId = guid; }
    VxGUID&                     getSessionId( void )                { return m_SessionId; }

    void                        setHostOnlineId( VxGUID& guid )     { m_HostOnlineId = guid; }
    VxGUID&                     getHostOnlineId( void )             { return m_HostOnlineId; }

    void                        setEventId( VxGUID& guid )          { m_EventId = guid; }
    VxGUID&                     getEventId( void )                  { return m_EventId; }

private:
    uint8_t                     m_HostType{ 0 };
    uint8_t                     m_Res1{ 0 };
    uint16_t                    m_Res2{ 0 };
    uint32_t                    m_Res3{ 0 };
    VxGUID                      m_SessionId;
    VxGUID                      m_HostOnlineId;
    VxGUID                      m_EventId;
};

class PktCalendarEventCancelReply : public VxPktHdr
{
public:
    PktCalendarEventCancelReply();

    void                        setHostType( EHostType hostType )   { m_HostType = ( uint8_t )hostType; }
    EHostType                   getHostType( void ) const           { return ( EHostType )m_HostType; }

    void                        setCommError( enum ECommErr commError ) { m_CommError = ( uint8_t )commError; }
    ECommErr                    getCommError( void ) const          { return ( ECommErr )m_CommError; }

    void                        setSessionId( VxGUID& guid )        { m_SessionId = guid; }
    VxGUID&                     getSessionId( void )                { return m_SessionId; }

    void                        setHostOnlineId( VxGUID& guid )     { m_HostOnlineId = guid; }
    VxGUID&                     getHostOnlineId( void )             { return m_HostOnlineId; }

    void                        setEventId( VxGUID& guid )          { m_EventId = guid; }
    VxGUID&                     getEventId( void )                  { return m_EventId; }

private:
    uint8_t                     m_HostType{ 0 };
    uint8_t                     m_CommError{ 0 };
    uint16_t                    m_Res1{ 0 };
    uint32_t                    m_Res2{ 0 };
    VxGUID                      m_SessionId;
    VxGUID                      m_HostOnlineId;
    VxGUID                      m_EventId;
};

#pragma pack(pop)
