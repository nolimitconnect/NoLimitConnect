//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletOfferRandSession.h"

#include "AppCommon.h"
#include "AppletInformation.h"
#include "AppSettings.h"

#include "ActivityMessageBox.h"

#include "MyIcons.h"

#include "GuiOfferSession.h"
#include "GuiHelpers.h"
#include "GuiParams.h"
#include "GuiRandConnectMgr.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxFileInfo.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxTime.h>

#include "ui_AppletOfferRandSession.h"

#include <QButtonGroup>

namespace
{
EOfferType PluginTypeToOfferType( EPluginType pluginType )
{
    switch( pluginType )
    {
    case ePluginTypeTruthOrDare:
        return eOfferTypeTruthOrDare;

    case ePluginTypeVoicePhone:
        return eOfferTypeVoicePhone;

    case ePluginTypeVideoChat:
    default:
        return eOfferTypeVideoChat;
    }
}
}

//============================================================================
AppletOfferRandSession::AppletOfferRandSession( AppCommon& app, QWidget* parent, QString launchParam )
    : AppletBase( OBJNAME_APPLET_OFFER_RAND_SESSION, app, parent )
    , ui(*(new Ui::AppletOfferRandSessionUi))
{
    setAppletType( eAppletOfferRandSession );
    ui.setupUi( getContentItemsFrame() );
    setTitleBarText( DescribeApplet( getAppletType() ) );

    ui.m_SelectTruthOrDareButton->setIcon( eMyIconOfferTruthOrDare );
    ui.m_SelectTruthOrDareButton->setFixedSize( eButtonSizeSmall );
    ui.m_SelectVideoChatButton->setIcon( eMyIconOfferVideoChat );
    ui.m_SelectVideoChatButton->setFixedSize( eButtonSizeSmall );
    ui.m_SelectVoicePhoneButton->setIcon( eMyIconOfferVoicePhone );
    ui.m_SelectVoicePhoneButton->setFixedSize( eButtonSizeSmall );

    ui.m_SelectedPluginButton->setFixedSize( eButtonSizeMedium );

    ui.m_OfferTruthOrDareFrame->setVisible( false );
    ui.m_OfferVideoFrame->setVisible( false );
    ui.m_OfferVoicePhoneFrame->setVisible( false );

    ui.m_OfferSendWidget->setCanViewOffer( false );
    ui.m_OfferSendWidget->showIdentityWidget( false );
    ui.m_OfferSendWidget->setExternalSendMode( true );

    connect( ui.m_OfferSendWidget, SIGNAL(signalOfferSent(bool)), this, SLOT(slotOfferSent(bool)) );
    connect( ui.m_OfferSendWidget, SIGNAL(signalCancelButtonClicked()), this, SLOT(slotOfferCanceled()) );
    connect( ui.m_OfferSendWidget, SIGNAL(signalExternalSendRequested()), this, SLOT(slotSendRandomConnectOffer()) );

	connect( ui.m_ToTruthOrDareButton,	    SIGNAL(clicked()), this, SLOT(slotPluginRadioClick()) );
	connect( ui.m_ToVideoChatButton,		SIGNAL(clicked()), this, SLOT(slotPluginRadioClick()) );
	connect( ui.m_ToVoicePhoneButton,	    SIGNAL(clicked()), this, SLOT(slotPluginRadioClick()) );

    connect( ui.m_SelectTruthOrDareButton,	SIGNAL(clicked()), this, SLOT(slotSelectTruthOrDareClick()) );
	connect( ui.m_SelectVideoChatButton,	SIGNAL(clicked()), this, SLOT(slotSelectVideoChatClick()) );
	connect( ui.m_SelectVoicePhoneButton,	SIGNAL(clicked()), this, SLOT(slotSelectVoicePhoneClick()) );

    connect( ui.m_SelectedPluginButton,	    SIGNAL(clicked()), this, SLOT(slotSelectedPluginClick()) );

    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(ui.m_ToVoicePhoneButton);
    group->addButton(ui.m_ToVideoChatButton);
    group->addButton(ui.m_ToTruthOrDareButton);
    group->setExclusive(true); 

    m_MyApp.activityStateChange( this, true );
    ui.m_StatusMsgLabel->setText( "" );
    ui.m_OfferSendWidget->setFocusOnText();
}

//============================================================================
AppletOfferRandSession::~AppletOfferRandSession()
{
    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletOfferRandSession::statusMsg( QString strMsg )
{
    ui.m_StatusMsgLabel->setText( strMsg );
}

//============================================================================
void AppletOfferRandSession::slotOfferSent( bool sendSuccess )
{
    if(LogEnabled(eLogOffer))LogModule(eLogOffer, LOG_DEBUG, "AppletOfferRandSession::%s success ? %d", __func__, sendSuccess );
    closeApplet();
}

//============================================================================
void AppletOfferRandSession::slotOfferCanceled( void )
{
    if(LogEnabled(eLogOffer))LogModule(eLogOffer, LOG_DEBUG, "AppletOfferRandSession::%s", __func__ );
    closeApplet();
}

//============================================================================
bool AppletOfferRandSession::verifyFile( void )
{
    return true;
}

//============================================================================
void AppletOfferRandSession::setUser( GuiUser* guiUser )
{
    if( !guiUser )
    {
        LogMsg( LOG_ERROR, "AppletOfferRandSession::%s null guiUser", __func__ );
        vx_assert( false );
        return;
    }

    AppletBase::setUser( guiUser );
    m_HisIdent = guiUser;
    ui.m_IdentWidget->updateIdentity( guiUser );
    ui.m_OfferSendWidget->setUser( guiUser );       

    std::vector<EOfferType> allowedOfferTypes;
    if( m_MyApp.getUserMgr().getOfferAccessPersonToPerson( guiUser, ePluginTypeVideoChat ) == ePluginAccessOk )
	{
		allowedOfferTypes.emplace_back( eOfferTypeVideoChat );
        ui.m_OfferVideoFrame->setVisible( true );
	}

	if( m_MyApp.getUserMgr().getOfferAccessPersonToPerson( guiUser, ePluginTypeVoicePhone ) == ePluginAccessOk )
	{
		allowedOfferTypes.emplace_back( eOfferTypeVoicePhone );
        ui.m_OfferVoicePhoneFrame->setVisible( true );
	}

	if( m_MyApp.getUserMgr().getOfferAccessPersonToPerson( guiUser, ePluginTypeTruthOrDare ) == ePluginAccessOk )
	{
		allowedOfferTypes.emplace_back( eOfferTypeTruthOrDare );
        ui.m_OfferTruthOrDareFrame->setVisible( true );
	}

    if( allowedOfferTypes.empty() )
	{
		ActivityMessageBox errMsgBox( GetAppInstance(),
			getParentPageFrame(),
			LOG_INFO,
			"%s",
			QObject::tr( "No permitted session types are available for this Random Connect offer." ).toUtf8().constData() );
		errMsgBox.exec();
		return;
	}

    EOfferType defaultOfferType = m_MyApp.getAppSettings().getRandomConnectOfferType();
    if( std::find( allowedOfferTypes.begin(), allowedOfferTypes.end(), defaultOfferType ) == allowedOfferTypes.end() )
    {
        defaultOfferType = allowedOfferTypes[0];
    }

    updateOfferType( defaultOfferType );
}

//============================================================================
void AppletOfferRandSession::setPluginType( EPluginType pluginType )
{
    m_PluginType = pluginType;
    ui.m_OfferSendWidget->setPluginType( pluginType );
}

//============================================================================
void AppletOfferRandSession::setOfferSessionId( VxGUID& offerSessionId )
{
    m_OfferSessionId = offerSessionId;
    m_OfferInfo.setOfferId( offerSessionId );
    ui.m_OfferSendWidget->setOfferInfo( m_OfferInfo );
}

//============================================================================
bool AppletOfferRandSession::setOfferInfo( OfferBaseInfo& offerInfo )
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
void AppletOfferRandSession::setOfferMessage( QString msgText )
{
    ui.m_OfferSendWidget->setOfferMessage( msgText );
}

//============================================================================
bool AppletOfferRandSession::setOffer( EPluginType pluginType, GuiUser* guiUser, std::shared_ptr<GuiOfferSession> existingOffer )
{
    m_ExistingOffer = existingOffer;
    m_OfferInfo.fillOfferSend( pluginType, guiUser->getNetIdent() );
    if( existingOffer && existingOffer->getOfferId().isValid() )
    {
        m_OfferInfo.setOfferId( existingOffer->getOfferId() );
    }

    ui.m_OfferSendWidget->setOfferInfo( m_OfferInfo );
    setUser( guiUser );
    
    return true;
}

//============================================================================
void AppletOfferRandSession::slotPluginRadioClick( void )
{
    if( ui.m_ToTruthOrDareButton->isChecked() )
    {        
        updateOfferType( eOfferTypeTruthOrDare );
    }
    else if( ui.m_ToVideoChatButton->isChecked() )
    {        
        updateOfferType( eOfferTypeVideoChat );
    }
    else if( ui.m_ToVoicePhoneButton->isChecked() )
    {        
        updateOfferType( eOfferTypeVoicePhone );
    }
}

//============================================================================
void AppletOfferRandSession::slotSelectTruthOrDareClick()
{
    updateOfferType( eOfferTypeTruthOrDare );
}

//============================================================================
void AppletOfferRandSession::slotSelectVideoChatClick()
{
    updateOfferType( eOfferTypeVideoChat );
}

//============================================================================
void AppletOfferRandSession::slotSelectVoicePhoneClick()
{
    updateOfferType( eOfferTypeVoicePhone );
}

//============================================================================
void AppletOfferRandSession::updateOfferType( EOfferType offerType )
{
    m_MyApp.getAppSettings().setRandomConnectOfferType( offerType );
    if( offerType == eOfferTypeVoicePhone )
    {
        updatePluginType( ePluginTypeVoicePhone );
    }
    else if( offerType == eOfferTypeVideoChat )
    {
        updatePluginType( ePluginTypeVideoChat );
    }
    else if( offerType == eOfferTypeTruthOrDare )
    {
        updatePluginType( ePluginTypeTruthOrDare );
    }
    else
    {
        updatePluginType( ePluginTypeVideoChat );
    }
}

//============================================================================
void AppletOfferRandSession::updatePluginType( EPluginType pluginType )
{
    setPluginType( pluginType );
    if( pluginType == ePluginTypeVoicePhone )
    {
        ui.m_ToVoicePhoneButton->setChecked( true );
    }
    else if( pluginType == ePluginTypeVideoChat )
    {
        ui.m_ToVideoChatButton->setChecked( true );
    }
    else if( pluginType == ePluginTypeTruthOrDare )
    {
        ui.m_ToTruthOrDareButton->setChecked( true );
    }

    ui.m_OfferSendWidget->setPluginType( pluginType );

    EMyIcons pluginIcon = m_MyApp.getMyIcons().getPluginIcon( m_PluginType );
    ui.m_SelectedPluginButton->setIcon( pluginIcon );
    ui.m_SelectedPluginLabel->setText( GuiParams::describePluginType( m_PluginType ) );
}

//============================================================================
void AppletOfferRandSession::slotSelectedPluginClick( void )
{
    AppletInformation * activityInfo = new AppletInformation( m_MyApp, this );
    activityInfo->setPluginType( getPluginType() );
    activityInfo->show();
}

//============================================================================
void AppletOfferRandSession::slotSendRandomConnectOffer( void )
{
    if( !m_HisIdent || !m_HisIdent->isReachable() )
    {
        GuiHelpers::errorMsgBox( eErrMsgUserUnavailable, this, m_HisIdent );
        return;
    }

    EOfferType offerType = PluginTypeToOfferType( m_PluginType );
    if( !m_MyApp.getRandConnectMgr().sendRandConnectOfferRequest( m_HisIdent->getMyOnlineId(), offerType ) )
    {
        GuiHelpers::errorMsgBox( eErrMsgOfferSendFailed, this, m_HisIdent );
        return;
    }

    closeApplet();
}
