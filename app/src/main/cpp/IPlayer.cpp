//
// Created by CC000033 on 2018/8/9.
//

#include "IPlayer.h"
#include "IDemux.h"
#include "IDecode.h"
#include "IVideoView.h"
#include "IAudioPlay.h"
#include "IResample.h"
#include "XLog.h"
#include "FFPlayerBuilder.h"
#include "IPushStream.h"

IPlayer *IPlayer::Get(unsigned char index) {
    static IPlayer p[256];
    return &p[index];
}

void IPlayer::Close() {
    mutex.lock();
    //1 先关闭主体线程，再清理观察者
    //同步线程
    XThread::Stop();
    //解封装
    if (demux)
        demux->Stop();
    //解码
    if (vDecode)
        vDecode->Stop();
    if (aDecode)
        aDecode->Stop();
    if (pushStream)
        pushStream->Stop();
    if (audioPlay)
        audioPlay->Stop();

    //2 清理缓冲队列
    if (vDecode)
        vDecode->Clear();
    if (aDecode)
        aDecode->Clear();
    if (pushStream)
        pushStream->Clear();
    if (audioPlay)
        audioPlay->Clear();


    //3 清理资源
    if (audioPlay)
        audioPlay->Close();
    if (videoView)
        videoView->Close();
    if (vDecode)
        vDecode->Close();
    if (aDecode)
        aDecode->Close();
    if (pushStream)
        pushStream->Close();
    if (demux)
        demux->Close();
    mutex.unlock();

}

bool IPlayer::Open(const char *path) {
    Close();
    mutex.lock();
    //解封装
    if (!demux || !demux->Open(path)) {
        mutex.unlock();
        XLOGE("demux->open %s failed!", path);
        return false;
    }

    //解码 解码可能不需要，如果是解封之后就是原始数据
    if (!vDecode || !vDecode->Open(demux->GetVPara(), isHardDecode)) {
        XLOGE("vDecode->open %s failed!", path);
        //return false;
    }
    if (!aDecode || !aDecode->Open(demux->GetAPara())) {
        aDecode->isHaveAudio = false;
        XLOGE("adecode->open %s failed!", path);
        //return false;
    }
    //重采样 有可能不需要，解码后或者解封后可能是直接能播放的数据
    //if (outPara.sample_rate <= 0)
    if (aDecode->isHaveAudio) { // 有音频
        outPara = demux->GetAPara();
        outPara.channels = 2; //设备不支持单通道
        if (!resample || !resample->Open(demux->GetAPara(), outPara)) {
            XLOGE("resample->open %s failed!", path);
        }
    }

    mutex.unlock();
    return true;
}

bool IPlayer::Open(int type) {
    Close();
    mutex.lock();
    if (!demux || !demux->P2POpen()) {
        XLOGE("P2P demux->open %s failed!");
    }
    //解码 解码可能不需要，如果是解封之后就是原始数据
    if (!vDecode || !vDecode->P2POpen(type)) {
        mutex.unlock();
        XLOGE("P2P vDecode->open %s failed!");
        return false;
    }
    if (!aDecode || !aDecode->P2POpen(1)) {
        aDecode->isHaveAudio = false;
        mutex.unlock();
        XLOGE("P2P adecode->open %s failed!");
        return false;
    }
    //重采样 有可能不需要，解码后或者解封后可能是直接能播放的数据
    if (aDecode->isHaveAudio) { // 有音频
        outPara.channels = 2; //设备不支持单通道
        if (!resample || !resample->Open(XParameter(), outPara)) {
            XLOGE("resample->open failed!");
        }
    }
    mutex.unlock();
    return true;
}

void IPlayer::StartAudioPlay() {

    mutex.lock();
    if (audioPlay && demux && demux->isP2P)
        audioPlay->StartPlay(outPara);
    mutex.unlock();
}

bool IPlayer::Start() {

    mutex.lock();

    if (audioPlay && demux && !demux->isP2P)
        audioPlay->StartPlay(outPara);

    if (vDecode)
        vDecode->Start();

    if (demux && !demux->isP2P)
        demux->Start();

    if (aDecode)
        aDecode->Start();



    XThread::Start();
    mutex.unlock();
    return true;
}


void IPlayer::InitView(void *win) {
    mutex.lock();
    if (videoView) {
        videoView->Close();
        videoView->SetRender(win);
    }
    mutex.unlock();
}

void IPlayer::Main() {
    while (!isExit) {
        mutex.lock();
        if (!audioPlay || !vDecode) {
            mutex.unlock();
            XSleep(2);
            continue;
        }

        //同步
        //获取音频的pts 告诉视频
        int apts = audioPlay->pts;
//        XLOGE("apts = %d", apts);
        vDecode->synPts = apts;

        mutex.unlock();
        XSleep(200);
    }
}

IPlayer::IPlayer() {

}

void IPlayer::StartPushStream(const char *path, unsigned char i) {
    mutex.lock();
    if (demux) {
        pushStream = FFPlayerBuilder::Get()->CreatePushStream();
        bool status = 0;
        status = pushStream->Init(demux->GetAVFormatContext(), path, i);
        if (status) { //初始化成功
            pushStream->Start(); //先启动线程
            demux->AddObs(pushStream); //加入到观察者队列
            demux->isStopPush = false;
        }
    }
    mutex.unlock();
}

void IPlayer::StopPushStream() {
    mutex.lock();
    if (demux)
        demux->isStopPush = true;
    if (pushStream)
        pushStream->StopPushStream();
    mutex.unlock();
}

// destory Thread
void IPlayer::Destroy() {
    Close();
}

void IPlayer::SetShaderVertex(float widthRatioForScreen, float heightRationForScreen, float offset,
                              float screenRatio) {
    mutex.lock();
    videoView->SetShaderVertex(widthRatioForScreen, heightRationForScreen, offset, screenRatio);
    mutex.unlock();
}

void IPlayer::restartDemux() { // 重新打开demux
    mutex.lock();
    if (demux)
        demux->Stop();
    if (demux)
        demux->Close();
    if (pushStream)
        pushStream->Stop();
    if (pushStream)
        pushStream->Clear();
    if (pushStream)
        pushStream->Close();
    demux = FFPlayerBuilder::Get()->CreateDemux();
    demux->Start();

    //重新添加观察者
    if (vDecode)
        demux->AddObs(vDecode);
    if (aDecode)
        demux->AddObs(aDecode);
    if (pushStream) {
        demux->isStopPush = false;
        demux->AddObs(pushStream);
    }
    mutex.unlock();
}

void IPlayer::Register(void *xListener) {
    // 注册监听
    mutex.lock();
    mListener = (XListener *) xListener;
    mutex.unlock();
}


pthread_t callbackThread;
int msg_code = 0;

const char *msg_center = 0;


void *callBackT(void *data) {
    //获取WlListener指针
//    CallData *callData = (CallData *) data;
//    XListener *xListener = callData->xListener;
//    int code = callData->code;
//    const char *msg = callData->msg;
//
//    //在子线程中调用回调方法.
//    if (xListener)
//        xListener->onError(1, code, msg);

    //--------------test-------------------
    XListener *xListener = (XListener *) data;
    xListener->onError(1, msg_code, msg_center);
    pthread_exit(&callbackThread);
}

void IPlayer::SendMsgToJava(int code, const char *msg) {
    mutex.lock();
    //开启子线程，并把WlListener指针传递到子线程中
    if (!msg)
        return;
    if (!mListener)
        return;
    msg_code = code;
    msg_center = msg;
    pthread_create(&callbackThread, NULL, callBackT, mListener);
    mutex.unlock();
}

void IPlayer::setDemuxVideoData(u_int8_t *data, int size, unsigned int pts) {
    demux->SetP2PVideoData(data, size, pts);
}

void IPlayer::setDemuxAudioData(u_int8_t *data, int size, unsigned int pts) {
    demux->SetP2PAudioData(data, size, pts);
}





