//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiFriendRequestMgr.h"

#include "GuiFriendRequest.h"
#include "GuiFriendRequestCallback.h"
#include "AppCommon.h"

#include <FriendRequestMgr/FriendRequestInfo.h>
#include <FriendRequestMgr/FriendRequestMgr.h>
#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>
#include <PktLib/VxCommon.h>

//============================================================================
GuiFriendRequestMgr::GuiFriendRequestMgr( AppCommon& app )
    : QObject( &app )
    , m_MyApp( app )
{
}

//============================================================================
void GuiFriendRequestMgr::onAppCommonCreated( void )
{
    connect(this, &GuiFriendRequestMgr::signalInternalFriendRequestUpdated, this, &GuiFriendRequestMgr::slotInternalFriendRequestUpdated, Qt::QueuedConnection );

    connect( this, SIGNAL(signalInternalFriendRequestRemoved(VxGUID,VxGUID)), this, SLOT(slotInternalFriendRequestRemoved(VxGUID,VxGUID)), Qt::QueuedConnection );

    m_MyApp.getEngine().getFriendRequestMgr().addFriendRequestMgrClient( dynamic_cast<FriendRequestCallbackInterface*>(this), true );
}

//============================================================================
void GuiFriendRequestMgr::callbackFriendRequestUpdated( std::shared_ptr<FriendRequestInfo>& friendRequest )
{
    emit signalInternalFriendRequestUpdated( friendRequest );
}

//============================================================================
void GuiFriendRequestMgr::callbackFriendRequestRemoved( VxGUID& friendOnlineId, VxGUID& requestId )
{
    emit signalInternalFriendRequestRemoved( friendOnlineId, requestId );
}

//============================================================================
void GuiFriendRequestMgr::slotInternalFriendRequestUpdated( std::shared_ptr<FriendRequestInfo> friendRequest )
{
    bool found{ false };
    for( auto request : m_FriendRequestList )
    {
        if( request->getRequestId() == friendRequest->getRequestId() )
        {
            found = true;
            break;
        }
    }

    if( !found )
    {
        GuiFriendRequest* guiFriendRequest = new GuiFriendRequest( m_MyApp, friendRequest );
        m_FriendRequestList.emplace_back( guiFriendRequest );
        announceFriendRequestListUpdated( guiFriendRequest );
    }
}

//============================================================================
void GuiFriendRequestMgr::slotInternalFriendRequestRemoved( VxGUID friendOnlineId, VxGUID requestId )
{
    removeFriendRequest( requestId );
}

//============================================================================
void GuiFriendRequestMgr::wantFriendRequestListCallbacks( GuiFriendRequestListCallback* client, bool enable )
{
    for( auto iter = m_FriendRequestListClients.begin(); iter != m_FriendRequestListClients.end(); ++iter )
    {
        if( *iter == client )
        {
            m_FriendRequestListClients.erase( iter );
            break;
        }
    }

    if( enable )
    {
        m_FriendRequestListClients.emplace_back( client );
    }
}

//============================================================================
void GuiFriendRequestMgr::announceFriendRequestListUpdated( GuiFriendRequest* guiFriendRequest )
{
    for( auto& client : m_FriendRequestListClients )
    {
        client->callbackGuiFriendRequestListUpdated( guiFriendRequest );
    }
}

//============================================================================
void GuiFriendRequestMgr::announceFriendRequestListRemoved( VxGUID& requestId )
{
    for( auto& client : m_FriendRequestListClients )
    {
        client->callbackGuiFriendRequestListRemoved( requestId );
    }
}

//============================================================================
void GuiFriendRequestMgr::friendAccepted( GuiFriendRequest* friendRequest )
{
    VxGUID requestId = friendRequest->getRequestId();
    m_MyApp.getEngine().getFriendRequestMgr().friendRequestAcked( requestId, true );
    removeFriendRequest( requestId );
}

//============================================================================
void GuiFriendRequestMgr::friendRejected( GuiFriendRequest* friendRequest )
{
    VxGUID requestId = friendRequest->getRequestId();
    m_MyApp.getEngine().getFriendRequestMgr().friendRequestAcked( requestId, false );
    removeFriendRequest( requestId );
}

//============================================================================
void GuiFriendRequestMgr::removeFriendRequest( VxGUID& requestId )
{
    bool removed{ false };
    for( auto iter = m_FriendRequestList.begin(); iter != m_FriendRequestList.end(); iter++ )
    {
        if( (*iter)->getRequestId() == requestId )
        {
            removed = true;
            m_FriendRequestList.erase( iter );
            break;
        }
    }

    if( removed )
    {
        announceFriendRequestListRemoved( requestId );
        m_MyApp.getEngine().getFriendRequestMgr().removeFriendRequest( requestId );
    } 
}