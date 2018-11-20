//
// Created by CC000033 on 2018/8/9.
//

#include "FFPlayerBuilder.h"
#include "FFDemux.h"
#include "FFDecode.h"
#include "FFResample.h"
#include "GLVideoView.h"
#include "SLAudioPlay.h"

FFPlayerBuilder *FFPlayerBuilder::Get() {
    static FFPlayerBuilder ff; //必须是静态变量
    return &ff;
}

IDemux *FFPlayerBuilder::CreateDemux() {
    IDemux *ff = new FFDemux();
    ff->name = "Demux";
    return ff;
}

IDecode *FFPlayerBuilder::CreateDecode() {
    IDecode *ff = new FFDecode();
    ff->name = "IDecode";
    return ff;
}

IResample *FFPlayerBuilder::CreateResample() {
    IResample *ff = new FFResample();
    ff->name = "IResample";
    return ff;
}

IVideoView *FFPlayerBuilder::CreateVideoView() {
    IVideoView *ff = new GLVideoView();
    ff->name = "IVideoView";
    return ff;
}

IAudioPlay *FFPlayerBuilder::CreateAudioPlay() {
    IAudioPlay *ff = new SLAudioPlay();
    ff->name = "IAudioPlay";
    return ff;
}

IPushStream *FFPlayerBuilder::CreatePushStream() {
    IPushStream *ff = new FFPushStream();
    ff->name = "IPushStream";
    return ff;
}

IPlayer *FFPlayerBuilder::CreatePlayer(unsigned char index) {
    return IPlayer::Get(index);
}

void FFPlayerBuilder::InitHard(void *vm) {
    FFDecode::InitHard(vm);
}

