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

#include <QFrame>

class AppCommon;
class QLabel;
class VxPushButton;

//! visual row for one CalendarEventListItem -- name / description / formatted date-time /
//! recurrence label.
//! set via QListWidget::setItemWidget(), not a QListWidgetItem itself -- see
//! CalendarEventListItem.h. host admin ( per isAdmin, see CalendarEventListWidget::setIsAdmin )
//! additionally gets Edit and Cancel buttons -- server re-validates admin status regardless,
//! this only controls whether the buttons are shown.
//!
//! no rsvp/accept-decline control here -- events are announcement-only, with no per-identity
//! response recorded anywhere. see event-calendar design notes, "no per-identity RSVP record".
class CalendarEventRowWidget : public QFrame
{
    Q_OBJECT
public:
    CalendarEventRowWidget( QWidget* parent = nullptr );
    virtual ~CalendarEventRowWidget() = default;

    //! adminId identifies the host this event belongs to ( needed for the Edit/Cancel actions ).
    void                        setEventInfo( HostedId& adminId, CalendarEventInfo& eventInfo, int64_t occurrenceStartMs, bool isAdmin );

protected slots:
    void                        slotEditClicked( void );
    void                        slotCancelClicked( void );

protected:
    AppCommon&                  m_MyApp;

    QLabel*                     m_NameLabel{ nullptr };
    QLabel*                     m_DescriptionLabel{ nullptr };
    QLabel*                     m_TimeLabel{ nullptr };
    QLabel*                     m_RecurrenceLabel{ nullptr };
    VxPushButton*               m_EditButton{ nullptr };
    VxPushButton*               m_CancelButton{ nullptr };

    HostedId                    m_AdminId;
    CalendarEventInfo           m_EventInfo;
};
