//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "MessengerPage.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"

#include <CoreLib/VxDebug.h>

//============================================================================
MessengerPage::MessengerPage( AppCommon& app, QWidget* parent )
: ActivityBase( OBJNAME_APPLET_MESSAGER_PAGE, app, parent )
, m_IsVisible( false )
, m_IsFullSize( false )
{
	setTitleBarText( "" );
	m_IsFullSize = getMyApp().getAppSettings().getIsMessengerFullScreen();
	setExpandWindowVisibility( true );
	setMenuBottomVisibility( true );
}

//============================================================================
void MessengerPage::showEvent( QShowEvent* )
{
	if( false == m_IsVisible )
	{
		m_IsVisible = true;
		emit signalMessengerPageChanged( m_IsVisible, m_IsFullSize );
	}
}

//============================================================================
void MessengerPage::hideEvent( QHideEvent* )
{
	if( true == m_IsVisible )
	{
		m_IsVisible = false;
		emit signalMessengerPageChanged( m_IsVisible, m_IsFullSize );
	}
}

//============================================================================
void MessengerPage::slotBackButtonClicked( void )
{
	m_IsVisible = false;
	emit signalMessengerPageChanged( m_IsVisible, m_IsFullSize );
}

