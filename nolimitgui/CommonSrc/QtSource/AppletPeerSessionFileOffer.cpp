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

//============================================================================
AppletPeerSessionFileOffer::AppletPeerSessionFileOffer(	AppCommon& app, QWidget* parent )
: AppletPeerBase( OBJNAME_ACTIVITY_SESSION_FILE_OFFER, app, parent )
, m_ePluginType(ePluginTypePersonFileXfer)
{
    setPluginType( ePluginTypePersonFileXfer );
    setAppletType( eAppletPeerSessionFileOffer );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    /*
	ui.setupUi(this);
	ui.m_TitleBarWidget->setTitleBarText( QObject::tr("Offer File"));


    connectBarWidgets();

	setupStyledDlg(	
		poOffer->getHisIdent(), 		
		ui.FriendIdentWidget,
		m_ePluginType,
		ui.PermissionButton, 
		ui.PermissionLabel );
	ui.progressBar->setValue( poOffer->getProgress() );
    */

	connect( ui.AcceptButton, SIGNAL(clicked()), this, SLOT(onAcceptButClick()) );
	connect( ui.CancelButton, SIGNAL(clicked()), this, SLOT(onCancelButClick()) );
}

//============================================================================
//! Implement the OnClickListener callback    
void AppletPeerSessionFileOffer::onAcceptButClick()
{
	accept();
}

//============================================================================
//! Implement the OnClickListener callback    
void AppletPeerSessionFileOffer::onCancelButClick()
{
	//setOfferResponse( eOfferResponseCancelSession );

	reject();
}
