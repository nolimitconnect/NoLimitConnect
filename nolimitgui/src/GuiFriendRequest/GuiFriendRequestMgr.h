#pragma once
//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiFriendRequest.h"

#include <FriendRequestMgr/FriendRequestCallbackInterface.h>
#include <FriendRequestMgr/FriendRequestInfo.h>

#include <CoreLib/VxMutex.h>

#include <set>

#include <QObject>

class AppCommon;
class VxNetIdent;
class GuiFriendRequestListCallback;

class GuiFriendRequestMgr : public QObject, public FriendRequestCallbackInterface
{
    Q_OBJECT
public:
    GuiFriendRequestMgr() = delete;
    GuiFriendRequestMgr( AppCommon& app );
    GuiFriendRequestMgr( const GuiFriendRequestMgr& rhs ) = delete;
	virtual ~GuiFriendRequestMgr() = default;

    void                        onAppCommonCreated( void );

    void                        wantFriendRequestListCallbacks( GuiFriendRequestListCallback* client, bool enable );
    std::vector<GuiFriendRequest*>&  getRequestList( void ) { return m_FriendRequestList; }
    size_t                      getRequestCount( void ) { return m_FriendRequestList.size(); }

    void                        friendAccepted( GuiFriendRequest* friendRequest );
    void                        friendRejected( GuiFriendRequest* friendRequest );

signals:
    void                        signalInternalFriendRequestUpdated( std::shared_ptr<FriendRequestInfo> friendRequest );
    void                        signalInternalFriendRequestRemoved( VxGUID friendOnlineId, VxGUID requestId );

private slots:
    void                        slotInternalFriendRequestUpdated( std::shared_ptr<FriendRequestInfo> friendRequest );
    void                        slotInternalFriendRequestRemoved( VxGUID friendOnlineId, VxGUID requestId );

protected:  
    void				        callbackFriendRequestUpdated( std::shared_ptr<FriendRequestInfo>& friendRequest ) override;
    void				        callbackFriendRequestRemoved( VxGUID& friendOnlineId, VxGUID& requestId ) override;

    void                        announceFriendRequestListUpdated( GuiFriendRequest* guiFriendRequest );
    void                        announceFriendRequestListRemoved( VxGUID& requestId );

    void                        removeFriendRequest( VxGUID& requestId );

    AppCommon&                  m_MyApp;
    // map of online id to GuiFriendRequest
    std::vector<GuiFriendRequest*>  m_FriendRequestList;

    std::vector<GuiFriendRequestListCallback*>  m_FriendRequestListClients;
};
