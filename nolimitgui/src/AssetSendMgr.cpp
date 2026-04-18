//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AssetSendMgr.h"

#include "AppCommon.h"
#include "AppSettings.h"    
#include "GuiMemberActiveMgr.h" 
#include "GuiHelpers.h" 
#include <P2PEngine/P2PEngine.h>    

#include <CoreLib/VxDebug.h>

#include <set>

//============================================================================
AssetSendMgr::AssetSendMgr()
    : QObject()
{
}

//============================================================================
AssetSendMgr::~AssetSendMgr()
{
    if( m_CallbacksRegistered )
    {
        wantActivityCallbacks( false );
    }
}

//============================================================================
void AssetSendMgr::onAppCommonCreated( void )
{
    // Register Qt metatype for ESendResult signal/slot usage
    qRegisterMetaType<ESendResult>( "ESendResult" );
}

//============================================================================
AppCommon& AssetSendMgr::getMyApp( void )
{
    return GetAppInstance();
}

//============================================================================
void AssetSendMgr::wantActivityCallbacks( bool enable )
{
    if( enable != m_CallbacksRegistered )
    {
        m_CallbacksRegistered = enable;
        getMyApp().wantToGuiActivityCallbacks( this, enable );
    }
}

//============================================================================
bool AssetSendMgr::handleGroupieAssetAction( QWidget* parent, GroupieId& adminId, EAssetAction assetAction, AssetBaseInfo& assetInfo, bool fromAdmin )
{
    HostedId hostId = adminId.getHostedId();

    if( !hostId.isValid() )
    {
        GuiHelpers::showInvalidHostIdError( parent );
        return false;
    }

    if( eAssetActionAddAssetAndSend != assetAction && eAssetActionAssetSend != assetAction )
    {
        // just pass on to engine for non-send actions
        return getMyApp().getEngine().fromGuiAssetAction( assetAction, assetInfo );
    }

    // Get active members for this host
    std::set<VxGUID> memberSet;
    getMyApp().getMemberActiveMgr().getActiveMembers( hostId, memberSet );
    if( memberSet.empty() )
    {
        GuiHelpers::showNoMembersOnlineError( parent );
        return false;
    }

    // Add asset to asset manager first
    bool result = getMyApp().getEngine().fromGuiAssetAction( eAssetActionAddToAssetMgr, assetInfo );
    if( !result )
    {
        GuiHelpers::showAddAssetFailedError();
        return false;
    }

    assetInfo.setHostedId( hostId );
    bool isHostAdminSender = ( hostId.getHostOnlineId() == getMyApp().getMyOnlineId() );

    // CHAT ROOM: Special case - send only to host admin
    if( adminId.getHostType() == eHostTypeChatRoom )
    {
        LogModule( eLogChatRoom, LOG_INFO, "AssetSendMgr::handleGroupieAssetAction chatroom send mode %s members %u", 
                   isHostAdminSender ? "admin-via-host" : "client-to-host", (unsigned)memberSet.size() );
        assetInfo.setDestUserId( hostId.getHostOnlineId() );
        result = getMyApp().getEngine().fromGuiAssetAction( eAssetActionAssetSend, assetInfo );
        if( !result )
        {
            GuiHelpers::showFailedToSendError( QString( getMyApp().describeUser( hostId.getHostOnlineId() ).c_str() ) );
            return false;
        }
        return true;
    }

    // GROUP / RANDOM CONNECT: Sequential multi-member send
    // Build member list excluding self
    std::vector<VxGUID> memberList;
    VxGUID myOnlineId = getMyApp().getMyOnlineId();
    
    for( const auto& memberId : memberSet )
    {
        if( memberId == myOnlineId )
        {
            continue; // don't send to self
        }
        memberList.push_back( memberId );
    }

    if( memberList.empty() )
    {
        LogMsg( LOG_INFO, "AssetSendMgr::handleGroupieAssetAction no members to send to after filtering" );
        return true; // success - nothing to send
    }

    // If sending to a specific user (not broadcast), just send directly
    VxGUID specificUserId = adminId.getUserOnlineId();
    if( specificUserId.isVxGUIDValid() && specificUserId != myOnlineId )
    {
        assetInfo.setDestUserId( specificUserId );
        result = getMyApp().getEngine().fromGuiAssetAction( eAssetActionAssetSend, assetInfo );
        if( !result )
        {
            GuiHelpers::showFailedToSendError( QString( getMyApp().describeUser( specificUserId ).c_str() ) );
        }
        return result;
    }

    // Start sequential multi-member send
    return startMultiSend( parent, hostId, assetInfo, memberList, fromAdmin );
}

//============================================================================
bool AssetSendMgr::startMultiSend( QWidget* parent, HostedId& hostId, AssetBaseInfo& assetInfo, std::vector<VxGUID>& memberList, bool fromAdmin )
{
    if( m_Session.isActive )
    {
        LogMsg( LOG_WARNING, "AssetSendMgr::startMultiSend already have active session, canceling" );
        cancelMultiSend();
    }

    LogMsg( LOG_INFO, "AssetSendMgr::startMultiSend starting send to %u members (fromAdmin=%s)", 
            (unsigned)memberList.size(), fromAdmin ? "true" : "false" );

    // Initialize session
    m_Session = MultiSendSession();
    m_Session.assetInfo = assetInfo;
    m_Session.hostId = hostId;
    m_Session.memberQueue = memberList;
    m_Session.parentWidget = parent;
    m_Session.isActive = true;
    m_Session.canceled = false;
    m_Session.fromAdmin = fromAdmin;

    // Register for callbacks to track send progress
    wantActivityCallbacks( true );

    // Start sending to first member
    sendToNextMember();
    return true;
}

//============================================================================
void AssetSendMgr::sendToNextMember( void )
{
    if( !m_Session.isActive )
    {
        return;
    }

    if( m_Session.canceled )
    {
        LogMsg( LOG_INFO, "AssetSendMgr::sendToNextMember session was canceled" );
        finishMultiSend();
        return;
    }

    if( m_Session.memberQueue.empty() )
    {
        LogMsg( LOG_INFO, "AssetSendMgr::sendToNextMember queue empty, finishing" );
        finishMultiSend();
        return;
    }

    // Pop next member from queue
    m_Session.currentMemberId = m_Session.memberQueue.front();
    m_Session.memberQueue.erase( m_Session.memberQueue.begin() );

    // Get member name for progress display
    QString memberName = QString::fromStdString( getMyApp().describeUser( m_Session.currentMemberId ) );
    
    LogMsg( LOG_INFO, "AssetSendMgr::sendToNextMember sending to %s (%u remaining)", 
            memberName.toStdString().c_str(), (unsigned)m_Session.memberQueue.size() );

    // Emit signal for UI update
    emit signalSendingToMember( m_Session.currentMemberId, memberName );

    // Set destination and send
    m_Session.assetInfo.setDestUserId( m_Session.currentMemberId );
    
    // Store the asset unique ID for callback matching
    m_Session.currentAssetId = m_Session.assetInfo.getAssetUniqueId();

    bool sendResult = getMyApp().getEngine().fromGuiAssetAction( eAssetActionAssetSend, m_Session.assetInfo );
    if( !sendResult )
    {
        LogMsg( LOG_ERROR, "AssetSendMgr::sendToNextMember fromGuiAssetAction returned false for %s", 
                memberName.toStdString().c_str() );
        // Immediate failure - record error and continue to next
        onCurrentSendComplete( ESendResult::Error );
    }
    // If sendResult is true, wait for callback (toGuiClientAssetAction) to complete
}

//============================================================================
void AssetSendMgr::toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 )
{
    if( !m_Session.isActive )
    {
        return;
    }

    // Check if this callback is for our current send
    if( assetId != m_Session.currentAssetId )
    {
        return; // not our asset
    }

    switch( assetAction )
    {
    case eAssetActionTxBegin:
        LogMsg( LOG_VERBOSE, "AssetSendMgr::toGuiClientAssetAction TxBegin" );
        emit signalSendProgress( m_Session.currentMemberId, 0 );
        break;

    case eAssetActionTxProgress:
        emit signalSendProgress( m_Session.currentMemberId, pos0to100000 );
        break;

    case eAssetActionTxSuccess:
        LogMsg( LOG_INFO, "AssetSendMgr::toGuiClientAssetAction TxSuccess" );
        onCurrentSendComplete( ESendResult::Success );
        break;

    case eAssetActionTxError:
        LogMsg( LOG_INFO, "AssetSendMgr::toGuiClientAssetAction TxError" );
        onCurrentSendComplete( ESendResult::Error );
        break;

    case eAssetActionTxCancel:
        LogMsg( LOG_INFO, "AssetSendMgr::toGuiClientAssetAction TxCancel" );
        onCurrentSendComplete( ESendResult::Canceled );
        break;

    case eAssetActionTxPermission:
        LogMsg( LOG_INFO, "AssetSendMgr::toGuiClientAssetAction TxPermission" );
        onCurrentSendComplete( ESendResult::PermissionError );
        break;

    default:
        break;
    }
}

//============================================================================
void AssetSendMgr::onCurrentSendComplete( ESendResult result )
{
    if( !m_Session.isActive )
    {
        return;
    }

    // Record result for this member
    m_Session.results[m_Session.currentMemberId] = result;

    // Emit signal for UI update
    emit signalMemberSendComplete( m_Session.currentMemberId, result );

    // Show error to user if failed (but continue with remaining sends)
    if( result == ESendResult::Error || result == ESendResult::PermissionError )
    {
        QString memberName = QString::fromStdString( getMyApp().describeUser( m_Session.currentMemberId ) );
        GuiHelpers::showFailedToSendError( memberName );
    }

    // Continue to next member
    sendToNextMember();
}

//============================================================================
void AssetSendMgr::cancelMultiSend( void )
{
    if( !m_Session.isActive )
    {
        return;
    }

    LogMsg( LOG_INFO, "AssetSendMgr::cancelMultiSend canceling session" );

    m_Session.canceled = true;

    // Cancel current transfer in engine
    if( m_Session.currentMemberId.isVxGUIDValid() )
    {
        getMyApp().getEngine().fromGuiAssetAction( eAssetActionTxCancel, m_Session.assetInfo );
    }

    // Mark all remaining members as canceled
    for( const auto& memberId : m_Session.memberQueue )
    {
        m_Session.results[memberId] = ESendResult::Canceled;
    }
    m_Session.memberQueue.clear();

    // finishMultiSend will be called from sendToNextMember when current send completes/cancels
}

//============================================================================
void AssetSendMgr::finishMultiSend( void )
{
    if( !m_Session.isActive )
    {
        return;
    }

    LogMsg( LOG_INFO, "AssetSendMgr::finishMultiSend completing session" );

    // Calculate results
    int successCount = 0;
    int failCount = 0;

    for( const auto& [memberId, result] : m_Session.results )
    {
        if( result == ESendResult::Success )
        {
            successCount++;
        }
        else
        {
            failCount++;
        }
    }

    bool allSucceeded = ( failCount == 0 && successCount > 0 );

    LogMsg( LOG_INFO, "AssetSendMgr::finishMultiSend success=%d fail=%d allSucceeded=%s", 
            successCount, failCount, allSucceeded ? "true" : "false" );

    // Unregister callbacks
    wantActivityCallbacks( false );

    // Mark session as inactive (but keep results for retry/inspection)
    m_Session.isActive = false;

    // Emit completion signal
    emit signalMultiSendComplete( allSucceeded, successCount, failCount );
}

//============================================================================
bool AssetSendMgr::retryFailedSends( void )
{
    if( m_Session.isActive )
    {
        LogMsg( LOG_WARNING, "AssetSendMgr::retryFailedSends cannot retry while send is active" );
        return false;
    }

    // Build list of failed members
    std::vector<VxGUID> failedMembers;
    for( const auto& [memberId, result] : m_Session.results )
    {
        if( result != ESendResult::Success )
        {
            failedMembers.push_back( memberId );
        }
    }

    if( failedMembers.empty() )
    {
        LogMsg( LOG_INFO, "AssetSendMgr::retryFailedSends no failed sends to retry" );
        return true;
    }

    LogMsg( LOG_INFO, "AssetSendMgr::retryFailedSends retrying %u failed sends", (unsigned)failedMembers.size() );

    // Save fromAdmin before clearing results
    bool wasFromAdmin = m_Session.fromAdmin;

    // Clear old results for retry members
    for( const auto& memberId : failedMembers )
    {
        m_Session.results.erase( memberId );
    }

    // Start new session with failed members
    return startMultiSend( m_Session.parentWidget, m_Session.hostId, m_Session.assetInfo, failedMembers, wasFromAdmin );
}
