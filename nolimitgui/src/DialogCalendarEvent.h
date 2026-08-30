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

#include <QDialog>

class AppCommon;
class AcceptCancelWidget;
class VxPushButton;
class QLineEdit;
class QPlainTextEdit;
class QDateEdit;
class QTimeEdit;
class QSpinBox;
class QComboBox;
class QCheckBox;
class QLabel;

//! host-admin-only create/edit form for one calendar event. modal ( see showEvent() note below )
//! -- Save sends the update request and stays open showing "Saving..." until the ack arrives via
//! GuiCalendarMgr::signalCalendarEventUpdateStatus, then closes on success or shows the error and
//! re-enables the form on failure.
//!
//! modality note: the ack correlates to this dialog purely by matching hostOnlineId ( there is no
//! session/request id in toGuiCalendarEventUpdateStatus -- see event-calendar design notes ).
//! that is only safe because the dialog is application-modal ( exec() ), so no other calendar
//! update request for the same host can be in flight while it is open.
//!
//! invite-list ( ECalendarAttendance::eCalendarAttendanceUserList ) picker UI is deliberately NOT
//! implemented yet -- no multi-select member picker exists in the codebase to build on
//! ( GuiUserMultiListWidget is single-select despite the name ). Attendance mode is fixed to
//! "Everybody" in this pass; see event-calendar design notes for the explicit deferral.
class DialogCalendarEvent : public QDialog
{
    Q_OBJECT
public:
    //! create-new constructor
    DialogCalendarEvent( HostedId& adminId, QWidget* parent = nullptr );
    //! edit-existing constructor -- pre-fills the form from eventInfo, keeps its event id
    DialogCalendarEvent( HostedId& adminId, CalendarEventInfo& eventInfo, QWidget* parent = nullptr );
    virtual ~DialogCalendarEvent() = default;

protected slots:
    void                        slotSaveClicked( void );
    void                        slotCancelClicked( void );
    void                        slotRecurrenceChanged( int index );
    void                        slotEventUpdateStatus( EHostType hostType, VxGUID hostOnlineId, VxGUID eventId, ECommErr commErr );
    //! lets the user browse this host's existing events without leaving the create/edit form --
    //! the panel's own list ( CalendarEventListPanel ) is blocked behind this modal dialog while
    //! it's open, so there was previously no way to check what already exists before creating.
    void                        slotViewExistingEventsClicked( void );

protected:
    void                        buildForm( void );
    void                        loadFromEventInfo( void );
    void                        setFormEnabled( bool enabled );

    //=== vars ===//
    AppCommon&                  m_MyApp;

    HostedId                    m_AdminId;
    CalendarEventInfo           m_EventInfo;
    bool                        m_IsEditing{ false };

    QLineEdit*                  m_NameEdit{ nullptr };
    QPlainTextEdit*             m_DescriptionEdit{ nullptr };
    QDateEdit*                  m_StartDateEdit{ nullptr };
    QTimeEdit*                  m_StartTimeEdit{ nullptr };
    QSpinBox*                   m_DurationHoursSpin{ nullptr };
    QSpinBox*                   m_DurationMinutesSpin{ nullptr };
    QComboBox*                  m_RecurrenceCombo{ nullptr };
    QSpinBox*                   m_RecurrenceDayOfMonthSpin{ nullptr };
    QLabel*                     m_RecurrenceDayOfMonthLabel{ nullptr };
    //! hours, not days -- range covers 1 hour to 1 year so "1 hour" retention ( eg for a short
    //! auto-hosted test event ) is directly reachable, same as any other duration previously
    //! only expressible in whole days. see event-calendar design notes.
    QSpinBox*                   m_RetentionHoursSpin{ nullptr };
    QCheckBox*                  m_VideoAllowedCheck{ nullptr };
    QLabel*                     m_StatusLabel{ nullptr };
    AcceptCancelWidget*         m_AcceptCancelWidget{ nullptr };
    VxPushButton*               m_ViewExistingEventsButton{ nullptr };
};
