//============================================================================
// Copyright (C) 2009 Brett R. Jones 
// Issued to MIT style license by Brett R. Jones in 2017
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

#include "AppCommon.h"	

#include <AppInterface/INlc.h>

//============================================================================
void AppCommon::fromGuiKeyPressEvent( EAppModule appModule, int key, int mod )
{
    m_Nlc.fromGuiKeyPressEvent( appModule, key, mod );
}

//============================================================================
void AppCommon::fromGuiKeyReleaseEvent( EAppModule appModule, int key, int mod )
{
    m_Nlc.fromGuiKeyReleaseEvent( appModule, key, mod );
}

//============================================================================
void AppCommon::fromGuiMousePressEvent( EAppModule appModule, int mouseXPos, int mouseYPos, int mouseButton )
{
    m_Nlc.fromGuiMousePressEvent( appModule, mouseXPos, mouseYPos, mouseButton );
}

//============================================================================
void AppCommon::fromGuiMouseReleaseEvent( EAppModule appModule, int mouseXPos, int mouseYPos, int mouseButton )
{
    m_Nlc.fromGuiMouseReleaseEvent( appModule, mouseXPos, mouseYPos, mouseButton );
}

//============================================================================
void AppCommon::fromGuiMouseMoveEvent( EAppModule appModule, int mouseXPos, int mouseYPos )
{
    m_Nlc.fromGuiMouseMoveEvent( appModule, mouseXPos, mouseYPos );
}

//============================================================================
void AppCommon::fromGuiResizeBegin( EAppModule appModule, int winWidth, int winHeight )
{
	m_Nlc.fromGuiResizeBegin( appModule, winWidth, winHeight );
}

//============================================================================
void AppCommon::fromGuiResizeEvent( EAppModule appModule, int winWidth, int winHeight )
{
    m_Nlc.fromGuiResizeEvent( appModule, winWidth, winHeight );
}

//============================================================================
void AppCommon::fromGuiResizeEnd( EAppModule appModule, int winWidth, int winHeight )
{
	m_Nlc.fromGuiResizeEnd( appModule, winWidth, winHeight );
}

//============================================================================
void AppCommon::fromGuiCloseEvent( EAppModule appModule )
{
    m_Nlc.fromGuiCloseEvent( appModule );
}

//============================================================================
void AppCommon::fromGuiVisibleEvent( EAppModule appModule, bool isVisible )
{
    m_Nlc.fromGuiVisibleEvent( appModule, isVisible );
}
