//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiUserMgr.h"
#include "AppCommon.h"
#include "AccountMgr.h"
#include "GuiUserUpdateCallback.h"

#include <ptop_src/ptop_engine_src/P2PEngine/P2PEngine.h>

#include <CoreLib/VxGlobals.h>

//============================================================================
GuiUserMgr::GuiUserMgr( AppCommon& app )
    : QObject( &app )
    , m_MyApp( app )
{
}

//============================================================================
void GuiUserMgr::onAppCommonCreated( void )
{
    m_MyApp.wantToGuiUserUpdateCallbacks( this, true );
}

//============================================================================
GuiUser* GuiUserMgr::getMyIdent( void )
{
    if( !m_MyIdent )
    {
        LogMsg( LOG_ERROR, "GuiUserMgr::getMyIdent null m_MyIdent");
        return nullptr;
    }

    if( !m_MyIdent->getNetIdent().isValidNetIdent() )
    {
        if( !m_MyApp.isMessengerReady() )
        {
            LogMsg( LOG_ERROR, "GuiUserMgr::getMyIdent called before ready");
        }

        m_MyIdent->setNetIdent( m_MyApp.getMyNetIdent() );
        if( !m_MyIdent->getNetIdent().isValidNetIdent() )
        {
            LogMsg( LOG_ERROR, "GuiUserMgr::getMyIdent called but net ident is invalid");
            return nullptr;
        }
    }

    return m_MyIdent;
}

//============================================================================
bool GuiUserMgr::isMessengerReady( void )
{
    return m_MyApp.isMessengerReady();
}

//============================================================================
void GuiUserMgr::toGuiIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp )
{
    if( !onlineId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiSaveMyIdent invalid onlineId" );
        return;
    }

    GuiUser* guiUser = getOrQueryUser( onlineId );
    if( guiUser )
    {
        switch( listType )
        {
        case eUserViewTypeDirectConnect:
            if( !guiUser->getNetIdent().isDirectConnected() )
            {
                LogMsg( LOG_VERBOSE, "GuiUserMgr::toGuiIndentListUpdate is direct connect %s", guiUser->getNetIdent().getOnlineName() );
            }
            break;
        case eUserViewTypeNearby:
            if( !guiUser->getNetIdent().isNearby() )
            {
                guiUser->getNetIdent().setIsNearby( true );
                LogMsg( LOG_VERBOSE, "GuiUserMgr::toGuiIndentListUpdate is nearby %s", guiUser->getNetIdent().getOnlineName() );
            }
            break;
        case eUserViewTypeOnline:
            if( !guiUser->getNetIdent().isOnline() )
            {
                LogMsg( LOG_VERBOSE, "GuiUserMgr::toGuiIndentListUpdate is NOT online %s", guiUser->getNetIdent().getOnlineName() );
            }
            break;

        default:
            break;
        }

        guiUser->setLastUpdateTime( timestamp );
        if( isMessengerReady() && onlineId.isVxGUIDValid() )
        {
            m_ClientListBusy = true;
            for( auto client : m_GuiUserUpdateClientList )
            {
                if( client && !isClientQueuedForRemoval( client ) )
                {
                    client->callbackIndentListUpdate( listType, onlineId, timestamp );
                    client->callbackUserUpdated( guiUser );
                }
            }

            m_ClientListBusy = false;
            updateClientList();
        }
    }
}

//============================================================================
void GuiUserMgr::toGuiIndentListRemove( EUserViewType listType, VxGUID& onlineId )
{
    if( !onlineId.isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiIndentListRemove invalid onlineId" );
        return;
    }

    GuiUser* guiUser = getOrQueryUser( onlineId );
    if( guiUser )
    {
        switch( listType )
        {
        case eUserViewTypeDirectConnect:
            if( guiUser->getNetIdent().isDirectConnected() )
            {
                LogMsg( LOG_VERBOSE, "GuiUserMgr::toGuiIndentListRemove Was direct connect %s", guiUser->getNetIdent().getOnlineName() );
            }
            break;

        case eUserViewTypeNearby:
            if( guiUser->getNetIdent().isNearby() )
            {
                guiUser->getNetIdent().setIsNearby( false );
                LogMsg( LOG_VERBOSE, "GuiUserMgr::toGuiIndentListRemove Was nearby %s", guiUser->getNetIdent().getOnlineName() );
            }
            break;

        case eUserViewTypeOnline:
            if( guiUser->getNetIdent().isOnline() )
            {
                LogMsg( LOG_VERBOSE, "GuiUserMgr::toGuiIndentListRemove Was online %s", guiUser->getNetIdent().getOnlineName() );
            }
            break;

        default:
            break;
        }

        if( isMessengerReady() && onlineId.isVxGUIDValid() )
        {
            m_ClientListBusy = true;
            for( auto client : m_GuiUserUpdateClientList )
            {
                if( client && !isClientQueuedForRemoval( client ) )
                {
                    client->callbackIndentListRemove( listType, onlineId );
                    client->callbackUserUpdated( guiUser );
                }
            }

            m_ClientListBusy = false;
            updateClientList();
        }
    }
}

//============================================================================
void GuiUserMgr::toGuiContactAdded( VxNetIdent* netIdent )
{
    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "GuiUserMgr::toGuiContactAdded invalid netIdent" );
        return;
    }

    updateUser( netIdent );
}

//============================================================================
void GuiUserMgr::toGuiContactRemoved( VxGUID& onlineId )
{
    GuiUser* guiUser = findUser( onlineId );
    if( guiUser )
    {
        onUserRemoved( onlineId );
    }

    removeUser( onlineId );
}

//============================================================================
void GuiUserMgr::toGuiContactUpdated( VxNetIdent* netIdent )
{
    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "GuiUserMgr::toGuiContactAdded invalid netIdent" );
        return;
    }

    updateUser( netIdent );
}

//============================================================================
bool GuiUserMgr::toGuiOnlineStatusChange( VxGUID& onlineId, bool isOnline )
{
    bool listUpdated = updateIsOnlineList( onlineId, isOnline );

    if( listUpdated )
    {
        GuiUser* guiUser = findUser( onlineId );
        if( guiUser )
        {
            LogModule( eLogUserGuiEvent, LOG_VERBOSE, "GuiUserMgr::toGuiOnlineStatusChange user %s online %d id %s", guiUser->getOnlineName().c_str(), isOnline, guiUser->getMyOnlineId().toOnlineIdString().c_str() );

            m_ClientListBusy = true;
            for( auto client : m_GuiUserUpdateClientList )
            {
                if( client && !isClientQueuedForRemoval( client ) )
                {
                    client->callbackOnlineStatusChange( guiUser, isOnline );
                }
            }

            m_ClientListBusy = false;
            updateClientList();
        }
        else
        {
            LogMsg( LOG_ERROR, "GuiUserMgr::toGuiOnlineStatusChange user not found %s", onlineId.toOnlineIdString().c_str() );
        }
    }

    return listUpdated;
}

//============================================================================
bool GuiUserMgr::updateIsOnlineList( VxGUID& onlineId, bool isOnline )
{
    bool listUpdated{ false };
    if( isOnline )
    {
        auto iter = m_OnlineUsers.find( onlineId );
        if( iter == m_OnlineUsers.end() )
        {
            m_OnlineUsers.insert( onlineId );
            listUpdated = true;
        }
    }
    else
    {
        auto iter = m_OnlineUsers.find( onlineId );
        if( iter != m_OnlineUsers.end() )
        {
            m_OnlineUsers.erase( iter );
            listUpdated = true;
        }
    }

    return listUpdated;
}

////============================================================================
//void GuiUserMgr::toGuiContactNameChange( VxNetIdent* netIdent )
//{
//    if( !netIdent || !netIdent->isValidNetIdent() )
//    {
//        LogMsg( LOG_ERROR, "AppCommon::toGuiContactNameChange invalid netIdent" );
//        return;
//    }
//
//    updateUser( netIdent );
//}
//
////============================================================================
//void GuiUserMgr::toGuiContactDescChange( VxNetIdent* netIdent )
//{
//    if( !netIdent || !netIdent->isValidNetIdent() )
//    {
//        LogMsg( LOG_ERROR, "AppCommon::toGuiContactDescChange invalid netIdent" );
//        return;
//    }
//
//    updateUser( netIdent );
//}
//
////============================================================================
//void GuiUserMgr::toGuiContactFriendshipChange( VxNetIdent* netIdent )
//{
//    if( !netIdent || !netIdent->isValidNetIdent() )
//    {
//        LogMsg( LOG_ERROR, "AppCommon::toGuiContactFriendshipChange invalid netIdent" );
//        return;
//    }
//
//    updateUser( netIdent );
//}
//
////============================================================================
//void GuiUserMgr::toGuiPluginPermissionChange( VxNetIdent* netIdent )
//{
//    if( !netIdent || !netIdent->isValidNetIdent() )
//    {
//        LogMsg( LOG_ERROR, "AppCommon::toGuiPluginPermissionChange invalid netIdent" );
//        return;
//    }
//
//    updateUser( netIdent );
//}
//
////============================================================================
//void GuiUserMgr::toGuiContactSearchFlagsChange( VxNetIdent* netIdent )
//{
//    if( !netIdent || !netIdent->isValidNetIdent() )
//    {
//        LogMsg( LOG_ERROR, "AppCommon::toGuiContactSearchFlagsChange invalid netIdent" );
//        return;
//    }
//
//    updateUser( netIdent );
//}

//============================================================================
void GuiUserMgr::toGuiContactLastSessionTimeChange( VxNetIdent* netIdent )
{
    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiContactLastSessionTimeChange invalid netIdent" );
        return;
    }

    updateUser( netIdent );
}

//============================================================================
void GuiUserMgr::toGuiUpdateMyIdent( VxNetIdent* netIdent )
{
    updateMyIdent( netIdent );
}

//============================================================================
void GuiUserMgr::toGuiSaveMyIdent( VxNetIdent* netIdent )
{
    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiSaveMyIdent invalid netIdent" );
        return;
    }

    memcpy( m_MyApp.getAppGlobals().getMyNetIdent(), netIdent, sizeof( VxNetIdent ) );
    m_MyApp.getAccountMgr().updateAccount( *netIdent );
    updateMyIdent( netIdent );
}

//============================================================================
void GuiUserMgr::updateOnlineStatus( VxNetIdent* netIdent, bool online )
{
    updateUser( netIdent );
    GuiUser* guiUser = findUser( netIdent->getMyOnlineId() );
    if( guiUser )
    {
        m_ClientListBusy = true;
        for( auto client : m_GuiUserUpdateClientList )
        {
            if( client && !isClientQueuedForRemoval( client ) )
            {
                client->callbackOnlineStatusChange( guiUser, guiUser->isOnline() );
            }
        }

        m_ClientListBusy = false;
        updateClientList();
    }
}

//============================================================================
GuiUser* GuiUserMgr::findUser( const VxGUID& onlineId )
{
    GuiUser* guiUser = nullptr;
    if( onlineId == m_MyOnlineId )
    {
        return m_MyIdent;
    }
    else
    {
        auto iter = m_UserList.find( onlineId );
        if( iter != m_UserList.end() )
        {
            guiUser = iter->second;
        }
    }

    return guiUser;
}

//============================================================================
GuiUser* GuiUserMgr::findOrAddUser( const VxGUID& onlineId )
{
    GuiUser* guiUser = findUser( onlineId );
    if( !guiUser )
    {
        VxNetIdent netIdent;
        if( m_MyApp.getEngine().fromGuiQueryIdentity( onlineId, netIdent ) && netIdent.isValidNetIdent() )
        {
            guiUser = m_MyApp.getUserMgr().updateUser( &netIdent );
        }
    }

    return guiUser;
}

//============================================================================
void GuiUserMgr::removeUser( const VxGUID& onlineId )
{
    auto iter = m_UserList.find( onlineId );
    if( iter != m_UserList.end() )
    {
        iter->second->deleteLater();
        m_UserList.erase( iter );
    }
}

//============================================================================
bool GuiUserMgr::isUserRelayed( VxGUID& onlineId )
{
    GuiUser* guiUser = findUser( onlineId );
    if( guiUser )
    {
        guiUser->updateIsRelayed();
        return guiUser->isRelayed();
    }

    return false;
}

//============================================================================
bool GuiUserMgr::isUserOnline( VxGUID& onlineId )
{
    return m_OnlineUsers.find( onlineId ) != m_OnlineUsers.end();
}

//============================================================================
bool GuiUserMgr::isUserInSession( VxGUID& onlineId )
{
    GuiUser* guiUser = findUser( onlineId );
    return guiUser && guiUser->isInSession();
}

//============================================================================
GuiUser* GuiUserMgr::getUser( const VxGUID& onlineId )
{
    GuiUser* guiUser = nullptr;
    if( onlineId == getMyOnlineId() )
    {
        LogMsg( LOG_WARNING, "GuiUserMgr::getUser getting my ident" );
        guiUser = getMyIdent();
    }
    else
    {
        guiUser = findUser( onlineId );
    }

    return guiUser;
}

//============================================================================
std::string GuiUserMgr::getUserOnlineName( VxGUID& onlineId )
{
    GuiUser* guiUser = getUser( onlineId );
    if( guiUser )
    {
        return guiUser->getOnlineName();
    }
    else
    {
        std::string onlineName( "Unknown User: " );
        onlineName += onlineId.toOnlineIdString();
        return onlineName;
    }
}

//============================================================================
GuiUser* GuiUserMgr::getOrQueryUser( VxGUID& onlineId )
{
    GuiUser* guiUser = getUser( onlineId );
    if( !guiUser )
    {
        VxNetIdent userIdent;
        if( m_MyApp.getEngine().getBigListMgr().queryIdent( onlineId, userIdent ) )
        {
            guiUser = updateUser( &userIdent );
        }
    }

    return guiUser;
}

//============================================================================
GuiUser* GuiUserMgr::updateUser( VxNetIdent* hisIdent, bool updateIsOnlineBecauseIsNowOnline )
{
    if( !hisIdent )
    {
        LogMsg( LOG_ERROR, "GuiUserMgr::updateUser invalid param" );
        return nullptr;
    }

    LogModule( eLogUserGuiEvent, LOG_VERBOSE, "GuiUserMgr::updateUser %s id %s force online %d", hisIdent->getOnlineName(), hisIdent->getMyOnlineId().toOnlineIdString().c_str(), updateIsOnlineBecauseIsNowOnline );

    if( updateIsOnlineBecauseIsNowOnline )
    {
        updateIsOnlineList( hisIdent->getMyOnlineId(), true );
    }

    if( hisIdent->getMyOnlineId() == m_MyApp.getMyOnlineId() )
    {
        return updateMyIdent( hisIdent );
    }
    else
    {
        GuiUser* guiUser = findUser( hisIdent->getMyOnlineId() );
        if( guiUser )
        {
            onUserUpdated( guiUser );
        }
        else
        {
            guiUser = new GuiUser( m_MyApp );
            guiUser->setNetIdent( hisIdent );
            m_UserList[guiUser->getMyOnlineId()] = guiUser;
            onUserAdded( guiUser );
        }

        return guiUser;
    }
}

//============================================================================
GuiUser* GuiUserMgr::updateMyIdent( VxNetIdent* myIdent )
{
    if( !m_MyIdent )
    {
        GuiUser* guiUser = new GuiUser( m_MyApp );
        guiUser->setNetIdent( myIdent );
        m_MyIdent = guiUser;
        m_MyOnlineId = m_MyIdent->getMyOnlineId();
    }
    else
    {
        m_MyIdent->setNetIdent( myIdent );
        m_MyOnlineId = m_MyIdent->getMyOnlineId();
    }

    if( isMessengerReady() )
    {
        m_ClientListBusy = true;
        for( auto client : m_GuiUserUpdateClientList )
        {
            if( client && !isClientQueuedForRemoval( client ) )
            {
                client->callbackMyIdentUpdated( m_MyIdent );
            }
        }

        m_ClientListBusy = false;
        updateClientList();
    }

    return m_MyIdent;
}

//============================================================================
void GuiUserMgr::onUserAdded( GuiUser* guiUser )
{
    if( isMessengerReady() && guiUser )
    {
        m_ClientListBusy = true;
        for( auto client : m_GuiUserUpdateClientList )
        {
            if( client && !isClientQueuedForRemoval( client ) )
            {
                client->callbackUserAdded( guiUser );
            }
        }

        m_ClientListBusy = false;
        updateClientList();
    }
}

//============================================================================
void GuiUserMgr::onUserRemoved( VxGUID& onlineId )
{
    if( isMessengerReady() )
    {
        m_ClientListBusy = true;
        for( auto client : m_GuiUserUpdateClientList )
        {
            if( client && !isClientQueuedForRemoval( client ) )
            {
                client->callbackUserRemoved( onlineId );
            }
        }

        m_ClientListBusy = false;
        updateClientList();
    }
}

//============================================================================
void GuiUserMgr::onUserNearbyStatusChange( GuiUser* guiUser )
{
    if( isMessengerReady() )
    {
        sendUserUpdatedToCallbacks( guiUser );
    }
}

//============================================================================
void GuiUserMgr::onUserRelayStatusChange( GuiUser* guiUser )
{
    if( isMessengerReady() )
    {
        bool wasOnline = guiUser->isOnline();

        if( guiUser->isOnline() )
        {
            onUserOnlineStatusChange( guiUser );
        }
        else
        {
            onUserUpdated( guiUser );
        }
    }
}

//============================================================================
void GuiUserMgr::onUserOnlineStatusChange( GuiUser* guiUser )
{
    if( isMessengerReady() )
    {
        m_ClientListBusy = true;
        for( auto client : m_GuiUserUpdateClientList )
        {
            if( client && !isClientQueuedForRemoval( client ) )
            {
                client->callbackOnlineStatusChange( guiUser, guiUser->isOnline() );
            }
        }

        m_ClientListBusy = false;
        updateClientList();
        sendUserUpdatedToCallbacks( guiUser );
    }
}

//============================================================================
void GuiUserMgr::onUserUpdated( GuiUser* guiUser )
{
    if( isMessengerReady() )
    {
        sendUserUpdatedToCallbacks( guiUser );
    }
}

//============================================================================
void GuiUserMgr::onMyIdentUpdated( GuiUser* guiUser )
{
    if( isMessengerReady() )
    {
        m_ClientListBusy = true;
        for( auto client : m_GuiUserUpdateClientList )
        {
            if( client && !isClientQueuedForRemoval( client ) )
            {
                client->callbackMyIdentUpdated( guiUser );
            }
        }

        m_ClientListBusy = false;
        updateClientList();
    }
}

//============================================================================
void GuiUserMgr::wantGuiUserUpdateCallbacks( GuiUserUpdateCallback* callback, bool wantCallback )
{
    if( !callback || (GuiUserUpdateCallback *)0xFFFFFFFFFFFFFFFF == callback )
    {
        LogMsg( LOG_ERROR, "GuiUserMgr::wantGuiUserUpdateCallbacks invalid param" );
        vx_assert( false );
        return;
    }

    static bool userCallbackShutdownComplete = false;
    if( VxIsAppShuttingDown() )
    {
        if( userCallbackShutdownComplete )
        {
            return;
        }

        userCallbackShutdownComplete = true;
        clearGuiUserUpdateClientList();
        return;
    }

    if( m_UpdatingClientList )
    {
         LogMsg( LOG_ERROR, "GuiUserMgr::wantGuiUserUpdateCallbacks SHOULD NEVER occur while updating list" );
    }

    if( m_ClientListBusy )
    {
        // the client list is in use. Save and the list will be updated after the client list is no longer in use
        // otherwise will crash because the list is changed during interation of the list
        LogModule( eLogUserGuiEvent, LOG_VERBOSE, "GuiUserMgr wantToGuiUserUpdateCallback Client List Busy queing %p want ? %d.. current update list size %d", callback, wantCallback, m_GuiUserUpdateClientList.size() );
        m_WantUpdateToDoList.push_back( std::make_pair( callback, wantCallback ) );
        return;
    }

    if( wantCallback )
    {
        for( auto client : m_GuiUserUpdateClientList )
        {
            if( client == callback )
            {
                LogMsg( LOG_WARN, "WARNING. Ignoring New wantToGuiUserUpdateCallbacks because already in list" );
                return;
            }
        }

        LogModule( eLogUserGuiEvent, LOG_VERBOSE, "GuiUserMgr Adding New wantToGuiUserUpdateCallback %p at index %d", callback, m_GuiUserUpdateClientList.size() );
        m_GuiUserUpdateClientList.push_back( callback );
        return;
    }
    else
    {
        int idx = 0;
        for( auto iter = m_GuiUserUpdateClientList.begin(); iter != m_GuiUserUpdateClientList.end(); ++iter )
        {
            GuiUserUpdateCallback* client = *iter;
            if( client == callback )
            {
                LogModule( eLogUserGuiEvent, LOG_VERBOSE, "GuiUserMgr wantToGuiUserUpdateCallback %p at index %d", callback, idx );
                m_GuiUserUpdateClientList.erase( iter );
                return;
            }

            idx++;
        }

        LogMsg( LOG_WARN, "WARNING. ToGuiUserUpdateInterface remove not found in list" );
    }
}

//============================================================================
void GuiUserMgr::clearGuiUserUpdateClientList( void )
{
    if( m_GuiUserUpdateClientList.size() )
    {
        m_GuiUserUpdateClientList.clear();
    }
}

//============================================================================
void GuiUserMgr::sendUserUpdatedToCallbacks( GuiUser* guiUser )
{
    if( guiUser )
    {
        LogMsg( LOG_ERROR, "GuiUserMgr::sendUserUpdatedToCallbacks to %d clients user %s %s",
               m_GuiUserUpdateClientList.size(), guiUser->getOnlineName().c_str(), guiUser->getMyOnlineId().toOnlineIdString().c_str() );
        if( guiUser->isMyself() )
        {
            LogMsg( LOG_ERROR, "GuiUserMgr::sendUserUpdatedToCallbacks updating myself" );
        }

        m_ClientListBusy = true;
        for( auto client : m_GuiUserUpdateClientList )
        {
            if( client && !isClientQueuedForRemoval( client ) )
            {
                client->callbackUserUpdated( guiUser );
            }
        }

        m_ClientListBusy = false;
        updateClientList();
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiUserMgr::sendUserUpdatedToCallbacks invalid guiUser" );
    }
}

//============================================================================
void GuiUserMgr::toGuiPushToTalkStatus( VxGUID& onlineId, EPushToTalkStatus pushToTalkStatus )
{
    GuiUser* guiUser = findUser( onlineId );
    if( guiUser )
    {
        guiUser->setPushToTalkStatus( pushToTalkStatus );    
    }

    if( guiUser )
    {
        sendUserUpdatedToCallbacks( guiUser );
    }

    m_ClientListBusy = true;
    for( auto client : m_GuiUserUpdateClientList )
    {
        if( client && !isClientQueuedForRemoval( client ) )
        {
            client->callbackPushToTalkStatus( onlineId, pushToTalkStatus );
        }
    }

    m_ClientListBusy = false;
    updateClientList();
}

//============================================================================
void GuiUserMgr::connnectIdNearbyStatusChange( VxGUID& onlineId, uint64_t nearbyTimeOrZeroIfNot )
{
    GuiUser* guiUser = findUser( onlineId );
    if( guiUser )
    {
        bool wasOnline = guiUser->isOnline();
        guiUser->updateIsNearby();
        if( wasOnline != guiUser->isOnline() )
        {
            onUserOnlineStatusChange( guiUser );
        }
        else
        {
            onUserUpdated( guiUser );
        }
 
        LogMsg( LOG_VERBOSE, "GuiUserMgr::connnectIdNearbyStatusChange is online ? %d %s", guiUser->isOnline(), getUserOnlineName( onlineId ).c_str() );
    }
}

//============================================================================
void GuiUserMgr::connnectIdRelayStatusChange( VxGUID& onlineId )
{
    GuiUser* guiUser = findUser( onlineId );
    if( guiUser )
    {      
        bool wasOnline = guiUser->isOnline();
        guiUser->updateIsRelayed();
        if( wasOnline != guiUser->isOnline() )
        {
            onUserOnlineStatusChange( guiUser );
        }
        else
        {
            onUserUpdated( guiUser );
        }

        bool isRelayed = guiUser->isRelayed();
        LogMsg( LOG_VERBOSE, "GuiUserMgr::connnectIdRelayStatusChange is relayed ? %d %s", isRelayed, getUserOnlineName( onlineId ).c_str() );
    }
}

//============================================================================
void GuiUserMgr::updateClientList( void )
{
    // update client list from que of want callback that happened while client list was busy
    m_UpdatingClientList = true;
    LogModule( eLogUserGuiEvent, LOG_VERBOSE, "GuiUserMgr updateClientList to do list size %d updateClientList size %d", m_WantUpdateToDoList.size(), m_GuiUserUpdateClientList.size());
    for( auto pair : m_WantUpdateToDoList )
    {
        if( pair.second )
        {
            bool isDuplicate{ false };
            for( auto client : m_GuiUserUpdateClientList )
            {
                if( client == pair.first )
                {
                    LogMsg( LOG_INFO, "WARNING. Ignoring New wantToGuiUserUpdateCallbacks because already in list" );
                    isDuplicate = true;
                    break;
                }
            }

            if( !isDuplicate )
            {

                LogModule( eLogUserGuiEvent, LOG_VERBOSE, "GuiUserMgr updateClientList adding client %p at index %d", pair.first, m_GuiUserUpdateClientList.size());
                m_GuiUserUpdateClientList.push_back( pair.first );
            }
        }
        else
        {
            int idx = 0;
            for( auto iter = m_GuiUserUpdateClientList.begin(); iter != m_GuiUserUpdateClientList.end(); ++iter )
            {
                GuiUserUpdateCallback* client = *iter;
                if( client == pair.first )
                {
                    LogModule( eLogUserGuiEvent, LOG_VERBOSE, "GuiUserMgr updateClientList erasing client %p at index %d", pair.first, m_GuiUserUpdateClientList.size());
                    m_GuiUserUpdateClientList.erase( iter );
                    break;
                }

                idx++;
            }
        }
    }

    m_WantUpdateToDoList.clear();
    m_UpdatingClientList = false;
}

//============================================================================
bool GuiUserMgr::isClientQueuedForRemoval( GuiUserUpdateCallback* client )
{
    bool isQueued{ false };
    for( auto pair : m_WantUpdateToDoList )
    {
        if( !pair.second && pair.first == client )
        {
            isQueued = true;
            break;
        }
    }

    return isQueued;
}

bool GuiUserMgr::getOfflineUsers( std::vector<std::pair<VxGUID, int64_t>>& idList )
{
    idList.clear();

    for( auto guiUserPair : m_UserList )
    {
        GuiUser* guiUser = guiUserPair.second;
        if( !guiUser->isOnline() && ( guiUser->isAdmin() || guiUser->isFriend() ) )
        {
            idList.push_back( std::make_pair(guiUserPair.first, guiUser->getLastUpdateTime() ) );
        }
    }

    return !idList.empty();
}
