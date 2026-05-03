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
    std::set<VxGUID> sendToSet;
    ECanSendState canSendState = getSendToSet( adminId, sendToSet );
    if( canSendState != ECanSendState::eCanSend )
    {
        GuiHelpers::showCannotSendReason( canSendState );
        return false;
    }

    switch( adminId.getHostType() )
    {
    case eHostTypeChatRoom:
        assetInfo.setPluginType( fromAdmin ? ePluginTypeHostChatRoom : ePluginTypeClientChatRoom );
        break;
    case eHostTypeGroup:
        assetInfo.setPluginType( fromAdmin ? ePluginTypeHostGroup : ePluginTypeClientGroup );
        break;
    case eHostTypeRandomConnect:
        assetInfo.setPluginType( fromAdmin ? ePluginTypeHostRandomConnect : ePluginTypeClientRandomConnect );
        break;
    default:
        GuiHelpers::showCannotSendReason( ECanSendState::eInvalidHostOrState );
        return false;    
    }

    LogModule( eLogAssets, LOG_DEBUG, "AssetSendMgr::%s action %s for host %s admin %s", __func__,
        DescribeAssetAction( assetAction ), DescribeHostType( adminId.getHostType() ), getMyApp().getUserName( adminId.getUserOnlineId() ).c_str() );
    if( eAssetActionAddAssetAndSend != assetAction && eAssetActionAssetSend != assetAction )
    {
        // just pass on to engine for non-send actions
        return getMyApp().getEngine().fromGuiAssetAction( assetAction, assetInfo );
    }

    // Add asset to asset manager first
    bool result = getMyApp().getEngine().fromGuiAssetAction( eAssetActionAddToAssetMgr, assetInfo );
    if( !result )
    {
        GuiHelpers::showAddAssetFailedError();
        return false;
    }


    // Start sequential multi-member send
    return startMultiSend( parent, adminId.getHostedId(), assetInfo, sendToSet, fromAdmin );
}

//============================================================================
bool AssetSendMgr::startMultiSend( QWidget* parent, HostedId& hostId, AssetBaseInfo& assetInfo, std::set<VxGUID>& sendToSet, bool fromAdmin )
{
    VxGUID assetId = assetInfo.getAssetUniqueId();
    if( isMultiSendActive( assetId ) )
    {
        LogModule( eLogAssets, LOG_WARNING, "AssetSendMgr::startMultiSend already have active session, canceling" );
        cancelMultiSend( assetId );
    }

    LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::startMultiSend starting send to %u members (fromAdmin=%s)", 
            (unsigned)sendToSet.size(), fromAdmin ? "true" : "false" );

    // Initialize session

    MultiSendSession multiSendSession( parent );
    multiSendSession.assetInfo = assetInfo;
    multiSendSession.assetId = assetId;
    multiSendSession.hostId = hostId;
    multiSendSession.sendToQueue = std::vector<VxGUID>(sendToSet.begin(), sendToSet.end());
    multiSendSession.parentWidget = parent;
    multiSendSession.isActive = true;
    multiSendSession.canceled = false;
    multiSendSession.fromAdmin = fromAdmin;
    m_SessionList.emplace_back( assetId, multiSendSession );

    // Register for callbacks to track send progress
    wantActivityCallbacks( true );

    // Start sending to first member
    sendToNextMember( assetId );
    return true;
}

//============================================================================
bool AssetSendMgr::sendToNextMember( VxGUID assetId )
{
    auto it = std::find_if( m_SessionList.begin(), m_SessionList.end(), [assetId]( const std::pair<VxGUID, MultiSendSession>& sessionPair ) {
        return sessionPair.first == assetId;
    } );
    if( it == m_SessionList.end() )
    {
        return false;
    }

    MultiSendSession& multiSendSession = it->second;
    if( multiSendSession.canceled )
    {
        LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::sendToNextMember session was canceled" );
        // Defer to next event loop iteration to avoid modifying callback list during iteration
        QTimer::singleShot( 0, this, [this, assetId]() { finishMultiSend( assetId ); } );
        return false;
    }

    // find next member to send to, or finish if no more members
    VxGUID sendToId;
    while( !sendToId.isValid() )
    {
        if( multiSendSession.sendToQueue.empty() )
        {
            LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::sendToNextMember queue empty, finishing" );
            // Defer to next event loop iteration to avoid modifying callback list during iteration
            QTimer::singleShot( 0, this, [this, assetId]() { finishMultiSend( assetId ); } );
            return false;
        }

        VxGUID nextSendToId = multiSendSession.sendToQueue.front();
        multiSendSession.sendToQueue.erase( multiSendSession.sendToQueue.begin() );
        if( !getMyApp().getUserMgr().isUserOnline( nextSendToId ) )
        {
            LogModule( eLogAssets, LOG_WARNING, "AssetSendMgr::sendToNextMember skipping offline member %s", 
                    getMemberDisplayName( getMyApp(), nextSendToId ).toStdString().c_str() );
            multiSendSession.results[nextSendToId] = ESendResult::eOffline;
            emit signalMemberSendComplete( multiSendSession.assetId, nextSendToId, ESendResult::eOffline );
            continue; // skip offline members
        }

        sendToId = nextSendToId;
    }

    multiSendSession.currentSendToId = sendToId;
    // make a new session id for this send to track in callbacks
    multiSendSession.currentSessionId.initializeWithNewVxGUID();
    // set asset with destination
    multiSendSession.assetInfo.setDestUserId( multiSendSession.currentSendToId );


    QString memberName = getMemberDisplayName( getMyApp(), multiSendSession.currentSendToId );
    LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::sendToNextMember sending to %s (%u remaining)", 
            memberName.toStdString().c_str(), (unsigned)multiSendSession.sendToQueue.size() );


    // Emit signal for UI update
    emit signalSendingToMember( multiSendSession.assetId, multiSendSession.currentSendToId, memberName );


    bool sendResult = getMyApp().getEngine().fromGuiAssetAction( eAssetActionAssetSend, multiSendSession.assetInfo );
    if( !sendResult )
    {
        LogModule( eLogAssets, LOG_ERROR, "AssetSendMgr::sendToNextMember fromGuiAssetAction returned false for %s", 
                memberName.toStdString().c_str() );
        // Immediate failure - record error and continue to next
        onCurrentSendComplete( multiSendSession.assetId, ESendResult::eError );
    }
    // If sendResult is true, wait for callback (toGuiClientAssetAction) to complete
    return sendResult;
}

//============================================================================
void AssetSendMgr::toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 )
{
    auto it = std::find_if( m_SessionList.begin(), m_SessionList.end(), [assetId]( const std::pair<VxGUID, MultiSendSession>& sessionPair ) {
        return sessionPair.first == assetId;
    } );
    if( it == m_SessionList.end() )
    {
        return;
    }

    MultiSendSession& multiSendSession = it->second;
    if( !multiSendSession.isActive )
    {
        return;
    }

    // Check if this callback is for our current send
    if( assetId != multiSendSession.assetId )
    {
        return; // not our asset
    }

    switch( assetAction )
    {
    case eAssetActionTxBegin:
        LogModule( eLogAssets, LOG_VERBOSE, "AssetSendMgr::toGuiClientAssetAction TxBegin" );
        emit signalSendProgress( assetId, 0 );
        break;

    case eAssetActionTxProgress:
        emit signalSendProgress( assetId, pos0to100000 );
        break;

    case eAssetActionTxSuccess:
        LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::toGuiClientAssetAction TxSuccess" );
        onCurrentSendComplete( assetId, ESendResult::eSuccess );
        break;

    case eAssetActionTxError:
        LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::toGuiClientAssetAction TxError" );
        onCurrentSendComplete( assetId, ESendResult::eError );
        break;

    case eAssetActionTxCancel:
        LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::toGuiClientAssetAction TxCancel" );
        onCurrentSendComplete( assetId, ESendResult::eCanceled );
        break;

    case eAssetActionTxPermission:
        LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::toGuiClientAssetAction TxPermission" );
        onCurrentSendComplete( assetId, ESendResult::ePermissionError );
        break;

    default:
        break;
    }
}

//============================================================================
void AssetSendMgr::onCurrentSendComplete( VxGUID assetId, ESendResult result )
{
    auto it = std::find_if( m_SessionList.begin(), m_SessionList.end(), [assetId]( const std::pair<VxGUID, MultiSendSession>& sessionPair ) {
        return sessionPair.first == assetId;
    } );
    if( it == m_SessionList.end() )
    {
        return;
    }

    MultiSendSession& multiSendSession = it->second;
    if( !multiSendSession.isActive )
    {
        return;
    }

    // Record result for this member
    multiSendSession.results[multiSendSession.currentSendToId] = result;

    // Emit signal for UI update
    emit signalMemberSendComplete( multiSendSession.assetId, multiSendSession.currentSendToId, result );

    // Continue to next member
    sendToNextMember( assetId );
}

//============================================================================
void AssetSendMgr::cancelMultiSend( VxGUID assetId )
{
    auto it = std::find_if( m_SessionList.begin(), m_SessionList.end(), [assetId]( const std::pair<VxGUID, MultiSendSession>& sessionPair ) {
        return sessionPair.first == assetId;
    } );
    if( it == m_SessionList.end() )
    {
        return;
    }

    MultiSendSession& multiSendSession = it->second;
    if( !multiSendSession.isActive )
    {
        return;
    }

    LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::cancelMultiSend canceling session" );

    multiSendSession.canceled = true;

    // Cancel current transfer in engine
    if( multiSendSession.currentSendToId.isValid() )
    {
        getMyApp().getEngine().fromGuiAssetAction( eAssetActionTxCancel, multiSendSession.assetInfo );
    }

    // Mark all remaining members as canceled
    for( const auto& memberId : multiSendSession.sendToQueue )
    {
        multiSendSession.results[memberId] = ESendResult::eCanceled;
    }

    multiSendSession.sendToQueue.clear();

    // finishMultiSend will be called from sendToNextMember when current send completes/cancels
}

//============================================================================
void AssetSendMgr::finishMultiSend( VxGUID assetId )
{
    auto it = std::find_if( m_SessionList.begin(), m_SessionList.end(), [assetId]( const std::pair<VxGUID, MultiSendSession>& sessionPair ) {
        return sessionPair.first == assetId;
    } );
    if( it == m_SessionList.end() )
    {
        return;
    }

    MultiSendSession& multiSendSession = it->second;
    if( !multiSendSession.isActive )
    {
        return;
    }

    LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::finishMultiSend completing session" );

    // Calculate results
    int successCount = 0;
    int failCount = 0;

    for( const auto& [memberId, result] : multiSendSession.results )
    {
        if( result == ESendResult::eSuccess )
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
    multiSendSession.isActive = false;

    // Show failures after the callback stack unwinds to avoid re-entrant GUI event handling.
    for( auto& [memberId, result] : multiSendSession.results )
    {
        if( result == ESendResult::eError || result == ESendResult::ePermissionError )
        {
            QString memberName = getMyApp().getUserName( memberId ).c_str();
            LogModule( eLogAssets, LOG_ERROR, "AssetSendMgr::finishMultiSend failed send popup for %s result=%d", memberName.toUtf8().constData(), (int)result );
            GuiHelpers::showFailedToSendMemberError( memberName );
        }
    }

    // Emit completion signal
    emit signalMultiSendComplete( multiSendSession.assetId, allSucceeded, successCount, failCount );
}

//============================================================================
bool AssetSendMgr::retryFailedSends( VxGUID assetId )
{
    if( isMultiSendActive( assetId ) )
    {
        LogModule( eLogAssets, LOG_WARNING, "AssetSendMgr::%s cannot retry while send is active", __func__ );
        return false;
    }

        auto it = std::find_if( m_SessionList.begin(), m_SessionList.end(), [assetId]( const std::pair<VxGUID, MultiSendSession>& sessionPair ) {
        return sessionPair.first == assetId;
    } );
    if( it == m_SessionList.end() )
    {
        return false;
    }

    MultiSendSession& multiSendSession = it->second;
    // Build list of failed members
    std::vector<VxGUID> failedMembers;
    for( const auto& [memberId, result] : multiSendSession.results )
    {
        if( result != ESendResult::eSuccess && result != ESendResult::eOffline ) // do not retry if user is offline
        {
            failedMembers.emplace_back( memberId );
        }
    }

    if( failedMembers.empty() )
    {
        LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::%s no failed sends to retry", __func__ );
        return true;
    }

    LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::%s retrying %u failed sends", __func__, (unsigned)failedMembers.size() );

    // Clear old results for retry members
    for( const auto& memberId : failedMembers )
    {
        multiSendSession.results.erase( memberId );
    }

    // Start new session with failed members
    return sendToNextMember( assetId );
}

//============================================================================
ECanSendState AssetSendMgr::getSendToSet( GroupieId& adminId, std::set<VxGUID>& sendToSet )
{
    HostedId hostId = adminId.getHostedId();

    if( !hostId.isValid() )
    {
        LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::getSendToSet invalid hostId" );
        return ECanSendState::eInvalidHostOrState;
    }

    if( hostId.getHostOnlineId() == getMyApp().getMyOnlineId() )
    {
        LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::getSendToSet host online id is self, no send" );
        return ECanSendState::eCannotSendToSelf;
    } 

    if( hostId.getHostType() == eHostTypeChatRoom )
    {
        // chat room special case - only send to host admin
        sendToSet.insert( adminId.getHostOnlineId() );
    }
    else
    {
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
    }

    if( !getMyApp().getUserMgr().isUserOnline( adminId.getUserOnlineId() ) )
    {
        LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::getSendToSet admin is offline" );
        return ECanSendState::eAdminIsOffline;
    }

    // Remove self from sendToSet if present
    sendToSet.erase( getMyApp().getMyOnlineId() );

    if( sendToSet.empty() )
    {
        LogModule( eLogAssets, LOG_INFO, "AssetSendMgr::getSendToSet no members to send to" );
        return ECanSendState::eNoMembersToSendTo;
    }

    return ECanSendState::eCanSend;
}

//============================================================================
bool AssetSendMgr::isMultiSendActive( VxGUID assetId )
{
    auto it = std::find_if( m_SessionList.begin(), m_SessionList.end(), [assetId]( const std::pair<VxGUID, MultiSendSession>& sessionPair ) {
        return sessionPair.first == assetId;
    } );
    if( it == m_SessionList.end() )
    {
        return false;
    }

    MultiSendSession& multiSendSession = it->second;
    return multiSendSession.isActive;
}

//============================================================================
std::string AssetSendMgr::getCurrentSendToUser( VxGUID assetId )
{
    auto it = std::find_if( m_SessionList.begin(), m_SessionList.end(), [assetId]( const std::pair<VxGUID, MultiSendSession>& sessionPair ) {
        return sessionPair.first == assetId;
    } );
    if( it == m_SessionList.end() )
    {
        return "";
    }

    MultiSendSession& multiSendSession = it->second;
    if( multiSendSession.currentSendToId.isValid() )
    {
        GuiUser* guiUser = getMyApp().getUserMgr().getUser( multiSendSession.currentSendToId );
        if( guiUser )
        {
            return guiUser->getOnlineName();
        }
    }
    return "";
}
