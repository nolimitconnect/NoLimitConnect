//============================================================================
// Copyright (C) 2009 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "MyIcons.h"
#include "AppCommon.h"
#include "AppGlobals.h"
#include "AccountMgr.h"
#include "ToGuiActivityInterface.h"

#include <CoreLib/VxGlobals.h>

//============================================================================
void AppCommon::updateFriendList( GuiUser* guiUser, bool sessionTimeChange )
{
}

//============================================================================
void AppCommon::toGuiIndentListUpdate( EUserViewType listType, VxGUID& onlineId, uint64_t timestamp )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    emit signalInternalToGuiIndentListUpdate( listType, onlineId, timestamp );
}

//============================================================================
void AppCommon::slotInternalToGuiIndentListUpdate( EUserViewType listType, VxGUID onlineId, uint64_t timestamp )
{
    LogMsg( LOG_INFO, "AppCommon::toGuiIndentListUpdate %d", listType );
    m_ToGuiUserUpdateClientBusy = true;
    for( auto iter = m_ToGuiUserUpdateClientList.begin(); iter != m_ToGuiUserUpdateClientList.end(); ++iter )
    {
        ToGuiUserUpdateInterface* client = *iter;
        client->toGuiIndentListUpdate( listType, onlineId, timestamp );
    }

    m_ToGuiUserUpdateClientBusy = false;
    getUserMgr().toGuiIndentListUpdate( listType, onlineId, timestamp );
}

//============================================================================
void AppCommon::toGuiIndentListRemove( EUserViewType listType, VxGUID& onlineId )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    emit signalInternalToGuiIndentListRemove( listType, onlineId );
}

//============================================================================
void AppCommon::slotInternalToGuiIndentListRemove( EUserViewType listType, VxGUID onlineId )
{
    LogMsg( LOG_INFO, "AppCommon::toGuiIndentListRemove %d", listType );
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    m_ToGuiUserUpdateClientBusy = true;
    for( auto iter = m_ToGuiUserUpdateClientList.begin(); iter != m_ToGuiUserUpdateClientList.end(); ++iter )
    {
        ToGuiUserUpdateInterface* client = *iter;
        client->toGuiIndentListRemove( listType, onlineId);
    }

    m_ToGuiUserUpdateClientBusy = false;
    getUserMgr().toGuiIndentListRemove( listType, onlineId );
}

//============================================================================
void AppCommon::toGuiContactAdded( VxNetIdent* netIdent )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiContactAdded invalid netIdent" );
        return;
    }

    LogModule( eLogUserGuiEvent, LOG_VERBOSE, " AppCommon::toGuiContactAdded %s id %s my friendship %s his friendship %s", 
               netIdent->getOnlineName(), netIdent->getMyOnlineId().toOnlineIdString().c_str(),
               DescribeFriendState( netIdent->getMyFriendshipToHim()),  DescribeFriendState( netIdent->getHisFriendshipToMe()) );

    emit signalInternalToGuiContactAdded( *netIdent );
}

//============================================================================
void AppCommon::slotInternalToGuiContactAdded( VxNetIdent netIdent )
{
    getUserMgr().toGuiContactAdded( &netIdent );
    GuiUser* user = getUserMgr().getUser( netIdent.getMyOnlineId() );
    if( user )
    {
        m_ToGuiActivityInterfaceBusy = true;
        for( auto client : m_ToGuiActivityInterfaceList )
        {
            client->toGuiContactAdded( user );
        }

        m_ToGuiActivityInterfaceBusy = false;
    }
}

//============================================================================
void AppCommon::toGuiContactRemoved( VxGUID& onlineId )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    emit signalInternalToGuiContactRemoved( onlineId );
}

//============================================================================
void AppCommon::slotInternalToGuiContactRemoved( VxGUID onlineId )
{
    m_ToGuiActivityInterfaceBusy = true;
    for( auto client : m_ToGuiActivityInterfaceList )
    {
        client->toGuiContactRemoved( onlineId );
    }

    m_ToGuiActivityInterfaceBusy = false;
    getUserMgr().toGuiContactRemoved( onlineId );
}

//============================================================================
void AppCommon::toGuiUserOnlineStatusChange( VxGUID& onlineId, bool isOnline )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

	emit signalInternalToGuiOnlineStatusChange( onlineId, isOnline );
}

//============================================================================
void AppCommon::slotInternalToGuiOnlineStatusChange( VxGUID onlineId, bool isOnline )
{
    getUserMgr().toGuiOnlineStatusChange( onlineId, isOnline );

    GuiUser* guiUser = m_UserMgr.getOrQueryUser( onlineId );
    if( guiUser )
    {
        LogModule( eLogUserGuiEvent, LOG_VERBOSE, " AppCommon::slotInternalToGuiOnlineStatusChange %s id %s my friendship %s his friendship %s",
                  guiUser->getOnlineName().c_str(), guiUser->getMyOnlineId().toOnlineIdString().c_str(),
                DescribeFriendState( guiUser->getMyFriendshipToHim()),  DescribeFriendState( guiUser->getHisFriendshipToMe()) );
        m_ToGuiActivityInterfaceBusy = true;
	    for( auto client : m_ToGuiActivityInterfaceList )
	    {
            if( isOnline )
            {
                client->toGuiContactOnline( guiUser );
            }
            else
            {
                client->toGuiContactOffline( guiUser );
            }
	    }

        m_ToGuiActivityInterfaceBusy = false;
    }
    else
    {
        // this may be normal if online user is host or service like network host
        LogMsg( LOG_VERBOSE, "AppCommon::slotInternalToGuiOnlineStatusChange null user %s", onlineId.toOnlineIdString().c_str() );
    } 
}

//============================================================================
void AppCommon::toGuiContactOnline( VxNetIdent* netIdent )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiContactOnline invalid netIdent" );
        return;
    }

    LogModule( eLogUserGuiEvent, LOG_VERBOSE, " AppCommon::toGuiContactOnline %s id %s my friendship %s his friendship %s", 
               netIdent->getOnlineName(), netIdent->getMyOnlineId().toOnlineIdString().c_str(),
               DescribeFriendState( netIdent->getMyFriendshipToHim()),  DescribeFriendState( netIdent->getHisFriendshipToMe()) );

    emit signalInternalToGuiContactOnline( *netIdent );
}

//============================================================================
void AppCommon::slotInternalToGuiContactOnline( VxNetIdent netIdent )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    GuiUser* user = getUserMgr().updateUser( &netIdent, true );

    m_ToGuiActivityInterfaceBusy = true;
    for( auto client : m_ToGuiActivityInterfaceList )
    {
        client->toGuiContactOnline( user );
    }

    m_ToGuiActivityInterfaceBusy = false;
}

//============================================================================
void AppCommon::toGuiContactNameChange( VxNetIdent* netIdent )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiContactNameChange invalid netIdent" );
        return;
    }

    LogModule( eLogUserGuiEvent, LOG_VERBOSE, " AppCommon::toGuiContactNameChange %s id %s my friendship %s his friendship %s", 
               netIdent->getOnlineName(), netIdent->getMyOnlineId().toOnlineIdString().c_str(),
               DescribeFriendState( netIdent->getMyFriendshipToHim()),  DescribeFriendState( netIdent->getHisFriendshipToMe()) );

    emit signalInternalToGuiContactNameChange( *netIdent );
}

//============================================================================
void AppCommon::slotInternalToGuiContactNameChange( VxNetIdent netIdent )
{
	LogMsg( LOG_INFO, "AppCommon::toGuiContactNameChange %s", netIdent.getOnlineName());
    getUserMgr().toGuiContactNameChange( &netIdent );
    m_ToGuiUserUpdateClientBusy = true;
    for( auto iter = m_ToGuiUserUpdateClientList.begin(); iter != m_ToGuiUserUpdateClientList.end(); ++iter )
    {
        ToGuiUserUpdateInterface* client = *iter;
        client->toGuiContactNameChange( &netIdent );
    }

    m_ToGuiUserUpdateClientBusy = false;
}

//============================================================================
//! called when description changes
void AppCommon::toGuiContactDescChange( VxNetIdent* netIdent )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiContactDescChange invalid netIdent" );
        return;
    }

    LogModule( eLogUserGuiEvent, LOG_VERBOSE, " AppCommon::toGuiContactDescChange %s id %s my friendship %s his friendship %s", 
               netIdent->getOnlineName(), netIdent->getMyOnlineId().toOnlineIdString().c_str(),
               DescribeFriendState( netIdent->getMyFriendshipToHim()),  DescribeFriendState( netIdent->getHisFriendshipToMe()) );

    emit signalInternalToGuiContactDescChange( *netIdent );
}

//============================================================================
void AppCommon::slotInternalToGuiContactDescChange( VxNetIdent netIdent )
{
	LogMsg( LOG_INFO, "AppCommon::toGuiContactDescChange %s", netIdent.getOnlineName());

    getUserMgr().toGuiContactDescChange( &netIdent );
    for( auto iter = m_ToGuiUserUpdateClientList.begin(); iter != m_ToGuiUserUpdateClientList.end(); ++iter )
    {
        ToGuiUserUpdateInterface* client = *iter;
        client->toGuiContactDescChange( &netIdent );
    }
}

//============================================================================
//! called when my friendship to him changes
void AppCommon::toGuiContactMyFriendshipChange( VxNetIdent* netIdent )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiContactMyFriendshipChange invalid netIdent" );
        return;
    }

    LogModule( eLogUserGuiEvent, LOG_VERBOSE, " AppCommon::toGuiContactMyFriendshipChange %s id %s my friendship %s his friendship %s", 
               netIdent->getOnlineName(), netIdent->getMyOnlineId().toOnlineIdString().c_str(),
               DescribeFriendState( netIdent->getMyFriendshipToHim()),  DescribeFriendState( netIdent->getHisFriendshipToMe()) );

    emit signalInternalToGuiContactMyFriendshipChange( *netIdent );
}

//============================================================================
void AppCommon::slotInternalToGuiContactMyFriendshipChange( VxNetIdent netIdent )
{
	LogMsg( LOG_INFO, "AppCommon::toGuiContactMyFriendshipChange %s", netIdent.getOnlineName());
    getUserMgr().toGuiContactMyFriendshipChange( &netIdent );
    m_ToGuiUserUpdateClientBusy = true;
    for( auto iter = m_ToGuiUserUpdateClientList.begin(); iter != m_ToGuiUserUpdateClientList.end(); ++iter )
    {
        ToGuiUserUpdateInterface* client = *iter;
        client->toGuiContactMyFriendshipChange( &netIdent );
    }

    m_ToGuiUserUpdateClientBusy = false;
}

//============================================================================
//! called when his friendship to me changes
void AppCommon::toGuiContactHisFriendshipChange( VxNetIdent* netIdent )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiContactHisFriendshipChange invalid netIdent" );
        return;
    }

    LogModule( eLogUserGuiEvent, LOG_VERBOSE, " AppCommon::toGuiContactHisFriendshipChange %s id %s my friendship %s his friendship %s", 
               netIdent->getOnlineName(), netIdent->getMyOnlineId().toOnlineIdString().c_str(),
               DescribeFriendState( netIdent->getMyFriendshipToHim()),  DescribeFriendState( netIdent->getHisFriendshipToMe()) );

    emit signalInternalToGuiContactHisFriendshipChange( *netIdent );
}

//============================================================================
void AppCommon::slotInternalToGuiContactHisFriendshipChange( VxNetIdent netIdent )
{
    getUserMgr().toGuiContactHisFriendshipChange( &netIdent );
    m_ToGuiUserUpdateClientBusy = true;
    for( auto iter = m_ToGuiUserUpdateClientList.begin(); iter != m_ToGuiUserUpdateClientList.end(); ++iter )
    {
        ToGuiUserUpdateInterface* client = *iter;
        client->toGuiContactHisFriendshipChange( &netIdent );
    }

    m_ToGuiUserUpdateClientBusy = false;
}

//============================================================================
//! called when plugin permission changes
void AppCommon::toGuiPluginPermissionChange( VxNetIdent* netIdent )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiPluginPermissionChange invalid netIdent" );
        return;
    }

    LogModule( eLogUserGuiEvent, LOG_VERBOSE, " AppCommon::toGuiPluginPermissionChange %s id %s my friendship %s his friendship %s", 
               netIdent->getOnlineName(), netIdent->getMyOnlineId().toOnlineIdString().c_str(),
               DescribeFriendState( netIdent->getMyFriendshipToHim()),  DescribeFriendState( netIdent->getHisFriendshipToMe()) );

    emit signalInternalToGuiPluginPermissionChange( *netIdent );
}

//============================================================================
void AppCommon::slotInternalToGuiPluginPermissionChange( VxNetIdent netIdent )
{
    getUserMgr().toGuiPluginPermissionChange( &netIdent );
    m_ToGuiUserUpdateClientBusy = true;
    for( auto iter = m_ToGuiUserUpdateClientList.begin(); iter != m_ToGuiUserUpdateClientList.end(); ++iter )
    {
        ToGuiUserUpdateInterface* client = *iter;
        client->toGuiPluginPermissionChange( &netIdent );
    }

    m_ToGuiUserUpdateClientBusy = false;
}

//============================================================================
//! called when search flags changes
void AppCommon::toGuiContactSearchFlagsChange( VxNetIdent* netIdent )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiContactSearchFlagsChange invalid netIdent" );
        return;
    }

    LogModule( eLogUserGuiEvent, LOG_VERBOSE, " AppCommon::toGuiContactSearchFlagsChange %s id %s my friendship %s his friendship %s", 
               netIdent->getOnlineName(), netIdent->getMyOnlineId().toOnlineIdString().c_str(),
               DescribeFriendState( netIdent->getMyFriendshipToHim()),  DescribeFriendState( netIdent->getHisFriendshipToMe()) );

    emit signalInternalToGuiContactSearchFlagsChange( *netIdent );
}

//============================================================================
void AppCommon::slotInternalToGuiContactSearchFlagsChange( VxNetIdent netIdent )
{
    getUserMgr().toGuiContactSearchFlagsChange( &netIdent );
    m_ToGuiUserUpdateClientBusy = true;
    for( auto iter = m_ToGuiUserUpdateClientList.begin(); iter != m_ToGuiUserUpdateClientList.end(); ++iter )
    {
        ToGuiUserUpdateInterface* client = *iter;
        client->toGuiContactSearchFlagsChange( &netIdent );
    }    

    m_ToGuiUserUpdateClientBusy = false;
}

//============================================================================
//! called when connection info changes
void AppCommon::toGuiContactConnectionChange( VxNetIdent* netIdent )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	// BRJ is this needed??
    LogModule( eLogUserGuiEvent, LOG_VERBOSE, " AppCommon::toGuiContactConnectionChange %s id %s my friendship %s his friendship %s", 
               netIdent->getOnlineName(), netIdent->getMyOnlineId().toOnlineIdString().c_str(),
               DescribeFriendState( netIdent->getMyFriendshipToHim()),  DescribeFriendState( netIdent->getHisFriendshipToMe()) );
}

//============================================================================
//! called when any contact info changes ( including any of the above )
void AppCommon::toGuiContactAnythingChange( VxNetIdent* netIdent )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    // BRJ is this needed??
    LogModule( eLogUserGuiEvent, LOG_VERBOSE, " AppCommon::toGuiContactAnythingChange %s id %s my friendship %s his friendship %s", 
               netIdent->getOnlineName(), netIdent->getMyOnlineId().toOnlineIdString().c_str(),
               DescribeFriendState( netIdent->getMyFriendshipToHim()),  DescribeFriendState( netIdent->getHisFriendshipToMe()) );
}

//============================================================================
//! called when any contact info changes ( including any of the above )
void AppCommon::toGuiContactLastSessionTimeChange( VxNetIdent* netIdent )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiContactLastSessionTimeChange invalid netIdent" );
        return;
    }

    LogModule( eLogUserGuiEvent, LOG_VERBOSE, " AppCommon::toGuiContactLastSessionTimeChange %s id %s my friendship %s his friendship %s", 
               netIdent->getOnlineName(), netIdent->getMyOnlineId().toOnlineIdString().c_str(),
               DescribeFriendState( netIdent->getMyFriendshipToHim()),  DescribeFriendState( netIdent->getHisFriendshipToMe()) );

    emit signalInternalToGuiContactLastSessionTimeChange( *netIdent );
}

//============================================================================
void AppCommon::slotInternalToGuiContactLastSessionTimeChange( VxNetIdent netIdent )
{
	LogMsg( LOG_INFO, "AppCommon::toGuiContactLastSessionTimeChange %s", netIdent.getOnlineName());
    m_ToGuiUserUpdateClientBusy = true;
    for( auto iter = m_ToGuiUserUpdateClientList.begin(); iter != m_ToGuiUserUpdateClientList.end(); ++iter )
    {
        ToGuiUserUpdateInterface* client = *iter;
        client->toGuiContactLastSessionTimeChange( &netIdent );
    }

    m_ToGuiUserUpdateClientBusy = false;
}

//============================================================================
void AppCommon::toGuiUpdateMyIdent( VxNetIdent* netIdent )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiUpdateMyIdent invalid netIdent" );
        return;
    }

    LogModule( eLogUserGuiEvent, LOG_VERBOSE, " AppCommon::toGuiUpdateMyIdent %s id %s my friendship %s his friendship %s", 
               netIdent->getOnlineName(), netIdent->getMyOnlineId().toOnlineIdString().c_str(),
               DescribeFriendState( netIdent->getMyFriendshipToHim()),  DescribeFriendState( netIdent->getHisFriendshipToMe()) );

    emit signalInternalToGuiUpdateMyIdent( *netIdent );
}

//============================================================================
void AppCommon::slotInternalToGuiUpdateMyIdent( VxNetIdent netIdent )
{
    getUserMgr().toGuiUpdateMyIdent( &netIdent );
}

//============================================================================
void AppCommon::toGuiSaveMyIdent( VxNetIdent* netIdent )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    if( !netIdent || !netIdent->isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::toGuiSaveMyIdent invalid netIdent" );
        return;
    }

    LogModule( eLogUserGuiEvent, LOG_VERBOSE, " AppCommon::toGuiSaveMyIdent %s id %s my friendship %s his friendship %s", 
               netIdent->getOnlineName(), netIdent->getMyOnlineId().toOnlineIdString().c_str(),
               DescribeFriendState( netIdent->getMyFriendshipToHim()),  DescribeFriendState( netIdent->getHisFriendshipToMe()) );

    emit signalInternalToGuiSaveMyIdent( *netIdent );
}

//============================================================================
void AppCommon::slotInternalToGuiSaveMyIdent( VxNetIdent netIdent )
{
    getUserMgr().toGuiSaveMyIdent( &netIdent );
}

//============================================================================
//! called when my identity changes
void AppCommon::onUpdateMyIdent( VxNetIdent* netIdent )
{
    if( netIdent )
    {
	    memcpy( getAppGlobals().getMyNetIdent(), netIdent, sizeof( VxNetIdent ) );
	    m_AccountMgr.updateAccount(*netIdent);
    }
}
