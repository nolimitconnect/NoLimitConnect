//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "CalendarClientMgr.h"

#include <P2PEngine/P2PEngine.h>
#include <Plugins/PluginBase.h>
#include <ConnectIdListMgr/ConnectIdListMgr.h>

#include <PktLib/PktsCalendar.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>
#include <CoreLib/GroupieId.h>

//============================================================================
CalendarClientMgr::CalendarClientMgr( P2PEngine& engine, VxNetIdent* myIdent, PluginBase& pluginBase )
    : m_Engine( engine )
    , m_MyIdent( myIdent )
    , m_Plugin( pluginBase )
{
}

//============================================================================
std::shared_ptr<VxSktBase> CalendarClientMgr::findHostSkt( VxGUID& hostOnlineId, EHostType hostType )
{
    GroupieId groupieId( m_Engine.getMyOnlineId(), hostOnlineId, hostType );
    return m_Engine.getConnectIdListMgr().findHostConnection( groupieId );
}

//============================================================================
bool CalendarClientMgr::sendCalendarEventListReq( std::shared_ptr<VxSktBase>& sktBase, VxGUID& hostOnlineId, uint32_t skipCount )
{
    if( !sktBase )
    {
        return false;
    }

    if( 0 == skipCount )
    {
        // starting a fresh fetch cycle -- reset pagination progress and drop this host's stale
        // cache now ( not on whatever reply eventually turns out to be "last" -- with chunked
        // replies that could wipe out earlier chunks already appended THIS cycle ). every reply
        // in the cycle, first through last, then does nothing but append.
        m_ListFetchStateMutex.lock();
        ListFetchState freshState;
        freshState.requestCount = 1; // this send IS request #1 of the cycle
        m_ListFetchState[ hostOnlineId ] = freshState;
        m_ListFetchStateMutex.unlock();

        m_CacheMutex.lock();
        auto it = m_EventCache.begin();
        while( it != m_EventCache.end() )
        {
            if( it->getHostOnlineId() == hostOnlineId )
            {
                it = m_EventCache.erase( it );
            }
            else
            {
                ++it;
            }
        }
        m_CacheMutex.unlock();
    }

    PktCalendarEventListReq pktReq;
    pktReq.setHostType( m_Plugin.getHostType() );
    VxGUID sessionId;
    VxGUID::generateNewVxGUID( sessionId );
    pktReq.setSessionId( sessionId );
    pktReq.setHostOnlineId( hostOnlineId );
    pktReq.setSkipCount( skipCount );

    if( !m_Plugin.txPacket( hostOnlineId, sktBase, &pktReq ) )
    {
        LogMsg( LOG_DEBUG, "CalendarClientMgr::sendCalendarEventListReq failed send" );
        return false;
    }

    if( LogEnabled( eLogCalendar ) ) LogModule( eLogCalendar, LOG_VERBOSE, "[%d ms] CalendarClientMgr::%s fetching calendar for host %s skip=%u",
        GetApplicationAliveMs(), __func__, hostOnlineId.toHexString().c_str(), skipCount );

    return true;
}

//============================================================================
bool CalendarClientMgr::sendCalendarEventUpdateReq( VxGUID& hostOnlineId, EHostType hostType, CalendarEventInfo& eventInfo )
{
    std::shared_ptr<VxSktBase> sktBase = findHostSkt( hostOnlineId, hostType );
    if( !sktBase )
    {
        return false;
    }

    PktCalendarEventUpdateReq pktReq;
    pktReq.setHostType( hostType );
    VxGUID sessionId;
    VxGUID::generateNewVxGUID( sessionId );
    pktReq.setSessionId( sessionId );
    pktReq.setHostOnlineId( hostOnlineId );

    pktReq.getBlobEntry().resetWrite();
    if( !eventInfo.serializeToBlob( pktReq.getBlobEntry() ) )
    {
        return false;
    }
    pktReq.calcPktLen();

    if( !m_Plugin.txPacket( hostOnlineId, sktBase, &pktReq ) )
    {
        LogMsg( LOG_DEBUG, "CalendarClientMgr::sendCalendarEventUpdateReq failed send" );
        return false;
    }

    return true;
}

//============================================================================
bool CalendarClientMgr::sendCalendarEventCancelReq( VxGUID& hostOnlineId, EHostType hostType, VxGUID& eventId )
{
    std::shared_ptr<VxSktBase> sktBase = findHostSkt( hostOnlineId, hostType );
    if( !sktBase )
    {
        return false;
    }

    PktCalendarEventCancelReq pktReq;
    pktReq.setHostType( hostType );
    VxGUID sessionId;
    VxGUID::generateNewVxGUID( sessionId );
    pktReq.setSessionId( sessionId );
    pktReq.setHostOnlineId( hostOnlineId );
    pktReq.setEventId( eventId );

    if( !m_Plugin.txPacket( hostOnlineId, sktBase, &pktReq ) )
    {
        LogMsg( LOG_DEBUG, "CalendarClientMgr::sendCalendarEventCancelReq failed send" );
        return false;
    }

    return true;
}

//============================================================================
void CalendarClientMgr::onPktCalendarEventListReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    PktCalendarEventListReply* pktReply = ( PktCalendarEventListReply* )pktHdr;
    if( eCommErrNone != pktReply->getCommError() )
    {
        LogMsg( LOG_DEBUG, "CalendarClientMgr::onPktCalendarEventListReply comm err %d", ( int )pktReply->getCommError() );
        return;
    }

    VxGUID hostOnlineId = pktReply->getHostOnlineId();
    if( LogEnabled( eLogCalendar ) ) LogModule( eLogCalendar, LOG_VERBOSE, "[%d ms] CalendarClientMgr::%s received %u events for host %s more=%s",
        GetApplicationAliveMs(), __func__, pktReply->getEventCountThisPkt(), hostOnlineId.toHexString().c_str(), pktReply->getMoreEventsExist() ? "true" : "false" );

    std::vector<CalendarEventInfo> receivedEvents;
    pktReply->getBlobEntry().resetRead();
    for( uint16_t i = 0; i < pktReply->getEventCountThisPkt(); ++i )
    {
        CalendarEventInfo eventInfo;
        if( !eventInfo.deserializeFromBlob( pktReply->getBlobEntry() ) )
        {
            break;
        }
        receivedEvents.push_back( eventInfo );
    }

    // this host's stale cache was already dropped once, when sendCalendarEventListReq() started
    // THIS fetch cycle ( skipCount == 0 ) -- every reply in the cycle, first through last, just
    // appends its own chunk.
    m_CacheMutex.lock();
    for( auto& eventInfo : receivedEvents )
    {
        m_EventCache.push_back( eventInfo );
    }
    m_CacheMutex.unlock();

    EHostType hostType = m_Plugin.getHostType();
    for( auto& eventInfo : receivedEvents )
    {
        m_Engine.getToGui().toGuiCalendarEventListResult( hostType, hostOnlineId, eventInfo );
    }

    m_ListFetchStateMutex.lock();
    ListFetchState& fetchState = m_ListFetchState[ hostOnlineId ];
    fetchState.receivedCount += pktReply->getEventCountThisPkt();
    uint32_t receivedCount = fetchState.receivedCount;
    uint32_t requestCount = fetchState.requestCount;
    m_ListFetchStateMutex.unlock();

    if( pktReply->getMoreEventsExist() && requestCount < MAX_LIST_REQUESTS_PER_HOST )
    {
        m_ListFetchStateMutex.lock();
        m_ListFetchState[ hostOnlineId ].requestCount = requestCount + 1;
        m_ListFetchStateMutex.unlock();

        if( LogEnabled( eLogCalendar ) ) LogModule( eLogCalendar, LOG_VERBOSE, "[%d ms] CalendarClientMgr::%s more events exist for host %s, requesting next chunk ( request %u of %u )",
            GetApplicationAliveMs(), __func__, hostOnlineId.toHexString().c_str(), requestCount + 1, MAX_LIST_REQUESTS_PER_HOST );
        sendCalendarEventListReq( sktBase, hostOnlineId, receivedCount );
        return; // wait for the next chunk before declaring the list complete
    }

    if( pktReply->getMoreEventsExist() )
    {
        // hit the request cap and the host is STILL claiming more events exist -- stop asking
        // rather than let a huge ( or deliberately abusive ) calendar drag this out indefinitely.
        // what we have is treated as complete even though it may not truly be everything.
        LogMsg( LOG_WARNING, "CalendarClientMgr::onPktCalendarEventListReply host %s still reports more events after %u requests ( %u events received ) -- giving up, calendar may be incomplete",
            hostOnlineId.toHexString().c_str(), MAX_LIST_REQUESTS_PER_HOST, receivedCount );
    }

    m_Engine.getToGui().toGuiCalendarEventListComplete( hostType, hostOnlineId );
}

//============================================================================
void CalendarClientMgr::onPktCalendarEventUpdateReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    PktCalendarEventUpdateReply* pktReply = ( PktCalendarEventUpdateReply* )pktHdr;
    if(LogEnabled(eLogPkt)) LogModule( eLogPkt, LOG_VERBOSE, "CalendarClientMgr::onPktCalendarEventUpdateReply comm err %d", ( int )pktReply->getCommError() );

    // the update reply only echoes the event id, not the full event -- re-fetch the list to
    // pick up the created/edited event rather than trying to merge a partial record.
    if( eCommErrNone == pktReply->getCommError() )
    {
        VxGUID hostOnlineId = pktReply->getHostOnlineId();
        sendCalendarEventListReq( sktBase, hostOnlineId );
    }

    VxGUID hostOnlineId = pktReply->getHostOnlineId();
    VxGUID eventId = pktReply->getEventId();
    m_Engine.getToGui().toGuiCalendarEventUpdateStatus( m_Plugin.getHostType(), hostOnlineId, eventId, pktReply->getCommError() );
}

//============================================================================
void CalendarClientMgr::onPktCalendarEventCancelReply( std::shared_ptr<VxSktBase>& sktBase, VxPktHdr* pktHdr, VxNetIdent* netIdent )
{
    PktCalendarEventCancelReply* pktReply = ( PktCalendarEventCancelReply* )pktHdr;
    if(LogEnabled(eLogPkt)) LogModule( eLogPkt, LOG_VERBOSE, "CalendarClientMgr::onPktCalendarEventCancelReply comm err %d", ( int )pktReply->getCommError() );

    if( eCommErrNone == pktReply->getCommError() )
    {
        VxGUID hostOnlineId = pktReply->getHostOnlineId();
        VxGUID eventId = pktReply->getEventId();

        m_CacheMutex.lock();
        auto it = m_EventCache.begin();
        while( it != m_EventCache.end() )
        {
            if( it->getHostOnlineId() == hostOnlineId && it->getEventId() == eventId )
            {
                it = m_EventCache.erase( it );
            }
            else
            {
                ++it;
            }
        }
        m_CacheMutex.unlock();
    }

    VxGUID cancelledHostOnlineId = pktReply->getHostOnlineId();
    VxGUID cancelledEventId = pktReply->getEventId();
    m_Engine.getToGui().toGuiCalendarEventCancelStatus( m_Plugin.getHostType(), cancelledHostOnlineId, cancelledEventId, pktReply->getCommError() );
}

//============================================================================
int64_t CalendarClientMgr::getActiveEventContentExpiresTime( VxGUID& hostOnlineId, int64_t nowMs )
{
    int64_t latestExpiresTime = 0;
    EPurgeEventHistoryType userOverride = m_Engine.getEngineSettings().getPurgeEventHistoryType();

    m_CacheMutex.lock();
    for( auto& eventInfo : m_EventCache )
    {
        if( eventInfo.getHostOnlineId() != hostOnlineId )
        {
            continue;
        }

        if( !eventInfo.isOccurrenceActiveAt( nowMs ) )
        {
            continue; // gates whether newly-received content counts as "during an event" at all
        }

        int64_t expiresTime = eventInfo.computeContentExpiresTime( nowMs, userOverride );
        if( expiresTime > latestExpiresTime )
        {
            latestExpiresTime = expiresTime;
        }
    }
    m_CacheMutex.unlock();

    return latestExpiresTime;
}

//============================================================================
void CalendarClientMgr::extendActiveEventExpiryTimes( int64_t nowMs )
{
    EPurgeEventHistoryType userOverride = m_Engine.getEngineSettings().getPurgeEventHistoryType();
    EHostType hostType = m_Plugin.getHostType();

    m_CacheMutex.lock();
    // copy out before unlocking -- findHostConnection()/extendCalendarEventExpiryTime() below
    // must not be called while holding this lock
    std::vector<CalendarEventInfo> eventsCopy = m_EventCache;
    m_CacheMutex.unlock();

    for( auto& eventInfo : eventsCopy )
    {
        int64_t expiresTime = eventInfo.computeContentExpiresTime( nowMs, userOverride );
        if( expiresTime <= 0 )
        {
            continue; // no occurrence to compute against yet
        }

        // only extend for a host we are ACTUALLY currently connected to -- merely still being
        // a member ( calendar still cached ) is not enough evidence the session is still
        // active; a live connection is. see event-calendar design notes on the purge-override
        // mechanism ( "session running long" judgment call ).
        VxGUID hostOnlineId = eventInfo.getHostOnlineId();
        GroupieId groupieId( m_Engine.getMyOnlineId(), hostOnlineId, hostType );
        if( !m_Engine.getConnectIdListMgr().findHostConnection( groupieId ) )
        {
            if( LogEnabled( eLogCalendar ) ) LogModule( eLogCalendar, LOG_VERBOSE, "[%d ms] CalendarClientMgr::%s not connected to host %s, not extending event %s",
                GetApplicationAliveMs(), __func__, hostOnlineId.toHexString().c_str(), eventInfo.getEventId().toHexString().c_str() );
            continue;
        }

        if( LogEnabled( eLogCalendar ) ) LogModule( eLogCalendar, LOG_VERBOSE, "[%d ms] CalendarClientMgr::%s still connected to host %s, extending event %s expires to %lld",
            GetApplicationAliveMs(), __func__, hostOnlineId.toHexString().c_str(), eventInfo.getEventId().toHexString().c_str(), expiresTime );

        m_Engine.getAssetMgr().extendCalendarEventExpiryTime( hostOnlineId, expiresTime );
    }
}

//============================================================================
void CalendarClientMgr::getEventList( VxGUID& hostOnlineId, std::vector<CalendarEventInfo>& retEventList )
{
    m_CacheMutex.lock();
    for( auto& eventInfo : m_EventCache )
    {
        if( eventInfo.getHostOnlineId() == hostOnlineId )
        {
            retEventList.push_back( eventInfo );
        }
    }
    m_CacheMutex.unlock();
}

//============================================================================
bool CalendarClientMgr::getEvent( VxGUID& hostOnlineId, VxGUID& eventId, CalendarEventInfo& retEventInfo )
{
    bool found = false;
    m_CacheMutex.lock();
    for( auto& eventInfo : m_EventCache )
    {
        if( eventInfo.getHostOnlineId() == hostOnlineId && eventInfo.getEventId() == eventId )
        {
            retEventInfo = eventInfo;
            found = true;
            break;
        }
    }
    m_CacheMutex.unlock();
    return found;
}

//============================================================================
void CalendarClientMgr::clearHostCache( VxGUID& hostOnlineId )
{
    if( LogEnabled( eLogCalendar ) ) LogModule( eLogCalendar, LOG_VERBOSE, "[%d ms] CalendarClientMgr::%s exited host %s, clearing cached calendar", GetApplicationAliveMs(), __func__, hostOnlineId.toHexString().c_str() );

    m_CacheMutex.lock();
    auto it = m_EventCache.begin();
    while( it != m_EventCache.end() )
    {
        if( it->getHostOnlineId() == hostOnlineId )
        {
            it = m_EventCache.erase( it );
        }
        else
        {
            ++it;
        }
    }
    m_CacheMutex.unlock();
}
