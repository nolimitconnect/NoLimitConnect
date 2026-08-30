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

#include "ListWidgetBase.h"

#include <CoreLib/HostedId.h>

class QWidget;

//! pops up a small read-only modal dialog listing adminId's calendar ( wraps a
//! CalendarEventListWidget in admin=false mode ) -- lets the user browse what already exists
//! without navigating away from wherever they clicked from. shared by DialogCalendarEvent's
//! "View Existing Events" button and CalendarEventListPanel's own button of the same name.
void                            ShowCalendarEventListPopup( HostedId& adminId, QWidget* parent );

//! shows one joined host's event calendar -- one CalendarEventRowWidget per event, each row's
//! occurrence chosen as the next upcoming occurrence as of refresh time. a non-recurring event
//! whose one occurrence has already ended is dropped from the list entirely on refresh rather
//! than shown with an "ended" label -- see refreshFromCache(). reads from GuiCalendarMgr's cache
//! ( already populated by the automatic on-join fetch, see event-calendar design notes Phase
//! 3a/4a ) rather than talking to the engine directly.
class CalendarEventListWidget : public ListWidgetBase
{
    Q_OBJECT
public:
    CalendarEventListWidget( QWidget* parent );
    virtual ~CalendarEventListWidget() = default;

    //! which host's calendar to show. triggers an immediate refresh request ( pull-to-refresh
    //! semantics, per fromGuiCalendarRefresh -- harmless even if the cache is already populated
    //! from the automatic on-join fetch ) and rebuilds the list from whatever is cached now.
    void                        setHostAdminId( HostedId& adminId );

    //! whether the viewing user is this host's admin -- controls Edit/Cancel button visibility
    //! on each row ( see CalendarEventRowWidget ). server re-validates admin status regardless;
    //! this only controls what the client shows.
    void                        setIsAdmin( bool isAdmin );
    bool                        getIsAdmin( void )                      { return m_IsAdmin; }

protected slots:
    void                        slotCalendarEventListUpdated( EHostType hostType, VxGUID hostOnlineId );

protected:
    void                        refreshFromCache( void );

    HostedId                    m_AdminId;
    bool                        m_IsAdmin{ false };
};
