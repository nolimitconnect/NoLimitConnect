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

#include <QListWidgetItem>

//! pure data holder for one row of CalendarEventListWidget -- the visual row is a separate
//! CalendarEventRowWidget set via QListWidget::setItemWidget(), not this class ( unlike
//! GuiOfferListItem's dual QWidget+QListWidgetItem inheritance, avoided here since that pattern
//! is tied to the .ui-based IdentWidget mixin this feature deliberately isn't using -- see
//! event-calendar design notes ).
class CalendarEventListItem : public QListWidgetItem
{
public:
    CalendarEventListItem();
    virtual ~CalendarEventListItem() = default;

    void                        setEventInfo( CalendarEventInfo& eventInfo )   { m_EventInfo = eventInfo; }
    CalendarEventInfo&          getEventInfo( void )                          { return m_EventInfo; }

    //! the specific occurrence this row represents -- the next upcoming occurrence as of when
    //! the row was built, or -1 if the event has no future occurrence ( fully past, non-recurring ).
    void                        setOccurrenceStartMs( int64_t occurrenceStartMs ) { m_OccurrenceStartMs = occurrenceStartMs; }
    int64_t                     getOccurrenceStartMs( void )                  { return m_OccurrenceStartMs; }

    QSize                       calculateSizeHint( void );

protected:
    CalendarEventInfo           m_EventInfo;
    int64_t                     m_OccurrenceStartMs{ -1 };
};
