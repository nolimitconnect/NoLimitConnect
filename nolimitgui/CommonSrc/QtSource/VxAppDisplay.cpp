//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxAppDisplay.h"

#include "AppCommon.h"
#include "GuiParams.h"

#include <QApplication>
#include <QScreen>
#include <QTimer>

//============================================================================
VxAppDisplay::VxAppDisplay( AppCommon& appCommon, QWidget* parent )
: QWidget( parent )
, m_MyApp( appCommon )
, m_OrientationCheckTimer( new QTimer( this ) )
{
}

//============================================================================
void VxAppDisplay::initializeAppDisplay( void )
{
    if( !m_Initialized )
    {
        m_Initialized = true;
        QScreen * dispScreen = m_MyApp.getQApplication().primaryScreen();
        if( dispScreen )
        {
            if( dispScreen->isPortrait( dispScreen->orientation() ) )
            {
                m_Orientation = Qt::Vertical;
            }
            else
            {
                m_Orientation = Qt::Horizontal;
            }
        }

        connect( m_OrientationCheckTimer, SIGNAL(timeout()), this, SLOT(slotCheckOrientationTimer()) );
        m_OrientationCheckTimer->setInterval( 2400 );
        m_OrientationCheckTimer->start();
        forceOrientationUpdate();
    }
}

//============================================================================
void VxAppDisplay::slotCheckOrientationTimer( void )
{
    Qt::Orientation curOrientation = getCurrentOrientation();
    if( curOrientation != m_Orientation )
    {
        m_Orientation = curOrientation;
        emit signalDeviceOrientationChanged( (int) m_Orientation );
    }
}

//============================================================================
void VxAppDisplay::setOrientation( Qt::Orientation orientation )
{
    m_Orientation = orientation; 

    emit signalDeviceOrientationChanged( ( int )m_Orientation );
}

//============================================================================
Qt::Orientation VxAppDisplay::getCurrentOrientation( void )
{
    initializeAppDisplay();
    QScreen * dispScreen = m_MyApp.getQApplication().primaryScreen();
    if( dispScreen && dispScreen->isPortrait( dispScreen->orientation() ) )
    {
        return Qt::Vertical;
    }
    else
    {
        return Qt::Horizontal;
    }
}

//============================================================================
Qt::Orientation VxAppDisplay::forceOrientationUpdate( void ) 
{ 
    QScreen *screen = QGuiApplication::primaryScreen();
    if( screen && screen->isPortrait( screen->orientation() ) )
    {
        m_Orientation = Qt::Vertical;
    }
    else
    {
        m_Orientation = Qt::Horizontal;
    }

    emit signalDeviceOrientationChanged( ( int )m_Orientation ); 
    return m_Orientation; 
}




