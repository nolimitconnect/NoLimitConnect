# Camera Capture

The Camera Capture module handles video chat and video recording/playback across various system modules.

## Goals

*   **Capture Source:** Use `QCamera` as the primary capture source.
*   **Motion Detection:** Implement frame-by-frame detection with a value range of 0–100,000.
*   **Efficiency & Performance:**
    *   **Frame Rate:** 15 fps to conserve bandwidth and CPU usage.
    *   **Resolution:** 320x240 pixel frames to minimize CPU load.
    *   **Format:** JPEG for easy MJPEG integration and reduced resource consumption.



