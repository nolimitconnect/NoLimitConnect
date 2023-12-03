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

#include <QVector>

class AppCommon;
class VxWidgetBase;
class QWidget;

class VxTilePositioner 
{
public:
	VxTilePositioner( AppCommon& myApp );
    virtual ~VxTilePositioner() = default;

	static void					repositionTiles( QVector<VxWidgetBase *>& widgetList, QWidget* parentWindow, int padding = 5 );

protected:
	AppCommon&					m_MyApp;
};
