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

#include "ActivityBase.h"

class VxWidgetBase;

class AppletLaunchPage : public ActivityBase
{
	Q_OBJECT
public:
	AppletLaunchPage( AppCommon& app, QWidget* parent, EApplet eAppletType, const char* objName );
	virtual ~AppletLaunchPage() = default;

	void                        startSpinner( void );
    void                        stopSpinner( void );

protected slots:
    void						slotPowerButtonClicked( void ) override;

protected:
	void						showEvent( QShowEvent* ) override;

	void						resizeEvent( QResizeEvent* )  override;

private:
	void						setupAppletLaunchPage( void );

	bool						m_IsInitialized;
	QVector<VxWidgetBase*>		m_AppletList;
	
    WaitingSpinnerWidget*       m_BusySpinner{ nullptr };
};


