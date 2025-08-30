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

class AppletSearchPage : public AppletLaunchPage
{
	Q_OBJECT
public:
	AppletSearchPage( AppCommon& app, QWidget* parent );
	virtual ~AppletSearchPage() = default;


protected slots:
	void						slotPowerButtonClicked( void );

protected:
	virtual void				showEvent( QShowEvent* );
	//virtual void				hideEvent( QHideEvent* );
	void						resizeEvent( QResizeEvent* );

private:
	void						setupAppletSearchPage( void );

    bool						m_IsInitialized{ false };
	QVector<VxWidgetBase*>		m_AppletList;
};


