# Emoticon Pixmap Caching System

## Overview

This document describes the centralized emoticon pixmap caching system in GuiThumbMgr that eliminates the ~10 second delay when opening emoticon-using features.

## Problem

Loading 50 emoticon SVGs synchronously takes ~10 seconds. This delay occurred when opening:
- InputFaceWidget (face picker in chat)
- AppletGalleryEmoticon
- Random Person Connect
- Any feature needing emoticon pixmaps

Previously, each consumer loaded SVGs independently and synchronously.

## Solution

Centralize emoticon pixmap storage in GuiThumbMgr with:
1. **48×48 base pixmaps** cached in memory
2. **Background loading** after login via QTimer  
3. **Fast path**: Load from existing .nlt files (created by gallery system)
4. **Slow path**: Load from SVG resources (first time only)
5. **On-demand scaling**: Return scaled copies from the 48×48 cache

## Architecture

### Cache Storage (GuiThumbMgr)

```cpp
// 48x48 pixmap cache
static constexpr int kEmoticonCacheSize = 48;
static constexpr int kEmoticonCount = 50;
std::vector<QPixmap> m_EmoticonPixmapCache;  // 50 cached pixmaps
std::vector<bool>    m_EmoticonLoaded;       // tracking flags
bool                 m_EmoticonCacheReady;   // all loaded flag
```

### Loading Flow

1. **Login completion** → `onSystemReady(true)` → `startEmoticonCacheLoad()`
2. **Background loading** via QTimer: 5 emoticons per 50ms tick
3. For each emoticon:
   - Try loading from .nlt file (fast - these are PNG files)
   - Fallback: Load from SVG resource (slow)
   - Scale to 48×48 and store in cache
4. When complete: `m_EmoticonCacheReady = true`, emit `signalEmoticonCacheReady()`

### .nlt File Strategy

- **Cache does NOT write .nlt files** - only reads them if they exist
- **Gallery system** (`generateEmoticon()`) creates full-size 240×240 .nlt files
- If user opens gallery first → .nlt files are created → cache benefits from fast path
- If user opens face picker first → cache loads from SVG (slower, but only once per session)

## Public API

```cpp
// Get single emoticon scaled to requested size
bool getEmoticonPixmap(int emoticonNum, QSize imageSize, QPixmap& retPixmap);

// Get all 50 emoticons scaled to requested size
bool getAllEmoticonPixmaps(QSize imageSize, QVector<QPixmap>& retPixmaps);

// Check if background loading is complete
bool isEmoticonCacheReady() const;

// Manually trigger background loading (called automatically on login)
void startEmoticonCacheLoad();

// Signal emitted when cache is ready
void signalEmoticonCacheReady();
```

## Consumer Changes

### InputFaceWidget

- Removed SVG loading from constructor
- Uses `getEmoticonPixmap()` in `refreshScaledPixmaps()` 
- Loads pixmaps on-demand from cache (near-instant if cache ready)

### Legacy API

`getEmoticonImage()` now redirects to `getEmoticonPixmap()` for backward compatibility.

### AppletGalleryEmoticon

- Uses existing `generateEmoticonsIfNeeded()` flow (creates full-size .nlt files)
- Gallery thumbnails are 240×240 loaded directly from SVG for quality
- Once .nlt files exist, other features (like InputFaceWidget) benefit from fast loading

## Performance Characteristics

| Scenario | Time |
|----------|------|
| First session, open face picker early | ~10s (loads SVGs in background) |
| Subsequent face picker opens (same session) | Instant (cached) |
| After gallery has been opened once (creates .nlt files) | Fast (~1s to load PNGs) |

## Design Decisions

- **48×48 cache size**: Good balance of quality vs memory (~200KB total)
- **FastTransformation**: Prioritizes speed over quality for scaling
- **QTimer chunked loading**: Avoids UI freeze during background load
- **No cache invalidation**: Emoticons are only added, never changed
- **Separate gallery path**: Gallery uses SVG directly for 240×240 quality

## Files Modified

- [GuiThumbMgr.h](../nolimitgui/src/GuiThumbMgr.h) - Cache members, public API
- [GuiThumbMgr.cpp](../nolimitgui/src/GuiThumbMgr.cpp) - Implementation
- [InputFaceWidget.h](../nolimitgui/src/InputFaceWidget.h) - Removed unused members
- [InputFaceWidget.cpp](../nolimitgui/src/InputFaceWidget.cpp) - Use cache API