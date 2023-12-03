//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxTagLabel.h"

#include <CoreLib/VxDebug.h>

//============================================================================
VxTagLabel::VxTagLabel(QWidget* parent, Qt::WindowFlags f) 
: VxLabel(parent,f) 
{
}

//============================================================================
VxTagLabel::VxTagLabel(const QString &text, QWidget* parent, Qt::WindowFlags f) 
: VxLabel(text,parent,f) 
{
	setupQTagLabel();
}

//============================================================================
VxTagLabel::~VxTagLabel()
{
	setupQTagLabel();
}

//============================================================================
void VxTagLabel::setupQTagLabel( void )
{
    setObjectName( "VxTagLabel" );
	connect( this, SIGNAL(clicked()), this, SLOT(slotOnClicked()) );
}

//============================================================================
void VxTagLabel::slotOnClicked( void )
{
	if( 0 == m_AssetInfo )
	{
		return;
	}


}
