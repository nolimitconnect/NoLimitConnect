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

#include "AppletClientBase.h"
#include "GuiUserUpdateCallback.h"

#include <GuiInterface/IDefs.h>
#include <PluginSettings/PluginSetting.h>

#include <QString>
#include <QDialog>

#include "ui_AppletFriendListClient.h"

class VxNetIdent;
class GuiHostSession;

class AppletFriendListClient : public AppletClientBase
{
	Q_OBJECT
public:
	AppletFriendListClient(	AppCommon& app, QWidget* parent = nullptr );
	virtual ~AppletFriendListClient() override;

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

    void                        slotOfflineButtonClicked( void );
    void                        slotOfflineInfoButtonClicked( void );

    void                        onShowFriendTypeChanged( void );
    void                        onShowFriendList( void );
    void                        onShowIgnoreList( void );
    void                        onShowNearbyList( void );
    void                        onShowOfflineList( void );

protected:
    void						showEvent( QShowEvent* ev ) override;
    void						hideEvent( QHideEvent* ev ) override;

    void                        updateUser( EUserViewType listType, VxGUID& onlineId );
    void                        updateUser( GuiUser* guiUser ) override;

    void                        updateFriendList( EUserViewType listType, std::vector<std::pair<VxGUID, int64_t>> idList );

    //=== vars ===//
    Ui::AppletFriendListClientUi ui;
    EUserViewType             m_FriendListType{ eUserViewTypeFriendsOnline };
};
