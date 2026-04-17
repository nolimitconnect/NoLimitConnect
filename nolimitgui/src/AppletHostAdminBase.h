#pragma once
//============================================================================
// Copyright (C) 2026 Brett R. Jones
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

class AppletHostAdminBase : public AppletBase, public InputClientBaseCallback
{
	Q_OBJECT
public:
    AppletHostAdminBase( const char* ObjName, AppCommon& app, QWidget* parent );
	virtual ~AppletHostAdminBase() override;

    AppCommon&                  getMyApp( void ) override { return m_MyApp; }
    EPluginType			        getInputClientPluginType( void ) override { return ePluginTypeClientGroup; }

protected:
protected slots:
    void                        slotSetMembersVisible( bool visible );
    void                        slotSetSessionVisible( bool visible );

    void                        slotUserSelected( GuiUser* guiUser );

protected:
    bool						checkIfCanSend( void ) override;
    bool						handleAssetAction( EAssetAction assetAction, AssetBaseInfo& assetInfo ) override;

    //=== vars ===//
    Ui::AppletHostClientUi&     ui;
    GuiUser*                    m_SelectedUser{nullptr};

};


