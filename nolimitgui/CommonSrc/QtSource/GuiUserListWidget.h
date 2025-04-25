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

#include "ListWidgetBase.h"
#include "AppDefs.h"
#include "GuiThumbCallback.h"
#include "GuiUserUpdateCallback.h"
#include "GuiOnlineStatusCallback.h"
#include "GuiMemberActiveCallback.h"
#include "PushToTalk/PushToTalkCallback.h"

#include <CoreLib/GroupieId.h>

class GuiUserMgr;
class GuiUserSessionBase;
class GuiUserListItem;
class PluginSetting;
class VxNetIdent;

class GuiUserListWidget : public ListWidgetBase, 
    public GuiUserUpdateCallback, 
    public GuiThumbCallback,
    public GuiOnlineStatusCallback,
    public GuiMemberActiveCallback
{
	Q_OBJECT

public:
	GuiUserListWidget( QWidget* parent );
    virtual ~GuiUserListWidget() override;

    void						setAppletType( EApplet appletType )         { m_AppletType = appletType; };
    EApplet						getAppletType( void )                       { return m_AppletType; };

    void                        setUserViewType( EUserViewType viewType );
    EUserViewType               getUserViewType( void )                     { return m_ViewType; };

    void                        setHostAdminId( GroupieId& adminId )        { m_HostAdminId = adminId; };
    GroupieId&                  getHostAdminId( void )                      { return m_HostAdminId; };

    void                        clearUserList( void );
    void                        disconnectUserUpdates( void );

    void                        addSessionToList( EHostType hostType, VxGUID& sessionId, GuiUser* userIdent );
    GuiUserListItem*            addOrUpdateSession( GuiUserSessionBase* userSession );

    GuiUserSessionBase*         findSession( VxGUID& lclSessionId );
    GuiUserListItem*            findListEntryWidgetBySessionId( VxGUID& sessionId );
    GuiUserListItem*            findListEntryWidgetByOnlineId( VxGUID& onlineId );

    void                        updateUser( VxGUID& onlineId );
    void                        updateUser( GuiUser* guiUser );
    void                        removeUser( VxGUID& onlineId );
    virtual GuiUserSessionBase* makeSession( GuiUser* guiUser );

    void                        searchTextChanged( QString& searchText );

    void                        callbackOnlineStatusChange( VxGUID& onlineId, bool isOnline ) override;
    void				        callbackGuiMemberActive( GroupieId& groupieId, bool isActive ) override;

signals:
    void                        signalUserAvatarClicked( GuiUser* guiUser );

    void                        signalUserListItemClicked( GuiUserSessionBase* userSession, GuiUserListItem* userItem );
    void                        signalAvatarButtonClicked( GuiUserSessionBase* userSession, GuiUserListItem* userItem );
    void                        signalFriendshipButtonClicked( GuiUserSessionBase* userSession, GuiUserListItem* userItem );
    void                        signalOfferViewButtonClicked( GuiUserSessionBase* userSession, GuiUserListItem* userItem );
    void                        signalOfferAcceptButtonClicked( GuiUserSessionBase* userSession, GuiUserListItem* userItem );
    void                        signalOfferRejectButtonClicked( GuiUserSessionBase* userSession, GuiUserListItem* userItem );
    void                        signalPushToTalkButtonClicked( GuiUserSessionBase* userSession, GuiUserListItem* userItem );
    void                        signalMenuButtonClicked( GuiUserSessionBase* userSession, GuiUserListItem* userItem ); 

protected slots:    
	void						slotItemClicked( QListWidgetItem* item );

    void                        slotAvatarButtonClicked( GuiUserListItem* userItem );
    void                        slotFriendshipButtonClicked( GuiUserListItem* userItem );
    void                        slotOfferViewButtonClicked( GuiUserListItem* userItem );
    void                        slotOfferAcceptButtonClicked( GuiUserListItem* userItem );
    void                        slotOfferRejectButtonClicked( GuiUserListItem* userItem );
    void                        slotPushToTalkButtonClicked( GuiUserListItem* userItem );
    void                        slotMenuButtonClicked( GuiUserListItem* userItem ); 

protected:
    void				        callbackThumbAdded( GuiThumb* guiThumb ) override;
    void                        callbackThumbUpdated( GuiThumb* guiThumb ) override;
    void				        callbackThumbRemoved( VxGUID& thumbId ) override;

    void                        callbackMyIdentUpdated( GuiUser* guiUser ) override;
    void				        callbackUserAdded( GuiUser* guiUser ) override;
    void				        callbackUserRemoved( VxGUID& onlineId ) override;
    void                        callbackUserUpdated( GuiUser* guiUser ) override;
    void                        callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline ) override;

    bool                        isMemberView( void );
    bool                        isListViewMatch( GuiUser* guiUser );

    GuiUserListItem*            sessionToWidget( GuiUserSessionBase* userSession );
    GuiUserSessionBase*			widgetToSession( GuiUserListItem* userItem );

    virtual void                onUserListItemClicked( GuiUserListItem* userItem );
    virtual void                onAvatarButtonClicked( GuiUserListItem* userItem );
    virtual void                onFriendshipButtonClicked( GuiUserListItem* userItem );
    virtual void                onOfferViewButtonClicked( GuiUserListItem* userItem );
    virtual void                onOfferAcceptButtonClicked( GuiUserListItem* userItem );
    virtual void                onOfferRejectButtonClicked( GuiUserListItem* userItem );
    virtual void                onPushToTalkButtonClicked( GuiUserListItem* userItem );
    virtual void                onMenuButtonClicked( GuiUserListItem* userItem );

    virtual void                onListItemAdded( GuiUserSessionBase* userSession, GuiUserListItem* userItem );
    virtual void                onListItemUpdated( GuiUserSessionBase* userSession, GuiUserListItem* userItem );

    void                        refreshUserList( void );
    void                        updateEntryWidget( VxGUID& onlineId );

    void                        updateThumb( GuiThumb* guiThumb );

	//=== vars ===//
    EApplet						m_AppletType{ eAppletUnknown };
    EUserViewType               m_ViewType{ eUserViewTypeNone };
    GroupieId                   m_HostAdminId;

    std::map<VxGUID, GuiUserSessionBase*> m_UserCache;

    bool                        m_AllowMyselfInList{ false };
};

