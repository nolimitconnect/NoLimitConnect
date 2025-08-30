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

#include "AppletClientBase.h"
#include "InputClientBaseCallback.h"

#include <GuiInterface/IDefs.h>
#include <GuiInterface/IToGui.h> 

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletRandomConnectClientUi;
}
QT_END_NAMESPACE

class AppletRandomConnectClient : public AppletClientBase, public InputClientBaseCallback
{
	Q_OBJECT
public:
	AppletRandomConnectClient( AppCommon& app, QWidget* parent );
	virtual ~AppletRandomConnectClient() override;

    AppCommon&                  getMyApp( void ) override { return m_MyApp; }
    EPluginType			        getInputClientPluginType( void ) override { return AppletClientBase::getPluginType(); }

    void				        userJoinedHost( GuiHosted* guiHosted ) override;

protected slots:
    void                        slotSetSessionVisible( bool makeVisible );

    void						slotViewChanged( EUserViewType viewType );

protected:
    void                        showEvent( QShowEvent* ev ) override;

    bool						checkIfCanSend( void ) override;
    bool						handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo ) override;

    //=== vars ===//
    Ui::AppletRandomConnectClientUi&	ui;
};


