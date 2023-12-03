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

#include "ActivityBase.h"

class P2PEngine;
class GuiOfferSession;

class ActivitySessionOptions : public ActivityBase
{
	Q_OBJECT

public:

	ActivitySessionOptions(	AppCommon&			app,
							VxNetIdent*			netIdent, 
							QWidget*			parent = nullptr );

	ActivitySessionOptions(	AppCommon&				app,
							GuiOfferSession*		poOffer, 
							QWidget*				parent = nullptr );
	virtual ~ActivitySessionOptions() override = default;

    // overrides required for dialogs with there own title bar and bottom bar widgets
    virtual TitleBarWidget *	getTitleBarWidget( void ) override { return ui.m_TitleBarWidget; }
    virtual BottomBarWidget *	getBottomBarWidget( void ) override { return ui.m_BottomBarWidget; }

	void						playVideoFrame( unsigned char * pu8Jpg, unsigned long u32JpgLen, int motion0To100000 );

protected:
	void						setup();

	//=== vars ===//
};
