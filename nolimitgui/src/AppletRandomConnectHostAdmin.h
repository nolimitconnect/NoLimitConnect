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
    class AppletHostClientUi;
}
QT_END_NAMESPACE

class AppletRandomConnectHostAdmin : public AppletBase, public InputClientBaseCallback
{
	Q_OBJECT
public:
    AppletRandomConnectHostAdmin( AppCommon& app, QWidget* parent );
	virtual ~AppletRandomConnectHostAdmin() override;

    AppCommon&                  getMyApp( void ) override { return m_MyApp; }
    EPluginType			        getInputClientPluginType( void ) override { return ePluginTypeClientRandomConnect; }

protected:
protected slots:
    void                        slotSetMembersVisible( bool visible );
    void                        slotSetSessionVisible( bool visible );

protected:
    bool						checkIfCanSend( void ) override;
    bool						handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo ) override;

    //=== vars ===//
    Ui::AppletHostClientUi&     ui;
};


