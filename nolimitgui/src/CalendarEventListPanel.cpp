//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "CalendarEventListPanel.h"
#include "CalendarEventListWidget.h"
#include "DialogCalendarEvent.h"
#include "VxPushButton.h"
#include "AppCommon.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFontMetrics>

//============================================================================
// VxPushButton::sizeHint() always returns a fixed square icon-button size
// ( GuiParams::getButtonSize() ), regardless of the button's text -- fine for
// the icon-only buttons it was designed for, but it clips a text label like
// "View Existing Events" down to a tiny square. Widen/heighten the button to
// actually fit its own label.
static void sizeButtonToFitText( VxPushButton* button )
{
    QFontMetrics fontMetrics( button->font() );
    QSize textSize = fontMetrics.size( Qt::TextSingleLine, button->text() );
    button->setMinimumWidth( textSize.width() + 24 );
    button->setMinimumHeight( textSize.height() + 12 );
}

//============================================================================
CalendarEventListPanel::CalendarEventListPanel( QWidget* parent )
    : QWidget( parent )
    , m_MyApp( GetAppInstance() )
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 4 );

    m_AttendingLabel = new QLabel( this );
    layout->addWidget( m_AttendingLabel );

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins( 0, 0, 0, 0 );

    m_CreateButton = new VxPushButton( tr( "Create Event" ), this );
    m_CreateButton->setVisible( false );
    sizeButtonToFitText( m_CreateButton );
    connect( m_CreateButton, SIGNAL(clicked()), this, SLOT(slotCreateClicked()) );
    buttonLayout->addWidget( m_CreateButton );

    m_ViewExistingEventsButton = new VxPushButton( tr( "View Existing Events" ), this );
    sizeButtonToFitText( m_ViewExistingEventsButton );
    connect( m_ViewExistingEventsButton, SIGNAL(clicked()), this, SLOT(slotViewExistingEventsClicked()) );
    buttonLayout->addWidget( m_ViewExistingEventsButton );

    buttonLayout->addStretch( 1 );
    layout->addLayout( buttonLayout );

    m_ListWidget = new CalendarEventListWidget( this );
    layout->addWidget( m_ListWidget );

    setLayout( layout );
}

//============================================================================
void CalendarEventListPanel::setHostAdminId( HostedId& adminId, bool isAdmin )
{
    if( LogEnabled( eLogCalendar ) ) LogModule( eLogCalendar, LOG_VERBOSE, "[%d ms] CalendarEventListPanel::%s host %s isAdmin=%d",
        GetApplicationAliveMs(), __func__, adminId.getHostOnlineId().toHexString().c_str(), isAdmin );
    m_AdminId = adminId;
    m_CreateButton->setVisible( isAdmin );
    m_ListWidget->setHostAdminId( adminId );
    m_ListWidget->setIsAdmin( isAdmin );
    refreshAttendingCount();
}

//============================================================================
void CalendarEventListPanel::refreshAttendingCount( void )
{
    if( !m_AdminId.isValid() )
    {
        return;
    }

    uint32_t attendingCount = m_MyApp.getCalendarMgr().getAttendingCount( m_AdminId );
    m_AttendingLabel->setText( tr( "%1 attending now" ).arg( attendingCount ) );
}

//============================================================================
void CalendarEventListPanel::showEvent( QShowEvent* event )
{
    QWidget::showEvent( event );
    refreshAttendingCount();

    // re-fetch the calendar list every time the panel becomes visible ( eg "Show Events"
    // toggled on ) -- setHostAdminId() only ever ran once, at join time, so an event
    // created/canceled after that point never showed up until something else happened to
    // repopulate the cache. setHostAdminId() is idempotent ( same admin id/host type ), so
    // calling it again here is harmless even when the cache is already fresh.
    if( m_AdminId.isValid() )
    {
        m_ListWidget->setHostAdminId( m_AdminId );
    }
}

//============================================================================
void CalendarEventListPanel::slotCreateClicked( void )
{
    DialogCalendarEvent createDlg( m_AdminId, this );
    createDlg.exec();
}

//============================================================================
void CalendarEventListPanel::slotViewExistingEventsClicked( void )
{
    ShowCalendarEventListPopup( m_AdminId, this );
}
