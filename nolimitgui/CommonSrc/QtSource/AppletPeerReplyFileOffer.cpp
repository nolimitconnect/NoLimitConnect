//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletPeerReplyFileOffer.h"

#include "GuiOfferSession.h"

//============================================================================
AppletPeerReplyFileOffer::AppletPeerReplyFileOffer(	AppCommon& app, 
												    QWidget* parent )
: AppletPeerBase( OBJNAME_ACTIVITY_REPLY_FILE_OFFER, app, parent )
{
    setPluginType( ePluginTypePersonFileXfer );
    setAppletType( eAppletPeerReplyOfferFile );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );
   
    //setGuiOfferSession( poOffer );
    
    connectBarWidgets();
    /*
	setupStyledDlg( poOffer->getHisIdent(), 
		            ui.FriendIdentWidget,
		            m_ePluginType,
		            ui.PermissionButton, 
		            ui.PermissionLabel );
                    */

	QString strRxFile = QObject::tr("Receive File");
	ui.m_PermissionLabel->setText( strRxFile );

    /*
	ui.m_FileNameEdit->setText( poOffer->getFileName().c_str() );
	ui.m_OfferMsgEdit->setPlainText( poOffer->getOfferMsg().c_str() );

	connect( ui.m_AcceptButton, SIGNAL(clicked()), this, SLOT(onReceiveFileButClick()) );
	connect( ui.m_CancelButton, SIGNAL(clicked()), this, SLOT(onCancelButClick()) );
    */
}

//============================================================================
//! Implement the OnClickListener callback    
void AppletPeerReplyFileOffer::onReceiveFileButClick()
{
	//setOfferResponse( eOfferResponseAccept );
	m_OfferSessionLogic.sendOfferReply( eOfferResponseAccept );
	accept();
}

//============================================================================
//! Implement the OnClickListener callback    
void AppletPeerReplyFileOffer::onCancelButClick()
{
	m_OfferSessionLogic.sendOfferReply( eOfferResponseReject );
	reject();
}

