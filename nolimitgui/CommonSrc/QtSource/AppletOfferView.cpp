//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletOfferView.h"

#include "AppCommon.h"
#include "AppSettings.h"
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

#include "ui_AppletOfferView.h"

//============================================================================
AppletOfferView::AppletOfferView( AppCommon& app, QWidget* parent, QString launchParam )
    : AppletBase( OBJNAME_APPLET_OFFER_VIEW, app, parent )
    , ui(*(new Ui::AppletOfferViewUi))
{
    setAppletType( eAppletOfferView );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( getAppletType() ) );

    ui.m_OfferSendWidget->setCanViewOffer( false );

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletOfferView::~AppletOfferView()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletOfferView::statusMsg( QString strMsg )
{
    //LogMsg( LOG_INFO, strMsg.toStdString().c_str() );
    ui.m_StatusMsgLabel->setText( strMsg );
}

//============================================================================
void AppletOfferView::slotHomeButtonClicked( void )
{
    closeApplet();
}

//============================================================================
bool AppletOfferView::verifyFile( void )
{
    return true;
}

//============================================================================
void AppletOfferView::setUser( GuiUser* guiUser )
{
    if( guiUser )
    {
        AppletBase::setUser( guiUser );
        m_HisIdent = guiUser;
        ui.m_IdentWidget->updateIdentity( guiUser );
        ui.m_OfferSendWidget->setUser( guiUser );
        ui.m_OfferSendWidget->showIdentityWidget( false );
        ui.m_OfferBarWidget->setUser( guiUser );
    }
}

//============================================================================
void AppletOfferView::setPluginType( EPluginType pluginType )
{
    m_PluginType = pluginType;
    ui.m_OfferSendWidget->setPluginType( pluginType );
    ui.m_OfferBarWidget->setPluginType( pluginType );
}

//============================================================================
void AppletOfferView::setOfferSessionId( VxGUID& offerSessionId )
{
    m_OfferSessionId = offerSessionId;
    m_OfferInfo.setOfferId( offerSessionId );
    ui.m_OfferSendWidget->setOfferInfo( m_OfferInfo );
    ui.m_OfferBarWidget->setOfferInfo( m_OfferInfo );
}

//============================================================================
bool AppletOfferView::setOfferInfo( OfferBaseInfo& offerInfo )
{
    m_OfferInfo = offerInfo;
    ui.m_OfferSendWidget->setOfferInfo( offerInfo );
    ui.m_OfferBarWidget->setOfferInfo( offerInfo );
    if( m_OfferInfo.isValid() && m_OfferInfo.isFileAsset() )
    {
        std::string fileName = m_OfferInfo.getAssetName();
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
void AppletOfferView::setOfferMessage( QString msgText )
{
    ui.m_OfferSendWidget->setOfferMessage( msgText );
}
