//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================
#pragma once

#include <GuiInterface/IDefs.h>

#include <QWidget>

class AppCommon;

class NetAvailStatusWidget : public QWidget
{
    Q_OBJECT
public:
    NetAvailStatusWidget( QWidget* parent );
    virtual ~NetAvailStatusWidget() override = default;

    void                        toGuiNetAvailStatus( ENetAvailStatus netStatus );

    virtual int					heightForWidth( int width ) const override;
    virtual QSize				sizeHint() const override;
    virtual QSize				getMinimumSizeHint( void ) { return sizeHint(); }
    virtual void				setFixedSize( const QSize & fixedSize );
    virtual void				setFixedSize( int width, int height );
    virtual void				setFixedWidth( int width );
    virtual void				setFixedHeight( int height );
    virtual void				setFixedSizeAbsolute( const QSize & fixedSize );
    virtual void				setFixedSizeAbsolute( int width, int height );
    virtual void				setMinimumSize( const QSize & minSize );
    virtual void				setMinimumSize( int minw, int minh );
    virtual void				setMaximumSize( const QSize & maxSize );
    virtual void				setMaximumSize( int maxw, int maxh );

signals:
    void                        clicked( void );

protected:
    virtual void                paintEvent( QPaintEvent* ev ) override;
    virtual void                resizeEvent( QResizeEvent* ev ) override;
    virtual void				mousePressEvent( QMouseEvent * ev ) override;

    void                        drawNetBars( QPainter& painter );
    void                        drawNetBar( QPainter& painter, QRect& widgetRect, int barNum );
    QColor                      determineBarColor( int barNum );

    //=== vars ===//
    AppCommon&                  m_MyApp;
    ENetAvailStatus             m_NetAvailStatus{ eNetAvailNoInternet };

    QColor                      m_NotAvailColor;
    QColor                      m_InProgressColor;
    QColor                      m_NoRelayColor;
    QColor                      m_WithRelayColor;
    QColor                      m_DirectConnectColor;
};
