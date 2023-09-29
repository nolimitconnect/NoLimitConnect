//============================================================================
// Copyright (C) 2021 Brett R. Jones
// Issued to MIT style license by Brett R. Jones in 2017
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiUserMgr.h"
#include "AppCommon.h"
#include "AccountMgr.h"
#include "GuiUserUpdateCallback.h"

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
            for( auto client : m_GuiUserUpdateClientList )
            {
                client->callbackIndentListUpdate( listType, onlineId, timestamp );
                client->callbackUserUpdated( guiUser );
            }
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
            for( auto client : m_GuiUserUpdateClientList )
            {
                client->callbackIndentListRemove( listType, onlineId );
                client->callbackUserUpdated( guiUser );
            }
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
void GuiUserMgr::toGuiContactOnline( VxNetIdent* netIdent )
{
    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiContactOnline invalid netIdent" );
        return;
    }

    updateOnlineStatus( netIdent, true );
}

//============================================================================
void GuiUserMgr::toGuiContactOffline( VxNetIdent* netIdent )
{
    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiContactOffline invalid netIdent" );
        return;
    }

    updateOnlineStatus( netIdent, false );
}

//============================================================================
void GuiUserMgr::toGuiContactOffline( VxGUID& onlineId )
{
    GuiUser* guiUser = findUser( onlineId );
    if( guiUser )
    {
        for( auto client : m_GuiUserUpdateClientList )
        {
            client->callbackOnlineStatusChange( guiUser, false );
        }
    }
}

//============================================================================
void GuiUserMgr::toGuiContactNameChange( VxNetIdent* netIdent )
{
    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiContactNameChange invalid netIdent" );
        return;
    }

    updateUser( netIdent );
}

//============================================================================
void GuiUserMgr::toGuiContactDescChange( VxNetIdent* netIdent )
{
    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiContactDescChange invalid netIdent" );
        return;
    }

    updateUser( netIdent );
}

//============================================================================
void GuiUserMgr::toGuiContactMyFriendshipChange( VxNetIdent* netIdent )
{
    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiContactMyFriendshipChange invalid netIdent" );
        return;
    }

    updateUser( netIdent );
}

//============================================================================
void GuiUserMgr::toGuiContactHisFriendshipChange( VxNetIdent* netIdent )
{
    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiContactHisFriendshipChange invalid netIdent" );
        return;
    }

    updateUser( netIdent );
}

//============================================================================
void GuiUserMgr::toGuiPluginPermissionChange( VxNetIdent* netIdent )
{
    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiPluginPermissionChange invalid netIdent" );
        return;
    }

    updateUser( netIdent );
}

//============================================================================
void GuiUserMgr::toGuiContactSearchFlagsChange( VxNetIdent* netIdent )
{
    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiContactSearchFlagsChange invalid netIdent" );
        return;
    }

    updateUser( netIdent );
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
        for( auto client : m_GuiUserUpdateClientList )
        {
            client->callbackOnlineStatusChange( guiUser, guiUser->isOnline() );
        }
    }
}

//============================================================================
GuiUser* GuiUserMgr::findUser( VxGUID& onlineId )
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
void GuiUserMgr::removeUser( VxGUID& onlineId )
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
    GuiUser* guiUser = findUser( onlineId );
    if( guiUser )
    {
        guiUser->updateIsOnline();
        return guiUser->isOnline();
    }

    return false;
}

//============================================================================
bool GuiUserMgr::isUserInSession( VxGUID& onlineId )
{
    GuiUser* guiUser = findUser( onlineId );
    return guiUser && guiUser->isInSession();
}

//============================================================================
GuiUser* GuiUserMgr::getUser( VxGUID& onlineId )
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
GuiUser* GuiUserMgr::updateUser( VxNetIdent* hisIdent )
{
    if( !hisIdent )
    {
        LogMsg( LOG_ERROR, "GuiUserMgr::updateUserOnline invalid param" );
        return nullptr;
    }

    if( hisIdent->getMyOnlineId() == m_MyApp.getMyOnlineId() )
    {
        return updateMyIdent( hisIdent );
    }
    else
    {
        GuiUser* guiUser = findUser( hisIdent->getMyOnlineId() );
        if( guiUser && guiUser->getMyOnlineId() == hisIdent->getMyOnlineId() )
        {
            bool wasOnline = guiUser->isOnline();
            guiUser->updateIsOnline();
            if( wasOnline != guiUser->isOnline() )
            {
                onUserOnlineStatusChange( guiUser );
            }
            else
            {
                onUserUpdated( guiUser );
            }
        }
        else
        {
            guiUser = new GuiUser( m_MyApp );
            guiUser->setNetIdent( hisIdent );
            m_UserList[guiUser->getMyOnlineId()] = guiUser;
            guiUser->updateIsOnline();
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
            client->callbackMyIdentUpdated( m_MyIdent );
        }

        m_ClientListBusy = false;
        updateClientList();
    }

    return m_MyIdent;
}

//============================================================================
void GuiUserMgr::setUserOffline( VxGUID& onlineId )
{
    if( isMessengerReady() )
    {
        GuiUser* guiUser = findUser( onlineId );
        if( guiUser )
        {
            guiUser->updateIsOnline();
        }
    }
}

//============================================================================
void GuiUserMgr::onUserAdded( GuiUser* guiUser )
{
    if( isMessengerReady() && guiUser )
    {
        m_ClientListBusy = true;
        for( auto client : m_GuiUserUpdateClientList )
        {
            client->callbackUserAdded( guiUser );
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
            client->callbackUserRemoved( onlineId );
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
        guiUser->updateIsOnline();
        if( guiUser->isOnline() != wasOnline )
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
        guiUser->updateIsOnline();
        m_ClientListBusy = true;
        for( auto client : m_GuiUserUpdateClientList )
        {
            client->callbackOnlineStatusChange( guiUser, guiUser->isOnline() );
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
            client->callbackMyIdentUpdated( guiUser );
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

    if( m_ClientListBusy )
    {
        // the client list is in use. Save and the list will be updated after the client list is no longer in use
        // otherwise will crash because the list is changed during interation of the list
        m_WantUpdateToDoList.push_back( std::make_pair( callback, wantCallback ) );
        return;
    }

    if( wantCallback )
    {
        for( auto client : m_GuiUserUpdateClientList )
        {
            if( client == callback )
            {
                LogMsg( LOG_INFO, "WARNING. Ignoring New wantToGuiUserUpdateCallbacks because already in list" );
                return;
            }
        }

        m_GuiUserUpdateClientList.push_back( callback );
        return;
    }

    for( auto iter = m_GuiUserUpdateClientList.begin(); iter != m_GuiUserUpdateClientList.end(); ++iter )
    {
        GuiUserUpdateCallback* client = *iter;
        if( client == callback )
        {
            m_GuiUserUpdateClientList.erase( iter );
            return;
        }
    }

    LogMsg( LOG_INFO, "WARNING. ToGuiUserUpdateInterface remove not found in list" );
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
        m_ClientListBusy = true;
        for( auto client : m_GuiUserUpdateClientList )
        {
            client->callbackUserUpdated( guiUser );
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
void GuiUserMgr::toGuiUserOnlineStatus( VxNetIdent* netIdent, bool isOnline )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    updateUser( netIdent );
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
        if( client )
        {
            client->callbackPushToTalkStatus( onlineId, pushToTalkStatus );
        }
        else
        {
            LogMsg( LOG_ERROR, "GuiUserMgr::sendUserUpdatedToCallbacks invalid callback" );
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
void GuiUserMgr::connnectIdOnlineStatusChange( VxGUID& onlineId )
{
    GuiUser* guiUser = findUser( onlineId );
    if( guiUser )
    {
        bool wasOnline = guiUser->isOnline();
        guiUser->updateIsOnline();
        if( wasOnline != guiUser->isOnline() )
        {
            LogMsg( LOG_VERBOSE, "GuiUserMgr::connnectIdOnlineStatusChange is online ? %d %s", guiUser->isOnline(), getUserOnlineName( onlineId ).c_str() );
            onUserOnlineStatusChange( guiUser );
        }
        else
        {
            onUserUpdated( guiUser );
        }
    }
}

//============================================================================
void GuiUserMgr::updateClientList( void )
{
    // update client list from que of want callback the happend while client list was busy
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
                m_GuiUserUpdateClientList.push_back( pair.first );
            }
        }
        else
        {
            for( auto iter = m_GuiUserUpdateClientList.begin(); iter != m_GuiUserUpdateClientList.end(); ++iter )
            {
                GuiUserUpdateCallback* client = *iter;
                if( client == pair.first )
                {
                    m_GuiUserUpdateClientList.erase( iter );
                    break;
                }
            }
        }
    }

    m_WantUpdateToDoList.clear();
}