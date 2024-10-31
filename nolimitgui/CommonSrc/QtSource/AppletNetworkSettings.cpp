//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletNetworkSettings.h"

#include "ActivityInformation.h"
#include "ActivityNetworkState.h"
#include "ActivityYesNoMsgBox.h"
#include "AppletIsPortOpenTest.h"

#include "AccountMgr.h"
#include "AppCommon.h"
#include "AppGlobals.h"
#include "MyIconsDefs.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/ObjectCommonDefs.h>
#include <CoreLib/VxDebug.h>
#include <CoreLib/VxSktUtil.h>

#include <NetLib/NetHostSetting.h>
#include <NetLib/NetHostSettingDefs.h>
#include <NetLib/VxGetRandomPort.h>
#include <NetLib/VxPortForward.h>

#include "ui_AppletNetworkSettings.h"

//============================================================================
AppletNetworkSettings::AppletNetworkSettings( AppCommon& app, QWidget* parent )
: AppletBase( OBJNAME_APPLET_NETWORK_SETTINGS, app, parent )
, ui(*(new Ui::AppletNetworkSettingsUi))
, m_UpdateTimer( new QTimer(this) )
{
	setAppletType( eAppletNetworkSettings );
	ui.setupUi( getContentItemsFrame() );
	setTitleBarText( DescribeApplet( m_EAppletType ) );

    ui.m_NetworkHostInfoButton->setIcon( eMyIconInformation );
    ui.m_NetworkKeyInfoButton->setIcon( eMyIconInformation );
    ui.m_ConnectTestUrlInfoButton->setIcon( eMyIconInformation );
    ui.m_ConnectIsOpenInfoButton->setIcon( eMyIconInformation );
    ui.m_Ipv6InfoButton->setIcon( eMyIconInformation );

    ui.m_SaveSettingsButton->setIcon( eMyIconFileSave );
    ui.m_SaveSettingsButton->setFixedSize( eButtonSizeSmall );
    ui.m_DeleteSettingsButton->setIcon( eMyIconTrash );
    ui.m_DeleteSettingsButton->setFixedSize( eButtonSizeSmall );

    updateDlgFromSettings( true );

    connectSignals();

#if defined(DEBUG)
    m_OrigLogPortForward = IsLogEnabled( eLogPortForward );
#endif // defined(DEBUG)

	m_MyApp.activityStateChange( this, true );
    fillMyNodeUrl( ui.m_NodeUrlLabel );
    m_UpdateTimer->setInterval( 2000 );
    m_UpdateTimer->start();
}

//============================================================================
AppletNetworkSettings::~AppletNetworkSettings()
{
    #if defined(DEBUG)
    if( m_OrigLogPortForward != IsLogEnabled( eLogPortForward ) )
    {
        VxSetLogModuleEnable( eLogPortForward, m_OrigLogPortForward );
    }
    #endif // defined(DEBUG)

    m_MyApp.activityStateChange( this, false );
}

//============================================================================
void AppletNetworkSettings::connectSignals( void )
{
    connect( ui.m_NetworkHostInfoButton, SIGNAL(clicked()), this, SLOT(slotShowNetworkHostInformation()) );
    connect( ui.m_NetworkKeyInfoButton, SIGNAL(clicked()), this, SLOT(slotShowNetworkKeyInformation()) );

    connect( ui.AutoDetectProxyRadioButton, SIGNAL(clicked()), this, SLOT(slotAutoDetectProxyClick()) );
    connect( ui.AssumeNoProxyRadioButton, SIGNAL(clicked()), this, SLOT(slotNoProxyClick()) );
    connect( ui.AssumeProxyRadioButton, SIGNAL(clicked()), this, SLOT(slotYesProxyClick()) );

    connect( ui.RandomPortButton, SIGNAL(clicked()), this, SLOT(slotRandomPortButtonClick()) );
    connect( ui.m_UseUpnpCheckBox, SIGNAL(clicked()), this, SLOT(slotUseUpnpCheckBoxClick()) );
    connect( ui.m_ConnectTestUrlInfoButton, SIGNAL(clicked()), this, SLOT(slotShowConnectTestUrlInformation()) );
    connect( ui.m_ConnectIsOpenInfoButton, SIGNAL(clicked()), this, SLOT(slotShowConnectTestSettingsInformation()) );

    connect( ui.m_SaveSettingsButton, SIGNAL(clicked()), this, SLOT(onSaveButtonClick()) );
    connect( ui.m_SaveSettingsLabel, SIGNAL(clicked()), this, SLOT(slotSaveLabelClick()) );

    connect( ui.m_DeleteSettingsButton, SIGNAL(clicked()), this, SLOT(onDeleteButtonClick()) );
    connect( ui.m_DeleteSettingsLabel, SIGNAL(clicked()), this, SLOT(slotDeleteLabelClick()) );

    connect( ui.m_CopyToClipboardButton, SIGNAL(clicked()), this, SLOT(slotCopyMyUrlToClipboard()) );
    connect( ui.m_TestIsPortOpenButton, SIGNAL(clicked()), this, SLOT(slotTestIsMyPortOpenButtonClick()) );

    connect( ui.m_NetworkSettingsNameComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxSelectionChange(int)) );

    connect( m_UpdateTimer, SIGNAL(timeout()), this, SLOT(slotUpdateTimer()) );

    connect( ui.m_TestUpnpButton, SIGNAL(clicked()), this, SLOT(slotTestUpnpButtonClick()) ); 

    connect( ui.m_Ipv6InfoButton, SIGNAL(clicked()), this, SLOT(slotShowIpv6Information()) );
    connect( ui.m_UseIpv6Network, SIGNAL(clicked()), this, SLOT(slotUseIpv6CheckBoxClick()) );
}

//============================================================================
QString	AppletNetworkSettings::getNetworkKey( void ) { return ui.m_NetworkKeyEdit->text(); }

//============================================================================
void AppletNetworkSettings::updateDlgFromSettings( bool origSettings )
{
    ui.m_NetworkSettingsNameComboBox->clear();
    ui.AutoDetectProxyRadioButton->setChecked( false );
    ui.AssumeNoProxyRadioButton->setChecked( false );
    ui.AssumeProxyRadioButton->setChecked( false );

    bool validDbSettings = false;
    AccountMgr& dataHelper = m_MyApp.getAccountMgr();
    std::vector<NetHostSetting> netSettingList;
    std::string lastSettingsName = dataHelper.getLastNetHostSettingName();
    int selectedIdx = 0;
    int currentSettingIdx = 0;
    if( ( 0 != lastSettingsName.length() )
        && dataHelper.getAllNetHostSettings( netSettingList )
        && ( 0 != netSettingList.size() ) )
    {
        std::vector<NetHostSetting>::iterator iter;
        for( iter = netSettingList.begin(); iter != netSettingList.end(); ++iter )
        {
            NetHostSetting& netHostSetting = *iter;
            ui.m_NetworkSettingsNameComboBox->addItem( netHostSetting.getNetHostSettingName().c_str() );
            if( netHostSetting.getNetHostSettingName() == lastSettingsName )
            {
                // found last settings used
                selectedIdx = currentSettingIdx;
                validDbSettings = true;
                populateDlgFromNetHostSetting( netHostSetting );
            }

            currentSettingIdx++;
        }
    }

    if( validDbSettings )
    {
        ui.m_NetworkSettingsNameComboBox->setCurrentIndex( selectedIdx );
    }
    else if( netSettingList.size() )
    {
        ui.m_NetworkSettingsNameComboBox->addItem( netSettingList[ 0 ].getNetHostSettingName().c_str() );
        populateDlgFromNetHostSetting( netSettingList[ 0 ] );
        dataHelper.updateLastNetHostSettingName( netSettingList[ 0 ].getNetHostSettingName().c_str() );
    }
    else
    {
        ui.m_NetworkSettingsNameComboBox->addItem( "default" );
        NetHostSetting engineHostSetting;
        fillNetHostSettingFromEngine( engineHostSetting );
        populateDlgFromNetHostSetting( engineHostSetting );        
    }

    if( origSettings )
    {
        m_OriginalNetworkKey = ui.m_NetworkKeyEdit->text();
    }
}

//============================================================================
void AppletNetworkSettings::fillNetHostSettingFromEngine( NetHostSetting& netSetting )
{
    std::string strValue;

    m_Engine.getEngineSettings().getNetworkHostUrl( strValue );
    netSetting.setNetworkHostUrl( strValue.c_str() );

    m_Engine.getEngineSettings().getNetworkKey( strValue );
    netSetting.setNetworkKey( strValue.c_str() );

    m_Engine.getEngineSettings().getConnectTestUrl( strValue );
    netSetting.setConnectTestUrl( strValue.c_str() );

    bool ipv6 = m_Engine.getEngineSettings().getUseIpv6();
    netSetting.setUseIpv6( ipv6 );

    std::string externIP;
    m_Engine.getEngineSettings().getUserSpecifiedExternIpAddr( externIP, ipv6 );
    netSetting.setUserSpecifiedExternIpAddr( externIP.c_str() );

    netSetting.setUseIpv6( m_Engine.getEngineSettings().getUseIpv6() );

    uint16_t u16Port = m_Engine.getEngineSettings().getTcpIpPort();
    netSetting.setTcpPort( u16Port );

    EFirewallTestType eFirewallType = m_Engine.getEngineSettings().getFirewallTestSetting();
    int32_t fireWallType = 0;
    switch( eFirewallType )
    {
    case  eFirewallTestAssumeNoFirewall:
        fireWallType = 1;
        break;

    case eFirewallTestAssumeFirewalled:
        fireWallType = 2;
        break;

    case eFirewallTestUrlConnectionTest:
    default:
        fireWallType = 0;
    }

    netSetting.setFirewallTestType( fireWallType );

    bool useUpnp = m_Engine.getEngineSettings().getUseUpnpPortForward();
    netSetting.setUseUpnpPortForward( useUpnp );
}

//============================================================================
void AppletNetworkSettings::applySettingsToEngine( void )
{
    NetHostSetting netHostSetting;
    populateNetHostSettingsFromDlg( netHostSetting );
    applyEngineSettingsFromHostSetting( netHostSetting );
}

//============================================================================
void AppletNetworkSettings::applyEngineSettingsFromHostSetting( NetHostSetting& netHostSetting )
{
    if( 1 == netHostSetting.getFirewallTestType() && !netHostSetting.getUserSpecifiedExternIpAddr().empty() )
    {
        m_MyApp.getAppGlobals().getMyNetIdent()->setOnlineIpAddress( netHostSetting.getUserSpecifiedExternIpAddr().c_str());
    }

    if( m_Engine.getMyNetIdent()->getOnlinePort() != netHostSetting.getTcpPort() )
    {
        m_Engine.getMyNetIdent()->setOnlinePort( netHostSetting.getTcpPort() );
        m_Engine.setPktAnnLastModTime( GetTimeStampMs() );
    }

    m_Engine.fromGuiApplyNetHostSettings(netHostSetting);
}

//============================================================================
void AppletNetworkSettings::updateSettingsFromDlg()
{
    NetHostSetting netHostSetting;
    populateNetHostSettingsFromDlg( netHostSetting );
    if( netHostSetting != m_OriginalSettings )
    {
        LogMsg( LOG_DEBUG, "AppletNetworkSettings has changed" );

        m_OriginalSettings = netHostSetting;

        m_MyApp.getAccountMgr().updateNetHostSetting( netHostSetting );
        m_MyApp.getAccountMgr().updateLastNetHostSettingName( netHostSetting.getNetHostSettingName().c_str() );
    }
    else
    {
        LogMsg( LOG_DEBUG, "AppletNetworkSettings no change" );
    }
}

//============================================================================
void AppletNetworkSettings::populateNetHostSettingsFromDlg( NetHostSetting& netHostSetting )
{
    // get user name of the setting
    std::string netSettingsName = ui.m_NetworkSettingsNameComboBox->currentText().toUtf8().constData();
    if( 0 == netSettingsName.length() )
    {
        netSettingsName = "default";
    }

    netHostSetting.setNetHostSettingName( netSettingsName.c_str() );

    std::string strValue;
    strValue = ui.m_NetworkHostUrlEdit->text().toUtf8().constData();
    netHostSetting.setNetworkHostUrl( strValue.c_str() );

    strValue = ui.m_NetworkKeyEdit->text().toUtf8().constData();
    netHostSetting.setNetworkKey( strValue.c_str() );

    strValue = ui.m_ConnectTestUrlEdit->text().toUtf8().constData();
    netHostSetting.setConnectTestUrl( strValue.c_str() );

    netHostSetting.setUseIpv6( ui.m_UseIpv6Network->isChecked() );

    uint16_t u16TcpPort = ui.PortEdit->text().toUShort();
    if( 0 != u16TcpPort )
    {
        netHostSetting.setTcpPort( u16TcpPort );
    }
    else
    {
        netHostSetting.setTcpPort( NET_DEFAULT_NETSERVICE_PORT );
    }

    std::string externIp = ui.m_ExternIpEdit->text().toUtf8().constData();
    if( externIp.length() )
    {
        netHostSetting.setUserSpecifiedExternIpAddr( externIp.c_str() );
    }
    else
    {
        externIp = "";
        netHostSetting.setUserSpecifiedExternIpAddr( externIp.c_str() );
    }

    EFirewallTestType eFirewallTestType = getFirewallTestType();

    switch( eFirewallTestType )
    {
    case eFirewallTestAssumeNoFirewall:
        netHostSetting.setFirewallTestType( 1 );
        break;
 
    case eFirewallTestAssumeFirewalled:
        netHostSetting.setFirewallTestType( 2 );
        break;

    case eFirewallTestUrlConnectionTest:
    default:
        netHostSetting.setFirewallTestType( 0 );
    }
    
    netHostSetting.setUseUpnpPortForward( ui.m_UseUpnpCheckBox->isChecked() );
}

//============================================================================
void AppletNetworkSettings::slotExitButtonClick()
{
    closeApplet();
}

//============================================================================
void AppletNetworkSettings::slotGoToNetHostSettingsButtonClick()
{
    ActivityNetworkState * activityInfo = new ActivityNetworkState( m_MyApp, this );
    activityInfo->show();
}

//============================================================================
void AppletNetworkSettings::slotAutoDetectProxyClick( void )
{
    setFirewallTestType( eFirewallTestUrlConnectionTest );
}

//============================================================================
void AppletNetworkSettings::slotNoProxyClick( void )
{
    setFirewallTestType( eFirewallTestAssumeNoFirewall );
}

//============================================================================
void AppletNetworkSettings::slotYesProxyClick( void )
{
    setFirewallTestType( eFirewallTestAssumeFirewalled );
}

//============================================================================
void AppletNetworkSettings::setFirewallTestType( EFirewallTestType eFirewallType )
{
    ui.AutoDetectProxyRadioButton->setChecked( false );
    ui.AssumeNoProxyRadioButton->setChecked( false );
    ui.AssumeProxyRadioButton->setChecked( false );

    switch( eFirewallType )
    {
    case eFirewallTestAssumeFirewalled:
        ui.AssumeProxyRadioButton->setChecked( true );
        break;

    case eFirewallTestAssumeNoFirewall:
        ui.AssumeNoProxyRadioButton->setChecked( true );
        break;

    case eFirewallTestUrlConnectionTest:
    case eMaxFirewallTestType:
    default:
        ui.AutoDetectProxyRadioButton->setChecked( true );
        break;
    }
}

//============================================================================
EFirewallTestType AppletNetworkSettings::getFirewallTestType( void )
{
    EFirewallTestType eFirewallTestType = eFirewallTestUrlConnectionTest;
    if( ui.AssumeNoProxyRadioButton->isChecked() )
    {
        eFirewallTestType = eFirewallTestAssumeNoFirewall;
    }
    else if( ui.AssumeProxyRadioButton->isChecked() )
    {
        eFirewallTestType = eFirewallTestAssumeFirewalled;
    }

    return eFirewallTestType;
}

//============================================================================
void AppletNetworkSettings::slotRandomPortButtonClick( void )
{
    uint16_t u16TcpPort = VxGetRandomTcpPort();
    ui.PortEdit->setText( std::to_string(u16TcpPort).c_str() );
    updateSettingsFromDlg();
}

//============================================================================
void AppletNetworkSettings::slotTestIsMyPortOpenButtonClick( void )
{
    if( ui.m_UseIpv6Network->isChecked() && !verifyIpv6Capable() )
    {
        return;
    }

    uint16_t u16Port = ui.PortEdit->text().toUShort();
    if( 0 != u16Port )
    {
        updateSettingsFromDlg();
        AppletIsPortOpenTest* applet = dynamic_cast<AppletIsPortOpenTest*>(m_MyApp.launchApplet( eAppletIsPortOpenTest, getParentPageFrame() ) );
        if( applet )
        {
            applet->setTestOpenPort( u16Port );
        }
    }
    else
    {
        QMessageBox::information( this, QObject::tr( "TCP Listen Port Error" ), QObject::tr( "TCP Listen Port cannot be zero." ) );
    }
}

//============================================================================
void AppletNetworkSettings::slotTestUpnpButtonClick( void )
{
    uint16_t u16Port = ui.PortEdit->text().toUShort();
    if( 0 != u16Port && ui.m_UseUpnpCheckBox->isChecked() )
    {
        m_MyApp.launchApplet( eAppletLogView, getParentPageFrame() );
        updateSettingsFromDlg();
        VxPortForward::setEnablePortForward( true );
        VxPortForward::addPortForward( false, m_MyApp.getEngine().getNetStatusAccum().getLocalIpAddress(), u16Port );
        VxPortForward::listPortForward( false );
    }
    else
    {
        QMessageBox::information( this, QObject::tr( "UPNP Error" ), QObject::tr( "UPNP must be enabled and Listen Port cannot be zero." ) );
    }
}

//============================================================================
void AppletNetworkSettings::slotUseUpnpCheckBoxClick( void )
{
}

//============================================================================
void AppletNetworkSettings::onComboBoxSelectionChange( int )
{
    QString netSettingName = ui.m_NetworkSettingsNameComboBox->currentText();
    AccountMgr& dataHelper = m_MyApp.getAccountMgr();
    NetHostSetting netSetting;
    if( dataHelper.getNetHostSettingByName( netSettingName.toUtf8(), netSetting ) )
    {
        populateDlgFromNetHostSetting( netSetting );
        updateSettingsFromDlg();
    }
}

//============================================================================
void AppletNetworkSettings::populateDlgFromNetHostSetting( NetHostSetting& netSetting )
{
    ui.AutoDetectProxyRadioButton->setChecked( false );
    ui.AssumeNoProxyRadioButton->setChecked( false );
    ui.AssumeProxyRadioButton->setChecked( false );

    ui.m_NetworkHostUrlEdit->setText( netSetting.getNetworkHostUrl().c_str() );
    ui.m_NetworkKeyEdit->setText( netSetting.getNetworkKey().c_str() );
    m_OriginalNetworkKey = ui.m_NetworkKeyEdit->text();
    ui.m_ConnectTestUrlEdit->setText( netSetting.getConnectTestUrl().c_str() );
    ui.m_ExternIpEdit->setText( netSetting.getUserSpecifiedExternIpAddr().c_str() );

    int32_t firewallType = netSetting.getFirewallTestType();
    switch( firewallType )
    {
    case 2:
        ui.AssumeProxyRadioButton->setChecked( true );
        break;

    case 1:
        ui.AssumeNoProxyRadioButton->setChecked( true );
        break;

    default:
        ui.AutoDetectProxyRadioButton->setChecked( true );
        break;
    }

    ui.m_UseUpnpCheckBox->setChecked( netSetting.getUseUpnpPortForward() );

    uint16_t u16Port = netSetting.getTcpPort();
    ui.PortEdit->setText( QString( "%1" ).arg( u16Port ) );

    ui.m_UseIpv6Network->setChecked( netSetting.getUseIpv6() );
}

//============================================================================
void AppletNetworkSettings::closeEvent( QCloseEvent * event )
{
    QWidget::closeEvent( event );
}

//============================================================================
void AppletNetworkSettings::slotSaveLabelClick( void )
{
    ui.m_SaveSettingsButton->emulateUserClicked();
}

//============================================================================
void AppletNetworkSettings::onSaveButtonClick( void )
{
    if( ui.m_UseIpv6Network->isChecked() )
    {
        if( !verifyIpv6Capable() )
        {
            return;
        }

		QString title = QObject::tr( "Confirm Use Of Experimental IPv6 Network?" );
		QString bodyText = QObject::tr( "IPv6 is experimental and the network for IPv6 is not visible on the IPv4 network" );

		ActivityYesNoMsgBox dlg( m_MyApp, &m_MyApp, title, bodyText );
		dlg.makeNeverShowAgainVisible( false );
        bool confirmed = (QDialog::Accepted == dlg.exec());
        if( !confirmed )
		{
			return;
		}
    }

    if( !verifyFirewallSettings() )
    {
        return;
    }

    QString keyVal = getNetworkKey();
    if( verifyNetworkKey( keyVal ) )
    {
        std::string netSettingsName;
        netSettingsName = ui.m_NetworkSettingsNameComboBox->currentText().toUtf8().constData();
        if( netSettingsName.empty() )
        {
            QMessageBox::information( this, QObject::tr( "Network Setting" ), QObject::tr( "Network setting name cannot be blank." ) );
            return;
        }

        std::string netHostUrl;
        netHostUrl = ui.m_NetworkHostUrlEdit->text().toUtf8().constData();
        if( netHostUrl.empty() )
        {
            QMessageBox::information( this, QObject::tr( "Network Setting" ), QObject::tr( "Network host URL cannot be blank." ) );
            return;
        }

        std::string keyString = keyVal.toUtf8().constData();
        if( keyString.empty() )
        {
            QMessageBox::information( this, QObject::tr( "Network Setting" ), QObject::tr( "Network key cannot be blank." ) );
            return;
        }

         EFirewallTestType firewallTestType = getFirewallTestType();
         if( eFirewallTestAssumeNoFirewall != firewallTestType )
         {
             std::string connectTestUrl;
             connectTestUrl = ui.m_ConnectTestUrlEdit->text().toUtf8().constData();
             if( connectTestUrl.empty() )
             {
                 QMessageBox::information( this, QObject::tr( "Network Setting" ), QObject::tr( "Connection Test URL cannot be blank." ) );
                 return;
             }
         }

        NetHostSetting netSetting;
        if( m_MyApp.getAccountMgr().getNetHostSettingByName( netSettingsName.c_str(), netSetting ) )
        {
            // setting exists.. check if user wants to change setting name
            if( QMessageBox::Yes != QMessageBox::question( this, QObject::tr( "Network Setting" ), 
                                                           QObject::tr( "Are you sure you want to overwrite existing network setting?.\n The network setting name can be changed in Select Network Setting." ),
                                                           QMessageBox::Yes | QMessageBox::No ) )
            {
                return;
            }
        }

        m_MyApp.getEngine().getEngineSettings().setNetworkKey( keyString );
        updateSettingsFromDlg();
        // need to apply settings also or what is used in ptop engine may not be what is shown in dialog which is confusing
        applySettingsToEngine();
        QMessageBox::information( this, QObject::tr( "Network Setting" ), QObject::tr( "Network setting was saved." ) );
    }
}

//============================================================================
void AppletNetworkSettings::slotDeleteLabelClick( void )
{
    ui.m_DeleteSettingsButton->emulateUserClicked();
}

//============================================================================
void AppletNetworkSettings::onDeleteButtonClick( void )
{
    updateSettingsFromDlg();
    if( 1 >= ui.m_NetworkSettingsNameComboBox->count() )
    {
        QMessageBox::warning( this, tr( "Network Settings" ), tr( "You cannot delete the last network setting." ) );
        return;
    }

    std::string netSettingsName;
    netSettingsName = ui.m_NetworkSettingsNameComboBox->currentText().toUtf8().constData();
    if( 0 != netSettingsName.length() )
    {
        m_MyApp.getAccountMgr().removeNetHostSettingByName( netSettingsName.c_str() );
        updateDlgFromSettings( false );
    }
}

//============================================================================
void AppletNetworkSettings::slotShowNetworkHostInformation()
{
    ActivityInformation * activityInfo = new ActivityInformation( m_MyApp, this, eInfoTypeNetworkHost );
    activityInfo->show();
}

//============================================================================
void AppletNetworkSettings::slotShowNetworkKeyInformation()
{
    ActivityInformation * activityInfo = new ActivityInformation( m_MyApp, this, eInfoTypeNetworkKey );
    activityInfo->show();
}

//============================================================================
void AppletNetworkSettings::slotShowConnectTestUrlInformation( void )
{
    ActivityInformation * activityInfo = new ActivityInformation( m_MyApp, this, eInfoTypeConnectTestUrl );
    activityInfo->show();
}

//============================================================================
void AppletNetworkSettings::slotShowConnectTestSettingsInformation( void )
{
    ActivityInformation * activityInfo = new ActivityInformation( m_MyApp, this, eInfoTypeConnectTestSettings );
    activityInfo->show();
}

//============================================================================
void AppletNetworkSettings::slotShowRandomConnectUrlInformation( void )
{
    ActivityInformation * activityInfo = new ActivityInformation( m_MyApp, this, eInfoTypeRandomConnectUrl );
    activityInfo->show();
}

//============================================================================
void AppletNetworkSettings::slotShowDefaultGroupHostUrlInformation( void )
{
    ActivityInformation * activityInfo = new ActivityInformation( m_MyApp, this, eInfoTypeDefaultGroupHostUrl );
    activityInfo->show();
}

//============================================================================
void AppletNetworkSettings::slotShowDefaultChatRoomUrlInformation( void )
{
    ActivityInformation * activityInfo = new ActivityInformation( m_MyApp, this, eInfoTypeDefaultChatRoomHostUrl );
    activityInfo->show();
}

//============================================================================
void AppletNetworkSettings::slotUpdateTimer( void )
{
    ui.m_InternetStateLabel->setText( DescribeInternetStatus( m_MyApp.getEngine().fromGuiGetInternetStatus() ) );
    ui.m_NetAvailStateLabel->setText( DescribeNetAvailStatus( m_MyApp.getEngine().fromGuiGetNetAvailStatus() ) );
    fillMyNodeUrl( ui.m_NodeUrlLabel );
}

//============================================================================
bool AppletNetworkSettings::verifyNetworkKey( QString& keyVal )
{
    bool isValid = true;
    if( keyVal.size() < 6 )
    {
        isValid = false;
        QMessageBox::warning( this, QObject::tr( "Network Key" ), QObject::tr( "Network Key must be at least 6 characters ( 8 or more characters recommended )." ) );
    }

    if( keyVal != m_OriginalNetworkKey && !m_OriginalNetworkKey.isEmpty() )
    {
        if( QMessageBox::Yes != QMessageBox::question( this, QObject::tr( "Network Key" ),
            QObject::tr( "Are you sure you want to change the network key?\n All users of your network will need to have the same key or cannot connect." ),
            QMessageBox::Yes | QMessageBox::No ) )
        {
            ui.m_NetworkKeyEdit->setText( m_OriginalNetworkKey );
            return false;
        }
    }

    return isValid;
}

//============================================================================
bool AppletNetworkSettings::verifyFirewallSettings( void )
{
    EFirewallTestType firewallTestType = getFirewallTestType();
    if( eFirewallTestAssumeNoFirewall == firewallTestType )
    {
        QString title = QObject::tr( "Invalid External IP Address" );
        std::string externIp = ui.m_ExternIpEdit->text().toUtf8().constData();
        if( externIp.empty() )
        {
            QMessageBox::warning( this, title, QObject::tr( "If assumed user can direct connect then external IP must be specified." ) );
            return false;
        }

        EIpAddrType addrType = VxGetIpAddrType( externIp.c_str() );
        
        if( eIpAddrTypeUnknown == addrType )
        {
            QMessageBox::warning( this, title, QObject::tr( "External IP is invalid." ) );
            return false;
        }

        bool ipv6 = ui.m_UseIpv6Network->isChecked();
        if( ipv6 )
        {
            if( eIpAddrTypeIpv4 == addrType )
            {
                QMessageBox::warning( this, title, QObject::tr( "When using IPv4 network a IPv4 external IP must be specified." ) );
                return false;
            }
        }
        else
        {
            if( eIpAddrTypeIpv6 == addrType )
            {
                QMessageBox::warning( this, title, QObject::tr( "When using IPv6 network a IPv6 external IP must be specified." ) );
                return false;
            }
        }
    }
    
    return true;
}

//============================================================================
void AppletNetworkSettings::slotCopyMyUrlToClipboard( void )
{
    QClipboard * clipboard = QApplication::clipboard();
    clipboard->setText( ui.m_NodeUrlLabel->text() );
}

//============================================================================
void AppletNetworkSettings::slotShowIpv6Information( void )
{
    ActivityInformation * activityInfo = new ActivityInformation( m_MyApp, this, eInfoTypeIpv6 );
    activityInfo->show();
}

//============================================================================
void AppletNetworkSettings::slotUseIpv6CheckBoxClick( void )
{

}

//============================================================================
bool AppletNetworkSettings::verifyIpv6Capable( void )
{
    std::string ipv6LclIp;
    if( VxGetDefaultLocalIp( true, ipv6LclIp ) && !ipv6LclIp.empty() )
    {
        return true;
    }

    QString title = QObject::tr( "Your device does not seem to be capable of IPv6" );
    QString bodyText = QObject::tr( "Your device does not seem to be capable of IPv6. Continue Anyway?" );

    ActivityYesNoMsgBox dlg( m_MyApp, &m_MyApp, title, bodyText );
    bool confirmed = (QDialog::Accepted == dlg.exec());
    if( !confirmed )
    {
        return false;
    }

    return true;
}