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
    status = IPlayProxy::Get()->Open(url);
    if (status) { // 打开视频流正常才会使用
        IPlayProxy::Get()->Start();
    }

    env->ReleaseStringUTFChars(url_, url);
    return static_cast<jboolean>(status);
}


extern "C"
JNIEXPORT jboolean JNICALL
Java_com_common_view_XPlay_OpenP2P(JNIEnv *env, jobject instance, jstring uid_, jstring name_,
                                   jstring password_) {
    const char *uid = env->GetStringUTFChars(uid_, 0);
    const char *name = env->GetStringUTFChars(name_, 0);
    const char *password = env->GetStringUTFChars(password_, 0);

    bool status;
    status = IPlayProxy::Get()->Open(0);
    if (status) { // 打开视频流正常才会使用
        IPlayProxy::Get()->Start();
    }

    env->ReleaseStringUTFChars(uid_, uid);
    env->ReleaseStringUTFChars(name_, name);
    env->ReleaseStringUTFChars(password_, password);
    return static_cast<jboolean>(status);
}


// 设置P2P的视频数据
extern "C"
JNIEXPORT void JNICALL
Java_com_common_jniFun_PlayControl_setFrameVideoData(JNIEnv *env, jclass type,
                                                     jbyteArray videoData_,
                                                     jint size, jbyteArray frameInfo_) {
    jbyte *frameData = env->GetByteArrayElements(videoData_, NULL);
    jbyte *frameInfo = env->GetByteArrayElements(frameInfo_, NULL);

    FRAMEINFO_t *info = reinterpret_cast<FRAMEINFO_t *>(frameInfo);
//    XLOGE("frame is: %ud", info->timestamp);

    IPlayProxy::Get()->setDemuxVideoData(reinterpret_cast<u_int8_t *>(frameData), size,
                                         info->timestamp);

    env->ReleaseByteArrayElements(videoData_, frameData, 0);
    env->ReleaseByteArrayElements(frameInfo_, frameInfo, 0);
}


// 设置P2P的音频数据
bool first = true;
int number = 0;

extern "C"
JNIEXPORT void JNICALL
Java_com_common_jniFun_PlayControl_setFrameAudioData(JNIEnv *env, jclass type,
                                                     jbyteArray audioData_, jint ret,
                                                     jbyteArray frameInfo_) {
    jbyte *audioBuffer = env->GetByteArrayElements(audioData_, NULL);
    jbyte *frameInfo = env->GetByteArrayElements(frameInfo_, NULL);

    FRAMEINFO_t *info = reinterpret_cast<FRAMEINFO_t *>(frameInfo);


    IPlayProxy::Get()->setDemuxAudioData(reinterpret_cast<u_int8_t *>(audioBuffer), ret,
                                         info->timestamp);

    if (first && number > 10) {
        first = false;
        IPlayProxy::Get()->StartAudioPlay();
    } else {
        number++;
    }


    env->ReleaseByteArrayElements(audioData_, audioBuffer, 0);
    env->ReleaseByteArrayElements(frameInfo_, frameInfo, 0);
}