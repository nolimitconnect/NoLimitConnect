//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "CalendarEventListWidget.h"
#include "CalendarEventListItem.h"
#include "CalendarEventRowWidget.h"
#include "AppCommon.h"

#include <CoreLib/VxTime.h>

#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>

#include <algorithm>

//============================================================================
void ShowCalendarEventListPopup( HostedId& adminId, QWidget* parent )
{
    QDialog viewDlg( parent );
    viewDlg.setWindowTitle( QObject::tr( "Existing Events" ) );
    viewDlg.resize( 480, 400 );

    QVBoxLayout* layout = new QVBoxLayout( &viewDlg );

    CalendarEventListWidget* listWidget = new CalendarEventListWidget( &viewDlg );
    listWidget->setIsAdmin( false ); // browsing only -- Edit/Cancel controls not relevant here
    listWidget->setHostAdminId( adminId );
    layout->addWidget( listWidget );

    QDialogButtonBox* buttonBox = new QDialogButtonBox( QDialogButtonBox::Close, &viewDlg );
    QObject::connect( buttonBox, SIGNAL(rejected()), &viewDlg, SLOT(reject()) );
    QObject::connect( buttonBox, SIGNAL(accepted()), &viewDlg, SLOT(accept()) );
    layout->addWidget( buttonBox );

    viewDlg.setLayout( layout );
    GetAppInstance().getAppTheme().applyTheme( &viewDlg );
    viewDlg.exec();
}

//============================================================================
CalendarEventListWidget::CalendarEventListWidget( QWidget* parent )
    : ListWidgetBase( parent )
{
    connect( &getMyApp().getCalendarMgr(), SIGNAL(signalCalendarEventListUpdated(EHostType,VxGUID)),
             this, SLOT(slotCalendarEventListUpdated(EHostType,VxGUID)) );
}

//============================================================================
void CalendarEventListWidget::setHostAdminId( HostedId& adminId )
{
    m_AdminId = adminId;
    setHostType( adminId.getHostType() );

    getMyApp().getCalendarMgr().requestRefresh( m_AdminId );
    refreshFromCache();
}

//============================================================================
void CalendarEventListWidget::setIsAdmin( bool isAdmin )
{
    m_IsAdmin = isAdmin;
    refreshFromCache();
}

//============================================================================
void CalendarEventListWidget::slotCalendarEventListUpdated( EHostType hostType, VxGUID hostOnlineId )
{
    if( hostOnlineId != m_AdminId.getHostOnlineId() )
    {
        return; // a different host's calendar was updated
    }

    refreshFromCache();
}

//============================================================================
void CalendarEventListWidget::refreshFromCache( void )
{
    clear();

    VxGUID hostOnlineId = m_AdminId.getHostOnlineId();
    std::vector<CalendarEventInfo> events = getMyApp().getCalendarMgr().getEventList( hostOnlineId );

    int64_t nowMs = GetGmtTimeMs();
    std::vector<std::pair<int64_t, CalendarEventInfo>> sortedEvents;
    for( auto& eventInfo : events )
    {
        int64_t occurrenceStartMs = eventInfo.getNextOccurrenceStartMs( nowMs );
        if( occurrenceStartMs < 0 )
        {
            continue; // non-recurring event with no future occurrence -- drop it from the list
                      // entirely on refresh rather than showing a permanent "ended" row. the
                      // underlying record itself still exists ( and still purges on its own
                      // retention schedule ) -- this only affects what's displayed.
        }
        sortedEvents.push_back( std::make_pair( occurrenceStartMs, eventInfo ) );
    }

    // upcoming events first ( ascending occurrence time )
    std::sort( sortedEvents.begin(), sortedEvents.end(), []( const std::pair<int64_t, CalendarEventInfo>& lhs, const std::pair<int64_t, CalendarEventInfo>& rhs )
    {
        return lhs.first < rhs.first;
    } );

    for( auto& entry : sortedEvents )
    {
        int64_t occurrenceStartMs = entry.first;
        CalendarEventInfo& eventInfo = entry.second;

        CalendarEventListItem* item = new CalendarEventListItem();
        item->setEventInfo( eventInfo );
        item->setOccurrenceStartMs( occurrenceStartMs );
        item->setSizeHint( item->calculateSizeHint() );
        addItem( item );

        CalendarEventRowWidget* rowWidget = new CalendarEventRowWidget( this );
        rowWidget->setEventInfo( m_AdminId, eventInfo, occurrenceStartMs, m_IsAdmin );
        setItemWidget( item, rowWidget );
    }
}
