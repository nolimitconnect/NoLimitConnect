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

#include <CoreLib/HostedId.h>

#include <QWidget>

class AppCommon;
class CalendarEventListWidget;
class QLabel;
class VxPushButton;

//! self-contained calendar panel for one joined host -- an "N attending" live count, a
//! "Create Event" button ( admin-only, opens DialogCalendarEvent ), and a CalendarEventListWidget.
//! this is the single widget meant to be embedded into a host session screen ( eg
//! AppletChatRoomClient/GuiHostSession's "event mode" -- see event-calendar design notes ),
//! rather than wiring the pieces separately at every call site.
class CalendarEventListPanel : public QWidget
{
    Q_OBJECT
public:
    CalendarEventListPanel( QWidget* parent = nullptr );
    virtual ~CalendarEventListPanel() = default;

    //! which host's calendar to show, and whether the viewing user is that host's admin
    //! ( controls both the Create button here and each row's Edit/Cancel buttons ).
    void                        setHostAdminId( HostedId& adminId, bool isAdmin );

    CalendarEventListWidget*    getListWidget( void )                   { return m_ListWidget; }

protected slots:
    void                        slotCreateClicked( void );
    void                        slotViewExistingEventsClicked( void );

protected:
    //! re-reads the live connected-member count for m_AdminId and updates m_AttendingLabel.
    //! not push-updated ( no live "member joined/left" signal exists for this yet ) -- refreshed
    //! whenever the panel is targeted or shown, which is accurate enough for a number the user
    //! only looks at while the panel is open.
    void                        refreshAttendingCount( void );

    //! also re-triggers the calendar list refresh ( not just the attending count ) -- toggling
    //! the panel visible via "Show Events" was only ever refreshing once, at join time, so an
    //! event created/canceled after that point never appeared until something else happened to
    //! repopulate the cache. see event-calendar design notes.
    void                        showEvent( QShowEvent* event ) override;

    AppCommon&                  m_MyApp;

    QLabel*                     m_AttendingLabel{ nullptr };
    VxPushButton*               m_CreateButton{ nullptr };
    VxPushButton*               m_ViewExistingEventsButton{ nullptr };
    CalendarEventListWidget*    m_ListWidget{ nullptr };

    HostedId                    m_AdminId;
};
