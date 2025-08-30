#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QComboBox>

#include <CoreLib/VxFileInfo.h>

class VxComboBox : public QComboBox
{
	Q_OBJECT
public:
	VxComboBox(QWidget* parent=nullptr);
	virtual ~VxComboBox() = default;

    //void                        paintEvent (QPaintEvent *ev);
    void                        showEvent( QShowEvent*ev ) override;
};

