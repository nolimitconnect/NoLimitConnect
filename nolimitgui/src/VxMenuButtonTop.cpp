//============================================================================
// Copyright (C) 2017 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxMenuButtonTop.h"

//============================================================================
VxMenuButtonTop::VxMenuButtonTop( QWidget* parent )
: VxMenuButton( parent )
{
    setObjectName( "VxMenuButtonTop" );
    setMenuLocation( false );
}

//============================================================================
VxMenuButtonTop::VxMenuButtonTop( const QString & text, QWidget* parent )
: VxMenuButton( text, parent )
{
    setMenuLocation( false );
}

