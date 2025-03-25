package com.nolimitconnect.nolimitconnect;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.graphics.ImageFormat;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.media.Image;
import android.media.ImageReader;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;
import android.util.Range;
import androidx.annotation.Nullable;
import java.nio.ByteBuffer;
import java.util.Arrays;

public class Camera2Service extends Service {
    private static final String TAG = "Camera2Service";
    private static final int MIN_CAPTURE_INTERVAL_MS = 28; // limit to about 30 fps.. 15 fps still looks ok so could be 60
    private long m_LastCaptureTimeMs = 0;

    // native methods
    public native void camServiceStarted();
    public native void camServiceStopped();
    public native boolean canProcessCamCapture();
    public native void processCamCapture(   int imgWidth, int imgHeight, ByteBuffer y, ByteBuffer u, ByteBuffer v,
                                            int yPixelStride, int yRowStride,
                                            int uPixelStride, int uRowStride,
                                            int vPixelStride, int vRowStride );

    protected CameraDevice m_CameraDevice = null;
    protected CameraCaptureSession m_CaptureSession = null;
    protected CameraManager m_CameraManager = null;
    protected ImageReader m_ImageReader = null;
    protected String m_CameraId;

    public Camera2Service()
    {
        Log.d(TAG, "Camera2Service() with NO context Called");
    }

    public Camera2Service(Context context)
    {
        Log.d(TAG, "Camera2Service() WITH context Called");
    }

    public static void startCamServiceStatic(Context context) {
        Log.d(TAG, "startCamServiceStatic Called");
        context.startForegroundService(new Intent(context, Camera2Service.class));
        Log.d(TAG, "startCamServiceStatic Done");
    }

    // Binder class to allow clients to bind to the service
    private final IBinder localBinder = new LocalBinder();

    public class LocalBinder extends Binder {
        Camera2Service getService() {
            return Camera2Service.this;
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.d(TAG, "onBind Camera Service Called");
        return localBinder;
    }

    @Override
    public void onCreate() {
        Log.d(TAG, "onCreate Camera Service Created");
        super.onCreate();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, "onStartCommand Called");
        m_CameraManager = (CameraManager) getSystemService(CAMERA_SERVICE);
        super.onStartCommand(intent, flags, startId);

        camServiceStarted();

        // Keep service running until explicitly stopped
        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        Log.d(TAG, "Camera Service Destroyed");
        stopCameraCapture();
        camServiceStopped();

        super.onDestroy();   
    }


    public int getValue() {
        return 10;
    }

    public String[] getCameraIdList() {
        try {
            // Usually the first camera in list is rear-facing
            return m_CameraManager.getCameraIdList();
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }

        String[] emptyList = new String[0]; // Creates an empty array
        return emptyList;
    }

    public boolean isCameraBackFacing(String cameraId) {
        boolean isBackFacing = false;
        try {
            CameraCharacteristics characteristics = m_CameraManager.getCameraCharacteristics( cameraId );
            int facing = characteristics.get(CameraCharacteristics.LENS_FACING);
            if ( facing == CameraCharacteristics.LENS_FACING_BACK ) {
                isBackFacing = true;
            }
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }

        return isBackFacing;
    }

    public boolean startCameraCapture(String cameraId)
    {
        return openCamera( cameraId );
    }

    public void stopCameraCapture()
    {
        if (m_CaptureSession != null) {
            m_CaptureSession.close();
            m_CaptureSession = null;
        }

        if (m_CameraDevice != null) {
            m_CameraDevice.close();
            m_CameraDevice = null;
        }
    }

    private final CameraDevice.StateCallback m_CameraStateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(CameraDevice cameraDevice) {
            Log.d(TAG, "camera onOpened");
            m_CameraDevice = cameraDevice;
            createCameraCaptureSession( cameraDevice );
        }

        @Override
        public void onDisconnected(CameraDevice cameraDevice) {
            Log.d(TAG, "camera onDisconnected");
            cameraDevice.close();
            m_CameraDevice = null;
        }

        @Override
        public void onError(CameraDevice cameraDevice, int error) {
            Log.e(TAG, "camera onError " + error );
            cameraDevice.close();
            m_CameraDevice = null;
        }
    };

    private final CameraCaptureSession.StateCallback m_SessionStateCallback = new CameraCaptureSession.StateCallback() {
        @Override
        public void onReady(CameraCaptureSession session) {
            Log.d(TAG, "capture session onReady");
            try {
                session.setRepeatingRequest(createCameraCaptureRequest(), null, null);
            } catch (CameraAccessException e) {
                Log.e(TAG, e.getMessage());
            }
        }

        @Override
        public void onConfigured(CameraCaptureSession session) {
            Log.d(TAG, "capture session onConfigured");
        }

        @Override
        public void onConfigureFailed( CameraCaptureSession session) {
            Log.d(TAG, "capture session onConfigureFailed");
        }
    };

    public void createCameraCaptureSession(CameraDevice camera) {
        try {
            m_CameraDevice = camera;
            camera.createCaptureSession(
                Arrays.asList(m_ImageReader.getSurface()),
                m_SessionStateCallback,
                null);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    protected CaptureRequest createCameraCaptureRequest() {
        try {
            CaptureRequest.Builder builder = m_CameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_RECORD);
            CameraCharacteristics characteristics = m_CameraManager.getCameraCharacteristics(m_CameraId);

            // Get the available FPS ranges
            Range<Integer>[] fpsRanges = characteristics.get(CameraCharacteristics.CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES);

            // If FPS ranges are available, loop through and log or choose a suitable one
            if (fpsRanges != null) {
                for (Range<Integer> fpsRange : fpsRanges) {
                    int minFps = fpsRange.getLower(); // Minimum FPS in the range
                    int maxFps = fpsRange.getUpper(); // Maximum FPS in the range

                    // Log or print each range
                    System.out.println("Supported FPS Range: " + minFps + " - " + maxFps);
                }

                for (Range<Integer> fpsRange : fpsRanges) {
                    int minFps = fpsRange.getLower(); // Minimum FPS in the range
                    int maxFps = fpsRange.getUpper(); // Maximum FPS in the range
                    // Here, you could choose to set the FPS to a specific range, for example:
                    if (minFps >= 15 && maxFps <= 30) {
                        builder.set(CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE, fpsRange);
                        System.out.println("Selected FPS Range: " + minFps + " - " + maxFps);
                        break;
                    }
                }
            }

            builder.addTarget(m_ImageReader.getSurface());
            return builder.build();
        } catch (CameraAccessException e) {
            Log.e(TAG, e.getMessage());
            return null;
        }
    }

    protected ImageReader.OnImageAvailableListener onImageAvailableListener = new ImageReader.OnImageAvailableListener() {
        @Override
        public void onImageAvailable(ImageReader reader) {
            Image img = reader.acquireNextImage();
            if( img != null )
            {
                if (canProcessCamCapture())
                {
                    processImage(img);       
                }

                img.close();
            }
        }
    };

    private boolean openCamera(String cameraId) {
        boolean result = false;
        try {
            stopCameraCapture(); // in case was already running

            if( m_ImageReader == null )
            {
                m_ImageReader = ImageReader.newInstance(320, 240, ImageFormat.YUV_420_888, 2 /* number of images buffered */);
                m_ImageReader.setOnImageAvailableListener(onImageAvailableListener, null);
            }

            m_CameraId = cameraId;
            m_CameraManager.openCamera( cameraId, m_CameraStateCallback, null );

            result = true;

        } catch (Exception e) {
            Log.e(TAG, "Error opening camera", e);
        }

        return result;
    }

    private void processImage(Image image) {
        int width = image.getWidth();
        int height = image.getHeight();

        final Image.Plane[] planes = image.getPlanes();
        Image.Plane yPlane = planes[0];
        Image.Plane uPlane = planes[1];
        Image.Plane vPlane = planes[2];

        processCamCapture( width, height,
                        yPlane.getBuffer(),
                        uPlane.getBuffer(),
                        vPlane.getBuffer(),
                        yPlane.getPixelStride(),
                        yPlane.getRowStride(),
                        uPlane.getPixelStride(),
                        uPlane.getRowStride(),
                        vPlane.getPixelStride(),
                        vPlane.getRowStride() );
    }
}
