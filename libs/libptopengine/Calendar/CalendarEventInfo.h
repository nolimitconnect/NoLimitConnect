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

#include <CoreLib/VxGUID.h>
#include <CoreLib/PktBlobEntry.h>
#include <GuiInterface/IDefs.h>

#include <string>
#include <vector>
#include <utility>

//! how an event repeats. eCalendarRecurrenceEveryMonth always repeats on
//! m_RecurrenceDayOfMonth, clamped to the last day of a shorter month
//! ( eg the 31st becomes Feb 28th/29th that month only, then resumes on the
//! 31st in longer months ).
enum ECalendarRecurrence
{
    eCalendarRecurrenceNever       = 0,
    eCalendarRecurrenceEveryDay    = 1,
    eCalendarRecurrenceEveryWeek   = 2,
    eCalendarRecurrenceEveryMonth  = 3,
};

//! who may see/attend an event
enum ECalendarAttendance
{
    eCalendarAttendanceEverybody   = 0, // anyone with sufficient permission to join the host
    eCalendarAttendanceUserList    = 1, // only users on the invite list
};

//! a single event definition owned by a host ( Group / ChatRoom / RandomConnect ).
//! only the host admin may create or modify an event -- see CalendarMgr.
//! for a recurring event this describes the whole series; a specific
//! occurrence is identified by ( eventId, occurrenceStartMs ) -- see
//! getNextOccurrenceStartMs() / getCurrentOrPreviousOccurrenceStartMs().
class CalendarEventInfo
{
public:
    CalendarEventInfo() = default;
    CalendarEventInfo( const CalendarEventInfo& rhs ) = default;
    CalendarEventInfo& operator=( const CalendarEventInfo& rhs ) = default;
    virtual ~CalendarEventInfo() = default;

    VxGUID&                     getEventId( void )                                      { return m_EventId; }
    void                        setEventId( VxGUID& eventId )                           { m_EventId = eventId; }
    void                        generateNewEventId( void )                              { VxGUID::generateNewVxGUID( m_EventId ); }

    VxGUID&                     getHostOnlineId( void )                                 { return m_HostOnlineId; }
    void                        setHostOnlineId( VxGUID& hostOnlineId )                 { m_HostOnlineId = hostOnlineId; }
    EHostType                   getHostType( void )                                     { return m_HostType; }
    void                        setHostType( EHostType hostType )                       { m_HostType = hostType; }

    //! event name/description storage is capped in bytes -- setters silently truncate
    //! ( UTF-8-sequence-safe, see CalendarEventInfo.cpp ) rather than reject, so callers ( wire
    //! deserialize included ) never need to validate length themselves.
    static const size_t         MAX_EVENT_NAME_LEN = 256;
    static const size_t         MAX_EVENT_DESCRIPTION_LEN = 512;

    std::string&                getEventName( void )                                    { return m_EventName; }
    void                        setEventName( const std::string& eventName );
    std::string&                getEventDescription( void )                             { return m_EventDescription; }
    void                        setEventDescription( const std::string& eventDesc );

    //! ms since 1970-01-01 GMT that the FIRST occurrence starts
    int64_t                     getStartTimeMs( void )                                  { return m_StartTimeMs; }
    void                        setStartTimeMs( int64_t startTimeMs )                   { m_StartTimeMs = startTimeMs; }
    //! length of a single occurrence in ms
    int64_t                     getDurationMs( void )                                   { return m_DurationMs; }
    void                        setDurationMs( int64_t durationMs )                     { m_DurationMs = durationMs; }

    ECalendarRecurrence         getRecurrence( void )                                   { return m_Recurrence; }
    void                        setRecurrence( ECalendarRecurrence recurrence )         { m_Recurrence = recurrence; }
    //! only meaningful when getRecurrence() == eCalendarRecurrenceEveryMonth. 1-31.
    uint8_t                     getRecurrenceDayOfMonth( void )                         { return m_RecurrenceDayOfMonth; }
    void                        setRecurrenceDayOfMonth( uint8_t dayOfMonth )           { m_RecurrenceDayOfMonth = dayOfMonth; }
    bool                        isRecurring( void )                                     { return eCalendarRecurrenceNever != m_Recurrence; }

    //! how long after an occurrence ends before its content is purged from the host
    int64_t                     getRetentionMs( void )                                  { return m_RetentionMs; }
    void                        setRetentionMs( int64_t retentionMs )                   { m_RetentionMs = retentionMs; }

    bool                        isVideoPostingsAllowed( void )                          { return m_VideoPostingsAllowed; }
    void                        setVideoPostingsAllowed( bool videoAllowed )            { m_VideoPostingsAllowed = videoAllowed; }

    ECalendarAttendance         getAttendanceMode( void )                               { return m_AttendanceMode; }
    void                        setAttendanceMode( ECalendarAttendance attendanceMode ) { m_AttendanceMode = attendanceMode; }
    std::vector<VxGUID>&        getInviteList( void )                                   { return m_InviteList; }
    void                        addInvitee( VxGUID& onlineId );
    void                        removeInvitee( VxGUID& onlineId );
    //! does NOT check host plugin permission level -- see CalendarMgr::getPluginAccessState-style check
    bool                        isInvited( VxGUID& onlineId );

    VxGUID&                     getCreatedByOnlineId( void )                            { return m_CreatedByOnlineId; }
    void                        setCreatedByOnlineId( VxGUID& onlineId )                { m_CreatedByOnlineId = onlineId; }
    int64_t                     getCreatedTimeMs( void )                                { return m_CreatedTimeMs; }
    void                        setCreatedTimeMs( int64_t createdTimeMs )               { m_CreatedTimeMs = createdTimeMs; }
    int64_t                     getModifiedTimeMs( void )                               { return m_ModifiedTimeMs; }
    void                        setModifiedTimeMs( int64_t modifiedTimeMs )             { m_ModifiedTimeMs = modifiedTimeMs; }

    //=== recurrence math ( CalendarEventInfo.cpp ). all return -1 to mean "no such occurrence". ===//

    //! start time of the next occurrence strictly after afterMs
    //! ( or the first occurrence if afterMs is before the event ever started ).
    int64_t                     getNextOccurrenceStartMs( int64_t afterMs );
    //! start time of the occurrence in progress or most recently ended as of nowMs.
    int64_t                     getCurrentOrPreviousOccurrenceStartMs( int64_t nowMs );
    //! end time ( start + duration ) of the occurrence in progress or most recently ended as of nowMs.
    int64_t                     getCurrentOrPreviousOccurrenceEndMs( int64_t nowMs );
    //! true if nowMs falls within [ occurrence start, occurrence start + duration )
    bool                        isOccurrenceActiveAt( int64_t nowMs );

    //! true if this event's occurrences ever overlap in time with other's occurrences.
    //! checked over a bounded window ( starting at whichever event starts first, see
    //! getOccurrencesUntil() ) rather than generating a recurring series indefinitely --
    //! long enough that a real conflict between day/week/month recurring events will show up
    //! regardless of their relative phase. used by CalendarMgr to reject overlapping events
    //! within the SAME host's calendar -- different host types are never compared against each
    //! other since each hosted plugin instance owns its own separate CalendarDb, so "only allowed
    //! if the host type is different" falls out of that existing scoping for free, no explicit
    //! host-type comparison needed here. see event-calendar design notes.
    bool                        overlapsWith( CalendarEventInfo& other );

    //! the purge/expiry time for content posted to this event right now, given userOverride
    //! ( ePurgeEventHistoryUseHostDefault falls back to getRetentionMs() ). returns
    //! max( occurrenceEnd + purgeMs, nowMs + purgeMs ) so a session still running past its
    //! scheduled end never has its content expire "in the past" -- see event-calendar design
    //! notes on the purge-override mechanism ( recomputing this periodically with an advancing
    //! nowMs, while the caller confirms the session is still actually active, is what makes
    //! the expiry keep sliding forward for as long as the user stays in the session, and stop
    //! ( so it can finally elapse ) the moment they leave / stop checking ). shares this formula
    //! between CalendarMgr ( host ) and CalendarClientMgr ( client ) so both apply it identically.
    int64_t                     computeContentExpiresTime( int64_t nowMs, enum EPurgeEventHistoryType userOverride );

    //! serializes the full event ( including invite list ) into blob, appending at
    //! the blob's current write position -- caller controls resetWrite()/calcPktLen().
    //! used by PktCalendarEventListReply / PktCalendarEventUpdateReq -- see PktsCalendar.h.
    bool                        serializeToBlob( PktBlobEntry& blob );
    //! reads one event ( as written by serializeToBlob() ) from the blob's current
    //! read position -- caller controls resetRead().
    bool                        deserializeFromBlob( PktBlobEntry& blob );

protected:
    //! advance occurrenceStartMs by exactly one recurrence step
    //! ( month-end clamped for eCalendarRecurrenceEveryMonth ). caller must
    //! not call this when getRecurrence() == eCalendarRecurrenceNever.
    int64_t                     stepToNextOccurrence( int64_t occurrenceStartMs );

    //! every occurrence starting before windowEndMs, from this event's own first occurrence
    //! forward, as ( occurrenceStart, occurrenceEnd ) pairs. capped defensively so a degenerate
    //! recurrence can't loop indefinitely -- see overlapsWith().
    std::vector<std::pair<int64_t, int64_t>>  getOccurrencesUntil( int64_t windowEndMs );

    VxGUID                      m_EventId;
    VxGUID                      m_HostOnlineId;
    EHostType                   m_HostType{ eHostTypeUnknown };

    std::string                 m_EventName;
    std::string                 m_EventDescription;

    int64_t                     m_StartTimeMs{ 0 };
    int64_t                     m_DurationMs{ 0 };

    ECalendarRecurrence         m_Recurrence{ eCalendarRecurrenceNever };
    uint8_t                     m_RecurrenceDayOfMonth{ 0 };

    int64_t                     m_RetentionMs{ 0 };

    bool                        m_VideoPostingsAllowed{ false };

    ECalendarAttendance         m_AttendanceMode{ eCalendarAttendanceEverybody };
    std::vector<VxGUID>         m_InviteList;

    VxGUID                      m_CreatedByOnlineId;
    int64_t                     m_CreatedTimeMs{ 0 };
    int64_t                     m_ModifiedTimeMs{ 0 };
};
