//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletPeerSessionFileOffer.h"

#include "GuiOfferSession.h"

#include <CoreLib/ObjectCommonDefs.h>

#include "ui_AppletPeerSessionFileOffer.h"

//============================================================================
AppletPeerSessionFileOffer::AppletPeerSessionFileOffer(	AppCommon& app, QWidget* parent )
: AppletPeerBase( OBJNAME_ACTIVITY_SESSION_FILE_OFFER, app, parent )
, ui(*(new Ui::AppletPeerSessionFileOfferUi))
, m_ePluginType(ePluginTypePersonFileXfer)
{
    setPluginType( ePluginTypePersonFileXfer );
    setAppletType( eAppletPeerSessionFileOffer );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

	connect( ui.AcceptButton, SIGNAL(clicked()), this, SLOT(onAcceptButClick()) );
	connect( ui.CancelButton, SIGNAL(clicked()), this, SLOT(onCancelButClick()) );
}

//============================================================================
//! Implement the OnClickListener callback    
void AppletPeerSessionFileOffer::onAcceptButClick()
{
    closeApplet();
}

//============================================================================
//! Implement the OnClickListener callback    
void AppletPeerSessionFileOffer::onCancelButClick()
{
    closeApplet();
}
