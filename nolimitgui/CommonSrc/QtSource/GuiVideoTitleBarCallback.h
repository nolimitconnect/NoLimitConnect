#pragma once
//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <GuiInterface/IDefs.h>

class QPixmap;

class GuiVideoTitleBarCallback
{
public:
	virtual void				callbackGuiVideoTitleBarPixmap( QPixmap& vidPixmap ) {};
};

