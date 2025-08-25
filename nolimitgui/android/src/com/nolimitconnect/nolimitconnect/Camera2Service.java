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
import android.media.Image;
import android.media.ImageReader;
import android.os.Binder;
import android.os.IBinder;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.util.Range;
import androidx.annotation.Nullable;
import java.nio.ByteBuffer;
import java.util.Arrays;

public class Camera2Service extends Service {
    private static final String TAG = "NLC Camera2Service";

    private Handler m_MainThreadHandler = null;

    // native methods
    public native void camServiceStarted();
    public native void camServiceStopped();
    public native boolean canProcessCamCapture();
    public native void processCamCapture(   int imgWidth, int imgHeight, ByteBuffer y, ByteBuffer u, ByteBuffer v,
                                            int yPixelStride, int yRowStride,
                                            int uPixelStride, int uRowStride,
                                            int vPixelStride, int vRowStride );

    protected CameraManager m_CameraManager = null;
    protected CameraDevice m_CameraDevice = null;
    protected ImageReader m_ImageReader = null;
    protected String m_CameraId;
    private CameraCaptureSession m_CaptureSession = null;

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
        // for unknown reason foreground service cause gradlelock and other issues on some devices
        // do not run in forground
        // context.startForegroundService(new Intent(context, Camera2Service.class));

        context.startService( new Intent( context, Camera2Service.class ) );
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
        // Initialize the Handler for running tasks on the main thread
        m_MainThreadHandler = new Handler(Looper.getMainLooper());
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        super.onStartCommand(intent, flags, startId);

        m_CameraManager = (CameraManager) getSystemService(CAMERA_SERVICE);
        // must start quickly or will shutdown. use runnable
        m_MainThreadHandler.post(new Runnable() {
            @Override
            public void run() {
                Log.d(TAG, "onStartCommand Called");
                camServiceStarted();
            }
        });

        // Keep service running until explicitly stopped
        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        Log.d(TAG, "Camera Service Destroyed");
        internalStopCameraCapture();
        camServiceStopped();

        if (m_MainThreadHandler != null) {
            m_MainThreadHandler.removeCallbacksAndMessages(null);
            m_MainThreadHandler = null;
        }

        super.onDestroy();   
    }

    public void onConfigured(CameraCaptureSession session) {
        Log.d(TAG, "capture session onConfigured");
        m_CaptureSession = session;
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
        m_CameraId = cameraId;
        Log.d(TAG, "**** begin startCameraCapture " + cameraId );
        // has to run on main thread
        m_MainThreadHandler.post(new Runnable() {
            @Override
            public void run() {
                Log.d(TAG, "**** main thread openCamera " + cameraId );
                openCamera( m_CameraId );
            }
        });


        Log.d(TAG, "**** end startCameraCapture " + cameraId );
        return true;
    }

    public void stopCameraCapture()
    {
        Log.d(TAG, "**** begin stopCameraCapture " + m_CameraId );
        // has to run on main thread
        m_MainThreadHandler.post(new Runnable() {
            @Override
            public void run() {
                internalStopCameraCapture();
            }
       });

        Log.d(TAG, "**** end stopCameraCapture " + m_CameraId );
    }

    public void internalStopCameraCapture()
    {
        Log.d(TAG, "**** internalStopCameraCapture " + m_CameraId );

        if (m_CameraDevice != null) {
            m_CameraDevice.close();
            m_CameraDevice = null;
        }

        if (m_ImageReader != null) {
            m_ImageReader.close();
            m_ImageReader = null;
        }

        if (m_CaptureSession != null) {
            try {
                m_CaptureSession.stopRepeating();
                m_CaptureSession.abortCaptures();
                m_CaptureSession.close();
            } catch (CameraAccessException e) {
                Log.e(TAG, "Error stopping capture session", e);
            }
            m_CaptureSession = null;
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

            Range<Integer>[] fpsRanges = characteristics.get(CameraCharacteristics.CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES);
            Range<Integer> selectedRange = null;

            if (fpsRanges != null) {
                for (Range<Integer> range : fpsRanges) {
                    if (range.getLower() <= 15 && range.getUpper() >= 15) {
                        selectedRange = Range.create(15, 15); // Fix to 15 FPS
                        break;
                    }
                }

                if (selectedRange == null) {
                    // Fallback: pick the closest range below 30
                    for (Range<Integer> range : fpsRanges) {
                        if (range.getLower() >= 15 && range.getUpper() <= 30) {
                            selectedRange = range;
                            break;
                        }
                    }
                }

                if (selectedRange != null) {
                    builder.set(CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE, selectedRange);
                    Log.d(TAG, "Selected FPS Range: " + selectedRange);
                } else {
                    Log.w(TAG, "No suitable FPS range found. Using default.");
                }
            }

            builder.set(CaptureRequest.CONTROL_MODE, CaptureRequest.CONTROL_MODE_AUTO);
            builder.set(CaptureRequest.NOISE_REDUCTION_MODE, CaptureRequest.NOISE_REDUCTION_MODE_OFF);
            builder.set(CaptureRequest.EDGE_MODE, CaptureRequest.EDGE_MODE_OFF);
            builder.set(CaptureRequest.COLOR_CORRECTION_ABERRATION_MODE, CaptureRequest.COLOR_CORRECTION_ABERRATION_MODE_OFF);
            builder.set(CaptureRequest.CONTROL_VIDEO_STABILIZATION_MODE, CaptureRequest.CONTROL_VIDEO_STABILIZATION_MODE_OFF);

            builder.addTarget(m_ImageReader.getSurface());

            return builder.build();
        } catch (CameraAccessException e) {
            Log.e(TAG, "CameraAccessException in createCameraCaptureRequest: " + e.getMessage());
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
            internalStopCameraCapture(); // in case was already running

            if( m_ImageReader == null )
            {
                m_ImageReader = ImageReader.newInstance(320, 240, ImageFormat.YUV_420_888, 1 /* number of images buffered */);
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
        image.close();
    }
}
