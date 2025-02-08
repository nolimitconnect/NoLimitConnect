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
#include "GuiOnlineStatusCallback.h"

#include "AppCommon.h"
#include "GuiUserBase.h"
#include "GuiParams.h"

#include <ConnectIdListMgr/ConnectIdListMgr.h>
#include <P2PEngine/P2PEngine.h>

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
    connect( this, SIGNAL( signalInternalRelayStatusChange( ConnectId,bool) ),              this, SLOT( slotInternalRelayStatusChange( ConnectId,bool) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalOnlineStatusChange(VxGUID,bool) ),                 this, SLOT( slotInternalOnlineStatusChange(VxGUID,bool) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalConnectionStatusChange(ConnectId,bool) ),          this, SLOT( slotInternalConnectionStatusChange(ConnectId,bool) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalConnectionReason(VxGUID,EConnectReason,bool) ),    this, SLOT( slotInternalConnectionReason(VxGUID,EConnectReason,bool) ), Qt::QueuedConnection );
    connect( this, SIGNAL( signalInternalConnectionLost(VxGUID) ),                          this, SLOT( slotInternalConnectionLost(VxGUID) ), Qt::QueuedConnection );

    m_MyApp.getEngine().getConnectIdListMgr().wantConnectIdListCallback( this, true );
}

//============================================================================
bool GuiConnectIdListMgr::isMessengerReady( void )
{
    return m_MyApp.isMessengerReady();
}

//============================================================================
void GuiConnectIdListMgr::callbackRelayStatusChange( ConnectId& connectId, bool isRelayed )
{
    if( connectId.getUserOnlineId() == m_MyApp.getMyOnlineId() )
    {
        LogModule( eLogRelay, LOG_VERBOSE, "GuiConnectIdListMgr::callbackRelayStatusChange updating myself" );
    }

    emit signalInternalRelayStatusChange( connectId, isRelayed );
}

//============================================================================
void GuiConnectIdListMgr::callbackConnectionStatusChange( ConnectId& connectId, bool isConnected )
{
    if( connectId.getUserOnlineId() == m_MyApp.getMyOnlineId() )
    {
        LogMsg( LOG_ERROR, "GuiConnectIdListMgr::callbackConnectionStatusChange updating myself" );
    }

    emit signalInternalConnectionStatusChange( connectId, isConnected );
}

//============================================================================
void GuiConnectIdListMgr::callbackOnlineStatusChange( VxGUID& onlineId, bool isOnline )
{
    if( onlineId == m_MyApp.getMyOnlineId() )
    {
        LogMsg( LOG_ERROR, "GuiConnectIdListMgr::callbackConnectionStatusChange updating myself" );
    }

    emit signalInternalOnlineStatusChange( onlineId, isOnline );
}

//============================================================================
void GuiConnectIdListMgr::callbackConnectionReason( VxGUID& sktConnectId, EConnectReason connectReason, bool enableReason )
{
    emit signalInternalConnectionReason( sktConnectId, connectReason, enableReason );
}

//============================================================================
void GuiConnectIdListMgr::callbackConnectionLost( VxGUID& sktConnectId )
{
    emit signalInternalConnectionLost( sktConnectId );
}

//============================================================================
void GuiConnectIdListMgr::slotInternalRelayStatusChange( ConnectId connectId, bool isNowRelayed )
{
    LogModule( eLogRelay, LOG_VERBOSE, "GuiConnectIdListMgr::slotInternalRelayStatusChange user %s isRelayed %d", 
            m_MyApp.describeUser( connectId.getUserOnlineId() ).c_str(), isNowRelayed );
    auto iter = m_RelayedIdList.find( connectId );
    if( iter != m_RelayedIdList.end() )
    {
        if( !isNowRelayed )
        {
            m_RelayedIdList.erase( iter );
        }
    }
    else
    {
        if( isNowRelayed )
        {
            m_RelayedIdList.insert( connectId );
        }
    }

    onRelayStatusChange( connectId.getUserOnlineId(), isRelayed( connectId.getUserOnlineId() ) );
}

//============================================================================
void GuiConnectIdListMgr::slotInternalOnlineStatusChange( VxGUID onlineId, bool isOnline )
{
    LogModule( eLogUserEvent, LOG_VERBOSE, "GuiConnectIdListMgr::%s user %s isOnline %d",
            __func__, m_MyApp.describeUser( onlineId ).c_str(), isOnline );
    m_MyApp.doOnlineStatusChange( onlineId, isOnline );
}

//============================================================================
void GuiConnectIdListMgr::slotInternalConnectionStatusChange( ConnectId connectId, bool isConnected )
{
    LogModule( eLogUserEvent, LOG_VERBOSE, "GuiConnectIdListMgr::%s user %s isConnect %d to %s",
            __func__, m_MyApp.describeUser( connectId.getUserOnlineId() ).c_str(), isConnected,
            GuiParams::describeHostType( connectId.getHostType() ).toUtf8().constData() );
    if( isConnected )
    {
        if( m_ConnectIdList.find( connectId ) == m_ConnectIdList.end() )
        {
            m_ConnectIdList.insert( connectId );
        }

        onConnectionStatusChange( connectId, isConnected );
    }
    else
    {
        auto iter = m_ConnectIdList.find( connectId );
        if( iter != m_ConnectIdList.end() )
        {
            m_ConnectIdList.erase( iter );
        }

        onConnectionStatusChange( connectId, isConnected );
    }
}

//============================================================================
void GuiConnectIdListMgr::slotInternalConnectionReason( VxGUID socketId, EConnectReason connectReason, bool enableReason )
{
    auto iter = m_ConnectReasonList.find( socketId );
    if( iter != m_ConnectReasonList.end() )
    {
        if( enableReason )
        {
            iter->second.insert( connectReason );
        }
        else
        {
            auto iterReason = iter->second.find( connectReason );
            if( iterReason != iter->second.end() )
            {
                iter->second.erase( iterReason );
            }
        }
    }
    else
    {
        std::set<EConnectReason> reasonSet{ connectReason };
        m_ConnectReasonList[socketId] = reasonSet;
    }
}

//============================================================================
void GuiConnectIdListMgr::slotInternalConnectionLost( VxGUID socketId )
{
    auto iter = m_ConnectReasonList.find( socketId );
    if( iter != m_ConnectReasonList.end() )
    {
        m_ConnectReasonList.erase( iter );
    }
}

//============================================================================
void GuiConnectIdListMgr::onRelayStatusChange( VxGUID& onlineId, bool isRelayed )
{
    announceRelayStatusChange( onlineId, isRelayed );
}

//============================================================================
void GuiConnectIdListMgr::onOnlineStatusChange( VxGUID& onlineId, bool isOnline )
{
    announceOnlineStatusChange( onlineId, isOnline );
}

//============================================================================
void GuiConnectIdListMgr::announceOnlineStatusChange( VxGUID& onlineId, bool isOnline )
{
    LogModule( eLogUserEvent, LOG_VERBOSE, "GuiConnectIdListMgr::announceOnlineStatusChange user %s isOnline %d", 
                m_MyApp.describeUser( onlineId ).c_str(), isOnline );
    if( onlineId.isVxGUIDValid() )
    {
        for( auto client : m_GuiConnectIdClientList )
        {
            if( client )
            {
                client->callbackOnlineStatusChange( onlineId, isOnline );
            }
            else
            {
                LogMsg( LOG_ERROR, "GuiConnectIdListMgr::announceOnlineStatusChange invalid callback" );
            }
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiConnectIdListMgr::announceOnlineStatusChange invalid onlineId" );
    }
}

//============================================================================
void GuiConnectIdListMgr::announceRelayStatusChange( VxGUID& onlineId, bool isRelayed )
{
    if( onlineId.isVxGUIDValid() )
    {
        LogModule( eLogUserEvent, LOG_VERBOSE, "GuiConnectIdListMgr::announceRelayStatusChange user %s isRelayed %d", 
                m_MyApp.describeUser( onlineId ).c_str(), isRelayed );

        for( auto iter = m_GuiConnectIdClientList.begin(); iter != m_GuiConnectIdClientList.end(); ++iter )
        {
            GuiConnectIdListCallback* client = *iter;
            if( client )
            {
                client->callbackRelayStatusChange( onlineId, isRelayed );
            }
            else
            {
                LogMsg( LOG_ERROR, "GuiConnectIdListMgr::announceRelayStatusChange invalid callback" );
            }
        }
    }
    else
    {
        LogMsg( LOG_ERROR, "GuiConnectIdListMgr::announceRelayStatusChange invalid onlineId" );
    }
}

//============================================================================
void GuiConnectIdListMgr::onConnectionStatusChange( ConnectId& connectId, bool isConnected )
{
    announceConnectionStatusChange( connectId, isConnected );
}

//============================================================================
void GuiConnectIdListMgr::announceConnectionStatusChange( ConnectId& connectId, bool isConnected )
{
    LogModule( eLogUserEvent, LOG_VERBOSE, "GuiConnectIdListMgr::announceConnectionStatusChange user %s isConnected %d", 
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
                LogMsg( LOG_ERROR, "GuiConnectIdListMgr::announceConnectionStatusChange invalid callback" );
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
        for( auto client : m_GuiConnectIdClientList )
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
void GuiConnectIdListMgr::wantGuiOnlineStatusCallbacks( GuiOnlineStatusCallback* callback, bool wantCallback )
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
        for( auto client : m_GuiOnlineStatusClientList )
        {
            if( client == callback )
            {
                LogMsg( LOG_INFO, "WARNING. Ignoring New wantGuiConnectIdCallbacks because already in list" );
                return;
            }
        }

        m_GuiOnlineStatusClientList.emplace_back( callback );
        return;
    }

    for( auto iter = m_GuiOnlineStatusClientList.begin(); iter != m_GuiOnlineStatusClientList.end(); ++iter )
    {
        if( *iter == callback )
        {
            m_GuiOnlineStatusClientList.erase( iter );
            return;
        }
    }

    LogMsg( LOG_INFO, "WARNING. %s remove not found in list", __func__ );
    return;
}

//============================================================================
bool GuiConnectIdListMgr::isDirectConnect( VxGUID& onlineId )
{
    bool isDirectConnect{ false };
    if( onlineId.isVxGUIDValid() )
    {
        if( onlineId == m_MyApp.getMyOnlineId() )
        {
            return true;
        }

        for( auto &connectIdIn : m_ConnectIdList )
        {
            ConnectId& connectId = const_cast<ConnectId&>(connectIdIn);
            if( connectId.getUserOnlineId() == onlineId && eHostTypePeerUser == connectId.getHostType() )
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
    bool isRelayed{ false };
    bool isDirectConnected{ false };
    if( onlineId.isVxGUIDValid() )
    {
        if( onlineId == m_MyApp.getMyOnlineId() )
        {
            return false;
        }

        for( auto &connectIdIn : m_ConnectIdList )
        {
            ConnectId& connectId = const_cast<ConnectId&>(connectIdIn);
            if( connectId.getUserOnlineId() == onlineId && eHostTypePeerUser == connectId.getHostType() )
            {
                isDirectConnected = true;
                break;
            }
        }

        if( !isDirectConnected )
        {
            for( auto &connectIdIn : m_RelayedIdList )
            {
                ConnectId& connectId = const_cast<ConnectId&>(connectIdIn);
                if( connectId.getUserOnlineId() == onlineId )
                {
                    isRelayed = true;
                    break;
                }
            }
        }
    }
    else
    {
         LogMsg( LOG_ERROR, "GuiConnectIdListMgr::isRelayed invalid id" );
    }

    return isRelayed;
}

//============================================================================
bool GuiConnectIdListMgr::isConnected( GroupieId& groupieId )
{
    bool isConnected{ false };

    for( auto& connectIdIn : m_ConnectIdList )
    {
        ConnectId& connectId = const_cast<ConnectId&>(connectIdIn);
        if( connectId.getGroupieId() == groupieId )
        {
            isConnected = true;
            break;
        }
    }

    if( !isConnected )
    {
        for( auto& connectIdIn : m_RelayedIdList )
        {
            ConnectId& connectId = const_cast<ConnectId&>(connectIdIn);
            if( connectId.getGroupieId() == groupieId )
            {
                isConnected = true;
                break;
            }
        }
    }

    return isConnected;
}
