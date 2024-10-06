#pragma once
//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"
#include "AppDefs.h"

#include <NetLib/NetHostSetting.h>

#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletNetworkSettingsUi;
}
QT_END_NAMESPACE

class AppletNetworkSettings : public AppletBase
{
	Q_OBJECT
public:
	AppletNetworkSettings( AppCommon& app, QWidget* parent );
	virtual ~AppletNetworkSettings();

protected slots:
    void						slotExitButtonClick( void );
    void						slotGoToNetHostSettingsButtonClick( void );
    void						slotRandomPortButtonClick( void );
    void						slotTestIsMyPortOpenButtonClick( void );
    void						slotTestUpnpButtonClick( void );

    void						slotAutoDetectProxyClick( void );
    void						slotNoProxyClick( void );
    void						slotYesProxyClick( void );
    void						slotUseUpnpCheckBoxClick( void );

    void						onSaveButtonClick( void );
    void						slotSaveLabelClick( void );

    void						onDeleteButtonClick( void );
    void						slotDeleteLabelClick( void );

    void						onComboBoxSelectionChange( int );

    void                        slotShowNetworkHostInformation( void );
    void                        slotShowNetworkKeyInformation( void );
    void                        slotShowConnectTestUrlInformation( void );
    void                        slotShowConnectTestSettingsInformation( void );
    void						slotShowRandomConnectUrlInformation( void );
    void						slotShowDefaultGroupHostUrlInformation( void );
    void						slotShowDefaultChatRoomUrlInformation( void );

    void						slotCopyMyUrlToClipboard( void );

    void						slotUpdateTimer( void );

    void						slotShowIpv6Information( void );
    void						slotUseIpv6CheckBoxClick( void );

protected:
    void						closeEvent( QCloseEvent *event );

    void						connectSignals( void );

    QString						getNetworkKey( void );
    bool                        verifyNetworkKey( QString& keyVal );

    void						updateDlgFromSettings( bool initialSettings );
    void						updateSettingsFromDlg( void );
    void						populateDlgFromNetHostSetting( NetHostSetting& netSettings );
    void						setFirewallTest( EFirewallTestType eFirewallType );
    void                        populateNetHostSettingsFromDlg( NetHostSetting& netHostSetting );

    void						applySettingsToEngine( void );
    void                        applyEngineSettingsFromHostSetting( NetHostSetting& netHostSetting );

    void                        fillNetHostSettingFromEngine( NetHostSetting& netSettings );

	Ui::AppletNetworkSettingsUi&	ui;
    NetHostSetting              m_OriginalSettings;
    QTimer *                    m_UpdateTimer{ nullptr };
    QString                     m_OriginalNetworkKey;

    bool                        m_OrigLogPortForward{ false };
};


