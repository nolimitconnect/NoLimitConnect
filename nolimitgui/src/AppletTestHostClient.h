#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AppletTestBase.h"

#include "GuiUserUpdateCallback.h"
#include "GuiHostedListCallback.h"
#include "GuiGroupieListCallback.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class AppletTestHostClientUi;
}
QT_END_NAMESPACE

class QComboBox;

class AppletTestHostClient : public AppletTestBase, public GuiUserUpdateCallback, public GuiHostedListCallback, public GuiGroupieListCallback
{
	Q_OBJECT
public:
    AppletTestHostClient( AppCommon& app, QWidget* parent );
	virtual ~AppletTestHostClient();

    virtual void				callbackIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp ) override {};
    virtual void				callbackIndentListRemove( EUserViewType listType, VxGUID& onlineId ) override {};
    virtual void				callbackUserAdded( GuiUser* guiUser ) override;
    virtual void				callbackUserUpdated( GuiUser* guiUser ) override;
    virtual void				callbackUserRemoved( VxGUID& onlineId ) override {};

    virtual void				callbackGuiHostedListSearchResult( HostedId& hostedId, GuiHosted* guiHosted, VxGUID& sessionId ) override;

    virtual void				callbackGuiGroupieListSearchResult( GroupieId& groupieId, GuiGroupie* guiGroupie, VxGUID& sessionId ) override;

protected slots:
    void                        slotNetworkHostComboBoxSelectionChange( int comboIdx );
    void                        slotHostTypeComboBoxSelectionChange( int comboIdx );
    void                        slotHostListUrlSelectionChange( int comboIdx );

    void                        slotNetworkHostIdentityButtonClicked( void );
    void                        slotQueryHostListFromNetworkHostButtonClicked( void );
    void                        slotQueryGroupiesButtonClicked( void );

protected:
    void                        fillHostList( EHostType hostType, QComboBox* comboBox );
    void                        setNetworkHostIdFromNetHostComboBoxSelection( void );
    void                        updateHostType( EHostType hostType );
    bool                        updateHostedUrlComboBox( std::string& hostUrl );
    void                        updateHostedIdent( GuiHosted* guiHosted );

    Ui::AppletTestHostClientUi&  ui;
    VxGUID                      m_NetworkHostOnlineId;
};


