//
// Created by CC000033 on 2018/8/6.
//

#include "SLAudioPlay.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "XLog.h"


extern "C"
{
#include <libavutil/error.h>
}

/**
 * SL ES采用的是面向对象,和接口的设计方式
 * OpenSL ES 的 API 大都数是通过 “对象” 来调用的
 * 类似于我们调用 “JNI* env” 的函数也是这个样子去调用的
 * env->NewGlobalRef(instance)
 * OpenSL ES 有两个必须理解的概念，就是 Object 和 Interface，
 * Object 可以想象成 Java 的 Object 类，Interface 可以想象成 Java 的 Interface，
 * 但它们并不完全相同，下面进一步解释他们的关系：
 * （1） 每个 Object 可能会存在一个或者多个 Interface，官方为每一种 Object 都定义了一系列的 Interface
 * （2）每个 Object 对象都提供了一些最基础的操作，比如：Realize，Resume，GetState，Destroy 等等，如果
 *      希望使用该对象支持的功能函数，则必须通过其 GetInterface 函数拿到 Interface 接口， 然后通过 Interface 来访问功能函数
 * （3）并不是每个系统上都实现了 OpenSL ES 为 Object 定义的所有 Interface，所以在获取 Interface 的时候需要做一些选择和判断
 *
 */

static SLObjectItf engineSL = NULL;
static SLEngineItf eng = NULL;
static SLObjectItf mix = NULL;
static SLObjectItf player = NULL;
static SLPlayItf iplayer = NULL;
static SLAndroidSimpleBufferQueueItf pcmQue = NULL;


void SLAudioPlay::Close() {
    mutex.lock();
    // 停止播放
    if (iplayer && (*iplayer)) {
        (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_STOPPED);
    }
    //清理播放队列
    if (pcmQue && (*pcmQue)) {
        (*pcmQue)->Clear(pcmQue);
    }
    //销毁player对象
    if (player && (*player)) {
        (*player)->Destroy(player);
    }

    //销毁混音器
    if (mix && (*mix)) {
        (*mix)->Destroy(mix);
    }

    //销毁播放引擎
    if (engineSL && (*engineSL)) {
        (*engineSL)->Destroy(engineSL);
    }

    engineSL = NULL;
    eng = NULL;
    mix = NULL;
    player = NULL;
    iplayer = NULL;
    pcmQue = NULL;

    mutex.unlock();

}

SLAudioPlay::SLAudioPlay() {
    buf = new unsigned char[1024 * 1024];
}


SLAudioPlay::~SLAudioPlay() {
    delete buf;
    buf = 0;
}

static SLEngineItf CreateSL() {

    SLresult re;
    SLEngineItf en;
    re = slCreateEngine(&engineSL, 0, 0, 0, 0, 0);
    if (re != SL_RESULT_SUCCESS) return NULL;
    re = (*engineSL)->Realize(engineSL, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) return NULL;
    re = (*engineSL)->GetInterface(engineSL, SL_IID_ENGINE, &en);
    if (re != SL_RESULT_SUCCESS) return NULL;
    return en;
}

void SLAudioPlay::PlayCall(void *bufq) {
    if (!bufq)return;
    SLAndroidSimpleBufferQueueItf bf = (SLAndroidSimpleBufferQueueItf) bufq;
    //XLOGE("SLAudioPlay::PlayCall");
    //阻塞
    XData d = GetData();
    if (d.size <= 0) {
        XLOGE("GetData() size is 0");
        return;
    }
    if (!buf)
        return;
    memcpy(buf, d.data, d.size);
    mutex.lock();
    if (pcmQue && (*pcmQue))
        (*pcmQue)->Enqueue(pcmQue, buf, d.size);
    mutex.unlock();
    d.Drop();

}

static void PcmCall(SLAndroidSimpleBufferQueueItf bf, void *contex) {
    SLAudioPlay *ap = (SLAudioPlay *) contex;
    if (!ap) {
        XLOGE("PcmCall failed contex is null!");
        return;
    }
    ap->PlayCall((void *) bf);
}

bool SLAudioPlay::StartPlay(XParameter out) {
    Close();

    mutex.lock();
    isExit = false;
    //1 创建引擎
    eng = CreateSL();
    if (eng) {
        XLOGI("CreateSL success！ ");
    } else {
        mutex.unlock();
        XLOGE("CreateSL failed！ ");
        return false;
    }

    //2 创建混音器

    SLresult re = 0;
    re = (*eng)->CreateOutputMix(eng, &mix, 0, 0, 0);
    if (re != SL_RESULT_SUCCESS) {
        mutex.unlock();
        XLOGE("SL_RESULT_SUCCESS failed!");
        return false;
    }
    re = (*mix)->Realize(mix, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) {
        mutex.unlock();
        XLOGE("(*mix)->Realize failed!");
        return false;
    }
    SLDataLocator_OutputMix outmix = {SL_DATALOCATOR_OUTPUTMIX, mix};
    SLDataSink audioSink = {&outmix, 0};

    //3 配置音频信息
    //缓冲队列
    SLDataLocator_AndroidSimpleBufferQueue que = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 10};
    //音频格式
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            (SLuint32) out.channels,//    声道数
            (SLuint32) out.sample_rate * 1000,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN //字节序，小端
    };
    SLDataSource ds = {&que, &pcm};


    //4 创建播放器
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    re = (*eng)->CreateAudioPlayer(eng, &player, &ds, &audioSink,
                                   sizeof(ids) / sizeof(SLInterfaceID), ids, req);
    if (re != SL_RESULT_SUCCESS) {
        mutex.unlock();
        char buff[1024] = {0};
        av_strerror(re, buff, sizeof(buff));
        XLOGE("CreateAudioPlayer failed because: %s", buff);
        return false;
    } else {
        XLOGI("CreateAudioPlayer success!");
    }
    (*player)->Realize(player, SL_BOOLEAN_FALSE);
    //获取player接口
    re = (*player)->GetInterface(player, SL_IID_PLAY, &iplayer);
    if (re != SL_RESULT_SUCCESS) {
        mutex.unlock();
        XLOGE("GetInterface SL_IID_PLAY failed!");
        return false;
    }
    re = (*player)->GetInterface(player, SL_IID_BUFFERQUEUE, &pcmQue);
    if (re != SL_RESULT_SUCCESS) {
        mutex.unlock();
        XLOGE("GetInterface SL_IID_BUFFERQUEUE failed!");
        return false;
    }

    //设置回调函数，播放队列空调用
    // pcmQue -- 缓存队列, PcmCall -- 回调
    (*pcmQue)->RegisterCallback(pcmQue, PcmCall, this);

    //设置为播放状态
    (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_PLAYING);

    //启动队列回调
    (*pcmQue)->Enqueue(pcmQue, "", 1);
    mutex.unlock();
    XLOGI("SLAudioPlay::StartPlay success!");
    return true;

}
