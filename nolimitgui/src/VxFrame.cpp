//============================================================================
// Copyright (C) 2017 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxFrame.h"

#include <CoreLib/VxDebug.h>

//============================================================================
VxFrame::VxFrame( AppCommon& appCommon, QWidget* parent, Qt::WindowFlags f )
	: QFrame( parent, f )
    , m_MyApp( appCommon )
	, m_FrameId( 1 )
{
    setObjectName( "VxFrame" );
}

//============================================================================
void VxFrame::resizeEvent( QResizeEvent* ev )
{
	QFrame::resizeEvent( ev );
	emit signalFrameResized();
}

//============================================================================
void VxFrame::slotAppSystemMenuSelected( int menuId, QWidget* popupMenu )
{
    LogMsg( LOG_DEBUG, "slotAppSystemMenuSelected menu id %d", menuId );
    switch( menuId )
    {
    case 0: // debug settings
        LogMsg( LOG_DEBUG, "slotAppSystemMenuSelected debug settings menu id %d", menuId );
        break;

    case 1: // debug log
        LogMsg( LOG_DEBUG, "slotAppSystemMenuSelected debug log menu id %d", menuId );
        break;

    default:
        break;
    }
}
