#pragma once
//============================================================================
// Copyright (C) 2019 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletBase.h"
#include <GuiInterface/IDefs.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class InformationUi;
}
QT_END_NAMESPACE

class AppletInformation : public AppletBase
{
	Q_OBJECT

public:
    AppletInformation( AppCommon& app, QWidget* parent, EInfoType infoType = eInfoTypeInvalid );
	virtual ~AppletInformation() override = default;

    void						setPluginType( EPluginType pluginType ) override { m_InfoType = eInfoTypePlugin; m_PluginType = pluginType; updateInformation(); }
    void						setInformationType( EInfoType infoType ) { m_InfoType = infoType; updateInformation(); }

    EPluginType                 getPluginType() override { return m_PluginType; }

protected slots:
	void						slotCopyToClipboardButtonClicked( void );

protected:
    virtual void				showEvent( QShowEvent* ev ) override;

    void						updateInformation( void );
    QString                     getInfoText( void );

	//=== vars ===//
	Ui::InformationUi&		    ui;
    EInfoType                   m_InfoType = eInfoTypeInvalid;
    EPluginType                 m_PluginType = ePluginTypeInvalid;

    static QString              m_NoInfoAvailable;
    static QString              m_NetworkDesign;
    static QString              m_PluginDefinitions;
    static QString              m_Permissions;
    static QString              m_NetworkKey;
    static QString              m_NetworkHost;
    static QString              m_ConnectTestUrl;
    static QString              m_ConnectTestSettings;
    static QString              m_RandomConnectUrl;
    static QString              m_DefaultGroupHostUrl;
    static QString              m_DefaultChatRoomHostUrl;
    static QString              m_NetworkSettingsInvite;

    static QString              m_FriendList;
    static QString              m_IgnoredList;
    static QString              m_OfflineList;

    static QString              m_UserHostRequrements;
    static QString              m_NetworkHostRequrements;

    static QString              m_MaxMessageHistory;
    static QString              m_Ipv6;

    static QString              m_FriendRequest;
    static QString              m_WhatIsAInvite;
};
