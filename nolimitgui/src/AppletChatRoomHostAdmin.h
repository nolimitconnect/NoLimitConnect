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
#include "InputClientBaseCallback.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletChatRoomClientUi;
}
QT_END_NAMESPACE

class AppletChatRoomHostAdmin : public AppletBase, public InputClientBaseCallback
{
	Q_OBJECT
public:
    AppletChatRoomHostAdmin( AppCommon& app, QWidget* parent );
	virtual ~AppletChatRoomHostAdmin() override;

    AppCommon&                  getMyApp( void ) override { return m_MyApp; }
    EPluginType			        getInputClientPluginType( void ) override { return ePluginTypeClientChatRoom; }

protected:
    bool						checkIfCanSend( void ) override;
    bool						handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo ) override;

    //=== vars ===//
    Ui::AppletChatRoomClientUi& ui;
};


