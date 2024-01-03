//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "GuiOfferSession.h"
#include "ActivitySessionOptions.h"
#include "AppGlobals.h"
#include "MyIcons.h"
#include <P2PEngine/P2PEngine.h>

//============================================================================
ActivitySessionOptions::ActivitySessionOptions(	AppCommon&			app,
												VxNetIdent*			netIdent, 
												QWidget*				parent )
: ActivityBase( OBJNAME_ACTIVITY_SESSION_OPTIONS, app, parent )
{
	Q_UNUSED( netIdent )
	setup();
	//sendPluginOffer( NULL, NULL, NULL );
}

//============================================================================
ActivitySessionOptions::ActivitySessionOptions(	AppCommon&				app,
												GuiOfferSession*		poOffer, 
												QWidget*				parent )
: ActivityBase( OBJNAME_ACTIVITY_SESSION_OPTIONS, app, parent )
{
	Q_UNUSED( poOffer )
	setup();
}

//============================================================================
void ActivitySessionOptions::setup()
{
	//setContentWidget( &m_VideoChatWidget );
}

//============================================================================
void ActivitySessionOptions::playVideoFrame( unsigned char * pu8Jpg, unsigned long u32JpgLen, int motion0To100000 )
{
	//m_VideoChatWidget.playVideoFrame( pu8Jpg, u32JpgLen, motion0To100000 );
}
