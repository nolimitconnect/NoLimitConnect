//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "InviteUrlWidget.h"

#include "AppletInformation.h"
#include "AppCommon.h"
#include "GuiHelpers.h"
#include "GuiParams.h"
#include "MyIconsDefs.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/Invite.h>
#include <CoreLib/VxDebug.h>
#include <NetLib/NetHostSetting.h>

#include "ui_InviteUrlWidget.h"

//============================================================================
InviteUrlWidget::InviteUrlWidget( QWidget* parent )
: QWidget( parent )
, ui(*(new Ui::InviteUrlWidgetUi))
, m_MyApp( GetAppInstance() )
{
    ui.setupUi( this );

    m_HostTestList.emplace_back( eHostTypePeerUser );
    m_HostTestList.emplace_back( eHostTypeChatRoom );
    m_HostTestList.emplace_back( eHostTypeGroup );
    m_HostTestList.emplace_back( eHostTypeRandomConnect );

    ui.m_NetworkSettingsInfoButton->setIcon( eMyIconInformation );
    ui.m_NetworkSettingsInfoButton->setFixedSize( eButtonSizeSmall );

    ui.m_ChatRoomButton->setFixedSize( eButtonSizeSmall );
    ui.m_GroupButton->setFixedSize( eButtonSizeSmall );
    ui.m_RandomConnectButton->setFixedSize( eButtonSizeSmall );
    ui.m_NetworkButton->setFixedSize( eButtonSizeSmall );

    ui.m_ChatRoomButton->setIcon( eMyIconNetworking );
    ui.m_GroupButton->setIcon( eMyIconNetworking );
    ui.m_RandomConnectButton->setIcon( eMyIconNetworking );
    ui.m_NetworkButton->setIcon( eMyIconNetworking );

    ui.m_ChatRoomButton->setVisible( false );
    ui.m_GroupButton->setVisible( false );
    ui.m_RandomConnectButton->setVisible( false );
    ui.m_NetworkButton->setVisible( false );

    connect( ui.m_NetworkSettingsInfoButton, SIGNAL(clicked()), this, SLOT(slotNetworkSettingsInfoButtonClicked()) );

    connect( ui.m_PersonalCheckBox, SIGNAL(stateChanged(int)), this, SLOT( slotPersonalCheckBoxClicked(int)) );
    connect( ui.m_PersonalUrlEdit, SIGNAL(textChanged(const QString)), this, SLOT(slotUpdateInvite()) );
    connect( ui.m_ChatRoomCheckBox, SIGNAL(stateChanged(int)), this, SLOT( slotGroupCheckBoxClicked(int)) );
    connect( ui.m_ChatRoomUrlEdit, SIGNAL(textChanged(const QString)), this, SLOT(slotUpdateInvite()) );
    connect( ui.m_GroupCheckBox, SIGNAL(stateChanged(int)), this, SLOT( slotGroupCheckBoxClicked(int)) );
    connect( ui.m_GroupUrlEdit, SIGNAL(textChanged(const QString)), this, SLOT(slotUpdateInvite()) );
    connect( ui.m_RandomConnectCheckBox, SIGNAL(stateChanged(int)), this, SLOT( slotRandomConnectCheckBoxClicked(int)) );
    connect( ui.m_RandomConnectUrlEdit, SIGNAL(textChanged(const QString)), this, SLOT(slotUpdateInvite()) );
    connect( ui.m_NetworkSettingsCheckBox, SIGNAL(stateChanged(int)), this, SLOT( slotNetworkCheckBoxClicked(int)) );

    connect( ui.m_ChatRoomButton, SIGNAL(clicked()), this, SLOT(slotChatRoomButtonClicked()) );
    connect( ui.m_GroupButton, SIGNAL(clicked()), this, SLOT(slotGroupButtonClicked()) );
    connect( ui.m_RandomConnectButton, SIGNAL(clicked()), this, SLOT(slotRandomConnectButtonClicked()) );
    connect( ui.m_NetworkButton, SIGNAL(clicked()), this, SLOT(slotNetworkButtonClicked()) );
}

//============================================================================
void InviteUrlWidget::setupInvite( bool createInvite )
{
    m_IsCreateInvite = createInvite;
    if( m_IsCreateInvite )
    {
        updateUrls();
    }
    else
    {
        ui.m_InviteTextLabel->setText( QObject::tr( "Pasted invite text" ) );
    }
}

//============================================================================
void InviteUrlWidget::slotNetworkSettingsInfoButtonClicked( void )
{
    AppletInformation* activityInfo = new AppletInformation( m_MyApp, parentWidget(), eInfoTypeNetworkSettingsInvite );
    activityInfo->show();
}

//============================================================================
void InviteUrlWidget::setInviteText( std::string inviteText )
{
    if( !m_IsCreateInvite )
    {
        m_PastedInviteText = inviteText;
    }

    ui.m_InviteTextEdit->clear();
    ui.m_InviteTextEdit->appendPlainText( inviteText.c_str() );
    if( !m_IsCreateInvite )
    {
        parseInviteText( inviteText );
        if( inviteText.find( "NetworkKey-Private" ) != std::string::npos )
        {
            // using private key.. give warning
            QMessageBox::warning( this, QObject::tr( "Private Key Warning" ), 
                QObject::tr( "Be sure to get the Network Key for this Network before saving network settings.\nWrong network key will cause you to be banned as a Hacker" ), QMessageBox::Ok );
        }
    }
}

//============================================================================
std::string InviteUrlWidget::getInviteText( void )
{
    if( m_IsCreateInvite )
    {
        updateInviteText();
        return ui.m_InviteTextEdit->toPlainText().toUtf8().constData();
    }
    else
    {      
        return generateSelectedInviteText();
    } 
}

//============================================================================
int InviteUrlWidget::testAvailableUrls( void )
{
    int availableHostCnt{ 0 };
    m_HostValidList.clear();
    if( m_IsCreateInvite )
    {
        if( !m_MyApp.getEngine().getNetStatusAccum().isRxPortOpen() )
        {
            return availableHostCnt;
        }

        for( auto hostType : m_HostTestList )
        {
            if( hostType == eHostTypePeerUser )
            {
                // if have open port then personal urls are possible
                m_HostValidList.emplace_back( hostType );
                continue;
            }

            std::string myUrl = getHostedUrl( hostType );
            if( !myUrl.empty() )
            {
                m_HostValidList.emplace_back( hostType );
            }
        }
    }

    return static_cast<int>(m_HostValidList.size());
}

//============================================================================
void InviteUrlWidget::updateUrls( void )
{
    if( !m_IsCreateInvite )
    {
        return;
    }

    if( !testAvailableUrls() )
    {
        return;
    }

    for( auto hostType : m_HostValidList )
    {
        getUrlEdit( hostType )->setText( getHostedUrl( hostType ).c_str() );
    }
}

//============================================================================
std::string InviteUrlWidget::getHostedUrl( EHostType hostType )
{
    std::string url;
    if( !m_MyApp.getEngine().getNetStatusAccum().isRxPortOpen() )
    {
        return url;
    }

    if( m_MyUrl.empty() )
    {
        VxNetIdent myIdent;
        m_MyApp.getEngine().fromGuiQueryMyIdent( &myIdent );
        m_MyUrl = myIdent.getMyOnlineUrl( eHostTypeUnknown, false );
    }

    if( !m_MyUrl.empty() )
    {
        return m_MyUrl + Invite::getHostTypeSuffix( hostType );
    }

    return url;
}

//============================================================================
QCheckBox* InviteUrlWidget::getUrlCheckBox( EHostType hostType )
{
    switch( hostType )
    {
    case eHostTypePeerUser:
        return ui.m_PersonalCheckBox;

    case eHostTypeGroup:
        return ui.m_GroupCheckBox;

    case eHostTypeChatRoom:
        return ui.m_ChatRoomCheckBox;

    case eHostTypeRandomConnect:
        return ui.m_RandomConnectCheckBox;

    default:
        break;
    }

    return nullptr;
}

//============================================================================
QLineEdit* InviteUrlWidget::getUrlEdit( EHostType hostType )
{
    switch( hostType )
    {
    case eHostTypePeerUser:
        return ui.m_PersonalUrlEdit;

    case eHostTypeGroup:
        return ui.m_GroupUrlEdit;

    case eHostTypeChatRoom:
        return ui.m_ChatRoomUrlEdit;

    case eHostTypeRandomConnect:
        return ui.m_RandomConnectUrlEdit;

    default:
        break;
    }

    return nullptr;
}

//============================================================================
QFrame* InviteUrlWidget::getUrlFrame( EHostType hostType )
{
    switch( hostType )
    {
    case eHostTypePeerUser:
        return ui.m_PersonalFrame;

    case eHostTypeGroup:
        return ui.m_GroupFrame;

    case eHostTypeChatRoom:
        return ui.m_ChatRoomFrame;

    case eHostTypeRandomConnect:
        return ui.m_RandomConnectFrame;

    default:
        break;
    }

    return nullptr;
}

//============================================================================
void InviteUrlWidget::slotUpdateInvite( void )
{
    if( m_IsSettingUrls )
    {
        return; // while setting do not update with every change
    }

    updateUrls();
    updateInviteText();
    emit signalInviteChanged();
}

//============================================================================
void InviteUrlWidget::updateInviteText( void )
{
    if( !m_IsCreateInvite )
    {
        // do not change the pasted text
        return;
    }

    ui.m_InviteTextEdit->clear();
    bool haveInvite = false;
    for( auto hostType : m_HostTestList )
    {
        if( getUrlFrame( hostType )->isVisible() && getUrlCheckBox( hostType )->isChecked() )
        {
            QString url = getUrlEdit( hostType )->text();
            if( !url.isEmpty() )
            {
                if( !haveInvite )
                {
                    haveInvite = true;
                    ui.m_InviteTextEdit->appendPlainText( Invite::INVITE_BEGIN );
                }

                ui.m_InviteTextEdit->appendPlainText( url );
            }
        }
    }

    if( !m_UserMsg.empty() )
    {
        if( !haveInvite )
        {
            haveInvite = true;
            ui.m_InviteTextEdit->appendPlainText( Invite::INVITE_BEGIN );
        }

        ui.m_InviteTextEdit->appendPlainText( Invite::INVITE_HDR_MSG );
        ui.m_InviteTextEdit->appendPlainText( m_UserMsg.c_str() );
    }

    if( ui.m_NetworkSettingsCheckBox->isChecked() )
    {
        if( !haveInvite )
        {
            haveInvite = true;
            ui.m_InviteTextEdit->appendPlainText( Invite::INVITE_BEGIN );
        }

        P2PEngine& engine = m_MyApp.getEngine();
        ui.m_InviteTextEdit->appendPlainText( Invite::INVITE_HDR_NET_SETTING );
        std::string keyText = getNetworkKeyState();
        ui.m_InviteTextEdit->appendPlainText( keyText.c_str() );

        std::string strValue;

        engine.getEngineSettings().getNetworkHostUrl( strValue );
        populateNetSettingUrl( eHostTypeNetwork, strValue );

        engine.getEngineSettings().getConnectTestUrl( strValue );
        populateNetSettingUrl( eHostTypeConnectTest, strValue );

        engine.getEngineSettings().getGroupHostUrl( strValue );
        populateNetSettingUrl( eHostTypeChatRoom, strValue );

        engine.getEngineSettings().getChatRoomHostUrl( strValue );
        populateNetSettingUrl( eHostTypeGroup, strValue );

        engine.getEngineSettings().getRandomConnectUrl( strValue );
        populateNetSettingUrl( eHostTypeRandomConnect, strValue );

        haveInvite = true;
    }

    if( haveInvite )
    {
        ui.m_InviteTextEdit->appendPlainText( Invite::INVITE_END );
    }
}

//============================================================================
bool InviteUrlWidget::populateNetSettingUrl( EHostType hostType, std::string& ptopUrl )
{
    if( !ptopUrl.empty() )
    {
        std::string url = ptopUrl.c_str();
        Invite::appendHostTypeSuffix( hostType, url );
        ui.m_InviteTextEdit->appendPlainText( url.c_str() );
    }

    return true;
}

//============================================================================
bool InviteUrlWidget::setHostInviteType( EHostType hostTypeIn ) // limit to specific host type
{
    bool wasFound{ false };
    for( auto hostType : m_HostTestList )
    {
        getUrlFrame( hostType )->setVisible( false );
    }

    for( auto hostType : m_HostTestList )
    {
        if( hostType == hostTypeIn )
        {
            getUrlFrame( hostTypeIn )->setVisible( true );
            getUrlCheckBox( hostTypeIn )->setChecked( true );
            wasFound = true;
        }
    }

    return wasFound;
}

//============================================================================
void InviteUrlWidget::parseInviteText( std::string inviteText )
{
    Invite invite;
    invite.setInviteText( inviteText );

    if( !invite.getInviteUrls( m_HostUrls, m_NetworkUrls, m_UserMsg ) )
    {
        GuiHelpers::showInviteInvalidError( this );
        return;
    }

    for( auto hostType : m_HostTestList )
    {
        getUrlFrame( hostType )->setVisible( false );
    }

    if( m_NetworkUrls.empty() )
    {
        ui.m_NetworkFrame->setVisible( false );
    }

    m_IsSettingUrls = true; // do not say has changed until have set all the text
    for( auto ptopUrl : m_HostUrls )
    {
        EHostType hostType = ptopUrl.getHostType();
        if( hostType != eHostTypeUnknown )
        {
            std::string url = ptopUrl.getHostUrl().c_str();
            if( !url.empty() )
            {
                QFrame* frame = getUrlFrame( hostType );
                QLineEdit* lineEdit = getUrlEdit( hostType );
                QCheckBox* checkBox = getUrlCheckBox( hostType );
                if( frame && lineEdit && checkBox )
                {
                    QString urlText = url.c_str();
                    frame->setVisible( true );
                    lineEdit->setText( urlText );
                    checkBox->setChecked( true );
                    LogMsg( LOG_VERBOSE, "%s host %s url %s", __func__, DescribeHostType( hostType ), url.c_str() );
                }
                else
                {
                    LogMsg( LOG_VERBOSE, "%s null widget for host %s url %s", __func__, DescribeHostType( hostType ), url.c_str() );
                }
            }
            else
            {
                LogMsg( LOG_VERBOSE, "%s host %s empty url", __func__, DescribeHostType( hostType ) );
            }
        }
        else
        {
            LogMsg( LOG_VERBOSE, "%s Invalid host type %d", __func__, hostType );
        }
    }

    m_IsSettingUrls = false; 
    slotUpdateInvite(); // now say has changed
}

//============================================================================
std::string InviteUrlWidget::generateSelectedInviteText( void )
{
    std::string inviteText;
    updateAcceptUrls();
    if( m_HostUrls.empty() && m_NetworkUrls.empty() )
    {
        return inviteText;
    }

    inviteText += Invite::INVITE_BEGIN;
    inviteText += "\n";
    for( auto& ptopUrl : m_HostUrls )
    {
        if( getUrlCheckBox( ptopUrl.getHostType() )->isChecked() )
        {
            inviteText += ptopUrl.getHostUrl();
            inviteText += "\n";
        }
    }

    if( !m_UserMsg.empty() )
    {
        inviteText += Invite::INVITE_HDR_MSG;
        inviteText += "\n";
        inviteText += m_UserMsg;
        inviteText += "\n";
    }

    if( ui.m_NetworkSettingsCheckBox->isChecked() && !m_NetworkUrls.empty() )
    {
        inviteText += Invite::INVITE_HDR_NET_SETTING;
        inviteText += "\n";
        inviteText += getNetworkKeyState();
        inviteText += "\n";
        for( auto& ptopUrl : m_NetworkUrls )
        {
            if( ptopUrl.getHostType() == eHostTypeNetwork )
            {
                inviteText += ptopUrl.getHostUrl();
                if( inviteText[inviteText.length() - 1] != Invite::getHostTypeSuffix( eHostTypeNetwork ) )
                {
                    inviteText += Invite::getHostTypeSuffix( eHostTypeNetwork );
                }

                inviteText += "\n";
            }
            else if( ptopUrl.getHostType() == eHostTypeConnectTest )
            {
                inviteText += ptopUrl.getHostUrl();
                if( inviteText[inviteText.length() - 1] != Invite::getHostTypeSuffix( eHostTypeConnectTest ) )
                {
                    inviteText += Invite::getHostTypeSuffix( eHostTypeConnectTest );
                }
        
                inviteText += "\n";
            }

        }
    }

    inviteText += Invite::INVITE_END;

    return inviteText;
}

//============================================================================
std::string InviteUrlWidget::getNetworkKeyState( void )
{
    std::string keyText{ "NetworkKey-" };
    if( !m_IsCreateInvite && !m_PastedInviteText.empty() )
    {
        if( m_PastedInviteText.find( Invite::INVITE_PRIVATE_KEY ) != std::string::npos )
        {
            keyText += "Private";
            m_HasPrivateNetworkKey = true;
        }
        else
        {
            keyText += "Public";
            m_HasPrivateNetworkKey = false;
        }
    }
    else
    {
        keyText += m_MyApp.getEngine().getNetStatusAccum().hasDefaultNetworkKey() ? "Public" : "Private";
    }

    return keyText;
}

//============================================================================
void InviteUrlWidget::updateAcceptUrls( void )
{
    m_HostUrls.clear();
    for( auto hostType : m_HostTestList )
    {
        if( !getUrlCheckBox( hostType )->isChecked() )
        {
            continue;
        }

        VxPtopUrl ptopUrl( getUrlEdit( hostType )->text().toUtf8().constData() );
        if( !ptopUrl.isValid() || ptopUrl.getHostType() != hostType )
        {
            continue;
        }

        m_HostUrls.emplace_back( ptopUrl );
    }
}

//============================================================================
void InviteUrlWidget::slotChatRoomButtonClicked( void )
{
    testUrl( eHostTypeChatRoom );
}

//============================================================================
void InviteUrlWidget::slotGroupButtonClicked( void )
{
    testUrl( eHostTypeGroup );
}

//============================================================================
void InviteUrlWidget::slotRandomConnectButtonClicked( void )
{
    testUrl( eHostTypeRandomConnect );
}

//============================================================================
void InviteUrlWidget::slotNetworkButtonClicked( void )
{
    testUrl( eHostTypeNetwork );
}

//============================================================================
void InviteUrlWidget::testUrl( EHostType hostType )
{
    VxPtopUrl hostUrl;
    if( hostType == eHostTypeNetwork )
    {
        for( auto& ptopUrl : m_NetworkUrls )
        {
            if( ptopUrl.getHostType() == hostType )
            {
                hostUrl = ptopUrl;
                break;
            }
        }
    }
    else
    {
        for( auto& ptopUrl : m_HostUrls )
        {
            if( ptopUrl.getHostType() == hostType )
            {
                hostUrl = ptopUrl;
                break;
            }
        }
    }

    // TODO network and hosts tests
}

//============================================================================
void InviteUrlWidget::slotPersonalCheckBoxClicked( int checked )
{
    if( m_IsCreateInvite && checked )
    {
        if( !checkPortOpen( eHostTypePeerUser ) )
        {
            return;
        }
    }

    slotUpdateInvite();
}

//============================================================================
void InviteUrlWidget::slotChatRoomCheckBoxClicked( int checked )
{
    if( m_IsCreateInvite && checked )
    {
        if( !checkPortOpen( eHostTypeChatRoom ) )
        {
            return;
        }

        if( !checkHostEnabled( eHostTypeChatRoom ) )
        {
            return;
        }
    }

    slotUpdateInvite();
}

//============================================================================
void InviteUrlWidget::slotGroupCheckBoxClicked( int checked )
{
    if( m_IsCreateInvite && checked )
    {
        if( !checkPortOpen( eHostTypeGroup ) )
        {
            return;
        }

        if( !checkHostEnabled( eHostTypeGroup ) )
        {
            return;
        }
    }

    slotUpdateInvite();
}

//============================================================================
void InviteUrlWidget::slotRandomConnectCheckBoxClicked( int checked )
{
    if( m_IsCreateInvite && checked )
    {
        if( !checkPortOpen( eHostTypeRandomConnect ) )
        {
            return;
        }

        if( !checkHostEnabled( eHostTypeRandomConnect ) )
        {
            return;
        }

    }

    slotUpdateInvite();
}

//============================================================================
void InviteUrlWidget::slotNetworkCheckBoxClicked( int checked )
{
    if( m_IsCreateInvite && checked )
    {
        if( !checkPortOpen( eHostTypeNetwork ) )
        {
            return;
        }
    }

    slotUpdateInvite();
}

//============================================================================
bool InviteUrlWidget::checkPortOpen( EHostType hostType )
{
    bool isOpen = m_MyApp.getEngine().getNetStatusAccum().isRxPortOpen();
    if( !isOpen )
    {
        QMessageBox::warning( this, QObject::tr("Cannot create Invite"), QObject::tr( "Do NOT create Invite without a open Network Port.\nNo one can connect to you directly" ), QMessageBox::Ok);
        QCheckBox* checkBox = getUrlCheckBox( hostType );
        if( checkBox )
        {
            checkBox->setChecked( false );
            checkBox->setEnabled( false );
        }
    }

    return isOpen;
}

//============================================================================
bool InviteUrlWidget::checkHostEnabled( EHostType hostType )
{
    bool isHostEnabled{ true };
    if( hostType == eHostTypeNetwork || hostType == eHostTypeChatRoom || hostType == eHostTypeGroup || hostType == eHostTypeRandomConnect )
    {
        QMessageBox::warning( this, QObject::tr( "Hosting plugin not enabled" ), GuiParams::describeHostType( hostType ) + QObject::tr( " has permission level of disabled" ), QMessageBox::Ok );
        QCheckBox* checkBox = getUrlCheckBox( hostType );
        if( checkBox )
        {
            checkBox->setChecked( false );
            checkBox->setEnabled( false );
        }
    }
    
    return isHostEnabled;
}