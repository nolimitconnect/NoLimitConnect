//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletOfferInfo.h"

#include "AppCommon.h"
#include "AppSettings.h"

#include "AppletDownloads.h"
#include "ActivityBrowseFiles.h"
#include "ActivityMsgBoxYesNo.h"

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

#include "ui_AppletOfferInfo.h"

//============================================================================
AppletOfferInfo::AppletOfferInfo( AppCommon& app, QWidget* parent, QString launchParam )
    : AppletBase( OBJNAME_APPLET_OFFER_INFO, app, parent )
    , ui(*(new Ui::AppletOfferInfoUi))
{
    setAppletType( eAppletOfferInfo );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( getAppletType() ) );

    ui.m_FileIconButton->setFixedSize( eButtonSizeMedium );
    ui.m_MsgGroupBox->setVisible( false );
    ui.m_FileGroupBox->setVisible( false );

    m_MyApp.activityStateChange( this, true );
}

//============================================================================
AppletOfferInfo::~AppletOfferInfo()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletOfferInfo::statusMsg( QString strMsg )
{
    //LogMsg( LOG_INFO, strMsg.toStdString().c_str() );
    ui.m_StatusMsgLabel->setText( strMsg );
}

//============================================================================
bool AppletOfferInfo::verifyFile( void )
{
    return true;
}

//============================================================================
void AppletOfferInfo::setUser( GuiUser* guiUser )
{
    if( guiUser )
    {
        AppletBase::setUser( guiUser );
        m_HisIdent = guiUser;
        ui.m_IdentWidget->updateIdentity( guiUser );
    }
}

//============================================================================
void AppletOfferInfo::setPluginType( EPluginType pluginType )
{
    m_PluginType = pluginType;
}

//============================================================================
void AppletOfferInfo::setOfferSessionId( VxGUID& offerSessionId )
{
    m_OfferSessionId = offerSessionId;
    m_OfferInfo.setOfferId( offerSessionId );
}

//============================================================================
bool AppletOfferInfo::setOfferInfo( OfferBaseInfo& offerInfo )
{
    m_OfferInfo = offerInfo;
    if( !m_OfferInfo.isValid() )
    {
        statusMsg( QObject::tr( "Invalid Offer. Please Decline Offer.") );
        return false;
    }

    m_UserOnline = m_MyApp.getUserMgr().isUserOnline( m_OfferInfo.getFromOnlineId() );

    QString offerStatus = GuiParams::describeOfferStatus( m_OfferInfo, m_UserOnline );
    statusMsg( offerStatus );
    TimeWithZone lclTime( (int64_t)offerInfo.getOfferTimestamp() );
    ui.m_OfferTimeLabel->setText( lclTime.getLocalDateAndTimeWithNumberMonths().c_str() );

    if( m_OfferInfo.isFileAsset() )
    {
        ui.m_FileGroupBox->setVisible( true );
        std::string justFileName = m_OfferInfo.getFileName();     
        ui.m_FileName->setText( justFileName.c_str() );
        ui.m_FileSize->setText( GuiParams::describeFileLength( m_OfferInfo.getAssetLength() ) );
        ui.m_FileIconButton->setIcon( m_MyApp.getMyIcons().getFileIcon( m_OfferInfo.getAssetType() ) );
    }
   
    return m_OfferInfo.isValid();
}

//============================================================================
void AppletOfferInfo::setOfferMessage( QString msgText )
{
    if( !msgText.isEmpty() )
    {
        ui.m_MsgGroupBox->setVisible( true );
        ui.m_MsgTextEdit->setPlainText( msgText );
    }
}
