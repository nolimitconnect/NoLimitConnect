//============================================================================
// Copyright (C) 2017 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ImagePickBar.h"

#include "MyIcons.h"
#include "AppCommon.h"

//============================================================================
ImagePickBar::ImagePickBar( QWidget* parent )
    : QWidget( parent )
    , m_MyApp( GetAppInstance() )
{
}

