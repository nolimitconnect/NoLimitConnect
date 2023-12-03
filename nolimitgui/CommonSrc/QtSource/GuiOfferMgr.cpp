//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or linux Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "GuiOfferMgr.h"

#include "AppCommon.h"
#include "GuiOfferCallback.h"

//============================================================================
void GuiOfferMgr::onAppCommonCreated( void )
{
   // m_MyApp.getEngine().getOfferMgr().wantGuiOfferCallbacks( this, true );
}

//============================================================================
bool GuiOfferMgr::isMessengerReady( void )
{
    return m_MyApp.isMessengerReady();
}

//========================================================================
void GuiOfferMgr::wantGuiOfferCallbacks( GuiOfferCallback* clientInterface, bool wantCallbacks )
{
	for( auto iter = m_OfferCallbackList.begin(); iter != m_OfferCallbackList.end(); ++iter )
	{
		GuiOfferCallback* offerInterface = (*iter);
		if( offerInterface == clientInterface )
		{
			if( wantCallbacks )
			{
				// already in list
				return;
			}
			else
			{
				// remove from list
				m_OfferCallbackList.erase( iter );
				return;
			}
		}
	}

	if( wantCallbacks )
	{
		m_OfferCallbackList.push_back( clientInterface );
	}
}
