//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiRandConnectMgr.h"

#include "GuiRandConnectCallback.h"
#include "GuiHelpers.h"
#include "AppCommon.h"

#include <P2PEngine/P2PEngine.h>
#include <RandConnect/RandConnectMgr.h>

#include <CoreLib/VxDebug.h>
#include <PktLib/PktsRandConnectDefs.h>

//============================================================================
GuiRandConnectMgr::GuiRandConnectMgr()
    : QObject()
{
}

//============================================================================
void GuiRandConnectMgr::onAppCommonCreated( void )
{
    connect( this, SIGNAL(signalInternalRandConnect(GroupieId,ERandAction)), this, SLOT(slotInternalRandConnect(GroupieId,ERandAction)), Qt::QueuedConnection );

    GetPtoPEngine().getRandConnectMgr().wantRandConnectCallbacks( this, true );
}

//============================================================================
enum ERandAction GuiRandConnectMgr::getRandAction( VxGUID& onlineId )
{
    ERandAction randAction{ eRandActionNone };
    auto iter = std::find_if(m_MemberList.begin(), m_MemberList.end(), 
                              [&](const std::pair<GroupieId, enum ERandAction>& x) { return x.first.getUserOnlineId() == onlineId; });
    if( iter != m_MemberList.end() )
    {
        randAction = iter->second;
    }

    return randAction;
}

//============================================================================
void GuiRandConnectMgr::callbackRandConnect( GroupieId& groupieId, enum ERandAction randAction )
{
    emit signalInternalRandConnect( groupieId, randAction );
}

//============================================================================
void GuiRandConnectMgr::slotInternalRandConnect( GroupieId groupieId, enum ERandAction randAction )
{
    updateRandConnect( groupieId, randAction );
}

//============================================================================
void GuiRandConnectMgr::updateRandConnect( GroupieId& groupieId, enum ERandAction randAction )
{
    if( !groupieId.isValid() )
    {
        LogMsg( LOG_ERROR, "RandConnectMgr::updateRandConnect invalid groupieId %s", GetAppInstance().describeGroupieId(groupieId).c_str());
        return;
    }

    LogMsg( LOG_VERBOSE, "RandConnectMgr::updateRandConnect groupieId %s action %d", GetAppInstance().describeGroupieId( groupieId ).c_str(), randAction );

    bool wasUpdated = false;
    bool wasFound = false;
    for( auto iter = m_MemberList.begin(); iter != m_MemberList.end(); ++iter )
    {
        if( iter->first == groupieId )
        {
            wasFound = true;
            if( randAction == eRandActionNone )
            {
                m_MemberList.erase( iter );
                wasUpdated = true;
            }
            else
            {
                if( iter->second != randAction )
                {
                    iter->second = randAction;
                    wasUpdated = true;
                }
            }

            break;
        }
    }

    if( !wasFound && randAction != eRandActionNone )
    {
        m_MemberList.emplace_back( std::make_pair( groupieId, randAction) );
        wasUpdated = true;
    }

    if( wasUpdated )
    {
        announceRandConnect( groupieId.getUserOnlineId(), randAction);
    }
}

//============================================================================
void GuiRandConnectMgr::wantRandConnectCallback( GuiRandConnectCallback* client, bool enable )
{
    if( !client )
    {
        LogMsg( LOG_ERROR, "GuiRandConnectMgr null client" );
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
void GuiRandConnectMgr::announceRandConnect( VxGUID& onlineId, enum ERandAction randAction )
{
    for( auto& client : m_MemberClients )
    {
        client->callbackGuiRandConnect( onlineId, randAction );
    }
}
