# README - ChatRoom

This document summarizes how ChatRoom host/client behavior works and why ChatRoom restricts heavy media posting for host resource safety.

## Scope

ChatRoom is implemented as a host/client service using:

- GUI applets/widgets in `nolimitgui/src`
- host/client plugins in `libs/libptopengine/Plugins`
- shared asset history storage/replay in `libs/libptopengine/AssetBase`

## ChatRoom Client Analysis

### UI and session setup

`AppletChatRoomClient` configures the session widget for ChatRoom mode and applies a content limit:

- `ui.m_SessionWidget->setMediaModule( eMediaModuleChatRoomClient )`
- `ui.m_SessionWidget->setLimitToTextAndPhotos( true )`

The `setLimitToTextAndPhotos( true )` path is the key media restriction mechanism.

### How the restriction is applied

`SessionWidget::setLimitToTextAndPhotos()` forwards to `ChatEntryWidget::setLimitToTextAndPhotos()`.

`ChatEntryWidget::setLimitToTextAndPhotos()` then:

- hides video capture input
- hides audio capture input

Result: ChatRoom users cannot select video posting/capture from ChatRoom input controls.

Current code also hides mic input in this mode, so ChatRoom client UI currently behaves as text+photo only.

### History retrieval on client

`HistoryListWidget::setGroupieId()` calls:

- `m_Engine.fromGuiQuerySessionHistory( m_GroupieId )`

Assets received through history callbacks are displayed by type in the history widget list.

## ChatRoom Host Analysis

### Plugin behavior

`PluginChatRoomHost` extends `PluginBaseHostService` and uses shared host infrastructure (`HostServerMgr`) for:

- join/membership handling
- member list broadcast and sync
- relaying host traffic

### Member tracking and late-join state

`HostServerMgr` tracks active members in `m_ClientList`.

- `onUserJoinedHost(...)` adds joined user online IDs to `m_ClientList`
- `sendMemberListToClient(...)` sends existing member announces to newly connected clients

This supports late joiners seeing who is currently in the room.

## Asset Storage and History Retention

ChatRoom message/media assets are managed by `AssetBaseMgr`.

- `updateDatabase( AssetBaseInfo* )` persists assets to DB (`m_AssetBaseInfoDb.addAsset(...)`)
- `fromGuiQuerySessionHistory( GroupieId& )` enqueues history requests
- history is replayed to GUI from the in-memory/DB-managed asset list on a worker thread

Default max history is currently `g_MaxMessageHistory = 200` in `VxGlobals.cpp`.

This design means hosts keep assets/history available so users who join later can review prior chat activity.

## ChatRoom Restrictions

### Policy

- ChatRoom allows text and photos.
- ChatRoom does not allow video posting.
- Current ChatRoom client UI also hides voice recording input.

### Rationale

ChatRoom hosts may run on low-resource devices. Unlike ephemeral relay use-cases, ChatRoom history must remain available for late joiners, so the host has to retain posted assets. Video retention has significantly higher CPU/storage/network costs than text/photos, so video posting is restricted for ChatRoom.

### Enforcement details

Current enforcement is UI-based in chatroom client/session widgets by hiding video/audio capture controls (`setLimitToTextAndPhotos(true)`).

Note: engine/asset layers support video asset types in general; the ChatRoom restriction is applied at ChatRoom UI/input flow.

If product policy later decides ChatRoom should allow voice recordings while still blocking video, the current UI helper should be split so only video is hidden for ChatRoom client mode.

## Verified Post Routing Constraint

ChatRoom posting is already constrained by host context in the current send path.

### What is verified

1. Chatroom client send path uses the active ChatRoom `HostedId`:
   - `AppletHostClientBase::handleAssetAction()` calls `handleGroupieAssetAction(...)` with the current host admin groupie id.
   - `AppletBase::handleGroupieAssetAction(...)` sets `assetInfo.setHostedId( hostId )` before sending.

2. `setHostedId(...)` binds plugin/host type on the asset:
   - `AssetBaseInfo::setHostedId(...)` sets plugin type from host type (`HostTypeToClientPlugin`).

3. Transport selection respects plugin host type:
   - `AssetBaseXferMgr::fromGuiSendAssetBase(...)` resolves connection with `findBestUserOnlineConnection(sendToId, assetInfo.getPluginType())`.
   - `ConnectIdListMgr::findBestUserOnlineConnection(...)` derives host type from plugin type for relay hosts and searches connections matching that host type.

Result: a post is routed only through the connection context for its host type. In practice, posting to ChatRoom requires being in ChatRoom client/session context (or explicitly constructing a ChatRoom-hosted asset in code).

### Additional practical guard in UI flow

`AppletBase::handleGroupieAssetAction(...)` also requires active members for that specific `HostedId` (`getActiveMembers(hostId, ...)`), and aborts when none are online. This further prevents accidental sends outside an active joined host session.

## Comparison With Other Host Types

- Group Host client does not call `setLimitToTextAndPhotos(true)` in its applet setup, so it is not restricted in the same way.
- Random Connect client also applies `setLimitToTextAndPhotos(true)`.

## Summary

ChatRoom host/client uses the shared host and asset infrastructure, with a ChatRoom-specific UX restriction that blocks video posting at input controls. This aligns with ChatRoom's requirement to retain history for later viewers while keeping host resource use manageable on lower-end devices.
