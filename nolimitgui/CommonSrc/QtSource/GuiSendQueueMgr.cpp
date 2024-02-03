//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiSendQueueMgr.h"

#include "GuiSendQueueCallback.h"
#include "GuiHelpers.h"
#include "AppCommon.h"

#include <P2PEngine/P2PEngine.h>
#include <SendQueue/SendQueueMgr.h>

//============================================================================
GuiSendQueueMgr::GuiSendQueueMgr()
    : QObject()
{
}

//============================================================================
void GuiSendQueueMgr::onAppCommonCreated( void )
{
    connect( this, SIGNAL(signalInternalSendQueue(SendQueInfo)), this, SLOT(slotInternalSendQueue(SendQueInfo)), Qt::QueuedConnection );

    GetPtoPEngine().getSendQueueMgr().wantSendQueueCallbacks( this, true );
}

//============================================================================
bool GuiSendQueueMgr::isSendQueue( GroupieId& groupieId )
{
    auto iter = std::find_if(m_SendList.begin(), m_SendList.end(),
                             [&](SendQueInfo& x) { return x.getGroupieId() == groupieId; });
    return iter != m_SendList.end();
}

//============================================================================
bool GuiSendQueueMgr::isMemberOfHostType( EHostType hostType, VxGUID& onlineId )
{
    for( auto groupieId : m_SendList )
    {
        if( hostType == groupieId.getHostType() && groupieId.getUserOnlineId() == onlineId )
        {
            if( groupieId.getHostOnlineId() != onlineId )
            {
                return true;
            }
        }
    }

    return false;
}

//============================================================================
void GuiSendQueueMgr::getActiveMembers( HostedId& hostId, std::set<VxGUID>& memberList )
{
    memberList.clear();
    for( auto groupieId : m_SendList )
    {
        if( hostId == groupieId.getHostedId() )
        {
            memberList.insert( groupieId.getUserOnlineId() );
        }
    }
}

//============================================================================
void GuiSendQueueMgr::callbackSendQueInfo( SendQueInfo& sendInfo )
{
    emit signalInternalSendQueue( sendInfo );
}

//============================================================================
void GuiSendQueueMgr::slotInternalSendQueue( SendQueInfo sendInfo )
{
    updateSendQueue( sendInfo );
}

//============================================================================
void GuiSendQueueMgr::updateSendQueue( SendQueInfo& sendQueInfo )
{
    GroupieId groupieId = sendQueInfo.getGroupieId();
    if( !groupieId.isValid() )
    {
        LogMsg( LOG_ERROR, "SendQueueMgr::updateSendQueue invalid groupieId %s", GetAppInstance().describeGroupieId(groupieId).c_str());
        return;
    }

    LogMsg( LOG_VERBOSE, "SendQueueMgr::updateSendQueue groupieId %s state %d", GetAppInstance().describeGroupieId( groupieId ).c_str(), sendQueInfo.getSendQueState() );

    bool wasMember = isMemberOfHostType( groupieId.getHostType(), groupieId.getUserOnlineId() );

    bool wasUpdated = false;
    bool wasFound = false;
    for( auto iter = m_SendList.begin(); iter != m_SendList.end(); ++iter )
    {
        if( *iter == groupieId )
        {
            wasFound = true;
            if( sendQueInfo.getSendQueState() == eSendQueStateSendRemove )
            {
                m_SendList.erase( iter );
                wasUpdated = true;
            }
            else if( iter->getSendQueState() != sendQueInfo.getSendQueState() )
            {
                iter->setSendQueState( sendQueInfo.getSendQueState() );
                wasUpdated = true;
            }

            break;
        }
    }

    if( !wasFound && sendQueInfo.getSendQueState() != eSendQueStateSendRemove )
    {
        m_SendList.emplace_back( sendQueInfo );
        wasUpdated = true;
    }

    if( wasUpdated )
    {
        announceSendQueue( sendQueInfo );
    }
}

//============================================================================
void GuiSendQueueMgr::wantSendQueueCallback( GuiSendQueueCallback* client, bool enable )
{
    if( !client )
    {
        LogMsg( LOG_ERROR, "GuiSendQueueMgr null client" );
        return;
    }

    bool wasFound = false;
    for( auto iter = m_MemberClients.begin(); iter != m_MemberClients.end(); ++iter )
    {
        if( *iter == client )
        {
            wasFound = true;
            if( !enable )
            {
                m_MemberClients.erase( iter );
            }

            break;
        }
    }

    if( enable && !wasFound )
    {
        m_MemberClients.push_back( client );
    }
}

//============================================================================
void GuiSendQueueMgr::announceSendQueue( SendQueInfo& sendQueInfo )
{
    for( auto client : m_MemberClients )
    {
        client->callbackSendQueInfo( sendQueInfo );
    }
}

