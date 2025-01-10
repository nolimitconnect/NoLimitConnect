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
    if(LogEnabled(eLogUsers))LogMsg( LOG_INFO, "AppCommon::toGuiIndentListUpdate %d", listType );
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

    LogModule( eLogUserEvent, LOG_VERBOSE, " AppCommon::toGuiContactAdded %s id %s my friendship %s his friendship %s", 
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
void AppCommon::doOnlineStatusChange( VxGUID onlineId, bool isOnline )
{
    getUserMgr().toGuiOnlineStatusChange( onlineId, isOnline );

    GuiUser* guiUser = m_UserMgr.getOrQueryUser( onlineId );
    if( guiUser )
    {
        LogModule( eLogUserEvent, LOG_VERBOSE, " AppCommon::slotInternalToGuiOnlineStatusChange %s id %s my friendship %s his friendship %s",
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

    LogModule( eLogUserEvent, LOG_VERBOSE, " AppCommon::toGuiContactOnline %s id %s my friendship %s his friendship %s", 
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

    if( !netIdent.isValidNetIdent() )
    {
        LogMsg( LOG_ERROR, "AppCommon::slotInternalToGuiContactOnline invalide ident" );
        return;
    }

    bool wasOnline = getConnectIdListMgr().isOnline( netIdent.getMyOnlineId() );
    GuiUser* guiUser = getUserMgr().updateUser( &netIdent );
    bool isOnline = getConnectIdListMgr().isOnline( netIdent.getMyOnlineId() );
    if( guiUser && wasOnline != isOnline && isOnline )
    {
        m_ToGuiActivityInterfaceBusy = true;
        for( auto client : m_ToGuiActivityInterfaceList )
        {
            client->toGuiContactOnline( guiUser );
        }

        m_ToGuiActivityInterfaceBusy = false;
    }
}

//============================================================================
//! called when any contact info changes ( including any of the above )
void AppCommon::toGuiContactAnythingChange( VxNetIdent* netIdent )
{
    if( VxIsAppShuttingDown() )
    {
        return;
    }

    LogModule( eLogUserEvent, LOG_VERBOSE, " AppCommon::toGuiContactAnythingChange %s id %s my friendship %s his friendship %s", 
               netIdent->getOnlineName(), netIdent->getMyOnlineId().toOnlineIdString().c_str(),
               DescribeFriendState( netIdent->getMyFriendshipToHim()),  DescribeFriendState( netIdent->getHisFriendshipToMe()) );

    emit signalInternalToGuiContactUpdated( *netIdent );
}

//============================================================================
void AppCommon::slotInternalToGuiContactUpdated( VxNetIdent netIdent )
{
    getUserMgr().toGuiContactUpdated( &netIdent );
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

    LogModule( eLogUserEvent, LOG_VERBOSE, " AppCommon::toGuiContactLastSessionTimeChange %s id %s my friendship %s his friendship %s", 
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

    LogModule( eLogUserEvent, LOG_VERBOSE, " AppCommon::toGuiUpdateMyIdent %s id %s my friendship %s his friendship %s", 
               netIdent->getOnlineName(), netIdent->getMyOnlineId().toOnlineIdString().c_str(),
               DescribeFriendState( netIdent->getMyFriendshipToHim()),  DescribeFriendState( netIdent->getHisFriendshipToMe()) );

    emit signalInternalToGuiUpdateIdent( *netIdent );
}

//============================================================================
void AppCommon::slotInternalToGuiUpdateIdent( VxNetIdent netIdent )
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

    LogModule( eLogUserEvent, LOG_VERBOSE, " AppCommon::toGuiSaveMyIdent %s id %s my friendship %s his friendship %s", 
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
