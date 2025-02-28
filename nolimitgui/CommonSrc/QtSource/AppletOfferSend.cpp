//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletOfferSend.h"

#include "AppCommon.h"
#include "AppSettings.h"
#include "AppCommon.h"	
#include "AppSettings.h"

#include "AppletDownloads.h"
#include "ActivityBrowseFiles.h"
#include "ActivityMsgBoxYesNo.h"

#include "FileShareItemWidget.h"
#include "MyIconsDefs.h"
#include "AppGlobals.h"
#include "FileItemInfo.h"
#include "FileActionMenu.h"
#include "GuiHelpers.h"
#include "GuiOfferSession.h"
#include "GuiParams.h"

#include <P2PEngine/P2PEngine.h>
#include <AssetMgr/AssetMgr.h>
#include <AssetMgr/AssetInfo.h>

#include <PktLib/VxSearchDefs.h>
#include <NetLib/VxFileXferInfo.h>
#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxFileInfo.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>

#include "ui_AppletOfferSend.h"

//============================================================================
AppletOfferSend::AppletOfferSend( AppCommon& app, QWidget* parent, QString launchParam )
    : AppletBase( OBJNAME_APPLET_OFFER_SEND, app, parent )
    , ui(*(new Ui::AppletOfferSendUi))
{
    setAppletType( eAppletOfferSend );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( getAppletType() ) );

    ui.m_OfferSendWidget->setCanViewOffer( false );
    ui.m_OfferSendWidget->showIdentityWidget( false );

    connect( ui.m_OfferSendWidget, SIGNAL(signalOfferSent(bool)), this, SLOT(slotOfferSent(bool)) );
    connect( ui.m_OfferSendWidget, SIGNAL(signalCancelButtonClicked()), this, SLOT(slotOfferCanceled()) );

    m_MyApp.activityStateChange( this, true );
    ui.m_StatusMsgLabel->setText( "" );
    ui.m_OfferSendWidget->setFocusOnText();
}

//============================================================================
AppletOfferSend::~AppletOfferSend()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletOfferSend::statusMsg( QString strMsg )
{
    ui.m_StatusMsgLabel->setText( strMsg );
}

//============================================================================
void AppletOfferSend::slotHomeButtonClicked( void )
{
    closeApplet();
}

//============================================================================
void AppletOfferSend::slotOfferSent( bool sendSuccess )
{
    if(LogEnabled(eLogOffer))LogModule(eLogOffer, LOG_DEBUG, "AppletOfferSend::%s success ? %d", __func__, sendSuccess );
    closeApplet();
}

//============================================================================
void AppletOfferSend::slotOfferCanceled( void )
{
    if(LogEnabled(eLogOffer))LogModule(eLogOffer, LOG_DEBUG, "AppletOfferSend::%s", __func__ );
    closeApplet();
}

//============================================================================
bool AppletOfferSend::verifyFile( void )
{
    return true;
}

//============================================================================
void AppletOfferSend::setUser( GuiUser* guiUser )
{
    if( guiUser )
    {
        AppletBase::setUser( guiUser );
        m_HisIdent = guiUser;
        ui.m_IdentWidget->updateIdentity( guiUser );
        ui.m_OfferSendWidget->setUser( guiUser );       
    }
}

//============================================================================
void AppletOfferSend::setPluginType( EPluginType pluginType )
{
    m_PluginType = pluginType;
    ui.m_OfferSendWidget->setPluginType( pluginType );
}

//============================================================================
void AppletOfferSend::setOfferSessionId( VxGUID& offerSessionId )
{
    m_OfferSessionId = offerSessionId;
    m_OfferInfo.setOfferId( offerSessionId );
    ui.m_OfferSendWidget->setOfferInfo( m_OfferInfo );
}

//============================================================================
bool AppletOfferSend::setOfferInfo( OfferBaseInfo& offerInfo )
{
    m_OfferInfo = offerInfo;
    ui.m_OfferSendWidget->setOfferInfo( offerInfo );

    if( m_OfferInfo.getExpiresTime() )
    {
        //TimeWithZone lclTime( (int64_t)m_OfferInfo.getExpiresTime() );
        //ui.m_OfferExpireTime->setText( lclTime.getLocalDateAndTimeWithNumberMonths().c_str() );
    }
   
    return m_OfferInfo.isValid();
}

//============================================================================
void AppletOfferSend::setOfferMessage( QString msgText )
{
    ui.m_OfferSendWidget->setOfferMessage( msgText );
}

//============================================================================
bool AppletOfferSend::setOffer( EPluginType pluginType, GuiUser* guiUser, std::shared_ptr<GuiOfferSession> existingOffer )
{
    m_ExistingOffer = existingOffer;
    m_OfferInfo.fillOfferSend( pluginType, guiUser->getNetIdent() );
    if( existingOffer && existingOffer->getOfferId().isVxGUIDValid() )
    {
        m_OfferInfo.setOfferId( existingOffer->getOfferId() );
    }

    ui.m_OfferSendWidget->setOfferInfo( m_OfferInfo );
    setUser( guiUser );
    
    return true;
}
