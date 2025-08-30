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

#include "AppDefs.h"
#include "VxWidgetBase.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class LaunchWidgetClass;
}
QT_END_NAMESPACE

class AppCommon;
class AppletLaunchPage;
class VxPushButton;

class AppletLaunchWidget : public VxWidgetBase
{
	Q_OBJECT;
public:
	AppletLaunchWidget( AppCommon& myApp, EApplet appletType,  AppletLaunchPage * parent = 0 );
	virtual ~AppletLaunchWidget() override = default;

	VxPushButton*				getButton( void );

public slots:
	void						slotAppletIconPressed( void );

public:
	//=== vars ===//
	Ui::LaunchWidgetClass&		ui;
	AppCommon&					m_MyApp;
	EApplet						m_AppletType;
	AppletLaunchPage*			m_AppletLaunchPage;
};

