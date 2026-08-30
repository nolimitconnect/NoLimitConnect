//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "CalendarEventInfo.h"

#include <CoreLib/VxTimeDefs.h>

#include <time.h>

namespace
{
    //! true for a proleptic-Gregorian leap year
    bool isLeapYear( int year )
    {
        return ( ( 0 == year % 4 ) && ( 0 != year % 100 ) ) || ( 0 == year % 400 );
    }

    //! days in month ( 1-12 ) for the given year
    int getDaysInMonth( int year, int month1to12 )
    {
        static const int daysInMonthArr[ 12 ] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        if( month1to12 < 1 || month1to12 > 12 )
        {
            return 30; // defensive fallback; should not happen
        }

        int days = daysInMonthArr[ month1to12 - 1 ];
        if( 2 == month1to12 && isLeapYear( year ) )
        {
            days = 29;
        }

        return days;
    }

    //! comfortably longer than any single recurrence period ( day/week/month ) -- if two
    //! recurring events conflict at all, the conflict shows up somewhere within this window
    //! of whichever event starts first, regardless of their relative phase.
    const int64_t OVERLAP_CHECK_WINDOW_MS = 400LL * 24 * 60 * 60 * 1000;
    //! defensive cap on generated occurrences ( comfortably covers 400 days of even a daily
    //! recurrence -- this should never actually be hit ).
    const int MAX_OVERLAP_OCCURRENCES = 2000;

    //! truncates str to at most maxBytes bytes without cutting a multi-byte UTF-8 sequence in
    //! half. resize() first ( cheap byte-count cut ), then walk back over any dangling
    //! continuation bytes ( 10xxxxxx ) to find the start of the last, now-incomplete code point,
    //! and check whether that lead byte's full sequence actually fit -- if not, drop it too.
    void truncateUtf8ToMaxBytes( std::string& str, size_t maxBytes )
    {
        if( str.size() <= maxBytes )
        {
            return;
        }

        str.resize( maxBytes );

        size_t seqStart = str.size();
        while( seqStart > 0 && ( ( (unsigned char)str[ seqStart - 1 ] ) & 0xC0 ) == 0x80 )
        {
            --seqStart;
        }

        if( seqStart > 0 )
        {
            unsigned char leadByte = (unsigned char)str[ seqStart - 1 ];
            size_t seqLen = 1;
            if( ( leadByte & 0xE0 ) == 0xC0 )      { seqLen = 2; }
            else if( ( leadByte & 0xF0 ) == 0xE0 ) { seqLen = 3; }
            else if( ( leadByte & 0xF8 ) == 0xF0 ) { seqLen = 4; }

            if( ( seqStart - 1 ) + seqLen > str.size() )
            {
                // the lead byte's sequence got cut short by the byte-count resize -- the whole
                // ( incomplete ) code point has to go, not just its missing continuation bytes.
                str.resize( seqStart - 1 );
            }
        }
    }
}

//============================================================================
void CalendarEventInfo::setEventName( const std::string& eventName )
{
    m_EventName = eventName;
    truncateUtf8ToMaxBytes( m_EventName, MAX_EVENT_NAME_LEN );
}

//============================================================================
void CalendarEventInfo::setEventDescription( const std::string& eventDesc )
{
    m_EventDescription = eventDesc;
    truncateUtf8ToMaxBytes( m_EventDescription, MAX_EVENT_DESCRIPTION_LEN );
}

//============================================================================
void CalendarEventInfo::addInvitee( VxGUID& onlineId )
{
    if( !isInvited( onlineId ) )
    {
        m_InviteList.push_back( onlineId );
    }
}

//============================================================================
void CalendarEventInfo::removeInvitee( VxGUID& onlineId )
{
    for( auto iter = m_InviteList.begin(); iter != m_InviteList.end(); ++iter )
    {
        if( *iter == onlineId )
        {
            m_InviteList.erase( iter );
            return;
        }
    }
}

//============================================================================
bool CalendarEventInfo::isInvited( VxGUID& onlineId )
{
    for( auto& inviteeId : m_InviteList )
    {
        if( inviteeId == onlineId )
        {
            return true;
        }
    }

    return false;
}

//============================================================================
int64_t CalendarEventInfo::stepToNextOccurrence( int64_t occurrenceStartMs )
{
    switch( m_Recurrence )
    {
    case eCalendarRecurrenceEveryDay:
        return occurrenceStartMs + DAY_OF_MIILISECONDS;

    case eCalendarRecurrenceEveryWeek:
        return occurrenceStartMs + WEEK_OF_MIILISECONDS;

    case eCalendarRecurrenceEveryMonth:
    {
        time_t sec = ( time_t )( occurrenceStartMs / 1000 );
        int64_t msRemainder = occurrenceStartMs % 1000;
        struct tm tmVal = *gmtime( &sec ); // copy out of gmtime's static buffer before mutating

        tmVal.tm_mon += 1;
        if( tmVal.tm_mon > 11 )
        {
            tmVal.tm_mon = 0;
            tmVal.tm_year += 1;
        }

        int daysInTargetMonth = getDaysInMonth( tmVal.tm_year + 1900, tmVal.tm_mon + 1 );
        int targetDay = m_RecurrenceDayOfMonth ? ( int )m_RecurrenceDayOfMonth : 1;
        if( targetDay > daysInTargetMonth )
        {
            targetDay = daysInTargetMonth; // clamp to last day of month ( eg 31st -> Feb 28th/29th )
        }

        tmVal.tm_mday = targetDay;

        time_t newSec = timegm( &tmVal ); // portable timegm() supplied for Windows by VxFunctionsMissingInWindows.cpp
        return ( ( int64_t )newSec * 1000 ) + msRemainder;
    }

    case eCalendarRecurrenceNever:
    default:
        return occurrenceStartMs; // no next occurrence; callers must treat "no progress" as "stop"
    }
}

//============================================================================
int64_t CalendarEventInfo::getNextOccurrenceStartMs( int64_t afterMs )
{
    if( afterMs < m_StartTimeMs )
    {
        return m_StartTimeMs; // first occurrence hasn't happened yet
    }

    switch( m_Recurrence )
    {
    case eCalendarRecurrenceNever:
        return -1; // already had its only occurrence

    case eCalendarRecurrenceEveryDay:
    {
        int64_t stepsPassed = ( afterMs - m_StartTimeMs ) / DAY_OF_MIILISECONDS;
        return m_StartTimeMs + ( stepsPassed + 1 ) * DAY_OF_MIILISECONDS;
    }

    case eCalendarRecurrenceEveryWeek:
    {
        int64_t stepsPassed = ( afterMs - m_StartTimeMs ) / WEEK_OF_MIILISECONDS;
        return m_StartTimeMs + ( stepsPassed + 1 ) * WEEK_OF_MIILISECONDS;
    }

    case eCalendarRecurrenceEveryMonth:
    {
        // month lengths vary so this can't be done with fixed-size division; bounded
        // loop is fine here ( at most a few hundred/thousand steps even for old events ).
        int64_t occurrenceStartMs = m_StartTimeMs;
        while( occurrenceStartMs <= afterMs )
        {
            int64_t nextMs = stepToNextOccurrence( occurrenceStartMs );
            if( nextMs <= occurrenceStartMs )
            {
                return -1; // defensive; should not happen
            }
            occurrenceStartMs = nextMs;
        }
        return occurrenceStartMs;
    }

    default:
        return -1;
    }
}

//============================================================================
int64_t CalendarEventInfo::getCurrentOrPreviousOccurrenceStartMs( int64_t nowMs )
{
    if( nowMs < m_StartTimeMs )
    {
        return -1; // event has not started yet as of nowMs
    }

    switch( m_Recurrence )
    {
    case eCalendarRecurrenceNever:
        return m_StartTimeMs;

    case eCalendarRecurrenceEveryDay:
    {
        int64_t stepsPassed = ( nowMs - m_StartTimeMs ) / DAY_OF_MIILISECONDS;
        return m_StartTimeMs + stepsPassed * DAY_OF_MIILISECONDS;
    }

    case eCalendarRecurrenceEveryWeek:
    {
        int64_t stepsPassed = ( nowMs - m_StartTimeMs ) / WEEK_OF_MIILISECONDS;
        return m_StartTimeMs + stepsPassed * WEEK_OF_MIILISECONDS;
    }

    case eCalendarRecurrenceEveryMonth:
    {
        int64_t occurrenceStartMs = m_StartTimeMs;
        for( ;; )
        {
            int64_t nextMs = stepToNextOccurrence( occurrenceStartMs );
            if( nextMs > nowMs || nextMs <= occurrenceStartMs )
            {
                break;
            }
            occurrenceStartMs = nextMs;
        }
        return occurrenceStartMs;
    }

    default:
        return -1;
    }
}

//============================================================================
int64_t CalendarEventInfo::getCurrentOrPreviousOccurrenceEndMs( int64_t nowMs )
{
    int64_t occStartMs = getCurrentOrPreviousOccurrenceStartMs( nowMs );
    if( occStartMs < 0 )
    {
        return -1;
    }

    return occStartMs + m_DurationMs;
}

//============================================================================
bool CalendarEventInfo::isOccurrenceActiveAt( int64_t nowMs )
{
    int64_t occStartMs = getCurrentOrPreviousOccurrenceStartMs( nowMs );
    if( occStartMs < 0 )
    {
        return false;
    }

    return nowMs < ( occStartMs + m_DurationMs );
}

//============================================================================
std::vector<std::pair<int64_t, int64_t>> CalendarEventInfo::getOccurrencesUntil( int64_t windowEndMs )
{
    std::vector<std::pair<int64_t, int64_t>> occurrences;

    int64_t occStartMs = m_StartTimeMs;
    int iterations = 0;
    while( occStartMs >= 0 && occStartMs < windowEndMs && iterations++ < MAX_OVERLAP_OCCURRENCES )
    {
        occurrences.emplace_back( occStartMs, occStartMs + m_DurationMs );

        int64_t nextMs = getNextOccurrenceStartMs( occStartMs );
        if( nextMs <= occStartMs )
        {
            break; // eCalendarRecurrenceNever ( returns -1 ), or no further progress
        }

        occStartMs = nextMs;
    }

    return occurrences;
}

//============================================================================
bool CalendarEventInfo::overlapsWith( CalendarEventInfo& other )
{
    int64_t windowStartMs = ( m_StartTimeMs < other.m_StartTimeMs ) ? m_StartTimeMs : other.m_StartTimeMs;
    int64_t windowEndMs = windowStartMs + OVERLAP_CHECK_WINDOW_MS;

    std::vector<std::pair<int64_t, int64_t>> myOccurrences = getOccurrencesUntil( windowEndMs );
    std::vector<std::pair<int64_t, int64_t>> otherOccurrences = other.getOccurrencesUntil( windowEndMs );

    for( auto& mine : myOccurrences )
    {
        for( auto& theirs : otherOccurrences )
        {
            if( mine.first < theirs.second && theirs.first < mine.second )
            {
                return true;
            }
        }
    }

    return false;
}

//============================================================================
int64_t CalendarEventInfo::computeContentExpiresTime( int64_t nowMs, enum EPurgeEventHistoryType userOverride )
{
    int64_t occurrenceEndMs = getCurrentOrPreviousOccurrenceEndMs( nowMs );
    if( occurrenceEndMs < 0 )
    {
        return -1; // no occurrence to compute against
    }

    int64_t purgeMs = ( ePurgeEventHistoryUseHostDefault == userOverride ) ? getRetentionMs() : PurgeEventHistoryTypeToMs( userOverride );

    int64_t scheduledExpiresTime = occurrenceEndMs + purgeMs;
    int64_t sessionRunningExpiresTime = nowMs + purgeMs;

    return ( sessionRunningExpiresTime > scheduledExpiresTime ) ? sessionRunningExpiresTime : scheduledExpiresTime;
}

//============================================================================
bool CalendarEventInfo::serializeToBlob( PktBlobEntry& blob )
{
    bool result = blob.setValue( m_EventId );
    result &= blob.setValue( m_HostOnlineId );
    uint8_t hostType = ( uint8_t )m_HostType;
    result &= blob.setValue( hostType );

    result &= blob.setValue( m_EventName );
    result &= blob.setValue( m_EventDescription );

    result &= blob.setValue( m_StartTimeMs );
    result &= blob.setValue( m_DurationMs );

    uint8_t recurrence = ( uint8_t )m_Recurrence;
    result &= blob.setValue( recurrence );
    result &= blob.setValue( m_RecurrenceDayOfMonth );

    result &= blob.setValue( m_RetentionMs );
    result &= blob.setValue( m_VideoPostingsAllowed );

    uint8_t attendanceMode = ( uint8_t )m_AttendanceMode;
    result &= blob.setValue( attendanceMode );

    uint32_t inviteCount = ( uint32_t )m_InviteList.size();
    result &= blob.setValue( inviteCount );
    for( auto& inviteeId : m_InviteList )
    {
        result &= blob.setValue( inviteeId );
    }

    result &= blob.setValue( m_CreatedByOnlineId );
    result &= blob.setValue( m_CreatedTimeMs );
    result &= blob.setValue( m_ModifiedTimeMs );

    return result;
}

//============================================================================
bool CalendarEventInfo::deserializeFromBlob( PktBlobEntry& blob )
{
    bool result = blob.getValue( m_EventId );
    result &= blob.getValue( m_HostOnlineId );
    uint8_t hostType = 0;
    result &= blob.getValue( hostType );
    m_HostType = ( EHostType )hostType;

    result &= blob.getValue( m_EventName );
    result &= blob.getValue( m_EventDescription );

    result &= blob.getValue( m_StartTimeMs );
    result &= blob.getValue( m_DurationMs );

    uint8_t recurrence = 0;
    result &= blob.getValue( recurrence );
    m_Recurrence = ( ECalendarRecurrence )recurrence;
    result &= blob.getValue( m_RecurrenceDayOfMonth );

    result &= blob.getValue( m_RetentionMs );
    result &= blob.getValue( m_VideoPostingsAllowed );

    uint8_t attendanceMode = 0;
    result &= blob.getValue( attendanceMode );
    m_AttendanceMode = ( ECalendarAttendance )attendanceMode;

    m_InviteList.clear();
    uint32_t inviteCount = 0;
    result &= blob.getValue( inviteCount );
    for( uint32_t i = 0; result && i < inviteCount; i++ )
    {
        VxGUID inviteeId;
        result &= blob.getValue( inviteeId );
        if( result )
        {
            m_InviteList.push_back( inviteeId );
        }
    }

    result &= blob.getValue( m_CreatedByOnlineId );
    result &= blob.getValue( m_CreatedTimeMs );
    result &= blob.getValue( m_ModifiedTimeMs );

    return result;
}
