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

#include "CalendarDb.h"

#include <memory>

class P2PEngine;
class PluginBaseHostService;
class VxSktBase;
class VxPktHdr;
class VxNetIdent;

//! host-side authority for one host plugin instance's event calendar. composed into
//! PluginBaseHostService the same way HostServerMgr is -- see the event-calendar design
//! notes ( Calendar is not a separate plugin/host type ). client-side consumption of the
//! *Reply packets ( GUI display, local caching ) is a separate, not-yet-scoped slice; this
//! class only implements the host's authoritative request handling.
class CalendarMgr
{
    const int CALENDAR_DB_VERSION = 1;
public:
    CalendarMgr() = delete;
    //! pluginBase is narrowed to PluginBaseHostService& ( rather than the generic PluginBase& )
    //! since CalendarMgr is only ever constructed by PluginBaseHostService -- this lets
    //! extendActiveEventExpiryTimes() consult its admin-presence flag directly. see
    //! event-calendar design notes.
    CalendarMgr( P2PEngine& engine, VxNetIdent* myIdent, PluginBaseHostService& pluginBase );
    virtual ~CalendarMgr() = default;

    int32_t                     calendarMgrStartup( void );
    int32_t                     calendarMgrShutdown( void );

    //! deletes non-recurring events whose retention has elapsed -- call from the owning
    //! plugin's existing onThreadOncePer15Minutes() hook ( see PluginBaseHostService ).
    void                        onThreadOncePer15Minutes( void );

    //! 0 if no event has an occurrence active right now, else the expiry time
    //! ( occurrenceEnd + retention ) to tag newly-posted content with -- see
    //! BaseXferInterface::getActiveCalendarEventExpiresTime and the asset purge mechanism in
    //! the event-calendar design notes. if multiple events happen to have overlapping active
    //! occurrences, the LATEST resulting expiry wins ( keep as long as the most generous
    //! concurrently-active event wants -- an edge case, not something admins are expected to do
    //! deliberately, so this just needs to be a reasonable, non-surprising choice ).
    int64_t                     getActiveEventContentExpiresTime( int64_t nowMs );

    //! recomputes every event's content expiry with the current nowMs and pushes any
    //! already-tagged asset's ExpiresTime forward to match if it's later than what's currently
    //! stored -- this is what keeps a session that runs past its scheduled end from having its
    //! content expire out from under still-active use. call once a minute; see
    //! event-calendar design notes on the purge-override mechanism.
    void                        extendActiveEventExpiryTimes( int64_t nowMs );

    virtual void                onPktCalendarEventListReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent, enum ECommErr commErr );
    virtual void                onPktCalendarEventUpdateReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent, enum ECommErr commErr );
    virtual void                onPktCalendarEventCancelReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent, enum ECommErr commErr );

    //=== direct, in-process calendar management for THIS host's own admin ===//
    //! calendar events are created and served by the host -- the process actually running this
    //! host plugin never needs to round-trip a packet to itself over the loopback socket just to
    //! manage its own calendar ( that packet path exists for a REMOTE user who has been granted
    //! eFriendStateAdmin on this host, which still goes through onPktCalendarEventUpdateReq etc.
    //! above ). called from P2PEngine::fromGuiCalendarEventUpdate/EventCancel/Refresh whenever
    //! the GUI's target HostedId's hostOnlineId is this engine's own online id -- see
    //! event-calendar design notes.
    void                        localEventUpdate( CalendarEventInfo& eventInfo );
    void                        localEventCancel( VxGUID& eventId );
    void                        localEventListFetch( void );

protected:
    //! past-start check ( new events only ) + overlap check + db write, shared by the packet
    //! path ( onPktCalendarEventUpdateReq, after it deserializes and gates on isHostAdmin() ) and
    //! localEventUpdate ( after it fills in hostOnlineId/hostType/createdBy directly ). caller is
    //! responsible for those fields and for computing isNewEvent BEFORE generating a new event id
    //! ( eventId is always valid by the time this runs, so it can't tell create from edit itself ).
    enum ECommErr                addOrUpdateEventInternal( CalendarEventInfo& eventInfo, bool isNewEvent );
    //! looks up + removes the event, shared by onPktCalendarEventCancelReq ( after its own
    //! isHostAdmin() gate ) and localEventCancel.
    enum ECommErr                cancelEventInternal( VxGUID& eventId );
    //! true if netIdent may see/attend this event: eCalendarAttendanceEverybody, or on the
    //! invite list for eCalendarAttendanceUserList. does NOT check general host plugin
    //! permission ( that's the commErr already computed by the caller, mirroring
    //! GroupieListMgr's onPkt* pattern -- see PluginBaseHostService::getCommAccessState() ).
    bool                        isEventVisibleToUser( CalendarEventInfo& eventInfo, VxNetIdent* netIdent );
    //! only the host admin may create/edit/cancel events -- see event-calendar design notes.
    bool                        isHostAdmin( VxNetIdent* netIdent );
    //! true if candidateEvent's occurrences overlap any OTHER existing event's occurrences in
    //! this same calendar ( excludes candidateEvent's own prior self by event id, for the edit
    //! case ). only ever compares within one host's calendar -- see CalendarEventInfo::overlapsWith.
    bool                        isOverlappingExistingEvent( CalendarEventInfo& candidateEvent );

    P2PEngine&                  m_Engine;
    VxNetIdent*                 m_MyIdent{ nullptr };
    PluginBaseHostService&      m_Plugin;
    CalendarDb                  m_CalendarDb;

    //! logged the "admin not present, not extending" line since the admin was last present ( or
    //! since startup ) -- extendActiveEventExpiryTimes() runs once a minute, so without this the
    //! same line repeats every minute for as long as the admin stays away. reset to false the
    //! moment isAdminViewing() goes true again, so the NEXT time they leave it logs once more.
    bool                        m_LoggedAdminNotPresent{ false };
};
