#pragma once
//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
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

	//virtual void                fromGuiResizeBegin( EAppModule appModule, int winWidth, int winHeight ) = 0;
	//virtual void                fromGuiResizeEvent( EAppModule appModule, int winWidth, int winHeight ) = 0;
	//virtual void                fromGuiResizeEnd( EAppModule appModule, int winWidth, int winHeight ) = 0;
    //virtual void                fromGuiRenderWindowResize( EAppModule appModule, int winWidth, int winHeight ) = 0;

    virtual void                fromGuiCloseEvent( EAppModule appModule ) = 0;
    virtual void                fromGuiVisibleEvent( EAppModule appModule, bool isVisible ) = 0;

};
