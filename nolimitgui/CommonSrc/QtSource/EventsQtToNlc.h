#pragma once
//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

class AppCommon;

class EventsQtToNlc
{
public:
    EventsQtToNlc(  AppCommon& myApp );
    virtual ~EventsQtToNlc() = default;

    bool                        fromGuiKeyPressEvent( int key );
    bool                        fromGuiKeyReleaseEvent( int key );

    bool                        fromGuiMousePressEvent( int mouseXPos, int mouseYPos, int mouseButton );
    bool                        fromGuiMouseReleaseEvent( int mouseXPos, int mouseYPos, int mouseButton );
    bool                        fromGuiMouseMoveEvent( int mouseXPos, int mouseYPos );

	void                        fromGuiResizeBegin( int winWidth, int winHeight );
	void                        fromGuiResizeEvent( int winWidth, int winHeight );
	void                        fromGuiResizeEnd( int winWidth, int winHeight );

    void                        fromGuiCloseEvent( );
    void                        fromGuiVisibleEvent( bool isVisible );

    AppCommon&				    m_MyApp;
};
