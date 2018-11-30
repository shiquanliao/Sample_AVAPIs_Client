//
// Created by CC000033 on 2018/8/9.
//

#include "IPlayProxy.h"
#include "FFPlayerBuilder.h"

IPlayProxy *IPlayProxy::Get() {
    static IPlayProxy iPlayProxy;
    return &iPlayProxy;
}

void IPlayProxy::Init(void *vm) {
    mutex.lock();
    if (vm) {
        FFPlayerBuilder::Get()->InitHard(vm);
    }
    if (!player)
        player = FFPlayerBuilder::Get()->BuilderPlayer();
    mutex.unlock();
}

void IPlayProxy::restartDemux() {
    if (player) {
        player->restartDemux();
    }
}

bool IPlayProxy::Open(const char *path) {
    bool re = false;
    mutex.lock();
    if (player)
        re = player->Open(path);
    mutex.unlock();
    return re;
}


bool IPlayProxy::Open(XParameter para) {
    mutex.lock();
    bool state = false;
    if (player)
        state = player->Open(para);
    mutex.unlock();
    return state;
}

bool IPlayProxy::Start() {
    bool re = false;
    mutex.lock();
    if (player)
        re = player->Start();
    mutex.unlock();
    return re;
}

void IPlayProxy::InitView(void *win) {
    mutex.lock();
    if (player)
        player->InitView(win);
    mutex.unlock();
}

void IPlayProxy::Close() {
    mutex.lock();
    if (player)
        player->Close();
    mutex.unlock();
}

void IPlayProxy::StartPushStream(const char *path, unsigned char i) {
    mutex.lock();
    if (player)
        player->StartPushStream(path, i);
    mutex.unlock();
}

void IPlayProxy::StopPushStream() {
    mutex.lock();
    if (player)
        player->StopPushStream();
    mutex.unlock();
}

void IPlayProxy::Destroy() {
    mutex.lock();
    if (player)
        player->Destroy();
    mutex.unlock();
}

void
IPlayProxy::SetShaderVertex(float widthRatioForScreen, float heightRationForScreen, float offset,
                            float screenRatio) {
    mutex.lock();
    if (player)
        player->SetShaderVertex(widthRatioForScreen, heightRationForScreen, offset, screenRatio);
    mutex.unlock();
}

void IPlayProxy::Register(void *xListener) {
    mutex.lock();
    if (player)
        player->Register(xListener);
    mutex.unlock();
}

void IPlayProxy::SendMsgToJava(int code, const char *msg) {
    mutex.lock();
    if (player)
        player->SendMsgToJava(code, msg);
    mutex.unlock();
}



void IPlayProxy::setDemuxVideoData(u_int8_t *data, int size, unsigned int pts) {
    mutex.lock();
    if (player)
        player->setDemuxVideoData(data, size, pts);
    mutex.unlock();
}

void IPlayProxy::setDemuxAudioData(u_int8_t *data, int size, unsigned int pts) {
    mutex.lock();
    if (player)
        player->setDemuxAudioData(data, size, pts);
    mutex.unlock();
}

void IPlayProxy::StartAudioPlay() {
    mutex.lock();
    if (player)
        player->StartAudioPlay();
    mutex.unlock();
}






