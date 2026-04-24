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
    Pending,
    Success,
    Error,
    Canceled,
    PermissionError
};

/// Tracks state of a multi-member send session
struct MultiSendSession
{
    AssetBaseInfo               assetInfo;
    HostedId                    hostId;
    std::vector<VxGUID>         memberQueue;        // remaining members to send to
    VxGUID                      currentMemberId;    // member currently being sent to
    VxGUID                      currentAssetId;     // assetId for tracking callbacks
    std::map<VxGUID, ESendResult> results;          // memberId -> result
    bool                        canceled{ false };
    bool                        isActive{ false };
    bool                        fromAdmin{ false }; // true if host admin is sending
    QWidget*                    parentWidget{ nullptr };
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
    void                        cancelMultiSend( void );

    /// Check if a multi-send is currently active
    bool                        isMultiSendActive( void ) const { return m_Session.isActive; }

    /// Get results from last multi-send session
    const std::map<VxGUID, ESendResult>& getLastSendResults( void ) const { return m_Session.results; }

    /// Retry failed sends from last session
    bool                        retryFailedSends( void );

    // ToGuiActivityInterface overrides
    void                        toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 ) override;

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
    bool                        startMultiSend( QWidget* parent, HostedId& hostId, AssetBaseInfo& assetInfo, std::vector<VxGUID>& memberList, bool fromAdmin = false );

    /// Send to next member in queue, or finish if queue empty
    void                        sendToNextMember( void );

    /// Called when current send completes (success/error/cancel)
    void                        onCurrentSendComplete( ESendResult result );

    /// Finish the multi-send session and report results
    void                        finishMultiSend( void );

    /// Get AppCommon instance
    AppCommon&                  getMyApp( void );

    /// Register/unregister for activity callbacks
    void                        wantActivityCallbacks( bool enable );

    //=== vars ===//
    MultiSendSession            m_Session;
    bool                        m_CallbacksRegistered{ false };
};


