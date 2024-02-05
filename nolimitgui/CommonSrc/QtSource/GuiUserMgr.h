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

#include "GuiUser.h"
#include "GuiUserUpdateCallback.h"
#include "ToGuiUserUpdateInterface.h"

#include <CoreLib/VxMutex.h>

#include <QObject>

class AppCommon;
class GuiUserUpdateCallback;

class GuiUserMgr : public QObject, public ToGuiUserUpdateInterface
{
    Q_OBJECT
public:
    GuiUserMgr() = delete;
    GuiUserMgr( AppCommon& app );
    GuiUserMgr( const GuiUserMgr& rhs ) = delete;
	virtual ~GuiUserMgr() = default;
    virtual void                onAppCommonCreated( void );
    virtual void                onMessengerReady( bool ready ) { }
    virtual bool                isMessengerReady( void );
    virtual void                onSystemReady( bool ready ) { }

    GuiUser*                    getMyIdent( void );
    VxGUID                      getMyOnlineId( void )                       { return m_MyOnlineId; }  
    std::string                 getMyOnlineName( void );

    virtual void				toGuiIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp ) override;
    virtual void				toGuiIndentListRemove( EUserViewType listType, VxGUID& onlineId ) override;

    virtual void				toGuiContactAdded( VxNetIdent* netIdent ) override; 
    virtual void				toGuiContactRemoved( VxGUID& onlineId ) override;  

    virtual void				toGuiContactUpdated( VxNetIdent* netIdent ) override; 

    bool				        toGuiOnlineStatusChange( VxGUID& onlineId, bool isOnline );
    bool                        updateIsOnlineList( VxGUID& onlineId, bool isOnline );

    virtual void				toGuiContactLastSessionTimeChange( VxNetIdent* netIdent ) override; 

    virtual void				toGuiUpdateMyIdent( VxNetIdent* netIdent ) override;
    virtual void				toGuiSaveMyIdent( VxNetIdent* netIdent ) override; 

    bool                        isUserRelayed( VxGUID& onlineId );
    bool                        isUserOnline( VxGUID& onlineId );
    bool                        isUserInSession( VxGUID& onlineId );

    void                        onUserAdded( GuiUser* guiUser );
    void                        onUserRemoved( VxGUID& onlineId );
    void                        onUserUpdated( GuiUser* guiUser );
    void                        onUserNearbyStatusChange( GuiUser* guiUser );
    void                        onUserRelayStatusChange( GuiUser* guiUser );
    void                        onUserOnlineStatusChange( GuiUser* guiUser );
    void                        onMyIdentUpdated( GuiUser* guiUser );
    
    GuiUser*                    getUser( VxGUID onlineId, bool queryFromEngineIfMissing = false );
    GuiUser*                    getUserForTest( bool mustBeOnline = false );
    GuiUser*                    getOrQueryUser( VxGUID& onlineId );
    std::string                 getUserOnlineName( VxGUID& onlineId );
    
    std::map<VxGUID, GuiUser*>& getUserList( void )             { return m_UserList; }
    bool                        getOfflineUsers( std::vector<std::pair<VxGUID, int64_t>>& idList );

    void                        updateMyIdent( VxNetIdent* myIdent );
    GuiUser*                    updateUser( VxNetIdent* hisIdent );
    void                        updateOnlineStatus( VxNetIdent* netIdent, bool online );

    void                        wantGuiUserUpdateCallbacks( GuiUserUpdateCallback* callback, bool wantCallback );

    void                        connnectIdNearbyStatusChange( VxGUID& onlineId, uint64_t nearbyTimeOrZeroIfNot );

    void                        dumpUserInfo( GuiUser* guiUser, QWidget* parentFrame );
    void                        deleteUser( GuiUser* guiUser, QWidget* parentFrame );

protected:
    void                        removeUser( VxGUID onlineId );
    GuiUser*                    findUser( VxGUID onlineId );
    GuiUser*                    findOrAddUser( VxGUID onlineId );

    void                        clearGuiUserUpdateClientList( void );

    void                        sendUserUpdatedToCallbacks( GuiUser* guiUser );

    void                        updateClientList( void );

    bool                        isClientQueuedForRemoval( GuiUserUpdateCallback* client );
    
    AppCommon&                  m_MyApp;
    std::vector<GuiUserUpdateCallback*> m_GuiUserUpdateClientList;
    std::vector<std::pair<GuiUserUpdateCallback*, bool>> m_WantUpdateToDoList;
    bool                        m_ClientListBusy{ false };
    bool                        m_UpdatingClientList{ false };

    // map of online id to GuiUser
    std::map<VxGUID, GuiUser*>  m_UserList;
    VxGUID                      m_MyOnlineId;

    std::set<VxGUID>            m_OnlineUsers;

    VxNetIdent                  m_RawMyIdent;
    GuiUser*                    m_MyIdent{ nullptr };
};
