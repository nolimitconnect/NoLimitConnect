//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "CalendarEventListItem.h"

#include "GuiParams.h"

//============================================================================
CalendarEventListItem::CalendarEventListItem()
    : QListWidgetItem()
{
}

//============================================================================
QSize CalendarEventListItem::calculateSizeHint( void )
{
    return QSize( ( int )( GuiParams::getGuiScale() * 320 ), ( int )( GuiParams::getButtonSize().height() * 2 ) );
}
