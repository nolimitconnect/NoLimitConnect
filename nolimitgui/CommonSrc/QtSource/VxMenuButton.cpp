//============================================================================
// Copyright (C) 2017 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxMenuButton.h"

#include "AppCommon.h"
#include "HomeWindow.h"

//============================================================================
VxMenuButton::VxMenuButton( QWidget* parent )
: VxPushButton( parent )
, m_ContextMenu( getMyApp(), this )
{
    setObjectName( "VxMenuButton" );
    connect( this, SIGNAL(clicked()), this, SLOT(slotMenuButtonClicked()) );
    connect( &m_ContextMenu, SIGNAL(signalMenuItemSelected(int,EMenuItemType)), this, SLOT(slotMenuItemSelected(int,EMenuItemType)) );
}

//============================================================================
VxMenuButton::VxMenuButton( const QString & text, QWidget* parent )
: VxPushButton( text, parent )
, m_ContextMenu( getMyApp(), this )
{
    connect( this, SIGNAL(clicked()), this, SLOT(slotMenuButtonClicked()) );
    connect( &m_ContextMenu, SIGNAL(signalMenuItemSelected(int,EMenuItemType)), this, SLOT(slotMenuItemSelected(int,EMenuItemType)) );
}

//============================================================================
void VxMenuButton::slotMenuButtonClicked( void )
{
    QPoint rightPoint = m_MyApp.getHomeWindow().mapToGlobal( this->geometry().topRight() );
    QPoint topPoint;

    if( m_ContextMenu.getIsShowMenuAbove() )
    {
        topPoint = mapToGlobal( this->geometry().topRight() );
    }
    else
    {
        topPoint = mapToGlobal( this->geometry().bottomRight() );
    }

    m_ContextMenu.showMenu( QPoint( rightPoint.x(), topPoint.y() ) );
}

//============================================================================
void VxMenuButton::setMenuId( int menuId )
{
    m_ContextMenu.setMenuId( menuId );
}

//============================================================================
void VxMenuButton::slotMenuItemSelected( int menuId, EMenuItemType eMenuItem )
{
    emit signalMenuItemSelected( menuId, eMenuItem );
}

//============================================================================
void VxMenuButton::showMenu( const QPoint & globalPoint )
{
    m_ContextMenu.showMenu( globalPoint );
}

//============================================================================
void VxMenuButton::setMenuLocation( bool showAboveButton )
{
    m_ContextMenu.setMenuLocation( showAboveButton );
}

//============================================================================
void VxMenuButton::addMenuItem( EMenuItemType eMenuItem, QString menuText )
{
    m_ContextMenu.addMenuItem( eMenuItem, menuText );
}
