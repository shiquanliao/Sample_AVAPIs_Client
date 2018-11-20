#include <jni.h>
#include "FFDemux.h"
#include "IPlayProxy.h"
#include "XListener.h"
#include "P2PCam/AVFRAMEINFO.h"
#include "XLog.h"
#include <android/native_window_jni.h>

JavaVM *jvm;

extern "C"
JNIEXPORT
jint JNI_OnLoad(JavaVM *vm, void *res) {
    JNIEnv *env;
    jvm = vm;
    IPlayProxy::Get()->Init(vm);
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;

}


extern "C"
JNIEXPORT void JNICALL
Java_com_common_jniFun_PlayControl_startPushStream(JNIEnv *env, jclass instance,
                                                   jstring url_, jboolean isPushToRTMP) {
    const char *url = env->GetStringUTFChars(url_, 0);

    IPlayProxy::Get()->StartPushStream(url, isPushToRTMP);

    env->ReleaseStringUTFChars(url_, url);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_common_jniFun_PlayControl_stopPushStream(JNIEnv *env, jclass instance) {

    IPlayProxy::Get()->StopPushStream();

}

extern "C"
JNIEXPORT void JNICALL
Java_com_common_jniFun_PlayControl_destroy(JNIEnv *env, jclass instance) {

    // TODO
    IPlayProxy::Get()->Destroy();

}


extern "C"
JNIEXPORT jboolean JNICALL
Java_com_cvte_afsyb_jingzi_mvp_ui_activity_XPlay_Open(JNIEnv *env, jobject instance, jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);
    bool status;
    status = IPlayProxy::Get()->Open(url);
    if (status) { // 打开视频流正常才会使用
        IPlayProxy::Get()->Start();
    }

    env->ReleaseStringUTFChars(url_, url);
    return static_cast<jboolean>(status);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_common_jniFun_PlayControl_setVertexSize(JNIEnv *env, jclass type,
                                                 jfloat widthRatioForScreen,
                                                 jfloat heightRationForScreen,
                                                 jfloat offset, jfloat screenRatio) {

    IPlayProxy::Get()->SetShaderVertex(widthRatioForScreen, heightRationForScreen, offset,
                                       screenRatio);

}


//---------------------------线程回调---------------------------

extern "C"
JNIEXPORT void JNICALL
Java_com_common_jniFun_PlayControl_normalThread(JNIEnv *env, jobject instance) {

    // TODO

}

extern "C"
JNIEXPORT void JNICALL
Java_com_common_jniFun_PlayControl_mutexThread(JNIEnv *env, jobject instance) {

    // TODO

}


extern "C"
JNIEXPORT void JNICALL
Java_com_common_jniFun_PlayControl_callbackThread(JNIEnv *env, jobject instance) {

    XListener *xListener = new XListener(jvm, env, env->NewGlobalRef(instance));
    //在主线程中调用java方法
    xListener->onError(0, 200, "JNIENV thread running success!");
    IPlayProxy::Get()->Register(xListener);

}
///-------------------------------

extern "C"
JNIEXPORT jstring JNICALL
Java_com_cvte_afsyb_jingzi_jniFun_PlayControl_nativeToJavaMessage(JNIEnv *env, jclass type) {

    // TODO
    const char *cString = "c++ say hello";

    return env->NewStringUTF(cString);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_common_view_XPlay_InitView(JNIEnv *env, jobject instance,
                                    jobject surface) {

    ANativeWindow *win = ANativeWindow_fromSurface(env, surface);
    IPlayProxy::Get()->InitView(win);

}


extern "C"
JNIEXPORT jboolean JNICALL
Java_com_common_view_XPlay_Open(JNIEnv *env, jobject instance, jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);
    bool status;
//    status = IPlayProxy::Get()->Open(url);
    status = IPlayProxy::Get()->Open(0);
    if (status) { // 打开视频流正常才会使用
        IPlayProxy::Get()->Start();
    }

    env->ReleaseStringUTFChars(url_, url);
    return static_cast<jboolean>(status);
}


// 设置P2P的数据
extern "C"
JNIEXPORT void JNICALL
Java_com_common_jniFun_PlayControl_setFrameData(JNIEnv *env, jclass type, jbyteArray frameData_,
                                                jint size, jbyteArray frameInfo_) {
    jbyte *frameData = env->GetByteArrayElements(frameData_, NULL);
    jbyte *frameInfo = env->GetByteArrayElements(frameInfo_, NULL);
    FRAMEINFO_t *info = reinterpret_cast<FRAMEINFO_t *>(frameInfo);
    XLOGE("frame is ",info->timestamp);

    IPlayProxy::Get()->setDemuxData(reinterpret_cast<u_int8_t *>(frameData), size, info->timestamp);

    env->ReleaseByteArrayElements(frameData_, frameData, 0);
}

