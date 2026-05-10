# Player-NLC Remote Video Stream Path (Read/Seek)

## Scope

This document covers only the path where media is streamed from a remote peer into Player-NLC using file-transfer streaming semantics (remote read, seek, stream control).

It does not cover camera-live MJPEG broadcast paths.

## Entry Path: GUI -> Player-NLC Stream Mode

1. A stream asset is launched via Player-NLC.
2. `AppletPlayerNlcBase::playAsset(...)` checks `assetInfo.isStream()` and routes to `playStream(...)`.
3. `AppletPlayerNlcBase::playStream(...)` starts two coordinated paths:

- `GetVirtStreamMgr().fromGuiPlayStream(...)` to initialize virtual remote file state.
- `IMediaPlayerRequests::getNlcPlayer().fromGuiPlayStream(...)` to start the player module in stream mode.

Code:

- [nolimitgui/src/AppletPlayerNlcBase.cpp](../nolimitgui/src/AppletPlayerNlcBase.cpp)
- [nolimitgui/src/VirtStream/VirtStreamMgr.cpp](../nolimitgui/src/VirtStream/VirtStreamMgr.cpp)

## Stream Session Setup

`VirtStreamMgr::fromGuiPlayStream(...)` prepares the stream session:

1. Copies stream asset metadata into `m_LiveStream` and forces `isStream=true`.
2. Stores local session id.
3. Resolves best connection to the remote owner (`assetInfo.getDestUserId()`).
4. Registers for file-transfer callbacks.
5. Calls `PluginFileShareClient::startStream(...)`.

`PluginFileShareClient::startStream(...)` converts the asset to `FileInfo` (stream=true) and starts transfer via `FileInfoXferMgr::startDownload(...)`.

Code:

- [nolimitgui/src/VirtStream/VirtStreamMgr.cpp](../nolimitgui/src/VirtStream/VirtStreamMgr.cpp)
- [libs/libptopengine/Plugins/PluginFileShareClient.cpp](../libs/libptopengine/Plugins/PluginFileShareClient.cpp)
- [libs/libptopengine/Plugins/FileInfoXferMgr.cpp](../libs/libptopengine/Plugins/FileInfoXferMgr.cpp)

## Remote Read Path

The player reads via virtual file APIs; the stream manager serves bytes from remote-fed caches.

### 1) Virtual file open

- `VirtStreamMgr::fileOpen(...)` selects virtual stream handling.
- `VirtStreamMgr::virtFileOpen(...)` validates requested file name against active stream asset and creates `m_LiveStream.m_VFile` with logical file length.

Code:

- [nolimitgui/src/VirtStream/VirtStreamMgrFileIo.cpp](../nolimitgui/src/VirtStream/VirtStreamMgrFileIo.cpp)

### 2) Virtual read

- `VirtStreamMgr::fileRead(...)` dispatches to `virtFileRead(...)` for virtual stream files.
- `virtFileRead(...)`:
  1. Computes `readAttemptLen` based on file length and current offset.
  2. Calls `waitForStream(fileOffs, readAttemptLen)`.
  3. Reads bytes from `m_FileTail` first (if present), else from `m_StreamCache`.
  4. Advances virtual file offset when full read succeeds.

Code:

- [nolimitgui/src/VirtStream/VirtStreamMgrFileIo.cpp](../nolimitgui/src/VirtStream/VirtStreamMgrFileIo.cpp)
- [nolimitgui/src/VirtStream/VirtStreamMgr.cpp](../nolimitgui/src/VirtStream/VirtStreamMgr.cpp)

### 3) How cache gets filled

- Incoming transfer packets are delivered to `VirtStreamMgr::onFileXferPktRxed(...)`.
- `PKT_TYPE_FILE_CHUNK_REQ` is handled by `onPktFileChunkReq(...)` and written into `m_StreamCache` at packet chunk offsets.
- Optional tail data arrives as `PKT_TYPE_STREAM_CTRL_REPLY` with `eStreamReadTail` and is written into `m_FileTail`.

Code:

- [nolimitgui/src/VirtStream/VirtStreamMgr.cpp](../nolimitgui/src/VirtStream/VirtStreamMgr.cpp)
- [libs/libptopengine/Plugins/FileInfoXferMgr.cpp](../libs/libptopengine/Plugins/FileInfoXferMgr.cpp)

## Remote Seek Path

### 1) User seek from Player-NLC UI

- Slider change triggers `AppletPlayerNlcBase::slotSliderChanged(...)`.
- UI sends seek request to player module with `fromGuiMediaPlayerSeek(sliderPos)`.

Code:

- [nolimitgui/src/AppletPlayerNlcBase.cpp](../nolimitgui/src/AppletPlayerNlcBase.cpp)

### 2) Virtual stream seek decision

- `VirtStreamMgr::virtFileSeek(...)` updates logical offset.
- If target position is not already in cache/tail, it sends stream control seek:
  - `sendStreamSeek(newPos)`
  - Builds `PktStreamCtrlReq` with `eStreamSeek`, session ids, asset id, and start offset.

Code:

- [nolimitgui/src/VirtStream/VirtStreamMgrFileIo.cpp](../nolimitgui/src/VirtStream/VirtStreamMgrFileIo.cpp)
- [nolimitgui/src/VirtStream/VirtStreamMgr.cpp](../nolimitgui/src/VirtStream/VirtStreamMgr.cpp)

### 3) Server-side seek handling

- Engine routes `PKT_TYPE_STREAM_CTRL_REQ` to FileShareServer plugin.
- `FileInfoXferMgr::onPktStreamCtrlReq(...)` handles `eStreamSeek`:
  1. Finds TX session by stream session id.
  2. Seeks sender file handle to requested offset.
  3. Updates transfer offset for subsequent chunk sends.
  4. Sends stream control reply back.

Code:

- [libs/libptopengine/P2PEngine/P2PEnginePktHandlers.cpp](../libs/libptopengine/P2PEngine/P2PEnginePktHandlers.cpp)
- [libs/libptopengine/Plugins/PluginFileShareServer.cpp](../libs/libptopengine/Plugins/PluginFileShareServer.cpp)
- [libs/libptopengine/Plugins/FileInfoXferMgr.cpp](../libs/libptopengine/Plugins/FileInfoXferMgr.cpp)

## Stop/End Teardown

When playback stops or ends in Player-NLC:

1. `AppletPlayerNlcBase::onPlaybackStopped(...)` / `onPlaybackEnded(...)` calls `GetVirtStreamMgr().onPlaybackStopped(...)` / `onPlaybackEnded(...)` when streaming.
2. `VirtStreamMgr::onStreamStop()` notifies engine and clears `m_LiveStream` state/caches.

Code:

- [nolimitgui/src/AppletPlayerNlcBase.cpp](../nolimitgui/src/AppletPlayerNlcBase.cpp)
- [nolimitgui/src/VirtStream/VirtStreamMgr.cpp](../nolimitgui/src/VirtStream/VirtStreamMgr.cpp)

## Packet Types In This Path

- `PKT_TYPE_FILE_GET_REPLY`: establishes stream session mapping (client stores server session id).
- `PKT_TYPE_FILE_CHUNK_REQ`: carries streamed file chunks into client stream cache.
- `PKT_TYPE_STREAM_CTRL_REQ` (`eStreamSeek`): client requests remote seek.
- `PKT_TYPE_STREAM_CTRL_REPLY` (`eStreamReadTail` and seek reply): server response/control data.

Code:

- [nolimitgui/src/VirtStream/VirtStreamMgr.cpp](../nolimitgui/src/VirtStream/VirtStreamMgr.cpp)
- [libs/libptopengine/Plugins/FileInfoXferMgr.cpp](../libs/libptopengine/Plugins/FileInfoXferMgr.cpp)
- [libs/libptopengine/P2PEngine/P2PEnginePktHandlers.cpp](../libs/libptopengine/P2PEngine/P2PEnginePktHandlers.cpp)
