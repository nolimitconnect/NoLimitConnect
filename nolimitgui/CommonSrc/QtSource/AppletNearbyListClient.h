#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <GuiInterface/IDefs.h>
#if ENABLE_COMPONENT_NEARBY

#include "AppletClientBase.h"
#include "GuiUserUpdateCallback.h"

#include <PluginSettings/PluginSetting.h>

#include <QString>
#include <QDialog>

#include "ui_AppletNearbyListClient.h"

class VxNetIdent;
class GuiHostSession;

class AppletNearbyListClient : public AppletClientBase
{
	Q_OBJECT
public:
	AppletNearbyListClient(	AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletNearbyListClient() override;

    void                        infoMsg( const char* infoMsg, ... );
    void                        toGuiInfoMsg( char * logMsg );

    void						setStatusLabel( QString strMsg );

    EUserViewType               getListType( void )             { return m_FriendListType; }
    void                        clearList( void );
    void                        clearStatus( void );

signals:
    void						signalSearchResult( VxNetIdent* netIdent );
    void						signalSearchComplete( void );
    void                        signalLogMsg( const QString& logMsg );
    void                        signalInfoMsg( const QString& logMsg );

private slots:

    void                        slotFriendsButtonClicked( void );
    void                        slotFriendsInfoButtonClicked( void );
    void                        slotIgnoredButtonClicked( void );
    void                        slotIgnoredInfoButtonClicked( void );
    void                        slotNearbyButtonClicked( void );
    void                        slotNearbyInfoButtonClicked( void );

    void                        onShowFriendTypeChanged( void );
    void                        onShowFriendList( void );
    void                        onShowIgnoreList( void );
    void                        onShowNearbyList( void );

protected:
    virtual void				callbackIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp ) override;
    virtual void				callbackIndentListRemove( EUserViewType listType, VxGUID& onlineId ) override;

    virtual void				callbackUserAdded( GuiUser* guiUser ) override;
    virtual void				callbackUserUpdated( GuiUser* guiUser ) override;
    virtual void				callbackUserRemoved( VxGUID& onlineId ) override;
    virtual void				callbackPushToTalkStatus( VxGUID& onlineId, EPushToTalkStatus pushToTalkStatus ) override;

    void						showEvent( QShowEvent* ev ) override;
    void						hideEvent( QHideEvent* ev ) override;

    void                        updateUser( EUserViewType listType, VxGUID& onlineId );
    void                        updateUser( GuiUser* guiUser ) override;
    void                        removeUser( VxGUID& onlineId ) override;

    void                        updateFriendList( EUserViewType listType, std::vector<std::pair<VxGUID, int64_t>> idList );

    //=== vars ===//
    Ui::AppletNearbyListClientUi ui;
    EUserViewType             m_FriendListType{ eUserViewTypeFriends };
};

#endif // ENABLE_COMPONENT_NEARBY
