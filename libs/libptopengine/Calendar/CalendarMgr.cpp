//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "CalendarMgr.h"

#include <P2PEngine/P2PEngine.h>
#include <Plugins/PluginBaseHostService.h>

#include <PktLib/PktsCalendar.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>
#include <CoreLib/VxGlobals.h>

#include <string>

//============================================================================
CalendarMgr::CalendarMgr( P2PEngine& engine, VxNetIdent* myIdent, PluginBaseHostService& pluginBase )
    : m_Engine( engine )
    , m_MyIdent( myIdent )
    , m_Plugin( pluginBase )
{
}

//============================================================================
int32_t CalendarMgr::calendarMgrStartup( void )
{
    // one db file per host plugin instance ( ChatRoom / Group / RandomConnect each get
    // their own PluginBaseHostService instance and so need separate calendars )
    std::string dbFileName = VxGetUserSpecificDataDirectory() + "settings/";
    dbFileName += "calendar_";
    dbFileName += std::to_string( ( int )m_Plugin.getPluginType() );
    dbFileName += ".db";

    return m_CalendarDb.calendarDbStartup( CALENDAR_DB_VERSION, dbFileName.c_str() );
}

//============================================================================
int32_t CalendarMgr::calendarMgrShutdown( void )
{
    return m_CalendarDb.calendarDbShutdown();
}

//============================================================================
void CalendarMgr::onThreadOncePer15Minutes( void )
{
    if( LogEnabled( eLogCalendar ) ) LogModule( eLogCalendar, LOG_VERBOSE, "[%d ms] CalendarMgr::%s purging expired event definitions, plugin %s", GetApplicationAliveMs(), __func__, DescribePluginType( m_Plugin.getPluginType() ) );
    m_CalendarDb.purgeExpiredEvents( GetGmtTimeMs() );
}

//============================================================================
int64_t CalendarMgr::getActiveEventContentExpiresTime( int64_t nowMs )
{
    int64_t latestExpiresTime = 0;
    EPurgeEventHistoryType userOverride = m_Engine.getEngineSettings().getPurgeEventHistoryType();

    std::vector<CalendarEventInfo> allEvents;
    m_CalendarDb.getAllEvents( allEvents );
    for( auto& eventInfo : allEvents )
    {
        if( !eventInfo.isOccurrenceActiveAt( nowMs ) )
        {
            continue; // gates whether newly-posted content counts as "during an event" at all
        }

        int64_t expiresTime = eventInfo.computeContentExpiresTime( nowMs, userOverride );
        if( expiresTime > latestExpiresTime )
        {
            latestExpiresTime = expiresTime;
        }
    }

    return latestExpiresTime;
}

//============================================================================
void CalendarMgr::extendActiveEventExpiryTimes( int64_t nowMs )
{
    // extension only happens while the admin actually has this host's admin screen open --
    // m_Plugin.isAdminViewing(), set/cleared from fromGuiAdminViewHost(). if the admin never
    // shows up ( or leaves and doesn't come back ), content just keeps whatever expiry it was
    // originally tagged with at post time -- "the event ends according to the calendar entry."
    // only an admin who is actually present can keep a running-long session's content alive
    // past that. see event-calendar design notes.
    if( !m_Plugin.isAdminViewing() )
    {
        if( !m_LoggedAdminNotPresent )
        {
            if( LogEnabled( eLogCalendar ) ) LogModule( eLogCalendar, LOG_VERBOSE, "[%d ms] CalendarMgr::%s admin not present, not extending, plugin %s", GetApplicationAliveMs(), __func__, DescribePluginType( m_Plugin.getPluginType() ) );
            m_LoggedAdminNotPresent = true;
        }

        return;
    }

    m_LoggedAdminNotPresent = false;

    EPurgeEventHistoryType userOverride = m_Engine.getEngineSettings().getPurgeEventHistoryType();

    std::vector<CalendarEventInfo> allEvents;
    m_CalendarDb.getAllEvents( allEvents );
    for( auto& eventInfo : allEvents )
    {
        int64_t expiresTime = eventInfo.computeContentExpiresTime( nowMs, userOverride );
        if( expiresTime <= 0 )
        {
            continue; // no occurrence to compute against yet
        }

        if( LogEnabled( eLogCalendar ) ) LogModule( eLogCalendar, LOG_VERBOSE, "[%d ms] CalendarMgr::%s admin present, extending event %s expires to %lld",
            GetApplicationAliveMs(), __func__, eventInfo.getEventId().toHexString().c_str(), expiresTime );

        VxGUID myOnlineId = m_Engine.getMyOnlineId();
        m_Engine.getAssetMgr().extendCalendarEventExpiryTime( myOnlineId, expiresTime );
    }
}

//============================================================================
bool CalendarMgr::isEventVisibleToUser( CalendarEventInfo& eventInfo, VxNetIdent* netIdent )
{
    if( eCalendarAttendanceEverybody == eventInfo.getAttendanceMode() )
    {
        return true;
    }

    VxGUID userOnlineId = netIdent->getMyOnlineId();
    return eventInfo.isInvited( userOnlineId );
}

//============================================================================
bool CalendarMgr::isHostAdmin( VxNetIdent* netIdent )
{
    return netIdent->getMyFriendshipToHim() >= eFriendStateAdmin;
}

//============================================================================
bool CalendarMgr::isOverlappingExistingEvent( CalendarEventInfo& candidateEvent )
{
    std::vector<CalendarEventInfo> allEvents;
    m_CalendarDb.getAllEvents( allEvents );
    for( auto& existingEvent : allEvents )
    {
        if( existingEvent.getEventId() == candidateEvent.getEventId() )
        {
            continue; // editing an existing event -- don't conflict with its own prior self
        }

        if( candidateEvent.overlapsWith( existingEvent ) )
        {
            return true;
        }
    }

    return false;
}

//============================================================================
void CalendarMgr::onPktCalendarEventListReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent, ECommErr commErr )
{
    if( LogEnabled( eLogCalendar ) ) LogModule( eLogCalendar, LOG_VERBOSE, "[%d ms] CalendarMgr::%s calendar fetch from %s commErr %d", GetApplicationAliveMs(), __func__, netIdent->getOnlineName(), ( int )commErr );

    PktCalendarEventListReq* pktReq = ( PktCalendarEventListReq* )pktHdr;

    PktCalendarEventListReply pktReply;
    pktReply.setHostType( m_Plugin.getHostType() );
    pktReply.setSessionId( pktReq->getSessionId() );
    VxGUID hostOnlineId = m_Engine.getMyOnlineId();
    pktReply.setHostOnlineId( hostOnlineId );

    if( eCommErrNone == commErr )
    {
        std::vector<CalendarEventInfo> allEvents;
        m_CalendarDb.getAllEvents( allEvents );

        // skip past events already sent in an earlier chunk of this same fetch cycle -- see
        // PktCalendarEventListReq::setSkipCount(). counted over VISIBLE events only, since that's
        // the only ordering the requester ever actually observes.
        uint32_t skipCount = pktReq->getSkipCount();
        uint32_t visibleIndex = 0;

        pktReply.getBlobEntry().resetWrite();
        for( auto& eventInfo : allEvents )
        {
            if( !isEventVisibleToUser( eventInfo, netIdent ) )
            {
                continue;
            }

            if( visibleIndex < skipCount )
            {
                ++visibleIndex;
                continue;
            }
            ++visibleIndex;

            // rough estimate of the serialized size ( see CalendarEventInfo::serializeToBlob )
            // so a packet is never left half-written -- mirrors the haveRoom() check in
            // PktGroupieSearchReply::addGroupieInfo before writing a variable length record.
            int requiredSpace = 160 + ( int )eventInfo.getEventName().length() + ( int )eventInfo.getEventDescription().length()
                + ( int )eventInfo.getInviteList().size() * 16;
            if( !pktReply.getBlobEntry().haveRoom( requiredSpace ) )
            {
                pktReply.setMoreEventsExist( true );
                break;
            }

            if( eventInfo.serializeToBlob( pktReply.getBlobEntry() ) )
            {
                pktReply.incrementEventCount();
            }
        }
    }

    pktReply.setCommError( commErr );
    pktReply.calcPktLen();

    if( !m_Plugin.txPacket( pktHdr->getSrcOnlineId(), sktBase, &pktReply ) )
    {
        LogMsg( LOG_DEBUG, "CalendarMgr::onPktCalendarEventListReq failed send reply" );
    }
}

//============================================================================
void CalendarMgr::onPktCalendarEventUpdateReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent, ECommErr commErr )
{
    PktCalendarEventUpdateReq* pktReq = ( PktCalendarEventUpdateReq* )pktHdr;

    CalendarEventInfo eventInfo;
    if( eCommErrNone == commErr )
    {
        if( !isHostAdmin( netIdent ) )
        {
            commErr = eCommErrPluginPermission;
        }
        else
        {
            pktReq->getBlobEntry().resetRead();
            if( !eventInfo.deserializeFromBlob( pktReq->getBlobEntry() ) )
            {
                commErr = eCommErrInvalidPkt;
            }
        }
    }

    if( eCommErrNone == commErr )
    {
        int64_t nowMs = GetGmtTimeMs();
        VxGUID hostOnlineId = m_Engine.getMyOnlineId();
        eventInfo.setHostOnlineId( hostOnlineId );
        eventInfo.setHostType( m_Plugin.getHostType() );
        bool isNewEvent = !eventInfo.getEventId().isValid();
        if( isNewEvent )
        {
            eventInfo.generateNewEventId();
            VxGUID createdByOnlineId = netIdent->getMyOnlineId();
            eventInfo.setCreatedByOnlineId( createdByOnlineId );
            eventInfo.setCreatedTimeMs( nowMs );
        }
        eventInfo.setModifiedTimeMs( nowMs );

        commErr = addOrUpdateEventInternal( eventInfo, isNewEvent );
    }

    PktCalendarEventUpdateReply pktReply;
    pktReply.setHostType( m_Plugin.getHostType() );
    pktReply.setSessionId( pktReq->getSessionId() );
    VxGUID hostOnlineId = m_Engine.getMyOnlineId();
    pktReply.setHostOnlineId( hostOnlineId );
    VxGUID eventId = eventInfo.getEventId();
    pktReply.setEventId( eventId );
    pktReply.setCommError( commErr );

    if( LogEnabled( eLogCalendar ) ) LogModule( eLogCalendar, LOG_VERBOSE, "[%d ms] CalendarMgr::%s event %s upserted by %s commErr %d",
        GetApplicationAliveMs(), __func__, eventInfo.getEventId().toHexString().c_str(), netIdent->getOnlineName(), ( int )commErr );

    if( !m_Plugin.txPacket( pktHdr->getSrcOnlineId(), sktBase, &pktReply ) )
    {
        LogMsg( LOG_DEBUG, "CalendarMgr::onPktCalendarEventUpdateReq failed send reply" );
    }
}

//============================================================================
void CalendarMgr::onPktCalendarEventCancelReq( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent, ECommErr commErr )
{
    PktCalendarEventCancelReq* pktReq = ( PktCalendarEventCancelReq* )pktHdr;
    VxGUID eventId = pktReq->getEventId();

    if( eCommErrNone == commErr )
    {
        if( !isHostAdmin( netIdent ) )
        {
            commErr = eCommErrPluginPermission;
        }
        else
        {
            commErr = cancelEventInternal( eventId );
        }
    }

    PktCalendarEventCancelReply pktReply;
    pktReply.setHostType( m_Plugin.getHostType() );
    pktReply.setSessionId( pktReq->getSessionId() );
    VxGUID hostOnlineId = m_Engine.getMyOnlineId();
    pktReply.setHostOnlineId( hostOnlineId );
    pktReply.setEventId( eventId );
    pktReply.setCommError( commErr );

    if( LogEnabled( eLogCalendar ) ) LogModule( eLogCalendar, LOG_VERBOSE, "[%d ms] CalendarMgr::%s event %s canceled by %s commErr %d",
        GetApplicationAliveMs(), __func__, eventId.toHexString().c_str(), netIdent->getOnlineName(), ( int )commErr );

    if( !m_Plugin.txPacket( pktHdr->getSrcOnlineId(), sktBase, &pktReply ) )
    {
        LogMsg( LOG_DEBUG, "CalendarMgr::onPktCalendarEventCancelReq failed send reply" );
    }
}

//============================================================================
ECommErr CalendarMgr::addOrUpdateEventInternal( CalendarEventInfo& eventInfo, bool isNewEvent )
{
    // only a brand NEW event's start time is checked against "now" -- editing an existing
    // ( possibly still-recurring ) event whose original start has since passed must stay
    // editable ( eg tweaking the description of an event that already started its series ).
    if( isNewEvent && eventInfo.getStartTimeMs() < GetGmtTimeMs() )
    {
        return eCommErrEventStartInPast;
    }

    if( isOverlappingExistingEvent( eventInfo ) )
    {
        return eCommErrScheduleConflict;
    }

    if( !m_CalendarDb.addOrUpdateEvent( eventInfo ) )
    {
        return eCommErrInvalidParam;
    }

    return eCommErrNone;
}

//============================================================================
ECommErr CalendarMgr::cancelEventInternal( VxGUID& eventId )
{
    CalendarEventInfo existingEvent;
    if( !m_CalendarDb.getEvent( eventId, existingEvent ) )
    {
        return eCommErrNotFound;
    }

    m_CalendarDb.removeEvent( eventId );
    return eCommErrNone;
}

//============================================================================
void CalendarMgr::localEventUpdate( CalendarEventInfo& eventInfo )
{
    int64_t nowMs = GetGmtTimeMs();
    VxGUID hostOnlineId = m_Engine.getMyOnlineId();
    eventInfo.setHostOnlineId( hostOnlineId );
    eventInfo.setHostType( m_Plugin.getHostType() );
    bool isNewEvent = !eventInfo.getEventId().isValid();
    if( isNewEvent )
    {
        eventInfo.generateNewEventId();
        eventInfo.setCreatedByOnlineId( hostOnlineId ); // local path -- creator is always this host's own admin
        eventInfo.setCreatedTimeMs( nowMs );
    }
    eventInfo.setModifiedTimeMs( nowMs );

    ECommErr commErr = addOrUpdateEventInternal( eventInfo, isNewEvent );

    if( LogEnabled( eLogCalendar ) ) LogModule( eLogCalendar, LOG_VERBOSE, "[%d ms] CalendarMgr::%s local event %s upserted commErr %d",
        GetApplicationAliveMs(), __func__, eventInfo.getEventId().toHexString().c_str(), ( int )commErr );

    m_Engine.getToGui().toGuiCalendarEventUpdateStatus( m_Plugin.getHostType(), hostOnlineId, eventInfo.getEventId(), commErr );

    if( eCommErrNone == commErr )
    {
        // re-fetch the whole list rather than trying to merge the single changed record --
        // mirrors CalendarClientMgr::onPktCalendarEventUpdateReply's own "re-fetch after a
        // successful update" behavior for the remote path, so the admin's own list ( eg the
        // Host Admin screen's already-open panel ) picks up a newly created/edited event
        // immediately instead of only on the next unrelated refresh.
        localEventListFetch();
    }
}

//============================================================================
void CalendarMgr::localEventCancel( VxGUID& eventId )
{
    ECommErr commErr = cancelEventInternal( eventId );

    if( LogEnabled( eLogCalendar ) ) LogModule( eLogCalendar, LOG_VERBOSE, "[%d ms] CalendarMgr::%s local event %s canceled commErr %d",
        GetApplicationAliveMs(), __func__, eventId.toHexString().c_str(), ( int )commErr );

    m_Engine.getToGui().toGuiCalendarEventCancelStatus( m_Plugin.getHostType(), m_Engine.getMyOnlineId(), eventId, commErr );
}

//============================================================================
void CalendarMgr::localEventListFetch( void )
{
    VxGUID hostOnlineId = m_Engine.getMyOnlineId();
    EHostType hostType = m_Plugin.getHostType();

    if( LogEnabled( eLogCalendar ) ) LogModule( eLogCalendar, LOG_VERBOSE, "[%d ms] CalendarMgr::%s local list fetch, plugin %s", GetApplicationAliveMs(), __func__, DescribePluginType( m_Plugin.getPluginType() ) );

    std::vector<CalendarEventInfo> allEvents;
    m_CalendarDb.getAllEvents( allEvents );
    for( auto& eventInfo : allEvents )
    {
        // this host's own admin can always see everything -- no isEventVisibleToUser filtering
        m_Engine.getToGui().toGuiCalendarEventListResult( hostType, hostOnlineId, eventInfo );
    }
    m_Engine.getToGui().toGuiCalendarEventListComplete( hostType, hostOnlineId );
}
