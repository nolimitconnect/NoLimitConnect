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

#include <CoreLib/VxGUID.h>
#include <CoreLib/VxMutex.h>

#include <map>
#include <memory>
#include <vector>

class P2PEngine;
class PluginBase;
class VxSktBase;
class VxPktHdr;
class VxNetIdent;

//! client-side cache of one or more joined hosts' event calendars. composed into
//! PluginBaseHostClient the same way CalendarMgr is composed into PluginBaseHostService --
//! see the event-calendar design notes ( Phase 3 plan ). one instance per plugin type
//! ( ChatRoom-client / Group-client / RandomConnect-client are separate PluginBaseHostClient
//! instances ), holding events for every host of that type currently joined -- a user can be
//! joined to multiple hosts of the same type at once ( see PluginBaseHostClient::m_JoinedHosts ),
//! so the cache is keyed by hostOnlineId, not a single flat list.
//!
//! in-memory only for this slice -- no local sqlite cache across restarts ( not decided whether
//! that is ever needed; the host is always the source of truth and is re-fetched on every join ).
class CalendarClientMgr
{
public:
    //! hard cap on how many PktCalendarEventListReq round-trips one fetch cycle will send to a
    //! single host, even while that host keeps reporting MoreEventsExist -- bounds both an
    //! unreasonably large calendar and a host deliberately stringing a client along. see
    //! PktsCalendar.h and onPktCalendarEventListReply.
    static const uint32_t       MAX_LIST_REQUESTS_PER_HOST = 3;

    CalendarClientMgr() = delete;
    CalendarClientMgr( P2PEngine& engine, VxNetIdent* myIdent, PluginBase& pluginBase );
    virtual ~CalendarClientMgr() = default;

    //! fetches the calendar from a host just joined ( or being resynced ), starting at skipCount
    //! ( visible-to-us ) events in -- 0 for a fresh fetch cycle ( the only value external callers
    //! should ever pass; call from PluginBaseHostClient::onPktHostJoinReply -- see event-calendar
    //! design notes: calendar is fetched on join, never visible before ). nonzero skipCount is
    //! used internally by onPktCalendarEventListReply to resume a chunked reply.
    virtual bool                sendCalendarEventListReq( std::shared_ptr<VxSktBase>& sktBase, VxGUID& hostOnlineId, uint32_t skipCount = 0 );

    //! host admin only -- create ( event id left invalid ) or edit ( existing event id ) one
    //! event. host is the sole authority and re-validates admin status server-side regardless.
    virtual bool                sendCalendarEventUpdateReq( VxGUID& hostOnlineId, EHostType hostType, CalendarEventInfo& eventInfo );
    //! host admin only -- cancel/delete the whole event series by id.
    virtual bool                sendCalendarEventCancelReq( VxGUID& hostOnlineId, EHostType hostType, VxGUID& eventId );

    virtual void                onPktCalendarEventListReply     ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent );
    virtual void                onPktCalendarEventUpdateReply   ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent );
    virtual void                onPktCalendarEventCancelReply   ( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent );

    //! cached events for one host ( empty if that host's calendar has not been fetched yet )
    virtual void                getEventList( VxGUID& hostOnlineId, std::vector<CalendarEventInfo>& retEventList );
    virtual bool                getEvent( VxGUID& hostOnlineId, VxGUID& eventId, CalendarEventInfo& retEventInfo );

    //! 0 if hostOnlineId has no event with an occurrence active right now ( per our own cached
    //! copy of its calendar -- see BaseXferInterface::getActiveCalendarEventExpiresTime ), else
    //! the expiry time ( occurrenceEnd + retention ) to tag newly-received content with.
    virtual int64_t             getActiveEventContentExpiresTime( VxGUID& hostOnlineId, int64_t nowMs );

    //! recomputes expiry for every cached event and pushes ExpiresTime forward on already-tagged
    //! assets for hosts we currently have a LIVE connection to ( not just historical membership
    //! -- see event-calendar design notes ). call once a minute.
    virtual void                extendActiveEventExpiryTimes( int64_t nowMs );

    //! drop a host's cached calendar entirely -- call on leave/unjoin so a stale calendar is
    //! never shown for a host we are no longer a member of.
    virtual void                clearHostCache( VxGUID& hostOnlineId );

protected:
    //! host connection for hostOnlineId, or null if not currently connected/joined
    std::shared_ptr<VxSktBase>  findHostSkt( VxGUID& hostOnlineId, EHostType hostType );

    P2PEngine&                  m_Engine;
    VxNetIdent*                 m_MyIdent{ nullptr };
    PluginBase&                 m_Plugin;

    VxMutex                     m_CacheMutex;
    //! one CalendarEventInfo per (hostOnlineId, eventId); simple flat vector since the per-host
    //! event count is expected to stay small ( mirrors GroupieListMgr's flat-vector + linear
    //! filter-by-host-id convention rather than a nested map ).
    std::vector<CalendarEventInfo> m_EventCache;

    //! per-host progress through the CURRENT chunked fetch cycle -- how many ( visible ) events
    //! received so far ( becomes the next request's skipCount ) and how many requests already
    //! sent ( checked against MAX_LIST_REQUESTS_PER_HOST ). reset whenever
    //! sendCalendarEventListReq() starts a fresh cycle ( skipCount == 0 ).
    struct ListFetchState
    {
        uint32_t                receivedCount{ 0 };
        uint32_t                requestCount{ 0 };
    };
    VxMutex                     m_ListFetchStateMutex;
    std::map<VxGUID, ListFetchState, cmp_vxguid> m_ListFetchState;
};
