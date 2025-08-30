#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "IdentWidget.h"

class AppCommon;

class IdentOfferWidget : public IdentWidget
{
	Q_OBJECT

public:
	IdentOfferWidget( QWidget* parent = nullptr );

	void onIdentMenuButtonClicked( void ) override;

protected:
	AppCommon&					m_MyApp;
};
