#pragma once
//============================================================================
// Copyright (C) 2020 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"

#include <CoreLib/VxPtopUrl.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class InviteUrlWidgetUi;
}
QT_END_NAMESPACE

class AppCommon;
class QCheckBox;
class QFrame;
class QLineEdit;

class InviteUrlWidget : public QWidget
{
    Q_OBJECT
public:
    InviteUrlWidget( QWidget* parent );
    virtual ~InviteUrlWidget() = default;

    void                        setupInvite( bool createInvite ); 
    bool                        setHostInviteType( EHostType hostType ); // limit to specific host type

    void                        setInviteText( std::string inviteText ); // when accepting invite only
    std::string                 getInviteText( void );

    void                        setInviteUserMsg( std::string userMsg ) { m_UserMsg = userMsg; slotUpdateInvite(); }

signals:
    void                        signalInviteChanged( void );

protected slots:
    void                        slotNetworkSettingsInfoButtonClicked( void );
    void                        slotUpdateInvite( void );

    void                        slotChatRoomButtonClicked( void );
    void                        slotGroupButtonClicked( void );
    void                        slotRandomConnectButtonClicked( void );
    void                        slotNetworkButtonClicked( void );

    void                        slotPersonalCheckBoxClicked( int checked );
    void                        slotChatRoomCheckBoxClicked( int checked );
    void                        slotGroupCheckBoxClicked( int checked );
    void                        slotRandomConnectCheckBoxClicked( int checked );
    void                        slotNetworkCheckBoxClicked( int checked );

protected:
    int                         testAvailableUrls( void );
    void                        updateUrls( void );
    void                        updateInviteText( void );
    void                        parseInviteText( std::string inviteText );

    bool                        populateNetSettingUrl( EHostType hostType, std::string& ptopUrl );

    std::string                 getHostedUrl( EHostType hostType );

    QCheckBox*                  getUrlCheckBox( EHostType hostType );
    QLineEdit*                  getUrlEdit( EHostType hostType );
    QFrame*                     getUrlFrame( EHostType hostType );

    std::string                 generateSelectedInviteText( void );
    std::string                 getNetworkKeyState( void );

    void                        updateAcceptUrls( void );

    void                        testUrl( EHostType hostType );

    bool                        checkPortOpen( EHostType hostType );
    bool                        checkHostEnabled( EHostType hostType );
    
    //=== vars ===//
    Ui::InviteUrlWidgetUi&      ui;
    AppCommon&					m_MyApp;
    bool                        m_IsCreateInvite{ false };
    bool                        m_IsSettingUrls{ false };
    bool                        m_HasPrivateNetworkKey{ false };
    std::string                 m_PastedInviteText;

    std::vector<EHostType>      m_HostTestList;
    std::vector<EHostType>      m_HostValidList;
    std::string                 m_MyUrl;
    std::string                 m_UserMsg;

    std::vector<VxPtopUrl>      m_HostUrls;
    std::vector<VxPtopUrl>      m_NetworkUrls;
};



