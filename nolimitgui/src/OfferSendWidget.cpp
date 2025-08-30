//============================================================================
// Copyright (C) 2022 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "OfferSendWidget.h"

#include "ActivityGenerateHash.h"
#include "ActivityMessageBox.h"
#include "AppletOfferView.h"
#include "AppletMgr.h"
#include "AppCommon.h"
#include "GuiHelpers.h"
#include "GuiOfferMgr.h"
#include "MyIcons.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#include "ui_OfferSendWidget.h"

//============================================================================
OfferSendWidget::OfferSendWidget( QWidget* parent )
: QWidget( parent )
, ui(*(new Ui::OfferSendWidgetUi))
, m_MyApp( GetAppInstance() )
{
	ui.setupUi( this );
	ui.m_OfferSendButton->setFixedSize( eButtonSizeMedium );
    ui.m_OfferSendButton->setIcon( eMyIconSendArrowNormal );
    ui.m_OfferViewButton->setFixedSize( eButtonSizeMedium );
    ui.m_OfferCancelButton->setFixedSize( eButtonSizeMedium );
    ui.m_OfferCancelButton->setIconOverrideColor( m_MyApp.getAppTheme().getCancelColor() );
	ui.m_OfferCancelButton->setIcon( eMyIconRedX );

    ui.m_OfferMsgEdit->setFixedHeight( GuiParams::getButtonSize( eButtonSizeLarge ).height() );

    GuiHelpers::fillExpireTimeComboBox( ui.m_ExpiresComboBox );

	connect( ui.m_OfferSendButton,			SIGNAL(clicked()),	            this, SLOT(slotOfferSendButtonClicked()) );
    connect( ui.m_OfferViewButton,          SIGNAL(clicked()),              this, SLOT(slotOfferViewButtonClicked()) );
    //connect( ui.m_ExpiresComboBox,          SIGNAL(currentIndexChanged(int)), this, SLOT(slotExpiresTimeChange(int)) );
    connect( ui.m_OfferCancelButton,        SIGNAL(clicked()),              this, SLOT(slotOfferCancelButtonClicked()) );

    //ui.m_ExpiresComboBox->setCurrentIndex( (int)eExpireTimeWhenResponseRxed );
    ui.m_ExpiresLabel->setVisible( false );
    ui.m_ExpiresComboBox->setVisible( false );
}

//============================================================================
void OfferSendWidget::slotOfferSendButtonClicked( void )
{
    if( !m_HisIdent || !m_HisIdent->isOnline() )
    {
        emit signalOfferSent( false );
        GuiHelpers::errorMsgBox( eErrMsgUserUnavailable, this, m_HisIdent );
        return;
    }

    if( validateOffer( true ) )
    {
        if( false == m_MyApp.getOfferMgr().fromGuiMakePluginOffer( GuiHelpers::getParentPageFrame( this ), m_OfferInfo.getPluginType(), m_HisIdent, m_OfferInfo))
        {
            emit signalOfferSent( false );
            ActivityMessageBox errMsgBox( m_MyApp, this, LOG_INFO, "%s is offline", m_HisIdent->getOnlineName().c_str() );
            errMsgBox.exec();
            return;
        }
        else
        {
            if( !m_OfferInfo.isPhoneTypePlugin() )
            {
                GuiHelpers::errorMsgBox( eErrMsgOfferSent, this, m_HisIdent );
            }
            
            emit signalOfferSent( true );
        }
    }
    else
    {
        emit signalOfferSent( false );
    }
}

//============================================================================
bool OfferSendWidget::setUser( GuiUser* guiUser )
{
    if( guiUser )
    {
        m_HisIdent = guiUser;
        ui.m_IdentWidget->updateIdentity( guiUser );
        updateOfferInfo();
        return true;
    }

    return false;
}

//============================================================================
void OfferSendWidget::showIdentityWidget( bool showIdent )
{
    ui.m_IdentWidget->setVisible( showIdent );
}

//============================================================================
void OfferSendWidget::setPluginType( EPluginType pluginType )
{
    m_PluginType = pluginType;
    m_OfferInfo.setPluginType( pluginType );
    updateOfferInfo();
}

//============================================================================
bool OfferSendWidget::setOfferInfo( OfferBaseInfo& offerInfo, GuiUser* guiUser )
{
    m_OfferInfo = offerInfo;
    if( ePluginTypeInvalid != offerInfo.getPluginType() )
    {
        m_PluginType = offerInfo.getPluginType();
    }

    if( guiUser )
    {
        setUser( guiUser );
    }

    m_NeedFileHash = m_OfferInfo.isFileAsset() && !m_OfferInfo.isFileHashValid();
    if( m_NeedFileHash )
    {
        ActivityGenerateHash genHashDlg( m_MyApp, this, m_OfferInfo.getAssetName(), m_OfferInfo.getAssetNameAndPath(), m_OfferInfo.getAssetHashId() );
        if( QDialog::Accepted == genHashDlg.exec() && m_OfferInfo.getAssetHashId().isHashValid() )
        {
            m_MyApp.getEngine().fromGuiFileHashGenerated( m_OfferInfo.getAssetNameAndPath(), m_OfferInfo.getAssetLength(), m_OfferInfo.getAssetHashId() );
        }
    }

    updateOfferInfo();
    return m_OfferInfo.isValid();
}

//============================================================================
void OfferSendWidget::updateOfferInfo( void )
{
    if( ePluginTypeInvalid != m_PluginType && m_OfferInfo.isValid() )
    {
        ui.m_PluginNameLabel->setText( GuiParams::describePluginType( m_PluginType ) );
        ui.m_OfferViewButton->setIcon( m_MyApp.getMyIcons().getOfferIcon( m_OfferInfo, m_PluginType ) );
        if( IsPluginSingleSession( m_PluginType ) )
        {
            ui.m_ExpiresComboBox->setCurrentIndex( (int)eExpireTime20Seconds );
        }
        else
        {
            ui.m_ExpiresComboBox->setCurrentIndex( (int)eExpireTime24Hour );
        }

        if( ePluginTypePersonFileXfer == m_PluginType )
        {
            // so user can view file details
            ui.m_OfferViewButton->setEyeOverlayColor( m_MyApp.getAppTheme().getColor( eLayerNotifyOnlineColor ) );
            ui.m_OfferViewButton->setEyeOverlayEnabled( true );
        }

        ui.m_OfferViewButton->setEyeOverlayEnabled( true );       
    }  
}

//============================================================================
bool OfferSendWidget::validateOffer( bool showErrorMsg )
{
    if( ePluginTypeInvalid == m_PluginType )
    {
        if( showErrorMsg )
        {
            QMessageBox::information( this, QObject::tr( "Plugin Type Not Set" ), QObject::tr( "Plugin Type Not Set" ) );
        }
        
        return false;
    }

    if( !m_HisIdent )
    {
        if( showErrorMsg )
        {
            QMessageBox::information( this, QObject::tr( "User Not Set" ), QObject::tr( "User Not Set" ) );
        }

        return false;
    }

    if( !m_OfferInfo.isValid() )
    {
        if( showErrorMsg )
        {
            QMessageBox::information( this, QObject::tr( "Offer Not Set" ), QObject::tr( "Offer Not Set" ) );
        }

        return false;
    }

    if( m_OfferInfo.isFileAsset() && !m_OfferInfo.isFileHashValid() )
    {
        if( showErrorMsg )
        {
            QMessageBox::information( this, QObject::tr( "File Hash Is Not Valid" ), QObject::tr( "File Hash Is Not Valid" ) );
        }

        return false;
    }

    std::string msg = ui.m_OfferMsgEdit->toPlainText().toUtf8().constData();
    m_OfferInfo.setOfferMsg( msg );
    m_OfferInfo.fillOfferSend( m_PluginType, m_HisIdent->getNetIdent() );
    int64_t expireTime = GuiHelpers::expireTimeComboIdxToSeconds( ui.m_ExpiresComboBox->currentIndex() );
    if( expireTime )
    {
        expireTime += GetHighResolutionTimeMs() / 1000;
        m_OfferInfo.setOfferExpireTime( expireTime );
    }
    else
    {
        m_OfferInfo.setOfferExpireTime( 0 );
    }
    
    return true;
}

//============================================================================
void OfferSendWidget::slotOfferViewButtonClicked( void )
{
    if( validateOffer() && getCanViewOffer() )
    {
        AppletOfferView* applet = (AppletOfferView*)m_MyApp.getAppletMgr().launchApplet( eAppletOfferView, (QWidget *)parent() );
        if( applet )
        {
            applet->setPluginType( m_PluginType );
            applet->setUser( m_HisIdent );
            applet->setOfferInfo( getOfferInfo() );
            applet->setOfferMessage( ui.m_OfferMsgEdit->toPlainText() );
        }
    }
}

//============================================================================
void OfferSendWidget::slotExpiresTimeChange( int comboIdx )
{
    updateExpireStatus( (EExpireTime)comboIdx );
    m_ExpireSeconds = GuiHelpers::expireTimeComboIdxToSeconds( comboIdx );
    if( m_ExpireSeconds )
    {
        m_OfferInfo.setExpiresTime( GetHighResolutionTimeMs() + ( m_ExpireSeconds * 1000 ) );
    }
    else
    {
        m_OfferInfo.setExpiresTime( 0 );
    }
}

//============================================================================
void OfferSendWidget::setOfferMessage( QString msgText )
{
    ui.m_OfferMsgEdit->setPlainText( msgText );
}

//============================================================================
QString OfferSendWidget::getOfferMessage( void )
{
    return ui.m_OfferMsgEdit->toPlainText();
}

//============================================================================
void OfferSendWidget::updateExpireStatus( EExpireTime expireTime )
{

}

//============================================================================
void OfferSendWidget::slotOfferCancelButtonClicked( void )
{
    emit signalCancelButtonClicked();
}

//============================================================================
void OfferSendWidget::setFocusOnText( void )
{
    ui.m_OfferMsgEdit->setFocus();
}

//============================================================================
void OfferSendWidget::showEvent( QShowEvent* ev )
{
    QWidget::showEvent( ev );
    static bool firstShow{ true };
    if( firstShow )
    {
        firstShow = false;
        ui.m_OfferMsgEdit->setFocus();
    }
}

//============================================================================
void OfferSendWidget::setCancelVisible( bool visible )
{
    ui.m_CancelLabel->setVisible( visible );
    ui.m_OfferCancelButton->setVisible( visible );
}