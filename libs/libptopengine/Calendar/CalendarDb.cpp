//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "CalendarDb.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/sqlite3.h>

namespace
{
    const int COLUMN_IDX_EVENT_ID                  = 0;
    const int COLUMN_IDX_HOST_ONLINE_ID             = 1;
    const int COLUMN_IDX_HOST_TYPE                  = 2;
    const int COLUMN_IDX_EVENT_NAME                 = 3;
    const int COLUMN_IDX_EVENT_DESC                 = 4;
    const int COLUMN_IDX_START_TIME_MS              = 5;
    const int COLUMN_IDX_DURATION_MS                = 6;
    const int COLUMN_IDX_RECURRENCE                 = 7;
    const int COLUMN_IDX_RECURRENCE_DAY_OF_MONTH    = 8;
    const int COLUMN_IDX_RETENTION_MS               = 9;
    const int COLUMN_IDX_VIDEO_ALLOWED              = 10;
    const int COLUMN_IDX_ATTENDANCE_MODE            = 11;
    const int COLUMN_IDX_CREATED_BY_ONLINE_ID       = 12;
    const int COLUMN_IDX_CREATED_TIME_MS            = 13;
    const int COLUMN_IDX_MODIFIED_TIME_MS           = 14;

    const int COLUMN_IDX_INVITE_EVENT_ID            = 0;
    const int COLUMN_IDX_INVITE_USER_ONLINE_ID      = 1;
}

//============================================================================
CalendarDb::CalendarDb()
: DbBase( "CalendarDb" )
{
}

//============================================================================
int32_t CalendarDb::calendarDbStartup( int dbVersion, const char* dbFileName )
{
    dbShutdown();
    return dbStartup( dbVersion, dbFileName );
}

//============================================================================
int32_t CalendarDb::calendarDbShutdown( void )
{
    return dbShutdown();
}

//============================================================================
int32_t CalendarDb::onCreateTables( int iDbVersion )
{
    int32_t rc = sqlExec( "CREATE TABLE tblCalendarEvent ("
        "eventId TEXT, hostOnlineId TEXT, hostType INTEGER, "
        "eventName TEXT, eventDesc TEXT, "
        "startTimeMs BIGINT, durationMs BIGINT, "
        "recurrence INTEGER, recurrenceDayOfMonth INTEGER, "
        "retentionMs BIGINT, videoAllowed INTEGER, attendanceMode INTEGER, "
        "createdByOnlineId TEXT, createdTimeMs BIGINT, modifiedTimeMs BIGINT)" );
    vx_assert( 0 == rc );

    int32_t rc2 = sqlExec( "CREATE TABLE tblCalendarInvite (eventId TEXT, userOnlineId TEXT)" );
    vx_assert( 0 == rc2 );

    return rc ? rc : rc2;
}

//============================================================================
int32_t CalendarDb::onDeleteTables( int iOldVersion )
{
    int32_t rc = sqlExec( "DROP TABLE tblCalendarEvent" );
    vx_assert( 0 == rc );

    int32_t rc2 = sqlExec( "DROP TABLE tblCalendarInvite" );
    vx_assert( 0 == rc2 );

    return rc ? rc : rc2;
}

//============================================================================
bool CalendarDb::eventFromCursor( DbCursor* cursor, CalendarEventInfo& retEventInfo )
{
    VxGUID eventId;
    if( !eventId.fromVxGUIDHexString( cursor->getString( COLUMN_IDX_EVENT_ID ) ) )
    {
        return false;
    }
    retEventInfo.setEventId( eventId );

    VxGUID hostOnlineId;
    hostOnlineId.fromVxGUIDHexString( cursor->getString( COLUMN_IDX_HOST_ONLINE_ID ) );
    retEventInfo.setHostOnlineId( hostOnlineId );
    retEventInfo.setHostType( ( EHostType )cursor->getS32( COLUMN_IDX_HOST_TYPE ) );

    retEventInfo.setEventName( cursor->getString( COLUMN_IDX_EVENT_NAME ) );
    retEventInfo.setEventDescription( cursor->getString( COLUMN_IDX_EVENT_DESC ) );

    retEventInfo.setStartTimeMs( cursor->getS64( COLUMN_IDX_START_TIME_MS ) );
    retEventInfo.setDurationMs( cursor->getS64( COLUMN_IDX_DURATION_MS ) );

    retEventInfo.setRecurrence( ( ECalendarRecurrence )cursor->getS32( COLUMN_IDX_RECURRENCE ) );
    retEventInfo.setRecurrenceDayOfMonth( ( uint8_t )cursor->getS32( COLUMN_IDX_RECURRENCE_DAY_OF_MONTH ) );

    retEventInfo.setRetentionMs( cursor->getS64( COLUMN_IDX_RETENTION_MS ) );
    retEventInfo.setVideoPostingsAllowed( 0 != cursor->getS32( COLUMN_IDX_VIDEO_ALLOWED ) );
    retEventInfo.setAttendanceMode( ( ECalendarAttendance )cursor->getS32( COLUMN_IDX_ATTENDANCE_MODE ) );

    VxGUID createdByOnlineId;
    createdByOnlineId.fromVxGUIDHexString( cursor->getString( COLUMN_IDX_CREATED_BY_ONLINE_ID ) );
    retEventInfo.setCreatedByOnlineId( createdByOnlineId );
    retEventInfo.setCreatedTimeMs( cursor->getS64( COLUMN_IDX_CREATED_TIME_MS ) );
    retEventInfo.setModifiedTimeMs( cursor->getS64( COLUMN_IDX_MODIFIED_TIME_MS ) );

    return true;
}

//============================================================================
void CalendarDb::getInviteListForEvent( VxGUID& eventId, std::vector<VxGUID>& retInviteList )
{
    std::string eventIdHex = eventId.toHexString();

    lockDb();

    DbCursor* cursor = startQuery( "SELECT * FROM tblCalendarInvite WHERE eventId=?", eventIdHex.c_str() );
    if( cursor )
    {
        while( cursor->getNextRow() )
        {
            VxGUID inviteeId;
            if( inviteeId.fromVxGUIDHexString( cursor->getString( COLUMN_IDX_INVITE_USER_ONLINE_ID ) ) )
            {
                retInviteList.push_back( inviteeId );
            }
        }

        cursor->close();
    }

    unlockDb();
}

//============================================================================
bool CalendarDb::addOrUpdateEvent( CalendarEventInfo& eventInfo )
{
    if( !eventInfo.getEventId().isValid() )
    {
        LogMsg( LOG_ERROR, "CalendarDb::addOrUpdateEvent INVALID EVENT ID" );
        return false;
    }

    std::string eventIdHex = eventInfo.getEventId().toHexString();
    std::string hostOnlineIdHex = eventInfo.getHostOnlineId().toHexString();
    std::string createdByHex = eventInfo.getCreatedByOnlineId().toHexString();

    lockDb();

    // add-or-replace: remove existing event + invite rows first ( mirrors GroupieListDb::saveGroupie )
    DbBindList delEventBind( eventIdHex.c_str() );
    sqlExec( "DELETE FROM tblCalendarEvent WHERE eventId=?", delEventBind );
    DbBindList delInviteBind( eventIdHex.c_str() );
    sqlExec( "DELETE FROM tblCalendarInvite WHERE eventId=?", delInviteBind );

    DbBindList bindList( eventIdHex.c_str() );
    bindList.add( hostOnlineIdHex.c_str() );
    bindList.add( ( int64_t )eventInfo.getHostType() );
    bindList.add( eventInfo.getEventName().c_str() );
    bindList.add( eventInfo.getEventDescription().c_str() );
    bindList.add( eventInfo.getStartTimeMs() );
    bindList.add( eventInfo.getDurationMs() );
    bindList.add( ( int64_t )eventInfo.getRecurrence() );
    bindList.add( ( int64_t )eventInfo.getRecurrenceDayOfMonth() );
    bindList.add( eventInfo.getRetentionMs() );
    bindList.add( ( int64_t )( eventInfo.isVideoPostingsAllowed() ? 1 : 0 ) );
    bindList.add( ( int64_t )eventInfo.getAttendanceMode() );
    bindList.add( createdByHex.c_str() );
    bindList.add( eventInfo.getCreatedTimeMs() );
    bindList.add( eventInfo.getModifiedTimeMs() );

    int32_t rc = sqlExec( "INSERT INTO tblCalendarEvent (eventId, hostOnlineId, hostType, eventName, eventDesc, "
        "startTimeMs, durationMs, recurrence, recurrenceDayOfMonth, retentionMs, videoAllowed, attendanceMode, "
        "createdByOnlineId, createdTimeMs, modifiedTimeMs) VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)", bindList );

    if( 0 == rc )
    {
        for( auto& inviteeId : eventInfo.getInviteList() )
        {
            std::string inviteeHex = inviteeId.toHexString();
            DbBindList inviteBind( eventIdHex.c_str() );
            inviteBind.add( inviteeHex.c_str() );
            sqlExec( "INSERT INTO tblCalendarInvite (eventId, userOnlineId) VALUES(?,?)", inviteBind );
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "CalendarDb::addOrUpdateEvent: ERROR %d saving event", rc );
    }

    unlockDb();
    return 0 == rc;
}

//============================================================================
bool CalendarDb::removeEvent( VxGUID& eventId )
{
    if( !eventId.isValid() )
    {
        return false;
    }

    std::string eventIdHex = eventId.toHexString();

    lockDb();

    DbBindList bindList( eventIdHex.c_str() );
    int32_t rc = sqlExec( "DELETE FROM tblCalendarEvent WHERE eventId=?", bindList );

    DbBindList inviteBind( eventIdHex.c_str() );
    sqlExec( "DELETE FROM tblCalendarInvite WHERE eventId=?", inviteBind );

    unlockDb();
    return 0 == rc;
}

//============================================================================
bool CalendarDb::getEvent( VxGUID& eventId, CalendarEventInfo& retEventInfo )
{
    if( !eventId.isValid() )
    {
        return false;
    }

    std::string eventIdHex = eventId.toHexString();
    bool found = false;

    lockDb();

    DbCursor* cursor = startQuery( "SELECT * FROM tblCalendarEvent WHERE eventId=?", eventIdHex.c_str() );
    if( cursor )
    {
        if( cursor->getNextRow() )
        {
            found = eventFromCursor( cursor, retEventInfo );
        }

        cursor->close();
    }

    unlockDb();

    if( found )
    {
        std::vector<VxGUID> inviteList;
        getInviteListForEvent( eventId, inviteList );
        for( auto& inviteeId : inviteList )
        {
            retEventInfo.addInvitee( inviteeId );
        }
    }

    return found;
}

//============================================================================
void CalendarDb::getAllEvents( std::vector<CalendarEventInfo>& retEventList )
{
    lockDb();

    DbCursor* cursor = startQuery( "SELECT * FROM tblCalendarEvent" );
    if( cursor )
    {
        while( cursor->getNextRow() )
        {
            CalendarEventInfo eventInfo;
            if( eventFromCursor( cursor, eventInfo ) )
            {
                retEventList.push_back( eventInfo );
            }
        }

        cursor->close();
    }

    unlockDb();

    // populate invite lists outside the tblCalendarEvent lock scope ( getInviteListForEvent locks its own query )
    for( auto& eventInfo : retEventList )
    {
        VxGUID eventId = eventInfo.getEventId();
        std::vector<VxGUID> inviteList;
        getInviteListForEvent( eventId, inviteList );
        for( auto& inviteeId : inviteList )
        {
            eventInfo.addInvitee( inviteeId );
        }
    }
}

//============================================================================
void CalendarDb::purgeExpiredEvents( int64_t nowMs )
{
    std::vector<CalendarEventInfo> allEvents;
    getAllEvents( allEvents );

    for( auto& eventInfo : allEvents )
    {
        if( eventInfo.isRecurring() )
        {
            // per-occurrence CONTENT purge for recurring events is not yet implemented --
            // see CalendarDb.h note. the event definition itself is never auto-deleted
            // while it is still recurring.
            continue;
        }

        int64_t occurrenceEndMs = eventInfo.getCurrentOrPreviousOccurrenceEndMs( nowMs );
        if( occurrenceEndMs < 0 )
        {
            continue; // event has not happened yet
        }

        if( nowMs >= ( occurrenceEndMs + eventInfo.getRetentionMs() ) )
        {
            VxGUID eventId = eventInfo.getEventId();
            removeEvent( eventId );
        }
    }
}
