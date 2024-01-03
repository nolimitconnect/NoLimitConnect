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

#include "ui_AppletChatRoomClient.h"

#include "AppletBase.h"
#include "InputClientBaseCallback.h"

class AppletRandomConnectHostAdmin : public AppletBase, public InputClientBaseCallback
{
	Q_OBJECT
public:
    AppletRandomConnectHostAdmin( AppCommon& app, QWidget* parent );
	virtual ~AppletRandomConnectHostAdmin() override;

    AppCommon&                  getMyApp( void ) override { return m_MyApp; }
    EPluginType			        getInputClientPluginType( void ) override { return ePluginTypeClientRandomConnect; }

protected:
    bool						checkIfCanSend( void ) override;
    bool						handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo ) override;

    //=== vars ===//
    Ui::AppletChatRoomClientUi ui;
};


