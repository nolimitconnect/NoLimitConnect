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

#include <QWidget>

class AppCommon;

class VxAppDisplay : public QWidget
{
    Q_OBJECT

public:
    VxAppDisplay( AppCommon& appCommon, QWidget* parent = nullptr );
    virtual ~VxAppDisplay() {};

    bool						isVerticalOrientation( void )                   { return ( m_Orientation == Qt::Orientation::Vertical ); }

    void                        setOrientation( Qt::Orientation orientation );
    Qt::Orientation             getCurrentOrientation( void );
    Qt::Orientation             forceOrientationUpdate( void );

    void                        initializeAppDisplay( void );
signals:
    void						signalDeviceOrientationChanged( int qtOrientation );

protected slots:
    void						slotCheckOrientationTimer( void );

protected:
    AppCommon &					m_MyApp;
    bool                        m_Initialized{ false };
    QTimer *                    m_OrientationCheckTimer{ nullptr };
    Qt::Orientation				m_Orientation{ Qt::Orientation::Horizontal };
    Qt::Orientation				m_LastOrientation{ Qt::Orientation::Horizontal };
};


