//============================================================================
// Copyright (C) 2018 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletCamClient.h"

#include "AppCommon.h"
#include "GuiParams.h"

#include <GuiInterface/IFromGui.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#include "ui_AppletCamClient.h"

//============================================================================
AppletCamClient::AppletCamClient( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_CAM_CLIENT, app, parent )
, ui(*(new Ui::AppletCamClientUi))
, m_CloseAppletTimer( new QTimer( this ) )
{
	setAppletType( eAppletCamClient );
	setPluginType( ePluginTypeCamClient );
    ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );
	connect( this, SIGNAL(signalBackButtonClicked()), this, SLOT( closeApplet()) );

	m_MyApp.activityStateChange( this, true );
	wantActivityCallbacks( true );
}

//============================================================================
AppletCamClient::~AppletCamClient()
{
    stopCamFeed();
    m_MyApp.activityStateChange( this, false );
	wantActivityCallbacks( false );
}

//============================================================================
void AppletCamClient::startCamFeed( void )
{
    if( !isCamFeedStarted() )
    {
        if( m_CamFeedIdent )
        {
            if( ePluginTypeInvalid != m_ePluginType )
            {
                m_FromGui.fromGuiStartPluginSession( m_ePluginType, m_CamFeedIdent->getMyOnlineId() );
            }

            setIsCamFeedStarted( true );
        }
        else
        {
            LogMsg( LOG_ERROR, "AppletCamClient::startCamFeed null feed ident" );
            vx_assert( false );
        }
    }
}

//============================================================================
void AppletCamClient::stopCamFeed( void )
{
    if( isCamFeedStarted() )
    {
        setIsCamFeedStarted( false );
        if( m_CamFeedIdent )
        {
            if( ePluginTypeInvalid != m_ePluginType )
            {
                m_FromGui.fromGuiStopPluginSession( m_ePluginType, m_CamFeedIdent->getMyOnlineId() );
            }
        }
        else
        {
            LogMsg( LOG_ERROR, "AppletCamClient::stopCamFeed null feed ident" );
            vx_assert( false );
        }
    }
}

//============================================================================
void AppletCamClient::showEvent( QShowEvent* ev )
{
    // don't call AppletPeerBase::showEvent ... we don't want plugin offer/response for web cam server or client
    AppletBase::showEvent( ev );
    wantActivityCallbacks( true );
}

//============================================================================
void AppletCamClient::hideEvent( QHideEvent* ev )
{
    // don't call AppletPeerBase::hideEvent ... we don't want plugin offer/response for web cam server or client
    wantActivityCallbacks( false );
    AppletBase::hideEvent( ev );
}

//============================================================================
void AppletCamClient::closeEvent( QCloseEvent* ev )
{
    // don't call AppletPeerBase::hideEvent ... we don't want plugin offer/response for web cam server or client
    stopCamFeed();
    AppletBase::closeEvent( ev );
}

//============================================================================
void AppletCamClient::resizeBitmapToFitScreen( QLabel* VideoScreen, QImage& oPicBitmap )
{
    QSize screenSize( VideoScreen->width(), VideoScreen->height() );
    oPicBitmap = oPicBitmap.scaled( screenSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );
}

//============================================================================
void AppletCamClient::webCamSourceOffline()
{
    if( m_CamFeedIdent )
    {
        std::string statMsg = m_CamFeedIdent->getOnlineName();
        statMsg += " Cam Is Offline";
        LogModule( eLogWebCam, LOG_DEBUG, "AppletCamClient::%s %s", __func__, statMsg.c_str() );
        setStatusText( statMsg.c_str() );
    }

    ui.m_CamVidWidget->showOfflineImage();
}

//============================================================================
void AppletCamClient::slotToGuiRxedOfferReply( GuiOfferSession* offerReply )
{
    if( (ePluginTypeCamServer == offerReply->getPluginType())
        && (m_HisIdent->getMyOnlineId() == offerReply->getUserIdent()->getMyOnlineId()) )
    {
        if( eOfferResponseBusy == offerReply->getOfferResponse() )
        {
            playSound( eSndDefBusy );
        }

        if( eOfferResponseAccept != offerReply->getOfferResponse() )
        {
            webCamSourceOffline();
        }
    }
};

//============================================================================
void AppletCamClient::slotToGuiSessionEnded( GuiOfferSession* offer )
{
    if( (ePluginTypeCamServer == offer->getPluginType())
        && (m_HisIdent->getMyOnlineId() == offer->getUserIdent()->getMyOnlineId()) )
    {
        webCamSourceOffline();
    }
};

//============================================================================
void AppletCamClient::slotToGuiContactOffline( VxNetIdent* friendIdent )
{
    if( m_HisIdent->getMyOnlineId() == friendIdent->getMyOnlineId() )
    {
        webCamSourceOffline();
    }
}

//============================================================================
void AppletCamClient::setupCamFeed( GuiUser* feedNetIdent )
{
    if( !feedNetIdent )
    {
        LogMsg( LOG_ERROR, "setupCamFeed null feed ident" );
        vx_assert( false );
        return;
    }

    m_CamFeedIdent = feedNetIdent;
    m_HisIdent = feedNetIdent;
    m_CamFeedId = feedNetIdent->getMyOnlineId();
    m_IsMyself = m_CamFeedId == m_MyApp.getMyOnlineId();
    if( m_IsMyself )
    {
        setMuteSpeakerVisibility( false );
        setMuteMicrophoneVisibility( true );
        setCameraButtonVisibility( true );
    }
    else
    {
        setMuteSpeakerVisibility( true );
        setMuteMicrophoneVisibility( false );
        setCameraButtonVisibility( false );
    }

    ui.m_CamVidWidget->showAllControls( true );
    ui.m_CamVidWidget->enableCamSourceControls( false );
    ui.m_CamVidWidget->setRecordFilePath( VxGetDownloadsDirectory().c_str() );
    ui.m_CamVidWidget->setRecordFriendName( m_CamFeedIdent->getOnlineName().c_str() );
    ui.m_CamVidWidget->setVideoFeedId( m_CamFeedId, eAppModuleCamClient );

    startCamFeed();
}

//============================================================================
void AppletCamClient::toGuiContactOffline( GuiUser* guiUser )
{
    if( m_CamFeedIdent && m_CamFeedIdent->getMyOnlineId() == guiUser->getMyOnlineId() )
    {
        QString statText = GuiParams::describeOnlineStatus( m_CamFeedIdent->getOnlineName().c_str(), false);
        LogModule( eLogWebCam, LOG_DEBUG, statText.toUtf8().constData() );
        ui.m_StatusMsgLabel->setText( statText );
    }
}
