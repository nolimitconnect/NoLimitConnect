//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxWidgetBase.h"

#include <CoreLib/ObjectCommonDefs.h>

//============================================================================
VxWidgetBase::VxWidgetBase( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
, ObjectCommon( OBJNAME_UNKNOWN )
{
}

//============================================================================
VxWidgetBase::VxWidgetBase( const char* objName, QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
, ObjectCommon( objName )
{
}

//============================================================================
void VxWidgetBase::mousePressEvent( QMouseEvent * event )
{
	QWidget::mousePressEvent( event );
	emit clicked();
}
