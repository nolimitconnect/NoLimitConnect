//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "CalendarEventRowWidget.h"
#include "DialogCalendarEvent.h"
#include "VxPushButton.h"
#include "AppCommon.h"

#include <CoreLib/VxTimeUtil.h>
#include <CoreLib/VxTime.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

namespace
{
QString describeRecurrence( ECalendarRecurrence recurrence )
{
    switch( recurrence )
    {
    case eCalendarRecurrenceEveryDay:
        return QObject::tr( "Repeats daily" );
    case eCalendarRecurrenceEveryWeek:
        return QObject::tr( "Repeats weekly" );
    case eCalendarRecurrenceEveryMonth:
        return QObject::tr( "Repeats monthly" );
    case eCalendarRecurrenceNever:
    default:
        return QString();
    }
}
}

//============================================================================
CalendarEventRowWidget::CalendarEventRowWidget( QWidget* parent )
    : QFrame( parent )
    , m_MyApp( GetAppInstance() )
{
    QVBoxLayout* outerLayout = new QVBoxLayout( this );
    outerLayout->setContentsMargins( 4, 4, 4, 4 );
    outerLayout->setSpacing( 2 );

    QHBoxLayout* titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins( 0, 0, 0, 0 );

    m_NameLabel = new QLabel( this );
    QFont nameFont = m_NameLabel->font();
    nameFont.setBold( true );
    m_NameLabel->setFont( nameFont );
    titleLayout->addWidget( m_NameLabel );
    titleLayout->addStretch( 1 );

    m_EditButton = new VxPushButton( this );
    m_EditButton->setIcon( eMyIconEditField );
    m_EditButton->setSquareButtonSize( eButtonSizeTiny );
    m_EditButton->setToolTip( tr( "Edit" ) );
    m_EditButton->setVisible( false );
    connect( m_EditButton, SIGNAL(clicked()), this, SLOT(slotEditClicked()) );
    titleLayout->addWidget( m_EditButton );

    m_CancelButton = new VxPushButton( this );
    m_CancelButton->setIcon( eMyIconTrash );
    m_CancelButton->setSquareButtonSize( eButtonSizeTiny );
    m_CancelButton->setToolTip( tr( "Cancel Event" ) );
    m_CancelButton->setVisible( false );
    connect( m_CancelButton, SIGNAL(clicked()), this, SLOT(slotCancelClicked()) );
    titleLayout->addWidget( m_CancelButton );

    outerLayout->addLayout( titleLayout );

    m_DescriptionLabel = new QLabel( this );
    m_DescriptionLabel->setWordWrap( true );
    QFont descFont = m_DescriptionLabel->font();
    descFont.setItalic( true );
    m_DescriptionLabel->setFont( descFont );
    outerLayout->addWidget( m_DescriptionLabel );

    QHBoxLayout* detailLayout = new QHBoxLayout();
    detailLayout->setContentsMargins( 0, 0, 0, 0 );
    detailLayout->setSpacing( 8 );

    m_TimeLabel = new QLabel( this );
    detailLayout->addWidget( m_TimeLabel );

    m_RecurrenceLabel = new QLabel( this );
    detailLayout->addWidget( m_RecurrenceLabel );

    detailLayout->addStretch( 1 );

    outerLayout->addLayout( detailLayout );

    setLayout( outerLayout );
}

//============================================================================
void CalendarEventRowWidget::setEventInfo( HostedId& adminId, CalendarEventInfo& eventInfo, int64_t occurrenceStartMs, bool isAdmin )
{
    m_AdminId = adminId;
    m_EventInfo = eventInfo;

    m_NameLabel->setText( QString::fromStdString( eventInfo.getEventName() ) );

    QString description = QString::fromStdString( eventInfo.getEventDescription() );
    m_DescriptionLabel->setText( description );
    m_DescriptionLabel->setVisible( !description.isEmpty() );

    if( occurrenceStartMs >= 0 )
    {
        // formatTimeStampIntoDateAndTimeWithTextMonths formats via gmtime() directly ( no
        // implicit local conversion, unlike its Hours/Minutes sibling ) -- pre-shift by the
        // local offset first, same idiom as TimeWithZone::getLocalDateAndTimeWithTextMonths.
        // storage/wire stays GMT throughout; only display converts to local.
        int64_t localOccurrenceStartMs = occurrenceStartMs + LocalTimeZoneDifferenceMs();
        m_TimeLabel->setText( QString::fromStdString( VxTimeUtil::formatTimeStampIntoDateAndTimeWithTextMonths( localOccurrenceStartMs ) ) );
    }
    else
    {
        m_TimeLabel->setText( tr( "Event ended" ) );
    }

    m_RecurrenceLabel->setText( describeRecurrence( eventInfo.getRecurrence() ) );

    m_EditButton->setVisible( isAdmin );
    m_CancelButton->setVisible( isAdmin );
}

//============================================================================
void CalendarEventRowWidget::slotEditClicked( void )
{
    DialogCalendarEvent editDlg( m_AdminId, m_EventInfo, this );
    editDlg.exec();
}

//============================================================================
void CalendarEventRowWidget::slotCancelClicked( void )
{
    QString eventName = QString::fromStdString( m_EventInfo.getEventName() );
    int result = QMessageBox::question( this, tr( "Cancel Event" ),
        tr( "Cancel \"%1\"? This deletes the whole event series." ).arg( eventName ),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
    if( QMessageBox::Yes != result )
    {
        return;
    }

    VxGUID eventId = m_EventInfo.getEventId();
    m_MyApp.getCalendarMgr().requestEventCancel( m_AdminId, eventId );
}
