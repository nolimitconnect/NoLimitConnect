//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "DialogCalendarEvent.h"
#include "AcceptCancelWidget.h"
#include "CalendarEventListWidget.h"
#include "VxPushButton.h"
#include "AppCommon.h"

#include <CoreLib/VxTime.h>
#include <CoreLib/VxDebug.h>

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QDateEdit>
#include <QTimeEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QMessageBox>

//============================================================================
DialogCalendarEvent::DialogCalendarEvent( HostedId& adminId, QWidget* parent )
    : QDialog( parent )
    , m_MyApp( GetAppInstance() )
    , m_AdminId( adminId )
    , m_IsEditing( false )
{
    // sensible defaults for a brand new event
    int64_t nowMs = GetGmtTimeMs();
    m_EventInfo.setStartTimeMs( nowMs + ( 60 * 60 * 1000 ) ); // starts an hour from now
    m_EventInfo.setDurationMs( 60 * 60 * 1000 ); // 1 hour
    m_EventInfo.setRetentionMs( 48LL * 60 * 60 * 1000 ); // 48 hours
    m_EventInfo.setVideoPostingsAllowed( true );

    setWindowTitle( tr( "Create Event" ) );
    buildForm();
    loadFromEventInfo();
}

//============================================================================
DialogCalendarEvent::DialogCalendarEvent( HostedId& adminId, CalendarEventInfo& eventInfo, QWidget* parent )
    : QDialog( parent )
    , m_MyApp( GetAppInstance() )
    , m_AdminId( adminId )
    , m_EventInfo( eventInfo )
    , m_IsEditing( true )
{
    setWindowTitle( tr( "Edit Event" ) );
    buildForm();
    loadFromEventInfo();
}

//============================================================================
void DialogCalendarEvent::buildForm( void )
{
    setModal( true );

    QVBoxLayout* mainLayout = new QVBoxLayout( this );

    m_ViewExistingEventsButton = new VxPushButton( tr( "View Existing Events" ), this );
    connect( m_ViewExistingEventsButton, SIGNAL(clicked()), this, SLOT(slotViewExistingEventsClicked()) );
    mainLayout->addWidget( m_ViewExistingEventsButton );

    QFormLayout* formLayout = new QFormLayout();

    m_NameEdit = new QLineEdit( this );
    formLayout->addRow( tr( "Event Name" ), m_NameEdit );

    m_DescriptionEdit = new QPlainTextEdit( this );
    m_DescriptionEdit->setMaximumHeight( 80 );
    formLayout->addRow( tr( "Description" ), m_DescriptionEdit );

    m_StartDateEdit = new QDateEdit( this );
    m_StartDateEdit->setCalendarPopup( true );
    m_StartDateEdit->setDisplayFormat( "yyyy-MM-dd" );
    formLayout->addRow( tr( "Start Date" ), m_StartDateEdit );

    m_StartTimeEdit = new QTimeEdit( this );
    m_StartTimeEdit->setDisplayFormat( "hh:mm" );
    formLayout->addRow( tr( "Start Time" ), m_StartTimeEdit );

    QWidget* durationWidget = new QWidget( this );
    QHBoxLayout* durationLayout = new QHBoxLayout( durationWidget );
    durationLayout->setContentsMargins( 0, 0, 0, 0 );
    m_DurationHoursSpin = new QSpinBox( this );
    m_DurationHoursSpin->setRange( 0, 999 );
    m_DurationHoursSpin->setSuffix( tr( " hr" ) );
    m_DurationMinutesSpin = new QSpinBox( this );
    m_DurationMinutesSpin->setRange( 0, 59 );
    m_DurationMinutesSpin->setSuffix( tr( " min" ) );
    durationLayout->addWidget( m_DurationHoursSpin );
    durationLayout->addWidget( m_DurationMinutesSpin );
    durationWidget->setLayout( durationLayout );
    formLayout->addRow( tr( "Duration" ), durationWidget );

    m_RecurrenceCombo = new QComboBox( this );
    m_RecurrenceCombo->addItem( tr( "Never" ), eCalendarRecurrenceNever );
    m_RecurrenceCombo->addItem( tr( "Every Day" ), eCalendarRecurrenceEveryDay );
    m_RecurrenceCombo->addItem( tr( "Every Week" ), eCalendarRecurrenceEveryWeek );
    m_RecurrenceCombo->addItem( tr( "Every Month" ), eCalendarRecurrenceEveryMonth );
    formLayout->addRow( tr( "Recurrence" ), m_RecurrenceCombo );
    connect( m_RecurrenceCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotRecurrenceChanged(int)) );

    m_RecurrenceDayOfMonthLabel = new QLabel( tr( "Day of Month" ), this );
    m_RecurrenceDayOfMonthSpin = new QSpinBox( this );
    m_RecurrenceDayOfMonthSpin->setRange( 1, 31 );
    formLayout->addRow( m_RecurrenceDayOfMonthLabel, m_RecurrenceDayOfMonthSpin );

    m_RetentionHoursSpin = new QSpinBox( this );
    m_RetentionHoursSpin->setRange( 1, 8760 ); // 1 hour .. 1 year
    m_RetentionHoursSpin->setSuffix( tr( " hours" ) );
    formLayout->addRow( tr( "Keep Content For" ), m_RetentionHoursSpin );

    m_VideoAllowedCheck = new QCheckBox( tr( "Allow video/audio postings during this event" ), this );
    formLayout->addRow( QString(), m_VideoAllowedCheck );

    mainLayout->addLayout( formLayout );

    m_StatusLabel = new QLabel( this );
    mainLayout->addWidget( m_StatusLabel );

    m_AcceptCancelWidget = new AcceptCancelWidget( this );
    m_AcceptCancelWidget->showCancelButton( true );
    mainLayout->addWidget( m_AcceptCancelWidget );

    connect( m_AcceptCancelWidget, SIGNAL(signalAccepted()), this, SLOT(slotSaveClicked()) );
    connect( m_AcceptCancelWidget, SIGNAL(signalCanceled()), this, SLOT(slotCancelClicked()) );

    connect( &m_MyApp.getCalendarMgr(), SIGNAL(signalCalendarEventUpdateStatus(EHostType,VxGUID,VxGUID,ECommErr)),
             this, SLOT(slotEventUpdateStatus(EHostType,VxGUID,VxGUID,ECommErr)) );

    setLayout( mainLayout );

    // dialog does not get palette from parent.. force palette update ( see DialogAddComment.cpp )
    m_MyApp.getAppTheme().applyTheme( this );
}

//============================================================================
void DialogCalendarEvent::loadFromEventInfo( void )
{
    m_NameEdit->setText( QString::fromStdString( m_EventInfo.getEventName() ) );
    m_DescriptionEdit->setPlainText( QString::fromStdString( m_EventInfo.getEventDescription() ) );
    QDateTime startDateTime = QDateTime::fromMSecsSinceEpoch( m_EventInfo.getStartTimeMs() );
    m_StartDateEdit->setDate( startDateTime.date() );
    m_StartTimeEdit->setTime( startDateTime.time() );

    int64_t durationMs = m_EventInfo.getDurationMs();
    m_DurationHoursSpin->setValue( ( int )( durationMs / ( 60 * 60 * 1000 ) ) );
    m_DurationMinutesSpin->setValue( ( int )( ( durationMs / ( 60 * 1000 ) ) % 60 ) );

    int recurrenceIndex = m_RecurrenceCombo->findData( m_EventInfo.getRecurrence() );
    m_RecurrenceCombo->setCurrentIndex( recurrenceIndex >= 0 ? recurrenceIndex : 0 );
    m_RecurrenceDayOfMonthSpin->setValue( m_EventInfo.getRecurrenceDayOfMonth() > 0 ? m_EventInfo.getRecurrenceDayOfMonth() : 1 );
    slotRecurrenceChanged( m_RecurrenceCombo->currentIndex() );

    int64_t retentionHours = m_EventInfo.getRetentionMs() / ( 60LL * 60 * 1000 );
    m_RetentionHoursSpin->setValue( retentionHours > 0 ? ( int )retentionHours : 48 );

    m_VideoAllowedCheck->setChecked( m_EventInfo.isVideoPostingsAllowed() );
}

//============================================================================
void DialogCalendarEvent::slotRecurrenceChanged( int index )
{
    bool isMonthly = ( eCalendarRecurrenceEveryMonth == m_RecurrenceCombo->itemData( index ).toInt() );
    m_RecurrenceDayOfMonthLabel->setVisible( isMonthly );
    m_RecurrenceDayOfMonthSpin->setVisible( isMonthly );
}

//============================================================================
void DialogCalendarEvent::setFormEnabled( bool enabled )
{
    m_NameEdit->setEnabled( enabled );
    m_DescriptionEdit->setEnabled( enabled );
    m_StartDateEdit->setEnabled( enabled );
    m_StartTimeEdit->setEnabled( enabled );
    m_DurationHoursSpin->setEnabled( enabled );
    m_DurationMinutesSpin->setEnabled( enabled );
    m_RecurrenceCombo->setEnabled( enabled );
    m_RecurrenceDayOfMonthSpin->setEnabled( enabled );
    m_RetentionHoursSpin->setEnabled( enabled );
    m_VideoAllowedCheck->setEnabled( enabled );
    m_AcceptCancelWidget->setEnabled( enabled );
}

//============================================================================
void DialogCalendarEvent::slotSaveClicked( void )
{
    QString name = m_NameEdit->text().trimmed();
    if( name.isEmpty() )
    {
        m_StatusLabel->setText( tr( "Event name is required" ) );
        return;
    }

    std::string nameStr = name.toStdString();
    std::string descStr = m_DescriptionEdit->toPlainText().toStdString();
    bool nameTruncated = nameStr.size() > CalendarEventInfo::MAX_EVENT_NAME_LEN;
    bool descTruncated = descStr.size() > CalendarEventInfo::MAX_EVENT_DESCRIPTION_LEN;

    // setEventName/setEventDescription silently truncate to their MAX_EVENT_*_LEN bytes -- warn
    // here so the truncation isn't invisible to the user.
    m_EventInfo.setEventName( nameStr );
    m_EventInfo.setEventDescription( descStr );

    if( nameTruncated || descTruncated )
    {
        QMessageBox::information( this, tr( "Text Too Long" ),
            tr( "Event name exceeded %1 bytes and/or description exceeded %2 bytes -- text was truncated." )
                .arg( CalendarEventInfo::MAX_EVENT_NAME_LEN )
                .arg( CalendarEventInfo::MAX_EVENT_DESCRIPTION_LEN ) );
    }
    QDateTime startDateTime( m_StartDateEdit->date(), m_StartTimeEdit->time() );
    m_EventInfo.setStartTimeMs( startDateTime.toMSecsSinceEpoch() );

    int64_t durationMs = ( int64_t )m_DurationHoursSpin->value() * 60 * 60 * 1000
                        + ( int64_t )m_DurationMinutesSpin->value() * 60 * 1000;
    m_EventInfo.setDurationMs( durationMs );

    ECalendarRecurrence recurrence = ( ECalendarRecurrence )m_RecurrenceCombo->currentData().toInt();
    m_EventInfo.setRecurrence( recurrence );
    m_EventInfo.setRecurrenceDayOfMonth( ( uint8_t )m_RecurrenceDayOfMonthSpin->value() );

    m_EventInfo.setRetentionMs( ( int64_t )m_RetentionHoursSpin->value() * 60 * 60 * 1000 );
    m_EventInfo.setVideoPostingsAllowed( m_VideoAllowedCheck->isChecked() );
    // attendance mode fixed to Everybody -- invite-list picker not implemented yet, see header note
    m_EventInfo.setAttendanceMode( eCalendarAttendanceEverybody );

    setFormEnabled( false );
    m_StatusLabel->setText( tr( "Saving..." ) );
    m_MyApp.getCalendarMgr().requestEventUpdate( m_AdminId, m_EventInfo );
}

//============================================================================
void DialogCalendarEvent::slotCancelClicked( void )
{
    QMessageBox::information( this, tr( "Canceled" ), tr( "Event not saved." ) );
    reject();
}

//============================================================================
void DialogCalendarEvent::slotEventUpdateStatus( EHostType hostType, VxGUID hostOnlineId, VxGUID eventId, ECommErr commErr )
{
    if( hostOnlineId != m_AdminId.getHostOnlineId() )
    {
        return; // ack for a different host -- see modality note in the header
    }

    if( eCommErrNone == commErr )
    {
        QMessageBox::information( this, tr( "Saved" ), tr( "Event saved successfully." ) );
        accept();
        return;
    }

    setFormEnabled( true );
    m_StatusLabel->setText( tr( "Error: %1" ).arg( DescribeCommError( commErr ) ) );
    QMessageBox::warning( this, tr( "Save Failed" ), tr( "Event was not saved: %1" ).arg( DescribeCommError( commErr ) ) );
}

//============================================================================
void DialogCalendarEvent::slotViewExistingEventsClicked( void )
{
    ShowCalendarEventListPopup( m_AdminId, this );
}
