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

#include <QTimer>
#include <set>

namespace
{
QString getMemberDisplayName( AppCommon& app, const VxGUID& memberId )
{
    std::string memberName = app.getUserName( memberId );
    if( memberName.empty() )
    {
        return QString::fromStdString( memberId.toHexString() );
    }

    return QString::fromStdString( memberName );
}
}

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

    LogModule( eLogAssets, LOG_DEBUG, "AssetSendMgr::%s action %s for host %s admin %s", __func__,
        DescribeAssetAction( assetAction ), DescribeHostType( hostId.getHostType() ), getMyApp().getUserName( adminId.getUserOnlineId() ).c_str() );
    if( eAssetActionAddAssetAndSend != assetAction && eAssetActionAssetSend != assetAction )
    {
        // just pass on to engine for non-send actions
        return getMyApp().getEngine().fromGuiAssetAction( assetAction, assetInfo );
    }

    std::set<VxGUID> sendToSet;
    if( hostId.getHostOnlineId() != adminId.getUserOnlineId() )
    {
        // user has selected a specific user to send to (not broadcast), so just send to that user
        sendToSet.insert( adminId.getUserOnlineId() );
    }
    else
    {
        // Get active members for this host
        getMyApp().getMemberActiveMgr().getActiveMembers( hostId, sendToSet );
    }

    if( sendToSet.empty() )
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
                   isHostAdminSender ? "admin-via-host" : "client-to-host", (unsigned)sendToSet.size() );
        assetInfo.setDestUserId( hostId.getHostOnlineId() );
        result = getMyApp().getEngine().fromGuiAssetAction( eAssetActionAssetSend, assetInfo );
        if( !result )
        {
            GuiHelpers::showFailedToSendError( QString( getMyApp().getUserName( hostId.getHostOnlineId() ).c_str() ) );
            return false;
        }
        return true;
    }

    // GROUP / RANDOM CONNECT: Sequential multi-member send
    // Build member list excluding self
    std::vector<VxGUID> memberList;
    VxGUID myOnlineId = getMyApp().getMyOnlineId();
    
    for( const auto& memberId : sendToSet )
    {
        if( memberId == myOnlineId )
        {
            continue; // don't send to self
        }
        memberList.emplace_back( memberId );
    }

    if( memberList.empty() )
    {
        LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::handleGroupieAssetAction no members to send to after filtering" );
        return true; // success - nothing to send
    }

    VxGUID assetUniqueId = assetInfo.getAssetUniqueId();
    // If sending to a specific user (not broadcast), just send directly
    if( memberList.size() == 1 )
    {
        VxGUID specificUserId = memberList[0];
        // Emit signal so progress bar shows recipient name
        QString memberName = getMyApp().getUserName( specificUserId ).c_str();
        emit signalSendingToMember( assetUniqueId, specificUserId, memberName );

        assetInfo.setDestUserId( specificUserId );
        result = getMyApp().getEngine().fromGuiAssetAction( eAssetActionAssetSend, assetInfo );
        if( !result )
        {
            LogModule( eLogAssets, LOG_ERROR, "AssetSendMgr::handleGroupieAssetAction failed direct send popup for %s", memberName.toUtf8().constData() );
            GuiHelpers::showFailedToSendError( memberName );
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
        LogModule( eLogAssets, LOG_WARNING, "AssetSendMgr::startMultiSend already have active session, canceling" );
        cancelMultiSend();
    }

    LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::startMultiSend starting send to %u members (fromAdmin=%s)", 
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
        LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::sendToNextMember session was canceled" );
        // Defer to next event loop iteration to avoid modifying callback list during iteration
        QTimer::singleShot( 0, this, &AssetSendMgr::finishMultiSend );
        return;
    }

    if( m_Session.memberQueue.empty() )
    {
        LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::sendToNextMember queue empty, finishing" );
        // Defer to next event loop iteration to avoid modifying callback list during iteration
        QTimer::singleShot( 0, this, &AssetSendMgr::finishMultiSend );
        return;
    }

    // Pop next member from queue
    m_Session.currentMemberId = m_Session.memberQueue.front();
    m_Session.memberQueue.erase( m_Session.memberQueue.begin() );

    // Get member name for progress display
    QString memberName = getMemberDisplayName( getMyApp(), m_Session.currentMemberId );
    
    LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::sendToNextMember sending to %s (%u remaining)", 
            memberName.toStdString().c_str(), (unsigned)m_Session.memberQueue.size() );

    VxGUID assetUniqueId = m_Session.assetInfo.getAssetUniqueId();
     m_Session.currentAssetId = assetUniqueId; // store for callback matching
    // Emit signal for UI update
    emit signalSendingToMember( assetUniqueId, m_Session.currentMemberId, memberName );

    // Set destination and send
    m_Session.assetInfo.setDestUserId( m_Session.currentMemberId );

    bool sendResult = getMyApp().getEngine().fromGuiAssetAction( eAssetActionAssetSend, m_Session.assetInfo );
    if( !sendResult )
    {
        LogModule( eLogAssets, LOG_ERROR, "AssetSendMgr::sendToNextMember fromGuiAssetAction returned false for %s", 
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
        LogModule( eLogAssets, LOG_VERBOSE, "AssetSendMgr::toGuiClientAssetAction TxBegin" );
        emit signalSendProgress( m_Session.currentAssetId, 0 );
        break;

    case eAssetActionTxProgress:
        emit signalSendProgress( m_Session.currentAssetId, pos0to100000 );
        break;

    case eAssetActionTxSuccess:
        LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::toGuiClientAssetAction TxSuccess" );
        onCurrentSendComplete( ESendResult::Success );
        break;

    case eAssetActionTxError:
        LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::toGuiClientAssetAction TxError" );
        onCurrentSendComplete( ESendResult::Error );
        break;

    case eAssetActionTxCancel:
        LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::toGuiClientAssetAction TxCancel" );
        onCurrentSendComplete( ESendResult::Canceled );
        break;

    case eAssetActionTxPermission:
        LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::toGuiClientAssetAction TxPermission" );
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
    emit signalMemberSendComplete( m_Session.currentAssetId, m_Session.currentMemberId, result );

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

    LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::cancelMultiSend canceling session" );

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

    LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::finishMultiSend completing session" );

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

    LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::finishMultiSend success=%d fail=%d allSucceeded=%s", 
            successCount, failCount, allSucceeded ? "true" : "false" );

    // Unregister callbacks
    wantActivityCallbacks( false );

    // Mark session as inactive (but keep results for retry/inspection)
    m_Session.isActive = false;

    // Show failures after the callback stack unwinds to avoid re-entrant GUI event handling.
    for( auto& [memberId, result] : m_Session.results )
    {
        if( result == ESendResult::Error || result == ESendResult::PermissionError )
        {
            QString memberName = getMyApp().getUserName( memberId ).c_str();
            LogModule( eLogAssets, LOG_ERROR, "AssetSendMgr::finishMultiSend failed send popup for %s result=%d", memberName.toUtf8().constData(), (int)result );
            GuiHelpers::showFailedToSendError( memberName );
        }
    }

    // Emit completion signal
    emit signalMultiSendComplete( m_Session.currentAssetId, allSucceeded, successCount, failCount );
}

//============================================================================
bool AssetSendMgr::retryFailedSends( void )
{
    if( m_Session.isActive )
    {
        LogModule( eLogAssets, LOG_WARNING, "AssetSendMgr::retryFailedSends cannot retry while send is active" );
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
        LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::retryFailedSends no failed sends to retry" );
        return true;
    }

    LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::retryFailedSends retrying %u failed sends", (unsigned)failedMembers.size() );

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
