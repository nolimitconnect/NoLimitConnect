//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiCalendarMgr.h"
#include "AppCommon.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>

//============================================================================
GuiCalendarMgr::GuiCalendarMgr( AppCommon& app )
    : QObject( &app )
    , m_MyApp( app )
{
}

//============================================================================
void GuiCalendarMgr::onAppCommonCreated( void )
{
    connect( this, SIGNAL(signalInternalEventListResult(CalendarEventInfo*,EHostType,VxGUID)), this, SLOT(slotInternalEventListResult(CalendarEventInfo*,EHostType,VxGUID)), Qt::QueuedConnection );
    connect( this, SIGNAL(signalInternalEventListComplete(EHostType,VxGUID)), this, SLOT(slotInternalEventListComplete(EHostType,VxGUID)), Qt::QueuedConnection );
    connect( this, SIGNAL(signalInternalEventUpdateStatus(EHostType,VxGUID,VxGUID,ECommErr)), this, SLOT(slotInternalEventUpdateStatus(EHostType,VxGUID,VxGUID,ECommErr)), Qt::QueuedConnection );
    connect( this, SIGNAL(signalInternalEventCancelStatus(EHostType,VxGUID,VxGUID,ECommErr)), this, SLOT(slotInternalEventCancelStatus(EHostType,VxGUID,VxGUID,ECommErr)), Qt::QueuedConnection );
}

//============================================================================
void GuiCalendarMgr::toGuiCalendarEventListResult( EHostType hostType, VxGUID& hostOnlineId, CalendarEventInfo& eventInfo )
{
    CalendarEventInfo* newEventInfo = new CalendarEventInfo( eventInfo );
    emit signalInternalEventListResult( newEventInfo, hostType, hostOnlineId );
}

//============================================================================
void GuiCalendarMgr::toGuiCalendarEventListComplete( EHostType hostType, VxGUID& hostOnlineId )
{
    emit signalInternalEventListComplete( hostType, hostOnlineId );
}

//============================================================================
void GuiCalendarMgr::toGuiCalendarEventUpdateStatus( EHostType hostType, VxGUID& hostOnlineId, VxGUID& eventId, ECommErr commErr )
{
    emit signalInternalEventUpdateStatus( hostType, hostOnlineId, eventId, commErr );
}

//============================================================================
void GuiCalendarMgr::toGuiCalendarEventCancelStatus( EHostType hostType, VxGUID& hostOnlineId, VxGUID& eventId, ECommErr commErr )
{
    emit signalInternalEventCancelStatus( hostType, hostOnlineId, eventId, commErr );
}

//============================================================================
void GuiCalendarMgr::requestEventUpdate( HostedId& adminId, CalendarEventInfo& eventInfo )
{
    m_MyApp.getEngine().fromGuiCalendarEventUpdate( adminId, eventInfo );
}

//============================================================================
void GuiCalendarMgr::requestEventCancel( HostedId& adminId, VxGUID& eventId )
{
    m_MyApp.getEngine().fromGuiCalendarEventCancel( adminId, eventId );
}

//============================================================================
void GuiCalendarMgr::requestRefresh( HostedId& adminId )
{
    if( LogEnabled( eLogCalendar ) ) LogModule( eLogCalendar, LOG_VERBOSE, "[%d ms] GuiCalendarMgr::%s host %s",
        GetApplicationAliveMs(), __func__, adminId.getHostOnlineId().toHexString().c_str() );
    m_MyApp.getEngine().fromGuiCalendarRefresh( adminId );
}

//============================================================================
uint32_t GuiCalendarMgr::getAttendingCount( HostedId& adminId )
{
    return m_MyApp.getEngine().fromGuiGetCalendarAttendingCount( adminId );
}

//============================================================================
void GuiCalendarMgr::markAllEventsViewed( VxGUID& hostOnlineId )
{
    m_CacheMutex.lock();
    for( auto& eventInfo : m_EventCache )
    {
        if( eventInfo.getHostOnlineId() == hostOnlineId )
        {
            m_ViewedEventIds.insert( eventInfo.getEventId() );
        }
    }
    m_CacheMutex.unlock();
}

//============================================================================
bool GuiCalendarMgr::hasUnviewedEvents( VxGUID& hostOnlineId )
{
    bool hasUnviewed = false;
    m_CacheMutex.lock();
    for( auto& eventInfo : m_EventCache )
    {
        if( eventInfo.getHostOnlineId() == hostOnlineId
            && m_ViewedEventIds.end() == m_ViewedEventIds.find( eventInfo.getEventId() ) )
        {
            hasUnviewed = true;
            break;
        }
    }
    m_CacheMutex.unlock();
    return hasUnviewed;
}

//============================================================================
std::vector<CalendarEventInfo> GuiCalendarMgr::getEventList( VxGUID& hostOnlineId )
{
    std::vector<CalendarEventInfo> retList;
    m_CacheMutex.lock();
    for( auto& eventInfo : m_EventCache )
    {
        if( eventInfo.getHostOnlineId() == hostOnlineId )
        {
            retList.push_back( eventInfo );
        }
    }
    m_CacheMutex.unlock();
    return retList;
}

//============================================================================
bool GuiCalendarMgr::getEvent( VxGUID& hostOnlineId, VxGUID& eventId, CalendarEventInfo& retEventInfo )
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
void GuiCalendarMgr::replaceHostCache( VxGUID& hostOnlineId, std::vector<CalendarEventInfo>& newEvents )
{
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

    for( auto& eventInfo : newEvents )
    {
        m_EventCache.push_back( eventInfo );
    }
    m_CacheMutex.unlock();
}

//============================================================================
void GuiCalendarMgr::slotInternalEventListResult( CalendarEventInfo* eventInfo, EHostType hostType, VxGUID hostOnlineId )
{
    m_PendingFetchEvents[ hostOnlineId ].push_back( *eventInfo );
    delete eventInfo;
}

//============================================================================
void GuiCalendarMgr::slotInternalEventListComplete( EHostType hostType, VxGUID hostOnlineId )
{
    auto iter = m_PendingFetchEvents.find( hostOnlineId );
    if( iter != m_PendingFetchEvents.end() )
    {
        replaceHostCache( hostOnlineId, iter->second );
        m_PendingFetchEvents.erase( iter );
    }
    else
    {
        // host had zero visible events this fetch -- still replace with an empty list so a
        // stale cache from a previous join doesn't linger
        std::vector<CalendarEventInfo> emptyList;
        replaceHostCache( hostOnlineId, emptyList );
    }

    emit signalCalendarEventListUpdated( hostType, hostOnlineId );
}

//============================================================================
void GuiCalendarMgr::slotInternalEventUpdateStatus( EHostType hostType, VxGUID hostOnlineId, VxGUID eventId, ECommErr commErr )
{
    emit signalCalendarEventUpdateStatus( hostType, hostOnlineId, eventId, commErr );
}

//============================================================================
void GuiCalendarMgr::slotInternalEventCancelStatus( EHostType hostType, VxGUID hostOnlineId, VxGUID eventId, ECommErr commErr )
{
    if( eCommErrNone == commErr )
    {
        // the engine-side caches ( CalendarMgr's db / CalendarClientMgr's cache ) already
        // dropped this event -- remove it here too rather than waiting for some future refresh
        // to notice it's gone, so a successful cancel disappears from the list immediately.
        m_CacheMutex.lock();
        auto it = m_EventCache.begin();
        while( it != m_EventCache.end() )
        {
            if( it->getEventId() == eventId )
            {
                it = m_EventCache.erase( it );
            }
            else
            {
                ++it;
            }
        }
        m_CacheMutex.unlock();

        emit signalCalendarEventListUpdated( hostType, hostOnlineId );
    }

    emit signalCalendarEventCancelStatus( hostType, hostOnlineId, eventId, commErr );
}
