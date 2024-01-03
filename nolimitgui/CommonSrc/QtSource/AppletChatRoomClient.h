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

#include "AppletClientBase.h"
#include "InputClientBaseCallback.h"

class AppletChatRoomClient : public AppletClientBase, public InputClientBaseCallback
{
	Q_OBJECT
public:
    AppletChatRoomClient( AppCommon& app, QWidget* parent );
	virtual ~AppletChatRoomClient() override;

    AppCommon&                  getMyApp( void ) override { return m_MyApp; }
    EPluginType			        getInputClientPluginType( void ) override { return AppletBase::getPluginType(); }

    void				        userJoinedHost( GuiHosted* guiHosted ) override;

protected slots:
    void                        slotSetSessionVisible( bool makeVisible );

    void						slotViewChanged( EUserViewType viewType );

protected:
    void                        showEvent( QShowEvent* ev ) override;

    bool						checkIfCanSend( void ) override;
    bool						handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo ) override;

    //=== vars ===//
    Ui::AppletChatRoomClientUi	ui;
};


