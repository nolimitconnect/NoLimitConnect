//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletOfferResponse.h"

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

#include "ui_AppletOfferResponse.h"

//============================================================================
AppletOfferResponse::AppletOfferResponse( AppCommon& app, QWidget* parent, QString launchParam )
    : AppletBase( OBJNAME_APPLET_OFFER_RESPONSE, app, parent )
    , ui(*(new Ui::AppletOfferResponseUi))
{
    setAppletType( eAppletOfferResponse );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( getAppletType() ) );

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletOfferResponse::~AppletOfferResponse()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletOfferResponse::statusMsg( QString strMsg )
{
    //LogMsg( LOG_INFO, strMsg.toStdString().c_str() );
    ui.m_StatusMsgLabel->setText( strMsg );
}

//============================================================================
void AppletOfferResponse::slotHomeButtonClicked( void )
{
    closeApplet();
}

//============================================================================
bool AppletOfferResponse::verifyFile( void )
{
    return true;
}

//============================================================================
void AppletOfferResponse::setUser( GuiUser* guiUser )
{
    if( guiUser )
    {
        AppletBase::setUser( guiUser );
        m_HisIdent = guiUser;
        ui.m_IdentWidget->updateIdentity( guiUser );
        ui.m_OfferBarWidget->setUser( guiUser );
    }
}

//============================================================================
void AppletOfferResponse::setPluginType( EPluginType pluginType )
{
    m_PluginType = pluginType;
    ui.m_OfferBarWidget->setPluginType( pluginType );
}

//============================================================================
void AppletOfferResponse::setOfferSessionId( VxGUID& offerSessionId )
{
    m_OfferId = offerSessionId;
    ui.m_OfferBarWidget->setOfferId( m_OfferId );
}

//============================================================================
bool AppletOfferResponse::setOfferInfo( OfferBaseInfo& offerInfo )
{
    m_OfferInfo = offerInfo;
    ui.m_OfferBarWidget->setOfferInfo( offerInfo );
    if( m_OfferInfo.isValid() && m_OfferInfo.isFileAsset() )
    {
        std::string fileName = m_OfferInfo.getAssetNameAndPath();
        std::string justFileName;
        std::string justPath;
        if( 0 == VxFileUtil::seperatePathAndFile( fileName, justPath, justFileName ) )
        {
            ui.m_Path->setText( justPath.c_str() );
            ui.m_FileName->setText( justFileName.c_str() );
            statusMsg( justFileName.c_str() );
        }
    }

    if( m_OfferInfo.getExpiresTime() )
    {
        TimeWithZone lclTime( (int64_t)m_OfferInfo.getExpiresTime() );
        ui.m_OfferExpireTime->setText( lclTime.getLocalDateAndTimeWithNumberMonths().c_str() );
    }
   
    return m_OfferInfo.isValid();
}

//============================================================================
void AppletOfferResponse::setOfferMessage( QString msgText )
{
    //ui.m_OfferBarWidget->setOfferMessage( msgText );
}
