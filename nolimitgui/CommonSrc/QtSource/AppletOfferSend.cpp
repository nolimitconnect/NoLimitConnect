//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletOfferSend.h"
#include "AppCommon.h"
#include "AppSettings.h"
#include "MyIcons.h"
#include "AppCommon.h"	
#include "AppSettings.h"

#include "AppletDownloads.h"
#include "ActivityBrowseFiles.h"
#include "ActivityYesNoMsgBox.h"

#include "FileShareItemWidget.h"
#include "MyIcons.h"
#include "AppGlobals.h"
#include "FileItemInfo.h"
#include "FileActionMenu.h"
#include "GuiHelpers.h"
#include "GuiOfferSession.h"
#include "GuiParams.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>
#include <ptop_src/ptop_engine_src/AssetMgr/AssetMgr.h>
#include <ptop_src/ptop_engine_src/AssetMgr/AssetInfo.h>

#include <PktLib/VxSearchDefs.h>
#include <NetLib/VxFileXferInfo.h>
#include <CoreLib/VxFileInfo.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>

//============================================================================
AppletOfferSend::AppletOfferSend( AppCommon& app, QWidget* parent, QString launchParam )
    : AppletBase( OBJNAME_APPLET_OFFER_SEND, app, parent )
{
    setAppletType( eAppletOfferSend );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( getAppletType() ) );

    ui.m_OfferSendWidget->setCanViewOffer( false );
    ui.m_OfferSendWidget->showIdentityWidget( false );

    connect( ui.m_OfferSendWidget, SIGNAL( signalOfferSent() ), this, SLOT( slotOfferSent() ) );

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletOfferSend::~AppletOfferSend()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletOfferSend::statusMsg( QString strMsg )
{
    //LogMsg( LOG_INFO, strMsg.toStdString().c_str() );
    ui.m_StatusMsgLabel->setText( strMsg );
}

//============================================================================
void AppletOfferSend::slotHomeButtonClicked( void )
{
    closeApplet();
}

//============================================================================
void AppletOfferSend::slotOfferSent( void )
{
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
bool AppletOfferSend::setOffer( EPluginType pluginType, GuiUser* guiUser, GuiOfferSession* existingOffer )
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