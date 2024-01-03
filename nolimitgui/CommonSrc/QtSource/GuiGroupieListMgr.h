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

#include "GuiGroupie.h"

#include <GroupieListMgr/GroupieListCallbackInterface.h>
#include <GroupieListMgr/GroupieInfo.h>

#include <CoreLib/VxMutex.h>

#include <set>

#include <QObject>

class AppCommon;
class VxNetIdent;
class GuiGroupieListCallback;

class GuiGroupieListMgr : public QObject, public GroupieListCallbackInterface
{
    Q_OBJECT
public:
    GuiGroupieListMgr() = delete;
    GuiGroupieListMgr( AppCommon& app );
    GuiGroupieListMgr( const GuiGroupieListMgr& rhs ) = delete;
	virtual ~GuiGroupieListMgr() = default;
    void                        onAppCommonCreated( void );
    void                        onMessengerReady( bool ready ) { }
    bool                        isMessengerReady( void );
    virtual void                onSystemReady( bool ready ) { }

    void                        toGuiGroupieSearchResult( EHostType hostType, VxGUID& sessionId, GroupieInfo& hostedInfo );
    void                        toGuiGroupieSearchComplete( EHostType hostType, VxGUID& sessionId );

    bool                        isGroupieInSession( GroupieId& groupieId );
    void                        setGroupieOffline( GroupieId& groupieId );

    void                        onGroupieAdded( GuiGroupie* guiGroupie );
    void                        onGroupieRemoved( VxGUID& groupieOnlineId, VxGUID& hostOnlineId, EHostType hostType );
    void                        onGroupieUpdated( GuiGroupie* guiGroupie );
    void                        onUserOnlineStatusChange( GuiGroupie* user, bool isOnline );
    void                        onMyIdentUpdated( GuiGroupie* guiGroupie );

    GuiGroupie*                 getGroupie( VxGUID& groupieOnlineId, VxGUID& hostOnlineId, EHostType hostType )   { return findGroupie( groupieOnlineId, hostOnlineId, hostType ); }
    GuiGroupie*                 getGroupie( GroupieId& groupieId )                    { return findGroupie( groupieId ); }
    std::map<GroupieId, GuiGroupie*>& getGroupieList( void )                            { return m_GroupieList; }
    GuiGroupie*                 updateGroupie( VxNetIdent* hisIdent, EHostType hostType );
    GuiGroupie*                 updateGroupie( GroupieId& groupieId, GuiUser* guiUser, VxGUID& sessionId );

    void                        wantGroupieListCallbacks( GuiGroupieListCallback* client, bool enable );

    void                        getHostedMembers( EHostType hostType, std::set<VxGUID> memberList );

signals:
    void				        signalMyIdentUpdated( GuiGroupie* guiGroupie );

    void				        signalGroupieRequested( GuiGroupie* guiGroupie ); 
    void                        signalGroupieUpdated( GuiGroupie* guiGroupie );
    void				        signalGroupieRemoved( VxGUID& groupieOnlineId, VxGUID& hostOnlineId, EHostType hostType );
    void                        signalGroupieOfferStateChange( VxGUID& userOnlineId, EHostType hostType, EJoinState hostOfferState );
    void                        signalGroupieOnlineStatus( GuiGroupie* guiGroupie, bool isOnline );

    void                        signalInternalGroupieUpdated( GroupieInfo* hostedInfo );
    void                        signalInternalGroupieRemoved( VxGUID groupieOnlineId, VxGUID hostOnlineId, EHostType hostType );
    void                        signalInternalGroupieSearchResult( GroupieInfo* hostedInfo, VxGUID sessionId );
    void                        signalInternalGroupieSearchComplete( EHostType hostType, VxGUID sessionId );

private slots:
    void                        slotInternalGroupieUpdated( GroupieInfo* hostedInfo );
    void                        slotInternalGroupieRemoved( VxGUID groupieOnlineId, VxGUID hostOnlineId, EHostType hostType );
    void                        slotInternalGroupieSearchResult( GroupieInfo* hostedInfo, VxGUID sessionId );
    void                        slotInternalGroupieSearchComplete( EHostType hostType, VxGUID sessionId );

protected:
    void                        removeGroupie( VxGUID& groupieOnlineId, VxGUID& hostOnlineId, EHostType hostType );
    GuiGroupie*                 findGroupie( VxGUID& groupieOnlineId, VxGUID& hostOnlineId, EHostType hostType );
    GuiGroupie*                 findGroupie( GroupieId& hostTypeId );
    GuiGroupie*                 updateGroupieInfo( GroupieInfo& hostedInfo );
    void                        updateHostSearchResult( GroupieInfo& hostedInfo, VxGUID& sessionId );
    
    virtual void				callbackGroupieInfoListUpdated( GroupieInfo* hostedInfo ) override;
    virtual void				callbackGroupieInfoListRemoved( VxGUID& groupieOnlineId, VxGUID& hostOnlineId, EHostType hostType ) override;
    virtual void				callbackGroupieInfoListSearchResult( GroupieInfo* hostedInfo, VxGUID& hostOnlineId ) override;

    void                        announceGroupieListUpdated( GroupieId& groupieId, GuiGroupie* guiGroupie );
    void                        announceGroupieListRemoved( GroupieId& groupieId );
    void                        announceGroupieListSearchResult( GroupieId& groupieId, GuiGroupie* guiGroupie, VxGUID& sessionId );
    void                        announceGroupieListSearchComplete( EHostType hostType, VxGUID& sessionId );

    AppCommon&                  m_MyApp;
    // map of online id to GuiGroupie
    std::map<GroupieId, GuiGroupie*>  m_GroupieList;

    std::vector<GuiGroupieListCallback*>  m_GroupieListClients;
};
