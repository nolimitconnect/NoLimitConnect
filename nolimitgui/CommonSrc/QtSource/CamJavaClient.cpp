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
CamJavaClient& GetCamClient() {
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

std::vector<std::pair<unsigned int, JNIEnv*>> g_JavaEnvList;

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

//============================================================================
void AndroidYUV420SPtoRGB(  uint8_t* rgbImage, int width, int height,
                            const uint8_t* yPlane, const uint8_t* uPlane, const uint8_t* vPlane,
                            int yPixelStride, int yRowStride,
                            int uPixelStride, int uRowStride,
                            int vPixelStride, int vRowStride )
{
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int Y = yPlane[col * yPixelStride + row * yRowStride];
            int U = uPlane[col / 2 * uPixelStride + row / 2 * uRowStride];
            int V = vPlane[col / 2 * vPixelStride + row / 2 * vRowStride];

            int R = (Y + (V - 128));
            int G = (Y - (U - 128) - (V - 128));
            int B = (Y + (U - 128));

            rgbImage[0] = (uint8_t)(R & 0xff);
            rgbImage[1] = (uint8_t)(G & 0xff);
            rgbImage[2] = (uint8_t)(B & 0xff);
            rgbImage += 3;
        }
    }
}

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
    GetCamClient().onCamServiceStarted();
}

JNIEXPORT void JNICALL Java_com_nolimitconnect_nolimitconnect_Camera2Service_camServiceStopped(JNIEnv *env, jobject obj) {
    g_CamServiceReady = false;
    LogMsg( LOG_VERBOSE, "%s ", __func__ );
}

JNIEXPORT bool JNICALL Java_com_nolimitconnect_nolimitconnect_Camera2Service_canProcessCamCapture(JNIEnv *env, jobject obj) {
    return GetCamClient().canProcessCamCapture();
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

        GetCamClient().processCamCapture(width, height, rgbData, dataLen);
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
        //LogMsg( LOG_VERBOSE, "%s time ok %d ms", __func__, (int)(timeNow - lastTimeMs) );
        return false;
    }
    
    bool result =  m_CamLogic.canProcessCamCapture();
    if( result )
    {
        lastTimeMs = timeNow;
    }

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
