#pragma once
//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ToGuiActivityInterface.h"

#include "CanSendInterface.h"

#include <GuiInterface/IFromGui.h>
#include <CoreLib/GroupieId.h>
#include <CoreLib/VxGUID.h>
#include <AssetMgr/AssetInfo.h>

#include <QObject>
#include <QWidget>

#include <map>
#include <vector>

class AppCommon;

/// Result of sending asset to one member
enum class ESendResult
{
    ePending,
    eSuccess,
    eError,
    eCanceled,
    eOffline,
    ePermissionError
};

/// Tracks state of a multi-member send session
class MultiSendSession
{
public:
    MultiSendSession( QWidget* parent = nullptr ) { parentWidget = parent; }

    QWidget*                    parentWidget{ nullptr };
    HostedId                    hostId;
    AssetBaseInfo               assetInfo;
    VxGUID                      assetId;           // for tracking callbacks  
    std::vector<VxGUID>         sendToQueue;        // remaining members to send to
    VxGUID                      currentSendToId;    // member currently being sent to
    VxGUID                      currentSessionId;    
    std::map<VxGUID, ESendResult> results;          // memberId -> result
    bool                        canceled{ false };
    bool                        isActive{ false };
    bool                        fromAdmin{ false }; // true if host admin is sending
};

class AssetSendMgr : public QObject, public ToGuiActivityInterface
{
    Q_OBJECT
public:
    AssetSendMgr();
    virtual ~AssetSendMgr();

    void                        onAppCommonCreated( void );

    /// Main entry point - handles asset send for groupie (chatroom/group/random)
    bool                        handleGroupieAssetAction( QWidget* parent, GroupieId& adminId, EAssetAction assetAction, AssetBaseInfo& assetInfo, bool fromAdmin = false );

    /// Cancel the current multi-send session
    void                        cancelMultiSend( VxGUID assetId );

    /// Check if a multi-send is currently active
    bool                        isMultiSendActive( VxGUID assetId );

    /// Retry failed sends from last session
    bool                        retryFailedSends( VxGUID assetId );

    // ToGuiActivityInterface overrides
    void                        toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 ) override;

    ECanSendState               getSendToSet( GroupieId& adminId, std::set<VxGUID>& sendToSet );

    std::string                 getCurrentSendToUser( VxGUID assetId );

signals:
    /// Emitted when starting to send to a member
    void                        signalSendingToMember( VxGUID assetId, VxGUID memberId, QString memberName );

    /// Emitted on progress update (0-100000)
    void                        signalSendProgress( VxGUID assetId, int progress );

    /// Emitted when send to one member completes
    void                        signalMemberSendComplete( VxGUID assetId, VxGUID memberId, ESendResult result );

    /// Emitted when entire multi-send session completes
    void                        signalMultiSendComplete( VxGUID assetId, bool allSucceeded, int successCount, int failCount );

private:
    /// Start multi-member send session
    bool                        startMultiSend( QWidget* parent, HostedId& hostId, AssetBaseInfo& assetInfo, std::set<VxGUID>& sendToSet, bool fromAdmin = false );

    /// Send to next member in queue, or finish if queue empty
    bool                        sendToNextMember( VxGUID assetId );

    /// Called when current send completes (success/error/cancel)
    void                        onCurrentSendComplete( VxGUID assetId, ESendResult result );

    /// Finish the multi-send session and report results
    void                        finishMultiSend( VxGUID assetId );

    /// Get AppCommon instance
    AppCommon&                  getMyApp( void );

    /// Register/unregister for activity callbacks
    void                        wantActivityCallbacks( bool enable );

    //=== vars ===//
    std::vector<std::pair<VxGUID, MultiSendSession>> m_SessionList;
    bool                        m_CallbacksRegistered{ false };
};


