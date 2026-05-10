# Camera Video AVI Files

## Current AVI Recording System

The current camera recording path is a custom AVI muxer in MediaToolsLib.

- Writer: `MJPEGWriter` in `libs/MediaToolsLib/MJPEGWriter_mtools.cpp`
- Reader: `MJPEGReader` in `libs/MediaToolsLib/MJPEGReader_mtools.cpp`
- AVI structure definitions: `libs/MediaToolsLib/AviDefs.h` and `libs/MediaToolsLib/AviDefs_mtools.cpp`
- Wiring into media tools: `libs/MediaToolsLib/MediaTools.h`

Record flow:

1. Start recording opens an AVI file and writes initial RIFF/AVI headers.
2. The writer subscribes to media input:
	- Video as JPG frames (`eMediaInputVideoJpg`)
	- Audio as PCM (`eMediaInputAudioPcm`)
3. Video chunks are written as MJPEG (`00dc` chunks).
4. Audio chunks are written as PCM (`01wb` chunks).
5. On stop, the writer appends `idx1`, closes, then reopens and rewrites headers with final totals/timing.

Playback flow:

1. Reader validates expected AVI layout/signature markers.
2. Reader parses chunk offsets and stream metadata.
3. Video chunks are sent to GUI as JPG frames.
4. Audio chunks are sent to mixer playback callbacks.

## Current AVI Limitations

- Large files: MJPEG video plus PCM audio grows quickly.
- RIFF size ceiling in this path: 2,147,483,648 bytes (2 GiB).
- Final file depends on successful stop-time finalization (`idx1` + header rewrite).
- Audio stream is PCM-oriented in AVI (`m_FormatTag = 1`), which is not a clean standard path for Opus-in-AVI interoperability.

## Approximate Max Record Time Before 2 GiB

Using sample file `nolimitgui/MediaTestFiles/TicTockCountCamClip.avi`:

- File size: 3,775,206 bytes
- Duration: about 14.8 seconds
- Average write rate: about 255,082 bytes/second

Estimated time to reach 2,147,483,648 bytes:

- About 8,418 seconds
- About 140.3 minutes
- About 2 hours 20 minutes

If using decimal 2,000,000,000 bytes, estimate is about 130.7 minutes.

## Why Move To MKV

MKV with MJPEG video plus Opus audio provides practical improvements:

- Standard VLC-friendly container/codec combination
- Much smaller audio track versus PCM
- No RIFF 2 GiB container behavior
- Cleaner, more robust finalization than custom AVI index/header rewrite flow
- Better long-term extensibility for modern stream metadata/codecs

## Expected Size Change For This Sample

For this sample, changing AVI (MJPEG + PCM) to MKV (MJPEG + Opus) is estimated to reduce total size by about 12%.

- Current sample: about 3.60 MB
- Estimated MKV equivalent: about 3.15 MB
- Estimated reduction: about 0.45 MB

The percentage gain is modest because MJPEG video dominates total size; the main win is replacing PCM audio with Opus.
