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

#include "CalendarEventInfo.h"

#include <CoreLib/DbBase.h>

#include <vector>

class DbCursor;

//! persists one host's event calendar ( events + invite lists ) to sqlite.
//! one CalendarDb instance per host plugin instance, owned by CalendarMgr, which
//! is composed into PluginBaseHostService the same way HostServerMgr is -- see
//! the event-calendar design notes.
//!
//! follows the FileInfoDb / GroupieListDb one-db-per-feature convention: file
//! lives at VxGetUserSpecificDataDirectory() + "settings/" + <dbFileName>
//! ( eg "calendar.db" ), wired up by the owning CalendarMgr.
class CalendarDb : public DbBase
{
public:
    CalendarDb();
    virtual ~CalendarDb() = default;

    int32_t                     calendarDbStartup( int dbVersion, const char* dbFileName );
    int32_t                     calendarDbShutdown( void );

    //=== events ===//

    //! add-or-replace ( keyed by eventId ). also replaces the invite list rows.
    bool                        addOrUpdateEvent( CalendarEventInfo& eventInfo );
    //! removes the event and its invite-list / rsvp rows
    bool                        removeEvent( VxGUID& eventId );
    bool                        getEvent( VxGUID& eventId, CalendarEventInfo& retEventInfo );
    void                        getAllEvents( std::vector<CalendarEventInfo>& retEventList );

    //! deletes non-recurring ( eCalendarRecurrenceNever ) events whose retention has
    //! elapsed as of nowMs. recurring events' definitions are never auto-deleted here --
    //! per-occurrence CONTENT purge for recurring events is not yet implemented ( it
    //! requires correlating posted chat/asset content to a specific occurrence, which
    //! is a GUI/session-layer concern, not this Db ).
    void                        purgeExpiredEvents( int64_t nowMs );

protected:
    virtual int32_t             onCreateTables( int iDbVersion ) override;
    virtual int32_t             onDeleteTables( int iOldVersion ) override;

    void                        getInviteListForEvent( VxGUID& eventId, std::vector<VxGUID>& retInviteList );

    bool                        eventFromCursor( DbCursor* cursor, CalendarEventInfo& retEventInfo );
};
