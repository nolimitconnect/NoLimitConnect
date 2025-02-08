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

#include "ActivityMessageBox.h"
#include "AppCommon.h"
#include "AccountMgr.h"
#include "GuiHelpers.h"
#include "GuiUserUpdateCallback.h"

#include <P2PEngine/P2PEngine.h>

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
    return findUser( m_MyOnlineId );
}

//============================================================================
std::string  GuiUserMgr::getMyOnlineName( void )
{
    GuiUser* guiUser = getMyIdent();
    if( guiUser )
    {
        return guiUser->getOnlineName();
    }
    else
    {
        return "My Ident is not set";
    }
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
            if( m_ClientListBusy )
            {
                LogMsg( LOG_ERROR, "GuiUserMgr::%s m_ClientListBusy", __func__ );
                vx_assert( false );
            }

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
            if( m_ClientListBusy )
            {
                LogMsg( LOG_ERROR, "GuiUserMgr::%s m_ClientListBusy", __func__ );
                vx_assert( false );
            }

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
            LogModule( eLogUserEvent, LOG_VERBOSE, "GuiUserMgr::toGuiOnlineStatusChange user %s online %d id %s", guiUser->getOnlineName().c_str(), isOnline, guiUser->getMyOnlineId().toOnlineIdString().c_str() );
            if( m_ClientListBusy )
            {
                LogMsg( LOG_ERROR, "GuiUserMgr::%s m_ClientListBusy", __func__ );
                vx_assert( false );
            }

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
        if( m_ClientListBusy )
        {
            LogMsg( LOG_ERROR, "GuiUserMgr::%s m_ClientListBusy", __func__ );
            vx_assert( false );
        }

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
GuiUser* GuiUserMgr::findUser( VxGUID onlineId )
{
    m_MyApp.checkIsGuiThread();
    GuiUser* guiUser = nullptr;
    if( onlineId == m_MyOnlineId )
    {
        if( !m_RawMyIdent.isValidNetIdent() )
        {
            return nullptr;
        }
        else if( !m_MyIdent )
        {
            GuiUser* guiUser = new GuiUser( m_MyApp );
            guiUser->setNetIdent( &m_RawMyIdent );
            m_MyIdent = guiUser;
            return guiUser;
        }
        else
        {
            return m_MyIdent;
        }
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
GuiUser* GuiUserMgr::findOrAddUser( VxGUID onlineId )
{
    GuiUser* guiUser = findUser( onlineId );
    if( !guiUser )
    {
        VxNetIdent netIdent;
        if( m_MyApp.getEngine().fromGuiQueryIdentity( onlineId, netIdent ) && netIdent.isValidNetIdent() )
        {
            guiUser = updateUser( &netIdent );
        }
    }

    return guiUser;
}

//============================================================================
void GuiUserMgr::removeUser( VxGUID onlineId )
{
    m_MyApp.checkIsGuiThread();
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
    return m_MyApp.getConnectIdListMgr().isRelayed( onlineId );
}

//============================================================================
bool GuiUserMgr::isUserOnline( VxGUID& onlineId )
{
    if( onlineId == m_MyOnlineId )
    {
        return true;
    }

    return m_MyApp.getConnectIdListMgr().isOnline( onlineId );
}

//============================================================================
bool GuiUserMgr::isUserInSession( VxGUID& onlineId )
{
    GuiUser* guiUser = findUser( onlineId );
    return guiUser && guiUser->isInSession();
}

//============================================================================
GuiUser* GuiUserMgr::getUser( VxGUID onlineId, bool queryFromEngineIfMissing )
{
    GuiUser* guiUser = nullptr;
    if( onlineId == getMyOnlineId() )
    {
        // LogMsg( LOG_WARNING, "GuiUserMgr::getUser getting my ident" );
        guiUser = getMyIdent();
    }
    else
    {
        if( queryFromEngineIfMissing )
        {
            guiUser = findOrAddUser( onlineId );
        }
        else
        {
            guiUser = findUser( onlineId );
        }
    }

    return guiUser;
}

//============================================================================
GuiUser* GuiUserMgr::getUserForTest( bool mustBeOnline )
{
    GuiUser* guiUser{ nullptr };
    for( auto& userPair : m_UserList )
    {
        if( mustBeOnline )
        {
            if( guiUser->isOnline() )
            {
                guiUser = userPair.second;
                break;
            }
        }
        else
        {
            guiUser = userPair.second;
            break;
        }
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
        return "Unknown User";
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
GuiUser* GuiUserMgr::updateUser( VxNetIdent* hisIdent )
{
    if( !hisIdent || !hisIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "GuiUserMgr::updateUser invalid param" );
        return nullptr;
    }

    LogModule( eLogUserEvent, LOG_VERBOSE, "GuiUserMgr::updateUser %s id %s", hisIdent->getOnlineName(), hisIdent->getMyOnlineId().toOnlineIdString().c_str() );

    if( hisIdent->getMyOnlineId() == m_MyApp.getMyOnlineId() )
    {
        updateMyIdent( hisIdent );
        return findUser( hisIdent->getMyOnlineId() );
    }
    else
    {
        GuiUser* guiUser = findUser( hisIdent->getMyOnlineId() );
        if( guiUser )
        {
            // only call updated if changes concern gui
            bool requiresUpdate{ false };
            VxNetIdent curIdent = guiUser->getNetIdent();
            if( !curIdent.isValidNetIdent() )
            {
                requiresUpdate = true;
                LogModule( eLogUserEvent, LOG_VERBOSE, "GuiUserMgr::updateUser update invalid ident %s", hisIdent->describeUser().c_str() );
            }
            else
            {
                // name changed
                requiresUpdate = 0 != strcmp( curIdent.getOnlineName(),  hisIdent->getOnlineName() );
                if( requiresUpdate )
                {
                    LogModule( eLogUserEvent, LOG_VERBOSE, "GuiUserMgr::updateUser name change %s", hisIdent->describeUser().c_str() );
                }

                if( !requiresUpdate )
                {
                    // description changed
                    requiresUpdate = 0 != strcmp( curIdent.getOnlineDescription(),  hisIdent->getOnlineDescription() );
                    if( requiresUpdate )
                    {
                        LogModule( eLogUserEvent, LOG_VERBOSE, "GuiUserMgr::updateUser decription change %s", hisIdent->describeUser().c_str() );
                    }
                }

                if( !requiresUpdate )
                {
                    // friendship changed
                    requiresUpdate = curIdent.getMyFriendshipToHim() != hisIdent->getMyFriendshipToHim() ||
                        curIdent.getHisFriendshipToMe() != hisIdent->getHisFriendshipToMe();
                    if( requiresUpdate )
                    {
                        LogModule( eLogUserEvent, LOG_VERBOSE, "GuiUserMgr::updateUser friendship change %s", hisIdent->describeUser().c_str() );
                    }
                }

                if( !requiresUpdate )
                {
                    // permission changed
                    requiresUpdate = 0 != memcmp( curIdent.getPluginPermissions(), hisIdent->getPluginPermissions(), PERMISSION_ARRAY_SIZE );
                    if( requiresUpdate )
                    {
                        LogModule( eLogUserEvent, LOG_VERBOSE, "GuiUserMgr::updateUser permission change %s", hisIdent->describeUser().c_str() );
                    }
                }
            }

            if( requiresUpdate )
            {
                guiUser->setNetIdent( hisIdent );
                onUserUpdated( guiUser );
            }
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
void GuiUserMgr::updateMyIdent( VxNetIdent* myIdent )
{
    if( !myIdent )
    {
        LogMsg( LOG_ERROR, "GuiUserMgr::updateMyIdent null myIdent" );
        return;
    }

    if( !myIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "GuiUserMgr::updateMyIdent invalid myIdent" );
        return;
    }

    if( !m_MyIdent )
    {
        memcpy( &m_RawMyIdent, myIdent, sizeof( VxNetIdent ) );
        m_MyOnlineId = m_RawMyIdent.getMyOnlineId();
    }
    else
    {
        m_MyIdent->setNetIdent( myIdent );
        m_MyOnlineId = m_MyIdent->getMyOnlineId();
    }

    if( m_MyIdent )
    {
        if( isMessengerReady() )
        {
            if( m_ClientListBusy )
            {
                LogMsg( LOG_ERROR, "GuiUserMgr::%s m_ClientListBusy", __func__ );
                vx_assert( false );
            }

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
    }
}

//============================================================================
void GuiUserMgr::onUserAdded( GuiUser* guiUser )
{
    if( isMessengerReady() && guiUser )
    {
        if( m_ClientListBusy )
        {
            LogMsg( LOG_ERROR, "GuiUserMgr::%s m_ClientListBusy", __func__ );
            vx_assert( false );
        }

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
        if( m_ClientListBusy )
        {
            LogMsg( LOG_ERROR, "GuiUserMgr::%s m_ClientListBusy", __func__ );
            vx_assert( false );
        }

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
        if( m_ClientListBusy )
        {
            LogMsg( LOG_ERROR, "GuiUserMgr::%s m_ClientListBusy", __func__ );
            vx_assert( false );
        }

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
        if( m_ClientListBusy )
        {
            LogMsg( LOG_ERROR, "GuiUserMgr::%s m_ClientListBusy", __func__ );
            vx_assert( false );
        }

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
        LogModule( eLogUserEvent, LOG_VERBOSE, "GuiUserMgr wantToGuiUserUpdateCallback Client List Busy queing %p want ? %d.. current update list size %d", callback, wantCallback, m_GuiUserUpdateClientList.size() );
        m_WantUpdateToDoList.push_back( std::make_pair( callback, wantCallback ) );
        return;
    }

    m_ClientListBusy = true;
    if( wantCallback )
    {
        for( auto client : m_GuiUserUpdateClientList )
        {
            if( client == callback )
            {
                LogMsg( LOG_WARN, "WARNING. Ignoring New wantToGuiUserUpdateCallbacks because already in list" );
                m_ClientListBusy = false;
                return;
            }
        }

        LogModule( eLogUserEvent, LOG_VERBOSE, "GuiUserMgr Add wantToGuiUserUpdateCallback %p at index %d", callback, m_GuiUserUpdateClientList.size() );
        m_GuiUserUpdateClientList.emplace_back( callback );
        m_ClientListBusy = false;
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
                LogModule( eLogUserEvent, LOG_VERBOSE, "GuiUserMgr Remove wantToGuiUserUpdateCallback %p at index %d", callback, idx );
                m_GuiUserUpdateClientList.erase( iter );
                m_ClientListBusy = false;
                return;
            }

            idx++;
        }

        LogMsg( LOG_WARN, "WARNING. ToGuiUserUpdateInterface remove not found in list" );
    }

    m_ClientListBusy = false;
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

        if( m_ClientListBusy )
        {
            LogMsg( LOG_ERROR, "GuiUserMgr::%s m_ClientListBusy", __func__ );
            vx_assert( false );
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
void GuiUserMgr::updateClientList( void )
{
    // update client list from que of want callback that happened while client list was busy
    if( m_ClientListBusy )
    {
        LogMsg( LOG_ERROR, "GuiUserMgr::%s m_ClientListBusy", __func__ );
        vx_assert( false );
    }

    m_UpdatingClientList = true;
    LogModule( eLogUserEvent, LOG_VERBOSE, "GuiUserMgr updateClientList to do list size %d updateClientList size %d", m_WantUpdateToDoList.size(), m_GuiUserUpdateClientList.size());
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

                LogModule( eLogUserEvent, LOG_VERBOSE, "GuiUserMgr updateClientList adding client %p at index %d", pair.first, m_GuiUserUpdateClientList.size());
                m_GuiUserUpdateClientList.emplace_back( pair.first );
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
                    LogModule( eLogUserEvent, LOG_VERBOSE, "GuiUserMgr updateClientList erasing client %p at index %d", pair.first, m_GuiUserUpdateClientList.size());
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

//============================================================================
bool GuiUserMgr::getOfflineUsers( std::vector<std::pair<VxGUID, int64_t>>& idList )
{
    idList.clear();

    for( auto guiUserPair : m_UserList )
    {
        GuiUser* guiUser = guiUserPair.second;
        if( !guiUser->isOnline() && ( guiUser->isAdmin() || guiUser->isFriend() ) )
        {
            idList.emplace_back( std::make_pair(guiUserPair.first, guiUser->getLastUpdateTime() ) );
        }
    }

    return !idList.empty();
}

//============================================================================
void GuiUserMgr::dumpUserInfo( GuiUser* guiUser, QWidget* parentFrame )
{
    if( !guiUser )
    {
        return;
    }

    std::string userInfo = guiUser->dumpUserInfo();

    ActivityMessageBox msgBox( m_MyApp, parentFrame, LOG_INFO, userInfo.c_str() );
    msgBox.exec();
}

//============================================================================
void GuiUserMgr::deleteUser( GuiUser* guiUser, QWidget* parentFrame )
{
    if( !guiUser )
    {
        return;
    }

    std::string userInfo = "Are you sure you want to delete user from database?\nChange do not take effect until restart.\n" + guiUser->dumpUserInfo();
    ActivityMessageBox msgBox( m_MyApp, parentFrame, LOG_INFO, userInfo.c_str() );
    msgBox.showCancelButton( true );
    msgBox.exec();
    
    if( msgBox.wasOkButtonClicked() )
    {
        bool userDeleted = m_MyApp.getEngine().fromGuiDeleteUser( guiUser->getMyOnlineId() );
        if( userDeleted )
        {
            QString warnTitle = QObject::tr( "User was deleted" );
            QString warnBody = QObject::tr( "User was deleted from database" );

            QMessageBox warnMsg( QMessageBox::Icon::Information, warnTitle, warnBody, QMessageBox::Ok );
            warnMsg.exec();
        }
        else
        {
            QString warnTitle = QObject::tr( "User was NOT deleted" );
            QString warnBody = QObject::tr( "User failed to be deleted from database" );

            QMessageBox warnMsg( QMessageBox::Icon::Information, warnTitle, warnBody, QMessageBox::Ok );
            warnMsg.exec();
        }
    }
}
