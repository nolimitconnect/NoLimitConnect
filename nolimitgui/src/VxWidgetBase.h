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

#include <CoreLib/ObjectCommon.h>
#include <QWidget>

class VxWidgetBase : public QWidget, public ObjectCommon
{
	Q_OBJECT
public:
    VxWidgetBase( QWidget* parent = nullptr, Qt::WindowFlags f = Qt::Widget );
	VxWidgetBase( const char* objName, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::Widget);
	virtual ~VxWidgetBase() = default;

signals:
	void						clicked();

protected:
	virtual void				mousePressEvent( QMouseEvent * event );
};


