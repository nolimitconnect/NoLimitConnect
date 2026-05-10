# Video Media Flow (Operator Guide)

## Purpose

Quick reference for how JPG video frames move through capture, GUI playback, and remote cam distribution.

## Core Components

- `CamLogic`: forwards completed camera JPG frames from `CamProcessor` into `MediaProcessor`.
- `MediaProcessor`: fan-out hub for JPG frames (`m_GuiPlayerCallback` + subscribed JPG clients).
- `GuiPlayerMgr`: local GUI playback endpoint (`callbackVideoJpg`, `toGuiPlayJpgVideo`).
- `PluginCamServer`: remote cam distribution path for shared webcam sessions.

## Flow A: Local Camera to GUI

```text
CamProcessor::processCamVideoIn
	-> CamLogic::processCamCapture
	-> MediaProcessor::processCamCaptureJpgVideo
	-> MediaProcessor::sendJpgVideo
	-> m_GuiPlayerCallback->callbackVideoJpg
	-> GuiPlayerMgr::callbackVideoJpg
	-> GuiPlayerMgr::signalInternalPlayCamJpg
	-> GuiPlayerMgr::slotInternalPlayCamJpg
```

Use this path to reason about local preview/live local display behavior.

Code links:

- [nolimitgui/src/CamProcessor.cpp](../nolimitgui/src/CamProcessor.cpp)
- [nolimitgui/src/CamLogic.cpp](../nolimitgui/src/CamLogic.cpp)
- [libs/libptopengine/MediaProcessor/MediaProcessor.cpp](../libs/libptopengine/MediaProcessor/MediaProcessor.cpp)
- [nolimitgui/src/GuiPlayerMgr.cpp](../nolimitgui/src/GuiPlayerMgr.cpp)

## Flow B: Camera Distribution to Remote Viewers

```text
MediaProcessor::sendCamPackets
	-> PluginCamServer::callbackVideoPktPic
	-> PluginCamServer::callbackVideoPktPicChunk
	-> PluginCamServer::sendVidPkt
	-> remote peers
```

This path is for cam sharing and is separate from local GUI playback.

Code links:

- [libs/libptopengine/MediaProcessor/MediaProcessor.cpp](../libs/libptopengine/MediaProcessor/MediaProcessor.cpp)
- [libs/libptopengine/Plugins/PluginCamServer.cpp](../libs/libptopengine/Plugins/PluginCamServer.cpp)
- [libs/libptopengine/Plugins/VideoFeedMgr.cpp](../libs/libptopengine/Plugins/VideoFeedMgr.cpp)

## Flow C: Non-Camera Sources to GUI

```text
MJPEGReader / plugin playback / friend feed reconstruction
	-> IToGui::toGuiPlayJpgVideo
	-> AppCommon::toGuiPlayJpgVideo
	-> GuiPlayerMgr::toGuiPlayJpgVideo
	-> GuiPlayerMgr::signalInternalPlayCamJpg
	-> GuiPlayerMgr::slotInternalPlayCamJpg
```

Code links:

- [libs/MediaToolsLib/MJPEGReader_mtools.cpp](../libs/MediaToolsLib/MJPEGReader_mtools.cpp)
- [libs/libptopengine/Plugins/PluginMgr.cpp](../libs/libptopengine/Plugins/PluginMgr.cpp)
- [nolimitgui/src/AppCommonMedia.cpp](../nolimitgui/src/AppCommonMedia.cpp)
- [nolimitgui/src/GuiPlayerMgr.cpp](../nolimitgui/src/GuiPlayerMgr.cpp)

## Debug Checklist

1. Track frame identity with `CamJpgVideo::m_FrameTag`.
2. Track origin with `CamJpgVideo::m_SourceModule`.
3. If `GuiPlayerMgr` logs `inSignal >= 1`, capture tag and source from that line.
4. Confirm whether the tag came from local capture flow (Flow A) or another source (Flow C).
5. If remote cam distribution is suspected, verify packet path activity (Flow B) and active peer sessions.

## Practical Notes

- `MediaProcessor::sendJpgVideo` delivers to both GUI callback and subscribed JPG clients.
- `MediaProcessor::sendCamPackets` is the packet distribution path used for remote viewers.
- Local GUI playback and remote packet distribution can be active at the same time.
