//============================================================================
// Copyright (C) 2017 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxMenuButtonBottom.h"

//============================================================================
VxMenuButtonBottom::VxMenuButtonBottom( QWidget* parent )
: VxMenuButton( parent )
{
    setObjectName( "VxMenuButtonBottom" );
    setMenuLocation( true );
}

//============================================================================
VxMenuButtonBottom::VxMenuButtonBottom( const QString & text, QWidget* parent )
: VxMenuButton( text, parent )
{
    setMenuLocation( true );
}

//============================================================================
void VxMenuButtonBottom::setMenuParams( const QPoint & globalPoint, int menuId )
{
}
