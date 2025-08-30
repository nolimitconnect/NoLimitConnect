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

#include "VxMenuButton.h"

class VxMenuButtonTop : public VxMenuButton
{
    Q_OBJECT
public:
    VxMenuButtonTop( QWidget* parent = nullptr );
    VxMenuButtonTop( const QString & text, QWidget* parent = nullptr );
    virtual ~VxMenuButtonTop() {};

    // setMenuParams  must be called before add menu item
    void                        setMenuParams( const QPoint & globalPoint, int menuId = 0 );

protected:
};

