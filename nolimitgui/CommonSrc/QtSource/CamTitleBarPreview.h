#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "VxLabel.h"

class CamTitleBarPreview : public VxLabel
{
    Q_OBJECT
public:
    CamTitleBarPreview( QWidget* parent=nullptr, Qt::WindowFlags f= Qt::Widget);
    CamTitleBarPreview( const QString &text, QWidget* parent=nullptr, Qt::WindowFlags f= Qt::Widget);
    virtual ~CamTitleBarPreview() = default;

};
