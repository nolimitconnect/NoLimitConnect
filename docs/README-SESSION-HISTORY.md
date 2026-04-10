# Session History

This document describes how session history is loaded, displayed, and trimmed.

## Overview

- Session history is represented as asset records (text, face, photo, voice, video).
- `HistoryListWidget` receives history items from engine callbacks and creates an asset widget per record.
- Widgets are inserted in creation-time order so the list remains chronological.

## Loading Flow

1. `HistoryListWidget::setGroupieId` clears current UI list and calls `fromGuiQuerySessionHistory`.
2. Engine callbacks deliver matching assets through `toGuiAssetSessionHistory`.
3. A concrete widget type is created based on asset type.
4. Widget type used can be `AssetTextWidget`, `AssetFaceWidget`, `AssetPhotoWidget`, `AssetVoiceWidget`, or `AssetVideoWidget`.

## Add To Library (Media)

- Media widgets expose a library button through avatar bars.
- Clicking that button emits `signalAddLibraryAsset(AssetBaseWidget*)`.
- `HistoryListWidget::slotAddAssetToLibrary` validates the asset is a file/media asset.
- `HistoryListWidget::slotAddAssetToLibrary` calls `P2PEngine::fromGuiSetFileIsInLibrary(fileInfo, true)`.
- `HistoryListWidget::slotAddAssetToLibrary` marks local asset state as in-library.
- `HistoryListWidget::slotAddAssetToLibrary` hides the library button for that widget.

## Max Message History Pruning

- `HistoryListWidget` enforces `VxGetMaxMessageHistory()` after adding each new history item.
- If item count exceeds max, oldest items are removed first.

Prune behavior for each removed item:

- Non-file assets are removed from asset manager (`eAssetActionRemoveFromAssetMgr`).
- File/media assets in Library are removed from asset manager only (`eAssetActionRemoveFromAssetMgr`).
- File/media assets not in Library are shredded/deleted (`eAssetActionShreadFile`).

This prevents orphaned media accumulation while preserving user-kept media in Library.
