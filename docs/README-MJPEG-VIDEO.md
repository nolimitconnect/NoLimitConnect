# MJPEG Video UI Modes

This document defines the `VidWidget` UI mode split to reduce confusion and save vertical space in screens that do not need full camera controls.

## Why

- Full camera controls are useful in chat/camera pages but too heavy in media asset and input contexts.
- Showing extra controls where they are not relevant causes user confusion.
- Smaller, purpose-specific layouts improve readability and message density in history and input areas.

## VidWidget Modes

`VidWidget` now supports 4 explicit UI modes:

- `eVideoUiModePhoto`
- `eVideoUiModeInputWidget`
- `eVideoUiModeAssetVideo`
- `eVideoUiModeChat`

## Mode Intent

- Photo mode:
  - No camera/record controls.
  - Keep only image rotation control.

- Input widget mode:
  - Rotate and select camera.
  - Record start/stop only.
  - Hide motion/snapshot/alarm controls that are not needed for this flow.

- Asset video mode:
  - Keep only image rotation in `VidWidget`.
  - Playback controls (play/pause/progress) remain owned by `AssetVideoWidget`.

- Chat mode:
  - Preserve full current control set and click-to-toggle behavior.

## Current Wiring

- `AssetPhotoWidget` sets `eVideoUiModePhoto`.
- `InputPhotoWidget` sets `eVideoUiModeInputWidget`.
- `InputVideoWidget` sets `eVideoUiModeInputWidget`.
- `AssetVideoWidget` sets `eVideoUiModeAssetVideo`.
- Default remains `eVideoUiModeChat` for existing chat/camera surfaces.

## Notes

- The mode split is UI-only and does not change media transport format or decode path.
- Existing media-module guards remain required so invalid module subscriptions do not reach `MediaProcessor::wantMediaInput`.
