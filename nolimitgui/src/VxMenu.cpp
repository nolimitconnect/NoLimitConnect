//============================================================================
// Copyright (C) 2017 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxMenu.h"
#include <QLabel>
#include <QWidgetAction>

//============================================================================
VxMenu::VxMenu( QWidget* parent )
: QMenu( parent )
, m_ShowAbove( false )
{
    //QString menuStyle(
    //    "QMenu::item{"
    //    "color: rgb(0, 0, 255);"
    //    "}"
    //);

    //this->setStyleSheet( menuStyle );
}

//============================================================================
QAction * VxMenu::exec( const QPoint &pos, QAction *at )
{
    QLabel* label = new QLabel( tr( "<b>Title</b>" ), this );
    label->setAlignment( Qt::AlignCenter );

    QWidgetAction* a = new QWidgetAction( this );
    a->setDefaultWidget( label );

    m_MenuPos = pos;
    m_OrigSize = geometry().size();
    return QMenu::exec( pos, at );
}

//============================================================================
void VxMenu::setShowAbove( bool showAbove )
{
    m_ShowAbove = showAbove;
}

//============================================================================
void VxMenu::resizeEvent( QResizeEvent* ev )
{
    QSize newSize = ev->size();
    QMenu::resizeEvent( ev );
    if( m_ShowAbove )
    {
        int oldHeight = m_OrigSize.height();
        int newHeight = newSize.height();
        QPoint newPos = m_MenuPos;
        if( oldHeight != newHeight )
        {
            int oldWidth = m_OrigSize.width();
            int newWidth = newSize.width();
            newPos.setY( m_MenuPos.y() - ( newHeight - oldHeight ) );
            newPos.setX( m_MenuPos.x() - ( newWidth - oldWidth ) );
            this->move( newPos );
        }

        m_MenuPos = newPos;
    }


    m_OrigSize = newSize;
}
