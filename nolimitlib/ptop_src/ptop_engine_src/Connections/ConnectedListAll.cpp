//============================================================================
// Copyright (C) 2021 Brett R. Jones 
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

#include "ConnectedListAll.h"
#include "ConnectedInfo.h"

#include <ptop_src/ptop_engine_src/BigListLib/BigListInfo.h>

#include <NetLib/VxSktBase.h>

//============================================================================
ConnectedListAll::ConnectedListAll( P2PEngine& engine )
    : m_Engine( engine )
{
}

//============================================================================
ConnectedInfo* ConnectedListAll::getOrAddConnectedInfo( const VxGUID& socketId, BigListInfo* bigListInfo )
{
    if( nullptr == bigListInfo )
    {
        LogMsg( LOG_ERROR, "ConnectedListAll::getOrAddConnectedInfo bigListInfo is NULL" );
        return nullptr;
    }

    if( !bigListInfo->getMyOnlineId().isVxGUIDValid() )
    {
        LogMsg( LOG_ERROR, "ConnectedListAll::getOrAddConnectedInfo id is INVALID" );
        return nullptr;
    }

    ConnectedInfo* connectedinfo{ nullptr };
    lockConnectedList();
    std::pair<VxGUID,VxGUID> sktOnlineIdPair( std::make_pair( socketId, bigListInfo->getMyOnlineId() ) );
    auto iter = m_ConnectList.find( sktOnlineIdPair );
    if( iter != m_ConnectList.end() )
    {
        connectedinfo = iter->second;
    }
    else
    {
        ConnectedInfo* newInfo = new ConnectedInfo( m_Engine, socketId, bigListInfo );
        m_ConnectList[ sktOnlineIdPair ] = newInfo;
        connectedinfo = newInfo;
    }

    unlockConnectedList();
    return connectedinfo;
}

//============================================================================
ConnectedInfo* ConnectedListAll::getConnectedInfo( const VxGUID& socketId, const VxGUID& onlineId )
{
    std::pair<VxGUID, VxGUID> sktOnlineIdPair( std::make_pair( socketId, onlineId ) );
    ConnectedInfo* connectedInfo = nullptr;
    lockConnectedList();
    auto iter = m_ConnectList.find( sktOnlineIdPair );
    if( iter != m_ConnectList.end() )
    {
        connectedInfo = iter->second;
        if( !connectedInfo->getSktBase() || !connectedInfo->getSktBase()->isConnected() )
        {
            connectedInfo = nullptr;
            removeConnectedInfo( socketId, onlineId, true );
        }
    }

    unlockConnectedList();
    return connectedInfo;
}


//============================================================================
ConnectedInfo* ConnectedListAll::getAnyConnectedInfo( const VxGUID& onlineId )
{
    ConnectedInfo* foundInfo = nullptr;
    lockConnectedList();

    for( auto iter = m_ConnectList.begin(); iter != m_ConnectList.end(); )
    {
        ConnectedInfo* connectedInfo = iter->second;
        if( !connectedInfo->getSktBase() || !connectedInfo->getSktBase()->isConnected() )
        {
            iter = m_ConnectList.erase( iter );
            connectedInfo->aboutToDelete();
            delete connectedInfo;
        }
        else
        {
            if( iter->first.second == onlineId )
            {
                // found a connection
                foundInfo = connectedInfo;
                break;
            }

            ++iter;
        }
    }

    unlockConnectedList();
    return foundInfo;
}


//============================================================================
void ConnectedListAll::removeConnectedInfo( const VxGUID& socketId, const VxGUID& onlineId, bool isLockedList )
{
    std::pair<VxGUID, VxGUID> sktOnlineIdPair( std::make_pair( socketId, onlineId ) );
    if( !isLockedList )
    {
        lockConnectedList();
    }
    
    auto iter = m_ConnectList.find( sktOnlineIdPair );
    if( iter != m_ConnectList.end() )
    {
        ConnectedInfo* connectedInfo = iter->second;
        if( connectedInfo )
        {
            m_ConnectList.erase( sktOnlineIdPair );
            connectedInfo->aboutToDelete();
            delete connectedInfo;
        }
    }

    if( !isLockedList )
    {
        unlockConnectedList();
    }
}

//============================================================================
bool ConnectedListAll::removeConnectedReason( VxGUID& sessionId, VxGUID& onlineId, IConnectRequestCallback* callback, EConnectReason connectReason, std::vector<std::shared_ptr<VxSktBase>>& retUnusedSkts )
{
    retUnusedSkts.clear();
    lockConnectedList();
    for( auto iter = m_ConnectList.begin(); iter != m_ConnectList.end(); ++iter )
    {
        if( iter->first.second == onlineId )
        {
            ConnectedInfo* connectedInfo = iter->second;
            if( connectedInfo  )
            {
                std::shared_ptr<VxSktBase> sktBase( nullptr );
                if( connectedInfo->removeConnectReason( sessionId, callback, connectReason, sktBase ) )
                {
                    if( sktBase )
                    {
                        retUnusedSkts.push_back( sktBase );
                    }                  
                }

            }
        }
    }

    unlockConnectedList();

    return !retUnusedSkts.empty();
}

//============================================================================
void ConnectedListAll::onSktDisconnected( const VxGUID& socketId )
{
    // if never recieved a PktAnnounce the online id is invalid
    // remove by socketId which is always valid
    lockConnectedList();

    for( auto iter = m_ConnectList.begin(); iter != m_ConnectList.end(); )
    {
        if( iter->first.first == socketId )
        {
            // found a connection
            ConnectedInfo* connectedInfo = iter->second;
            if( connectedInfo )
            {
                connectedInfo->onSktDisconnected( socketId );
            }

            iter = m_ConnectList.erase( iter );
            if( connectedInfo )
            {
                connectedInfo->aboutToDelete();
                delete connectedInfo;
            }
        }
        else
        {
            ++iter;
        }
    }

    unlockConnectedList();
}
