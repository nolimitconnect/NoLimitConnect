#pragma once

//============================================================================
// Copyright (C) 2024 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppDefs.h"

#include <QObject>

class AppCommon;

class AppModuleState : public QObject
{
    Q_OBJECT

public:
    AppModuleState( void );
    ~AppModuleState( void );

    void                        toGuiSetIsAppModuleRunning( EAppModule appModule, bool isRunning );
    bool                        toGuiGetIsAppModuleRunning( EAppModule appModule );

    bool                        toGuiRunModule( EAppModule appModule );

    bool                        toGuiStopModule( EAppModule appModule );

protected:
    bool                        m_IsRunning[ eMaxAppModule ];
};

