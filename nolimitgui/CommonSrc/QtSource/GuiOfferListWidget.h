#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones 
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
#include "GuiOfferCallback.h"
#include "GuiUserUpdateCallback.h"

class GuiOfferMgr;
class GuiOfferSession;
class GuiOfferListItem;
class PluginSetting;
class VxNetIdent;

class GuiOfferListWidget : public ListWidgetBase, public GuiUserUpdateCallback, public GuiThumbCallback, public GuiOfferCallback
{
	Q_OBJECT

public:
	GuiOfferListWidget( QWidget* parent );
    virtual ~GuiOfferListWidget();

    void						setAppletType( EApplet appletType )     { m_AppletType = appletType; };
    EApplet						getAppletType( void )                   { return m_AppletType; };

    void                        clearOfferList( void );

    GuiOfferListItem*           addOrUpdateSession( GuiOfferSession* userSession );

    GuiOfferSession*            findSession( VxGUID& lclSessionId );
    GuiOfferListItem*           findListEntryWidgetBySessionId( VxGUID& sessionId );
    GuiOfferListItem*           findListEntryWidgetByOnlineId( VxGUID& onlineId );
    GuiOfferListItem*           findListEntryWidgetByUniqueId( VxGUID& uniqueId );

    void                        updateUser( GuiUser* guiUser );
    void                        removeSession( VxGUID& uniqueId );

signals:
    void                        signalUserAvatarClicked( GuiUser* guiUser );

    void                        signalOfferListItemClicked( GuiOfferSession* userSession, GuiOfferListItem* userItem );

    void                        signalAvatarButtonClicked( GuiOfferSession* userSession, GuiOfferListItem* userItem );
    void                        signalFriendshipButtonClicked( GuiOfferSession* userSession, GuiOfferListItem* userItem );
    void                        signalOfferViewButtonClicked( GuiOfferSession* userSession, GuiOfferListItem* userItem );
    void                        signalOfferAcceptButtonClicked( GuiOfferSession* userSession, GuiOfferListItem* userItem );
    void                        signalOfferRejectButtonClicked( GuiOfferSession* userSession, GuiOfferListItem* userItem );
    void                        signalPushToTalkButtonClicked( GuiOfferSession* userSession, GuiOfferListItem* userItem );
    void                        signalMenuButtonClicked( GuiOfferSession* userSession, GuiOfferListItem* userItem );

protected slots:    
	void						slotItemClicked( QListWidgetItem* item );
    void                        slotUserListItemClicked( GuiOfferListItem* userItem );
    void                        slotAvatarButtonClicked( GuiOfferListItem* userItem );
    void                        slotFriendshipButtonClicked( GuiOfferListItem* userItem );
    void                        slotOfferViewButtonClicked( GuiOfferListItem* userItem );
    void                        slotOfferAcceptButtonClicked( GuiOfferListItem* userItem );
    void                        slotOfferRejectButtonClicked( GuiOfferListItem* userItem );
    void                        slotPushToTalkButtonClicked( GuiOfferListItem* userItem );
    void                        slotMenuButtonClicked( GuiOfferListItem* userItem ); 

protected:
    void				        callbackThumbAdded( GuiThumb* guiThumb ) override;
    void                        callbackThumbUpdated( GuiThumb* guiThumb ) override;
    void				        callbackThumbRemoved( VxGUID& thumbId ) override;

    void                        callbackMyIdentUpdated( GuiUser* guiUser ) override;
    void				        callbackUserAdded( GuiUser* guiUser ) override;
    void				        callbackUserRemoved( VxGUID& onlineId ) override;
    void                        callbackUserUpdated( GuiUser* guiUser ) override;
    void                        callbackOnlineStatusChange( GuiUser* guiUser, bool isOnline ) override;

    void				        callbackToGuiRxedOfferStateChange( std::shared_ptr<GuiOfferSession>& offerSession, EOfferState oldOfferState, EOfferState newOfferState ) override;

    GuiOfferListItem*           sessionToWidget( GuiOfferSession* userSession );
    GuiOfferSession*			widgetToSession( GuiOfferListItem* userItem );

    virtual void                onOfferListItemClicked( GuiOfferListItem* userItem );
    virtual void                onAvatarButtonClicked( GuiOfferListItem* userItem );
    virtual void                onFriendshipButtonClicked( GuiOfferListItem* userItem );
    virtual void                onOfferViewButtonClicked( GuiOfferListItem* userItem );
    virtual void                onOfferAcceptButtonClicked( GuiOfferListItem* userItem );
    virtual void                onOfferRejectButtonClicked( GuiOfferListItem* userItem );
    virtual void                onPushToTalkButtonClicked( GuiOfferListItem* userItem );
    virtual void                onMenuButtonClicked( GuiOfferListItem* userItem );

    virtual void                onListItemAdded( GuiOfferSession* userSession, GuiOfferListItem* userItem );
    virtual void                onListItemUpdated( GuiOfferSession* userSession, GuiOfferListItem* userItem );

    void                        updateEntryWidget( VxGUID& onlineId );

    void                        updateThumb( GuiThumb* guiThumb );

	//=== vars ===//
    EApplet						m_AppletType{ eAppletUnknown };
    EUserViewType               m_ViewType{ eUserViewTypeNone };
};

