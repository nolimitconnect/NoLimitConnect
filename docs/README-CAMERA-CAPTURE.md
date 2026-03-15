# Camera Capture

The Camera Capture module handles video chat and video recording/playback across various system modules.

## Purpose

Quick reference for camera capture flow, performance constraints, and Linux-specific capture behavior.

## Goals

*   **Capture Source:** Use `QCamera` as the primary capture source.
*   **Motion Detection:** Implement frame-by-frame detection with a value range of 0–100,000.
*   **Efficiency & Performance:**
    *   **Frame Rate:** 15 fps to conserve bandwidth and CPU usage.
    *   **Resolution:** 320x240 pixel frames to minimize CPU load.
    *   **Format:** JPEG for easy MJPEG integration and reduced resource consumption.

## Pipeline Flow

```
QCamera
  └─► QVideoSink (signal: videoFrameChanged)
      └─► CamFrameProcessor::slotVideoFrameChanged()
          │  Rate-limited: 60 ms interval (≈15 fps)
          │  Converts QVideoFrame → QImage::Format_RGB888
          │  Stride-safe copy: respects QImage::bytesPerLine()
          └─► CamProcessor::processCamCapture()   [caller thread]
              │  Wraps data in CamRgbVideo, pushes to std::queue
              │  Notifies condition_variable
              └─► processCamRgbThreaded()        [single worker thread]
                  │  Dequeues via condition_variable wait (abort-safe)
                  │  calculateImageMotion()    [1/4-pixel subsample, O(n/4)]
                  │  VxResizeRgbImage()        [if frame != 320×240 or rotation != 0]
                  │  VxBmp2Jpg()               [libjpeg-turbo TJSAMP_420, TJFLAG_NOREALLOC]
                  └─► CamLogic::processCamCapture()
                      └─► MediaProcessor::processCamCaptureJpgVideo()
```

Code links:

- [nolimitgui/src/CamLogic.cpp](nolimitgui/src/CamLogic.cpp)
- [nolimitgui/src/CamProcessor.cpp](nolimitgui/src/CamProcessor.cpp)
- [nolimitgui/src/CamFrameProcessor.cpp](nolimitgui/src/CamFrameProcessor.cpp)
- [nolimitgui/src/CamV4L2.cpp](nolimitgui/src/CamV4L2.cpp)
- [libs/libptopengine/MediaProcessor/MediaProcessor.cpp](libs/libptopengine/MediaProcessor/MediaProcessor.cpp)

## Linux Capture Note

On Linux builds, `CamV4L2` was added as a direct V4L2 capture path to avoid a Qt Multimedia issue seen in Ubuntu/VirtualBox environments where camera frames could appear scrambled or partially delivered.

Qt Multimedia remains part of the non-Linux capture path, but Linux prefers `CamV4L2` specifically to avoid that corruption behavior.

## Encoding Details

| Property       | Value                          | Notes                                     |
|---------------|-------------------------------|-------------------------------------------|
| Capture format | QImage::Format_RGB888         | Converted from any camera pixel format   |
| Output size    | 320 × 240 pixels              | Downscaled with Bresenham average filter |
| JPEG quality   | 75                            | Diminishing returns above 75             |
| Chroma subsampling | TJSAMP_420               | 4:2:0, standard for MJPEG / video        |
| DCT mode       | TJFLAG_FASTDCT                | Integer DCT, adequate quality vs speed   |
| Frame rate     | ≈15 fps (60 ms gate)          | `CamLogic::CAM_SNAPSHOT_INTERVAL_MS`     |
| Motion range   | 0 – 100,000                   | Normalised to `dataLen × 64` sensitivity |

## Development Notes

### Thread Architecture

A single worker thread (`processCamRgbThreaded`) handles the full pipeline: motion detection →
optional rescale/rotate → JPEG compression → dispatch to `MediaProcessor`. This replaces the
previous two-thread design (separate RGB and JPG queues) to eliminate the inter-thread semaphore
round-trip at 15 fps. The thread waits on a `std::condition_variable` and exits cleanly on
`m_Abort` without polling.

Code links:

- [nolimitgui/src/CamProcessor.cpp](nolimitgui/src/CamProcessor.cpp)
- [nolimitgui/src/CamProcessor.h](nolimitgui/src/CamProcessor.h)

### Queue

Frames are queued as `CamRgbVideo*` in a `std::queue<CamRgbVideo*>` (O(1) push/pop). The
`isStalled()` guard in `CamLogic::canProcessCamCapture()` drops incoming frames when the queue
depth exceeds 1, preventing unbounded memory growth on slow hardware.

Code links:

- [nolimitgui/src/CamLogic.cpp](nolimitgui/src/CamLogic.cpp)
- [nolimitgui/src/CamProcessor.cpp](nolimitgui/src/CamProcessor.cpp)

### Motion Detection Subsampling

`calculateImageMotion` samples every 4th pixel (`MOTION_STEP = 12`, i.e. every 4th RGB triplet)
giving an 8× speedup over full-scan with negligible accuracy loss for motion gating purposes.
The sensitivity denominator is scaled proportionally: `(dataLen / MOTION_STEP) × 64`.

Code links:

- [nolimitgui/src/CamProcessor.cpp](nolimitgui/src/CamProcessor.cpp)



