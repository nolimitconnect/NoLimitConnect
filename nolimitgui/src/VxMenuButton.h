#pragma once
//============================================================================
// Copyright (C) 2017 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "SoundDefs.h"
#include "MyIconsDefs.h"
#include "VxContextMenu.h"

#include "VxPushButton.h"

class VxContextMenu;
class AppCommon;
class MyIcons;
class QTimer;

class VxMenuButton : public VxPushButton
{
    Q_OBJECT
public:
    VxMenuButton( QWidget* parent = nullptr );
    VxMenuButton( const QString & text, QWidget* parent = nullptr );
    virtual ~VxMenuButton() {};

    // setMenuId  must be called before add menu item ( can be 0 )
    void                        setMenuId( int menuId );
    void                        addMenuItem( EMenuItemType eMenuItem, QString menuText = "" );
    void                        showMenu( const QPoint & globalPoint );

signals:
    void                        signalMenuItemSelected( int menuId, EMenuItemType eMenuItem );

protected slots:
    void                        slotMenuItemSelected( int menuId, EMenuItemType eMenuItem );
    void                        slotMenuButtonClicked( void );

protected:
    void                        setMenuLocation( bool showAboveButton );

    VxContextMenu               m_ContextMenu;
};

