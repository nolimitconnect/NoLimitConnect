#pragma once
//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ActivityBase.h"

class MessengerPage : public ActivityBase
{
	Q_OBJECT
public:
	MessengerPage( AppCommon& app, QWidget* parent );
	virtual ~MessengerPage() override = default;

signals:
	void						signalMessengerPageChanged( bool isVisible, bool isFullScreen );

protected slots :
    virtual void				slotBackButtonClicked( void ) override;

protected:
    virtual void				showEvent( QShowEvent* ) override;
    virtual void				hideEvent( QHideEvent* ) override;

	bool						m_IsVisible;
	bool						m_IsFullSize;
};


