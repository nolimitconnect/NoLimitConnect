
//============================================================================
// Copyright (C) 2019 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================
#pragma once

#include <QWidget>

class AppCommon;

class NetClientGuiLogic : public QWidget
{
    Q_OBJECT
public:
    NetClientGuiLogic( AppCommon& myApp, QWidget* parent );

protected:
    AppCommon&                  m_MyApp;
};
