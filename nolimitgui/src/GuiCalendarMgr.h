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

#include <Calendar/CalendarEventInfo.h>

#include <CoreLib/HostedId.h>
#include <CoreLib/VxMutex.h>
#include <CoreLib/VxGUID.h>

#include <map>
#include <set>
#include <vector>

#include <QObject>

class AppCommon;

//! GUI-side cache + dispatch for one or more joined hosts' event calendars -- the GUI-thread
//! counterpart of the engine-side CalendarClientMgr, mirroring how GuiGroupieListMgr pairs with
//! the engine's GroupieListMgr. Composed into AppCommon; AppCommon's toGuiCalendarX overrides
//! ( IToGui ) delegate straight here. Widgets read the cache via getEventList()/getEvent() and
//! issue actions via requestEventUpdate()/requestEventCancel()/requestRefresh(),
//! then connect to the external signals below for live updates -- see event-calendar design
//! notes, "Event session GUI".
class GuiCalendarMgr : public QObject
{
    Q_OBJECT
public:
    GuiCalendarMgr() = delete;
    GuiCalendarMgr( AppCommon& app );
    GuiCalendarMgr( const GuiCalendarMgr& rhs ) = delete;
    virtual ~GuiCalendarMgr() = default;

    void                        onAppCommonCreated( void );

    //=== toGui entry points -- called directly from AppCommon's IToGui overrides, on the
    //=== engine thread. each marshals onto the GUI thread via an internal QueuedConnection
    //=== signal before touching m_EventCache or emitting an external signal. ===//
    void                        toGuiCalendarEventListResult( EHostType hostType, VxGUID& hostOnlineId, CalendarEventInfo& eventInfo );
    void                        toGuiCalendarEventListComplete( EHostType hostType, VxGUID& hostOnlineId );
    void                        toGuiCalendarEventUpdateStatus( EHostType hostType, VxGUID& hostOnlineId, VxGUID& eventId, ECommErr commErr );
    void                        toGuiCalendarEventCancelStatus( EHostType hostType, VxGUID& hostOnlineId, VxGUID& eventId, ECommErr commErr );

    //=== fromGui action requests -- call from any GUI-thread widget. ===//
    void                        requestEventUpdate( HostedId& adminId, CalendarEventInfo& eventInfo );
    void                        requestEventCancel( HostedId& adminId, VxGUID& eventId );
    void                        requestRefresh( HostedId& adminId );

    //=== cache reads -- safe to call from the GUI thread at any time. ===//
    std::vector<CalendarEventInfo>  getEventList( VxGUID& hostOnlineId );
    bool                        getEvent( VxGUID& hostOnlineId, VxGUID& eventId, CalendarEventInfo& retEventInfo );

    //! live count of users currently connected to this host -- no rsvp/attendance record is
    //! kept anywhere for this, it is a synchronous read straight through to the engine's
    //! connection list ( IFromGui::fromGuiGetCalendarAttendingCount ). see event-calendar design
    //! notes, "no per-identity RSVP record" decision.
    uint32_t                    getAttendingCount( HostedId& adminId );

    //! local-only "have I looked at this event" tracking -- deliberately never sent to the host
    //! ( that is the whole point: no RSVP-style per-identity record ). in-memory only for this
    //! session, same scoped choice already made for the calendar toggle's own visibility state.
    void                        markAllEventsViewed( VxGUID& hostOnlineId );
    bool                        hasUnviewedEvents( VxGUID& hostOnlineId );

signals:
    //! fired after a list fetch/refresh finishes and the cache for this host is up to date --
    //! widgets should call getEventList() in response rather than the signal carrying the list.
    void                        signalCalendarEventListUpdated( EHostType hostType, VxGUID hostOnlineId );
    void                        signalCalendarEventUpdateStatus( EHostType hostType, VxGUID hostOnlineId, VxGUID eventId, ECommErr commErr );
    void                        signalCalendarEventCancelStatus( EHostType hostType, VxGUID hostOnlineId, VxGUID eventId, ECommErr commErr );

    void                        signalInternalEventListResult( CalendarEventInfo* eventInfo, EHostType hostType, VxGUID hostOnlineId );
    void                        signalInternalEventListComplete( EHostType hostType, VxGUID hostOnlineId );
    void                        signalInternalEventUpdateStatus( EHostType hostType, VxGUID hostOnlineId, VxGUID eventId, ECommErr commErr );
    void                        signalInternalEventCancelStatus( EHostType hostType, VxGUID hostOnlineId, VxGUID eventId, ECommErr commErr );

private slots:
    void                        slotInternalEventListResult( CalendarEventInfo* eventInfo, EHostType hostType, VxGUID hostOnlineId );
    void                        slotInternalEventListComplete( EHostType hostType, VxGUID hostOnlineId );
    void                        slotInternalEventUpdateStatus( EHostType hostType, VxGUID hostOnlineId, VxGUID eventId, ECommErr commErr );
    void                        slotInternalEventCancelStatus( EHostType hostType, VxGUID hostOnlineId, VxGUID eventId, ECommErr commErr );

protected:
    void                        replaceHostCache( VxGUID& hostOnlineId, std::vector<CalendarEventInfo>& newEvents );

    AppCommon&                  m_MyApp;

    VxMutex                     m_CacheMutex;
    std::vector<CalendarEventInfo>  m_EventCache;
    // events received during an in-progress list fetch, not yet swapped into m_EventCache --
    // keyed per host ( not a single shared buffer ) since a user can be joined to multiple
    // hosts of the same type at once and their fetches can interleave; kept separate from
    // m_EventCache so a chunked ( MoreEventsExist ) fetch doesn't show a partial calendar,
    // matching CalendarClientMgr's wholesale-replace-on-Complete behavior.
    std::map<VxGUID, std::vector<CalendarEventInfo>, cmp_vxguid>  m_PendingFetchEvents;

    //! event ids the user has already viewed ( panel opened while that event was in the cached
    //! list ) -- global across hosts since event ids are globally unique guids. drives the
    //! calendar toggle button's unread indicator; guarded by m_CacheMutex.
    std::set<VxGUID, cmp_vxguid>   m_ViewedEventIds;
};
