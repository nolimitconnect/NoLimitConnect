//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiConnectIdListMgr.h"

#include "GuiConnectIdListCallback.h"

#include "AppCommon.h"
#include "GuiUserBase.h"

#include <ConnectIdListMgr/ConnectIdListMgr.h>
#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

//============================================================================
GuiConnectIdListMgr::GuiConnectIdListMgr( AppCommon& app )
    : QObject( &app )
    , m_MyApp( app )
{
}

//============================================================================
void GuiConnectIdListMgr::onAppCommonCreated( void )
{
    connect( this, SIGNAL(signalInternalConnectionStatusChange(ConnectId,bool)),          this, SLOT(slotInternalConnectionStatusChange(ConnectId,bool)), Qt::QueuedConnection );
    connect( this, SIGNAL(signalInternalConnectionLost(VxGUID)),                          this, SLOT(slotInternalConnectionLost(VxGUID)), Qt::QueuedConnection );

    m_MyApp.getEngine().getConnectIdListMgr().wantConnectIdListCallback( this, true );
}

//============================================================================
bool GuiConnectIdListMgr::isMessengerReady( void )
{
    return m_MyApp.isMessengerReady();
}

//============================================================================
void GuiConnectIdListMgr::callbackConnectionStatusChange( ConnectId& connectId, bool isConnected )
{
    if( connectId.getUserOnlineId() == m_MyApp.getMyOnlineId() )
    {
        if(LogEnabled(eLogOnline))LogModule( eLogOnline, LOG_ERROR, "GuiConnectIdListMgr::%s updating myself connected %d connectId %s", __func__,
            isConnected, connectId.describeConnectId().c_str() );
    }

    emit signalInternalConnectionStatusChange( connectId, isConnected );
}

//============================================================================
void GuiConnectIdListMgr::slotInternalConnectionStatusChange( ConnectId connectId, bool isConnected )
{
    if(LogEnabled(eLogOnline))LogModule( eLogOnline, LOG_INFO, "GuiConnectIdListMgr::%s is connected %d connectId %s user %s", __func__,
                  isConnected, connectId.describeConnectId().c_str(), m_MyApp.describeUser( connectId.getUserOnlineId() ).c_str() );

    bool wasChanged{ false };
    auto iter = m_ConnectIdList.find( connectId );
    if( isConnected )
    {
        if( iter == m_ConnectIdList.end() )
        {
            wasChanged = true;
            m_ConnectIdList.insert( connectId );
        }
    }
    else
    {
        if( iter != m_ConnectIdList.end() )
        {
            wasChanged = true;
            m_ConnectIdList.erase( iter );
        }      
    }

    if( wasChanged )
    {
        announceConnectionStatusChange( connectId, isConnected );
        // force gui update
        m_MyApp.getUserMgr().refreshUser( connectId.getUserOnlineId() );
    }
}

//============================================================================
void GuiConnectIdListMgr::callbackConnectionLost( VxGUID& sktConnectId )
{
    emit signalInternalConnectionLost( sktConnectId );
}

//============================================================================
void GuiConnectIdListMgr::slotInternalConnectionLost( VxGUID sktConnectId )
{
    if(LogEnabled(eLogOnline))LogModule( eLogOnline, LOG_INFO, "GuiConnectIdListMgr::%s sktConnectId %s", __func__,
                  sktConnectId.toHexString().c_str() );

    for( auto iter = m_ConnectIdList.begin(); iter != m_ConnectIdList.end();  )
    {
        if( ( *iter ).getSocketId() == sktConnectId )
        {
            ConnectId connectId = *iter;
            iter = m_ConnectIdList.erase( iter );
            announceConnectionStatusChange( connectId, false );
            // force gui update
            m_MyApp.getUserMgr().refreshUser( connectId.getUserOnlineId() );
        }
        else
        {
            iter++;
        }
    }
}

//============================================================================
void GuiConnectIdListMgr::announceConnectionStatusChange( ConnectId& connectId, bool isConnected )
{
    if(LogEnabled(eLogOnline))LogModule( eLogOnline, LOG_VERBOSE, "GuiConnectIdListMgr::%s user %s isConnected %d", __func__,
                m_MyApp.describeUser( connectId.getUserOnlineId() ).c_str(), isConnected );
    if( connectId.isValid() )
    {
        for( auto iter = m_GuiConnectIdClientList.begin(); iter != m_GuiConnectIdClientList.end(); ++iter )
        {
            GuiConnectIdListCallback* client = *iter;
            if( client )
            {
                client->callbackConnectionStatusChange( connectId, isConnected );
            }
            else
            {
                LogMsg( LOG_ERROR, "GuiConnectIdListMgr::%s invalid callback", __func__ );
            }
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiConnectIdListMgr::announceConnectionStatusChange invalid connectId" );
    }
}

//============================================================================
void GuiConnectIdListMgr::wantGuiConnectIdCallbacks( GuiConnectIdListCallback* callback, bool wantCallback )
{
    static bool userCallbackShutdownComplete = false;
    if( VxIsAppShuttingDown() )
    {
        if( userCallbackShutdownComplete )
        {
            return;
        }

        userCallbackShutdownComplete = true;
        m_GuiConnectIdClientList.clear();
        return;
    }

    if( wantCallback )
    {
        for( auto& client : m_GuiConnectIdClientList )
        {
            if( client == callback )
            {
                LogMsg( LOG_INFO, "WARNING. Ignoring New wantGuiConnectIdCallbacks because already in list" );
                return;
            }
        }

        m_GuiConnectIdClientList.emplace_back( callback );
        return;
    }

    for( auto iter = m_GuiConnectIdClientList.begin(); iter != m_GuiConnectIdClientList.end(); ++iter )
    {
        if( *iter == callback )
        {
            m_GuiConnectIdClientList.erase( iter );
            return;
        }
    }

    LogMsg( LOG_INFO, "WARNING. wantGuiConnectIdCallbacks remove not found in list" );
}

//============================================================================
bool GuiConnectIdListMgr::isDirectConnect( VxGUID& onlineId )
{
    bool isDirectConnect{ false };
    if( onlineId.isValid() )
    {
        if( onlineId == m_MyApp.getMyOnlineId() )
        {
            return true;
        }

        for( auto& connectIdIn : m_ConnectIdList )
        {
            ConnectId& connectId = const_cast<ConnectId&>( connectIdIn );
            if( connectId.getUserOnlineId() == onlineId && !connectId.isRelayed() )
            {
                isDirectConnect = true;
                break;
            }
        }
    }
    else
    {
         LogMsg( LOG_ERROR, "GuiConnectIdListMgr::isDirectConnect invalid id" );
    }

    return isDirectConnect;
}

//============================================================================
bool GuiConnectIdListMgr::isRelayed( VxGUID& onlineId )
{
    if( !onlineId.isValid() )
    {
        LogMsg( LOG_ERROR, "GuiConnectIdListMgr::isRelayed invalid id" );
        return false;
    }

    if( onlineId == m_MyApp.getMyOnlineId() )
    {
        return false;
    }

    if( isDirectConnect( onlineId ) )
    {
        return false;
    }

    bool isRelayed{ false };
    for( auto &connectIdIn : m_ConnectIdList )
    {
        if( const_cast<ConnectId&>( connectIdIn ).getUserOnlineId() == onlineId && connectIdIn.isRelayed() )
        {
            isRelayed = true;
            break;
        }
    }

    return isRelayed;
}

//============================================================================
bool GuiConnectIdListMgr::isConnected( GroupieId& groupieId )
{
    bool isConnected{ false };
    for( auto& connectIdIn : m_ConnectIdList )
    {
        if( const_cast<ConnectId&>( connectIdIn ).getGroupieId() == groupieId )
        {
            isConnected = true;
            break;
        }
    }

    return isConnected;
}

//============================================================================
bool GuiConnectIdListMgr::isOnline( VxGUID& onlineId )
{
    if( onlineId == m_MyApp.getMyOnlineId() )
    {
        return true;
    }

    bool isOnline{ false };
    for( auto& connectIdIn : m_ConnectIdList )
    {
        if( const_cast<ConnectId&>( connectIdIn ).getUserOnlineId() == onlineId )
        {
            isOnline = true;
            break;
        }
    }

    return isOnline;
}

//============================================================================
void GuiConnectIdListMgr::dumpOnlineUsers()
{
    LogMsg( LOG_VERBOSE, "GuiConnectIdListMgr::%s connnected %zu",  __func__, m_ConnectIdList.size() );
    int idCnt = 0;
    for( auto& connectId : m_ConnectIdList )
    {
        idCnt++;
        LogMsg( LOG_VERBOSE, "GuiConnectIdListMgr::%s connected %d - %s %s",  __func__,
               idCnt, connectId.describeConnectId().c_str(), m_MyApp.describeUser( ((ConnectId)connectId).getUserOnlineId() ).c_str() );
    }
}

//============================================================================
void GuiConnectIdListMgr::dumpHostedUsers( HostedId hostId )
{
    LogMsg( LOG_VERBOSE, "GuiConnectIdListMgr::%s connnected %zu", __func__, m_ConnectIdList.size() );
    int idCnt = 0;
    for( auto& connectId : m_ConnectIdList )
    {
        if( ( (ConnectId)connectId ).getHostedId() == hostId )
        {
            idCnt++;
            LogMsg( LOG_VERBOSE, "GuiConnectIdListMgr::%s connected %d - %s %s", __func__,
                idCnt, connectId.describeConnectId().c_str(), m_MyApp.describeUser( ( (ConnectId)connectId ).getUserOnlineId() ).c_str() );
        }
    }
}