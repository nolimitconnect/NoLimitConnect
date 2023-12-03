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

#include <QMenu>
#include <QResizeEvent>

class VxMenu : public QMenu
{
    Q_OBJECT
public:
    VxMenu( QWidget* parent = nullptr );

    QAction *                   exec( const QPoint &pos, QAction *at = Q_NULLPTR );
    void                        setShowAbove( bool showAbove );

protected:
    virtual void                resizeEvent( QResizeEvent* ev ) override;

    bool                        m_ShowAbove{ false };
    QPoint                      m_MenuPos;
    QSize                       m_OrigSize;
};
