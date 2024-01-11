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

#include "MyIconsDefs.h"

#include <QPushButton>

class AppCommon;
class MyIcons;

class VxFlatButton : public QPushButton
{
	Q_OBJECT
public:
	VxFlatButton(QWidget* parent=nullptr);
	virtual ~VxFlatButton() = default;

	AppCommon&					getMyApp( void )						{ return m_MyApp; }
	virtual void				setMyIcon( EMyIcons myIcon )			{ m_MyIcon = myIcon; }
	MyIcons&					getMyIcons( void );

protected:
	virtual void                paintEvent( QPaintEvent* ev ) override;

	//=== vars ===//
	AppCommon&					m_MyApp;
	EMyIcons					m_MyIcon;
	QPixmap						m_IconImage;
	QColor						m_LastIconColor;
	QSize						m_LastIconSize;
};

