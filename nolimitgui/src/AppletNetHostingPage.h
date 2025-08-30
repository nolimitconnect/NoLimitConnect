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

class AppletNetHostingPage : public AppletLaunchPage
{
	Q_OBJECT
public:
	AppletNetHostingPage( AppCommon& app, QWidget* parent );
	virtual ~AppletNetHostingPage() = default;

protected:
	virtual void				showEvent( QShowEvent* );
	void						resizeEvent( QResizeEvent* );

private:
	void						setupAppletNetHostingPage( void );

    bool						m_IsInitialized{ false };
	QVector<VxWidgetBase*>		m_AppletList;
};


