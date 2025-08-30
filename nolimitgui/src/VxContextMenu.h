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

#include "MenuDefs.h"

#include <QMenu>
#include <QPair>

class AppCommon;
class VxMenu;

class VxContextMenu : public QWidget
{
    Q_OBJECT

public:
    VxContextMenu( AppCommon& appCommon, QWidget* parent = nullptr );
    virtual ~VxContextMenu() {};

    void                        setMenuLocation( bool showAboveButton );
    bool                        getIsShowMenuAbove( void )      { return m_ShowAboveButton; }
    // setMenuParams  must be called before add menu item ( can be 0 )
    void                        setMenuId( int menuId = 0 );
    void                        addMenuItem( EMenuItemType eMenuItem, QString menuText = "" );
    void                        showMenu( const QPoint & globalPoint );
    static QString              describeMenuItem( EMenuItemType eMenuItem );

signals:
    void                        signalMenuItemSelected( int menuId, EMenuItemType eMenuItem );

protected slots:
    void                        slotMenuItemSelected( int menuId, EMenuItemType eMenuItem );
    void                        slotMenuItemTriggered();

protected:
    void                        addMenuAction( EMenuItemType eMenuItem, QString& menuText );
    QPoint                      calculatePos( QWidget* menuWidget );


    AppCommon &					m_MyApp;
    int                         m_MenuId;
    QPoint                      m_GlobalPoint;
    bool                        m_ShowAboveButton; // bottom point or top point if false
    QVector< QPair< EMenuItemType, QString > >      m_MenuItemList;
    VxMenu *                     m_NewMenu;
};

