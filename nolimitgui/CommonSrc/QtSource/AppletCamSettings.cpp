//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletCamSettings.h"

#include "AppCommon.h"	
#include "AppSettings.h"
#include "ActivityMsgBoxOk.h"

#include <GuiInterface/IFromGui.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#include <QTimer>

#include "ui_AppletCamSettings.h"

//============================================================================
AppletCamSettings::AppletCamSettings( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_CAM_SETTINGS, app, parent )
, ui(*(new Ui::AppletCamSettingsUi))
{
    setAppletType( eAppletCamSettings );
    setPluginType( ePluginTypeCamServer );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( m_EAppletType ) );

    if( !m_MyApp.getCamLogic().isCamAvailable() )
    {
        ActivityMsgBoxOk msgBox( m_MyApp, this, QObject::tr( "Camera Capture" ), QObject::tr( "No Camera Source Available." ) );
        msgBox.exec();
        return;
    }

    if( m_HisIdent )
    {
        setupCamFeed( &m_HisIdent->getNetIdent() );
    }
    else
    {
        setupCamFeed( m_MyApp.getAppGlobals().getMyNetIdent() );
    }

    startCamFeed();
    if( m_IsMyself )
    {
        updateInVideoDevices();
        connect( ui.m_InDeviceComboBox, QOverload<int>::of(&QComboBox::activated), this, &AppletCamSettings::inDeviceChanged );
        connect( ui.m_ApplyVideoInDeviceButton, SIGNAL(clicked()), this, SLOT(slotApplyInDeviceChange()) );
    }
    else
    {
        ui.m_InDeviceComboBox->setVisible( false );
        ui.m_ApplyVideoInDeviceButton->setVisible( false );
        ui.m_InDeviceLabel->setVisible( false );
    }

    m_MyApp.activityStateChange( this, true );
    m_MyApp.getUserMgr().wantGuiUserUpdateCallbacks( this, true );
}

//============================================================================
AppletCamSettings::~AppletCamSettings()
{
    m_MyApp.getUserMgr().wantGuiUserUpdateCallbacks( this, false );
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletCamSettings::setupCamFeed( VxNetIdent* feedNetIdent )
{
    if( !feedNetIdent )
    {
        LogMsg( LOG_ERROR, "setupCamFeed null feed ident" );
        vx_assert( false );
        return;
    }

    m_CamFeedIdent = feedNetIdent;
    m_CamFeedId = feedNetIdent->getMyOnlineId();
    m_IsMyself = m_CamFeedId == m_MyApp.getMyOnlineId();

    ui.m_CamVidWidget->showAllControls( true );
    ui.m_CamVidWidget->enableCamSourceControls( false );
    ui.m_CamVidWidget->setRecordFilePath( VxGetDownloadsDirectory().c_str() );
    ui.m_CamVidWidget->setRecordFriendName( m_CamFeedIdent->getOnlineName() );
    ui.m_CamVidWidget->setVideoFeedId( m_CamFeedId, eMediaModuleCamClient );

    QString bkgFile = m_MyApp.getCamLogic().getCameraBackgroundFile();
    ui.m_CamVidWidget->setImageFromFile( bkgFile );
}

//============================================================================
void AppletCamSettings::startCamFeed( void )
{
    m_MyApp.getCamLogic().toGuiWantVideoCapture( eMediaModuleCamClient, true );
}

//============================================================================
void AppletCamSettings::stopCamFeed( void )
{
    m_MyApp.getCamLogic().toGuiWantVideoCapture( eMediaModuleCamClient, false );
}

//============================================================================
void AppletCamSettings::showEvent( QShowEvent* ev )
{
    // don't call AppletPeerBase::showEvent ... we don't want plugin offer/response for web cam server or client
    AppletBase::showEvent( ev );
    wantActivityCallbacks( true );
}

//============================================================================
void AppletCamSettings::hideEvent( QHideEvent* ev )
{
    // don't call AppletPeerBase::hideEvent ... we don't want plugin offer/response for web cam server or client
    wantActivityCallbacks( false );
    AppletBase::hideEvent( ev );
}

//============================================================================
void AppletCamSettings::closeEvent( QCloseEvent * ev )
{
    // don't call AppletPeerBase::hideEvent ... we don't want plugin offer/response for web cam server or client

    stopCamFeed();
    AppletBase::closeEvent( ev );
}

//============================================================================
void AppletCamSettings::resizeBitmapToFitScreen( QLabel * VideoScreen, QImage& oPicBitmap )
{
    QSize screenSize( VideoScreen->width(), VideoScreen->height() );
    oPicBitmap = oPicBitmap.scaled(screenSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );
}

//============================================================================
void AppletCamSettings::webCamSourceOffline()
{
    if( m_CamFeedIdent )
    {
        std::string statMsg = m_CamFeedIdent->getOnlineName();
        statMsg += "Cam Is Offline";
    }

    ui.m_CamVidWidget->showOfflineImage();
}

//============================================================================
void AppletCamSettings::slotToGuiRxedOfferReply( std::shared_ptr<GuiOfferSession> offerReply )
{
    if( ( ePluginTypeCamServer == offerReply->getPluginType() )
        && ( m_HisIdent->getMyOnlineId() == offerReply->getUserIdent()->getMyOnlineId() ) )
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
void AppletCamSettings::slotToGuiSessionEnded( std::shared_ptr<GuiOfferSession> offer )
{
    if( ( ePluginTypeCamServer == offer->getPluginType() )
        && ( m_HisIdent->getMyOnlineId() == offer->getUserIdent()->getMyOnlineId() ) )
    {
        webCamSourceOffline();
    }
}; 

//============================================================================
void AppletCamSettings::callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline )
{
    if( m_HisIdent->getMyOnlineId() == guiUser->getMyOnlineId() )
    {
        webCamSourceOffline();
    }
}

//============================================================================
void AppletCamSettings::inDeviceChanged( int index )
{
    QString vidInDevDescription = ui.m_InDeviceComboBox->currentText();
    if( !m_MyApp.getCamLogic().startCamCapture( vidInDevDescription.toUtf8().constData() ) )
    {
        ActivityMsgBoxOk msgBox( m_MyApp, this, QObject::tr( "Video In Device" ), vidInDevDescription + QObject::tr( " failed to initialize" ) );
        msgBox.exec();
    }
}

//============================================================================
void AppletCamSettings::updateInVideoDevices( void )
{
    ui.m_InDeviceComboBox->clear();

    std::vector<QString> camList;
    m_MyApp.getCamLogic().getAvailableCameras( camList );

    QString defaultCamId = m_MyApp.getAppSettings().getCamSourceId().c_str();

    int defaultIndex = -1;
    int devIndex = 0;
    for( auto& deviceDesc : camList )
    {
        if( defaultCamId == deviceDesc )
        {
            defaultIndex = devIndex;
        }

        ui.m_InDeviceComboBox->addItem( deviceDesc, QVariant::fromValue( devIndex ) );
        devIndex++;
    }

    if( defaultIndex >= 0 )
    {
        ui.m_InDeviceComboBox->setCurrentIndex( defaultIndex );
    }
}

//============================================================================
void AppletCamSettings::slotApplyInDeviceChange( void )
{
    QString vidInDevDescription = ui.m_InDeviceComboBox->currentText();
    if( !vidInDevDescription.isEmpty() )
    {
        if( m_MyApp.getCamLogic().startCamCapture( vidInDevDescription.toUtf8().constData() ) )
        {
            m_MyApp.getAppSettings().setCamSourceId( vidInDevDescription.toUtf8().constData() );
            ActivityMsgBoxOk msgBox( m_MyApp, this, QObject::tr( "Video In Device" ), vidInDevDescription + QObject::tr( " device is saved as preferred Video In Device" ) );
            msgBox.exec();
        }
        else
        {
            ActivityMsgBoxOk msgBox( m_MyApp, this, QObject::tr( "Video In Device" ), vidInDevDescription + QObject::tr( " failed to initialize" ) );
            msgBox.exec();
        }
    }
    else
    {
       ActivityMsgBoxOk msgBox( m_MyApp, this, QObject::tr( "Video In Device" ), QObject::tr( "No Video In Device Is Available" ) );
       msgBox.exec();
    }
}
