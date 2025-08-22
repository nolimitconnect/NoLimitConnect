//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#if defined(ENABLE_JAVA_CAM)

#include "CamJavaClient.h"

#include "CamLogic.h"
#include "CamProcessor.h"

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxJava.h>
#include <CoreLib/VxThread.h>
#include <CoreLib/VxTime.h>

#include <QTimer>
#include <QtCore/qcoreapplication.h>
#include <QtCore/private/qandroidextras_p.h>

#include <iostream>

#define CAM_CAPTURE_CLASS_NAME "com/nolimitconnect/nolimitconnect/Camera2Service"

namespace {
CamJavaClient* g_CamClient = nullptr;
CamJavaClient& GetCamJavaClient() {
    vx_assert( g_CamClient != nullptr );
    return *g_CamClient;
}

typedef struct CamServiceMethod {
    jmethodID methodID;
    const char* methodName;
    const char* methodSigniture;
} CamServiceMethod;

const int CAM_GET_IDS_IDX = 0;
const int CAM_IS_BACKFACING_IDX = 1;
const int CAM_START_CAPTURE_IDX = 2;
const int CAM_STOP_CAPTURE_IDX = 3;
const int CAM_GET_VALUE_IDX = 4;
CamServiceMethod g_CamMethods[] {
    {0, "getCameraIdList", "()[Ljava/lang/String;"},
    {0, "isCameraBackFacing", "(Ljava/lang/String;)Z"},
    {0, "startCameraCapture", "(Ljava/lang/String;)Z"},
    {0, "stopCameraCapture", "()V"},
    {0, "getValue", "()I"}
};

JNIEnv* g_CamEnv = nullptr;
jobject g_CamObj = nullptr;
unsigned int g_CamServiceThreadId = 0;
bool g_CamServiceReady = false;

static std::vector<std::pair<unsigned int, JNIEnv*>> g_JavaEnvList;

JNIEnv* GetJniEnv( void )
{
    unsigned int threadId = VxGetCurrentThreadId();
    if( threadId == g_CamServiceThreadId)
    {
        return g_CamEnv;
    }

    for(auto& pair : g_JavaEnvList)
    {
        if( pair.first == threadId)
        {
            return pair.second;
        }
    }

    JNIEnv* jniEnv = nullptr;
    if (GetJavaVM()->AttachCurrentThread(&jniEnv, NULL) == JNI_OK)
    {
        g_JavaEnvList.emplace_back(std::make_pair(threadId, jniEnv));
        return jniEnv;
    }
    else
    {
        LogMsg( LOG_ERROR, "%s AttachCurrentThread failed", __func__ );
        return nullptr;
    }
}

#if defined(TARGET_CPU_ARM64) && defined(TARGET_OS_ANDROID)
#include <arm_neon.h>
#include <stdint.h>

static inline uint8x8_t clamp_u8(int16x8_t val) {
    return vqmovun_s16(vcombine_s16(vqmovn_s32(vmovl_s16(vget_low_s16(val))),
                                    vqmovn_s32(vmovl_s16(vget_high_s16(val)))));
}
// optimized for neon
void AndroidYUV420SPtoRGB(uint8_t* rgbImage, int width, int height,
                               const uint8_t* yPlane, const uint8_t* uPlane, const uint8_t* vPlane,
                               int yPixelStride, int yRowStride,
                               int uPixelStride, int uRowStride,
                               int vPixelStride, int vRowStride) {
    for (int row = 0; row < height; row++) {
        const uint8_t* pY = yPlane + row * yRowStride;
        const uint8_t* pU = uPlane + (row / 2) * uRowStride;
        const uint8_t* pV = vPlane + (row / 2) * vRowStride;
        uint8_t* pRGB = rgbImage + row * width * 3;

        int col = 0;
        for (; col <= width - 8; col += 8) {
            // Load Y values
            uint8x8_t y = vld1_u8(pY + col * yPixelStride);

            // Load U and V (subsampled every 2 pixels)
            uint8x8_t u = vld1_u8(pU + (col / 2) * uPixelStride);
            uint8x8_t v = vld1_u8(pV + (col / 2) * vPixelStride);

            int16x8_t u_s16 = vreinterpretq_s16_u16(vmovl_u8(u));
            int16x8_t v_s16 = vreinterpretq_s16_u16(vmovl_u8(v));
            int16x8_t y_s16 = vreinterpretq_s16_u16(vmovl_u8(y));

            u_s16 = vsubq_s16(u_s16, vdupq_n_s16(128));
            v_s16 = vsubq_s16(v_s16, vdupq_n_s16(128));

            // Integer approximation of:
            // R = Y + 1.402 * V
            // G = Y - 0.344136 * U - 0.714136 * V
            // B = Y + 1.772 * U

            int16x8_t r = vaddq_s16(y_s16, vshrq_n_s16(vqdmulhq_s16(v_s16, vdupq_n_s16(22970)), 1)); // 1.402 * 2^14 ≈ 22970
            int16x8_t g = vsubq_s16(y_s16, vshrq_n_s16(vqdmulhq_s16(u_s16, vdupq_n_s16(11277)), 1)); // 0.344136 * 2^14 ≈ 11277
            g = vsubq_s16(g, vshrq_n_s16(vqdmulhq_s16(v_s16, vdupq_n_s16(23401)), 1)); // 0.714136 * 2^14 ≈ 23401
            int16x8_t b = vaddq_s16(y_s16, vshrq_n_s16(vqdmulhq_s16(u_s16, vdupq_n_s16(29032)), 1)); // 1.772 * 2^14 ≈ 29032

            // Clamp to [0, 255]
            uint8x8_t r_u8 = vqmovun_s16(r);
            uint8x8_t g_u8 = vqmovun_s16(g);
            uint8x8_t b_u8 = vqmovun_s16(b);

            // Interleave and store RGB
            uint8x8x3_t rgb;
            rgb.val[0] = r_u8;
            rgb.val[1] = g_u8;
            rgb.val[2] = b_u8;
            vst3_u8(pRGB + col * 3, rgb);
        }

        // Fallback for remaining pixels (non-NEON)
        for (; col < width; col++) {
            int Y = pY[col * yPixelStride];
            int U = pU[(col / 2) * uPixelStride] - 128;
            int V = pV[(col / 2) * vPixelStride] - 128;

            int R = Y + (1.402 * V);
            int G = Y - (0.344136 * U) - (0.714136 * V);
            int B = Y + (1.772 * U);

            R = R < 0 ? 0 : (R > 255 ? 255 : R);
            G = G < 0 ? 0 : (G > 255 ? 255 : G);
            B = B < 0 ? 0 : (B > 255 ? 255 : B);

            pRGB[col * 3 + 0] = (uint8_t)R;
            pRGB[col * 3 + 1] = (uint8_t)G;
            pRGB[col * 3 + 2] = (uint8_t)B;
        }
    }
}
#else
static inline uint8_t clamp(int value) {
    return (value < 0) ? 0 : ((value > 255) ? 255 : value);
}

void AndroidYUV420SPtoRGB(uint8_t* rgbImage, int width, int height,
                          const uint8_t* yPlane, const uint8_t* uPlane, const uint8_t* vPlane,
                          int yPixelStride, int yRowStride,
                          int uPixelStride, int uRowStride,
                          int vPixelStride, int vRowStride) {
    for (int row = 0; row < height; row++) {
        const uint8_t* pYRow = yPlane + row * yRowStride;
        const uint8_t* pURow = uPlane + (row / 2) * uRowStride;
        const uint8_t* pVRow = vPlane + (row / 2) * vRowStride;

        for (int col = 0; col < width; col++) {
            int yIndex = col * yPixelStride;
            int uvIndex = (col / 2) * uPixelStride;

            int Y = pYRow[yIndex];
            int U = pURow[uvIndex] - 128;
            int V = pVRow[uvIndex] - 128;

            int y1024 = Y << 10;

            int R = (y1024 + 1436 * V) >> 10;
            int G = (y1024 - 352 * U - 731 * V) >> 10;
            int B = (y1024 + 1814 * U) >> 10;

            *rgbImage++ = clamp(R);
            *rgbImage++ = clamp(G);
            *rgbImage++ = clamp(B);
        }
    }
}
#endif

} // namespace

// native methods called from Java
extern "C" {

JNIEXPORT void JNICALL Java_com_nolimitconnect_nolimitconnect_Camera2Service_camServiceStarted(JNIEnv *env, jobject obj) {
    g_CamEnv = env;
    g_CamServiceThreadId = VxGetCurrentThreadId();

    // Obtain a reference to the class of the passed object (obj)
    jclass clazz = env->GetObjectClass(obj);

    if (clazz == nullptr) {
        std::cerr << "Failed to find class" << std::endl;
        return;
    }

    // Hold a reference to the Java object
    // to prevent it from being garbage collected if needed
    g_CamObj = env->NewGlobalRef(obj);

    // Use the global reference (you could store it for later use)

    // Don't forget to delete the global reference when you're done
    //env->DeleteGlobalRef(g_CamObj);

    for( int i = 0; i < sizeof(g_CamMethods)/sizeof(CamServiceMethod); i++)
    {
        g_CamMethods[i].methodID = env->GetMethodID(clazz, g_CamMethods[i].methodName, g_CamMethods[i].methodSigniture);
        if (g_CamMethods[i].methodID == nullptr) {
            LogMsg( LOG_ERROR, "%s Failed to find method %s", __func__, g_CamMethods[i].methodName );
            vx_assert(false);
            return;
        }
    }

    // Test is valid by calling the 'getValue' method on the passed object and get the result
    jint value = env->CallIntMethod(obj, g_CamMethods[CAM_GET_VALUE_IDX].methodID);

    LogMsg( LOG_VERBOSE, "%s Received value: %d", __func__, value );

    g_CamServiceReady = true;
    GetCamJavaClient().onCamServiceStarted();
}

JNIEXPORT void JNICALL Java_com_nolimitconnect_nolimitconnect_Camera2Service_camServiceStopped(JNIEnv *env, jobject obj) {
    g_CamServiceReady = false;
    LogMsg( LOG_VERBOSE, "%s ", __func__ );
}

JNIEXPORT bool JNICALL Java_com_nolimitconnect_nolimitconnect_Camera2Service_canProcessCamCapture(JNIEnv *env, jobject obj) {
    return GetCamJavaClient().canProcessCamCapture();
}

bool GetJBufInfo( jobject byteBuffer, uint8_t*& byteBuf )
{
    // Get the ByteBuffer's class and its method for retrieving the address (the "address" field)
    jclass byteBufferClass = g_CamEnv->GetObjectClass(byteBuffer);
    // Get the "address" field of the ByteBuffer (this field points to the direct memory address)
    jfieldID addressFieldID = g_CamEnv->GetFieldID(byteBufferClass, "address", "J");
    if (addressFieldID == nullptr) {
        LogMsg( LOG_ERROR, "%s Failed to find 'address' field in ByteBuffer.", __func__ );
        return false;
    }
    // Get the direct buffer's native address (this should be a long type)
    jlong bufferAddress = g_CamEnv->GetLongField(byteBuffer, addressFieldID);
    if (bufferAddress == 0) {
        LogMsg( LOG_ERROR, "%s Invalid buffer address.", __func__ );
        return false;
    }

    // Now that we have the buffer's address and size, let's cast the address to a pointer and process it
    byteBuf = reinterpret_cast<uint8_t*>(bufferAddress);
    return true;
}

JNIEXPORT void JNICALL Java_com_nolimitconnect_nolimitconnect_Camera2Service_processCamCapture(JNIEnv *env, jobject obj,
                                                                                            int width, int height, jobject yBuf, jobject uBuf, jobject vBuf,
                                                                                            int yPixelStride, int yRowStride,
                                                                                            int uPixelStride, int uRowStride,
                                                                                            int vPixelStride, int vRowStride )
{
    if( width < 10 || height < 10 || width > 10000 || height > 10000)
    {
        LogMsg( LOG_ERROR, "%s invalid param width %d height %d", __func__, width, height );
    }

    uint8_t* y = 0;
    uint8_t* u = 0;
    uint8_t* v = 0;
    if(GetJBufInfo(yBuf, y) && GetJBufInfo(uBuf, u) && GetJBufInfo(vBuf, v))
    {
        int dataLen = width * height * 3;
        std::shared_ptr<uint8_t> rgbData( new uint8_t[dataLen] );

        AndroidYUV420SPtoRGB( rgbData.get(),
                              width, height, y, u, v,
                              yPixelStride, yRowStride,
                              uPixelStride, uRowStride,
                              vPixelStride, vRowStride );

        GetCamJavaClient().processCamCapture(width, height, rgbData, dataLen);
    }
    else
    {
        LogMsg( LOG_ERROR, "%s failed to get yuv buffers", __func__ );
    }
}

} // extern "C"

//============================================================================
CamJavaClient::CamJavaClient( AppCommon& myApp, CamLogic& camLogic, QObject *parent )
    : QObject(parent)
    , m_MyApp(myApp)
    , m_CamLogic( camLogic )
{
    g_CamClient = this;
}

//============================================================================
void CamJavaClient::startupCamLogic( void )
{
    LogMsg( LOG_VERBOSE, "%s GUI thread id %d", __func__, VxGetCurrentThreadId() );
    QJniObject appContext(QNativeInterface::QAndroidApplication::context());
    if(!appContext.isValid())
    {
        LogMsg( LOG_ERROR, "%s invalid app context", __func__ );
        return;
    }

    // start the service as foreground service
    QJniObject::callStaticMethod<void>(
        CAM_CAPTURE_CLASS_NAME, // class name
        "startCamServiceStatic", // method name
        "(Landroid/content/Context;)V", // signature
        appContext.object() );

    // wait for service to be started
}

//============================================================================
void CamJavaClient::shutdownCamLogic( void )
{
}

//============================================================================
void CamJavaClient::onCamServiceStarted( void )
{
    LogMsg( LOG_VERBOSE, "%s thread id %d", __func__, VxGetCurrentThreadId() );
    updateCameraList();
    m_CamLogic.onCamCaptureReady( true );
}

//============================================================================
bool CamJavaClient::canProcessCamCapture( void )
{
    static int64_t lastTimeMs = 0;
    int64_t timeNow = GetGmtTimeMs();
    if( timeNow < lastTimeMs + CamLogic::CAM_SNAPSHOT_INTERVAL_MS )
    {
        //LogMsg( LOG_VERBOSE, "%s time too short %d ms", __func__, (int)(timeNow - lastTimeMs) );
        return false;
    }
    
    bool result =  m_CamLogic.canProcessCamCapture();
    if( result )
    {
        lastTimeMs = timeNow;
    }
//    else
//    {
//        LogMsg( LOG_VERBOSE, "%s CamLogic returned false", __func__ );
//    }

    return result;
}

//============================================================================
void CamJavaClient::processCamCapture( int width, int height, std::shared_ptr<uint8_t>& rgbData, int dataLen )
{
    m_CamLogic.getCamProcessor().processCamCapture( width, height, rgbData, dataLen );
}

//============================================================================
void CamJavaClient::getCameraDevices( std::vector<std::pair<bool,std::string>>& camIdList )
{
    camIdList.clear();
    for(auto camId : m_CamIdList )
    {
        bool backFacing = isBackFacing( camId );
        camIdList.emplace_back( std::make_pair(backFacing, camId) );
    }
}

//============================================================================
bool CamJavaClient::isBackFacing( std::string& camId )
{
    if( !g_CamServiceReady )
    {
        LogMsg( LOG_ERROR, "%s !g_CamServiceReady ", __func__ );
        return false;
    }

    JNIEnv* jniEnv = GetJniEnv();
    jstring jCamId = jniEnv->NewStringUTF(camId.c_str());
    jboolean value = jniEnv->CallBooleanMethod( g_CamObj, g_CamMethods[CAM_IS_BACKFACING_IDX].methodID, jCamId );
    jniEnv->DeleteLocalRef(jCamId);

    return value ? true : false;
}

//============================================================================
void CamJavaClient::updateCameraList( void )
{
    m_CamIdList.clear();
    if( !g_CamServiceReady )
    {
        LogMsg( LOG_ERROR, "%s !g_CamServiceReady ", __func__ );
        return;
    }

    JNIEnv* jniEnv = GetJniEnv();
    jobjectArray stringArray  = (jobjectArray)jniEnv->CallObjectMethod( g_CamObj, g_CamMethods[CAM_GET_IDS_IDX].methodID );
    if (stringArray == nullptr) {
        LogMsg( LOG_ERROR, "%s string array in null", __func__ );
        return;
    }

    jsize length = jniEnv->GetArrayLength(stringArray);
    for (int i = 0; i < length; ++i) {
        jstring stringElement = (jstring)jniEnv->GetObjectArrayElement(stringArray, i);
        const char *charPtr = jniEnv->GetStringUTFChars(stringElement, nullptr);
        if (charPtr != nullptr) {
            m_CamIdList.emplace_back(charPtr);
            jniEnv->ReleaseStringUTFChars(stringElement, charPtr);
        }
        //  env->DeleteLocalRef(stringElement); //optional
    }

    LogMsg( LOG_VERBOSE, "%s %d cameras available", __func__, m_CamIdList.size() );
}

//============================================================================
bool CamJavaClient::startCamCapture( std::string camId )
{
    if( !g_CamServiceReady )
    {
        LogMsg( LOG_ERROR, "%s !g_CamServiceReady ", __func__ );
        return false;
    }

    if( camId.empty() )
    {
        LogMsg( LOG_ERROR, "%s camId.empty() ", __func__ );
        return false;
    }

    JNIEnv* jniEnv = GetJniEnv();
    jstring jCamId = jniEnv->NewStringUTF(camId.c_str());
    jboolean value = jniEnv->CallBooleanMethod( g_CamObj, g_CamMethods[CAM_START_CAPTURE_IDX].methodID, jCamId );
    jniEnv->DeleteLocalRef(jCamId);

    return value ? true : false;
}

//============================================================================
void CamJavaClient::stopCamCapture( void )
{
    if( !g_CamServiceReady )
    {
        LogMsg( LOG_ERROR, "%s !g_CamServiceReady ", __func__ );
        return;
    }

    JNIEnv* jniEnv = GetJniEnv();
    jniEnv->CallVoidMethod( g_CamObj, g_CamMethods[CAM_STOP_CAPTURE_IDX].methodID  );
}

#endif // defined(ENABLE_JAVA_CAM)
