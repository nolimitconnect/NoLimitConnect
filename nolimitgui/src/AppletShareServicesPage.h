#pragma once
//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletLaunchPage.h"

class VxWidgetBase;

class AppletShareServicesPage : public AppletLaunchPage
{
	Q_OBJECT
public:
	AppletShareServicesPage( AppCommon& app, QWidget* parent );
	virtual ~AppletShareServicesPage() = default;

protected slots:
	void						slotPowerButtonClicked( void );

protected:
	virtual void				showEvent( QShowEvent* );
	void						resizeEvent( QResizeEvent* );

private:
	void						setupAppletShareServicesPage( void );

	bool						m_IsInitialized;
	QVector<VxWidgetBase*>		m_AppletList;
};


