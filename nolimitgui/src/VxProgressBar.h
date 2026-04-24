//============================================================================
// Copyright (C) 2026 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================
#pragma once

#include <QProgressBar>
#include <QPainter>

class VxProgressBar : public QProgressBar
{
    Q_OBJECT
public:
    VxProgressBar( QWidget* parent = nullptr );

    void setText(QString text)
    {
        m_Text = text;
    }

protected:
    void paintEvent(QPaintEvent* event) override
    {
        QProgressBar::paintEvent( event );

        if( !m_Text.isEmpty() )
        {
            QPainter painter( this );
            painter.setPen( Qt::black );
            painter.drawText( rect(), Qt::AlignCenter, m_Text );
        }
    }

    QString m_Text;
};