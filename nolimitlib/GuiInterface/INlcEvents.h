#pragma once
//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "IDefs.h"

class INlcEvents
{
public:
    virtual void                fromGuiKeyPressEvent( EAppModule appModule, int key, int mod ) = 0;
    virtual void                fromGuiKeyReleaseEvent( EAppModule appModule, int key, int mod ) = 0;

    virtual void                fromGuiMousePressEvent( EAppModule appModule, int mouseXPos, int mouseYPos, int mouseButton ) = 0;
    virtual void                fromGuiMouseReleaseEvent( EAppModule appModule, int mouseXPos, int mouseYPos, int mouseButton ) = 0;
    virtual void                fromGuiMouseMoveEvent( EAppModule appModule, int mouseXPos, int mouseYPos ) = 0;

	virtual void                fromGuiResizeBegin( EAppModule appModule, int winWidth, int winHeight ) = 0;
	virtual void                fromGuiResizeEvent( EAppModule appModule, int winWidth, int winHeight ) = 0;
	virtual void                fromGuiResizeEnd( EAppModule appModule, int winWidth, int winHeight ) = 0;

    virtual void                fromGuiCloseEvent( EAppModule appModule ) = 0;
    virtual void                fromGuiVisibleEvent( EAppModule appModule, bool isVisible ) = 0;

};
