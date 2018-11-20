//
// Created by CC000033 on 2018/8/9.
//

#include "IPlayerBuilder.h"
#include "IVideoView.h"
#include "IResample.h"
#include "IDecode.h"
#include "IAudioPlay.h"
#include "IDemux.h"

IPlayer *IPlayerBuilder::BuilderPlayer(unsigned char index) {
    IPlayer *play = CreatePlayer(index);

    //解封装
    IDemux *demux = CreateDemux();

    //视频解码
    IDecode *vDecode = CreateDecode();

    //音频解码
    IDecode *aDecode = CreateDecode();

    //解码器观察解封装
    demux->AddObs(vDecode);
    demux->AddObs(aDecode);

    //显示观察视频解码器
    IVideoView *view = CreateVideoView();
    vDecode->AddObs(view);

    //重采样观察音频解码器
    IResample *resample = CreateResample();
    aDecode->AddObs(resample);

    //音频播放观察重采样
    IAudioPlay *audioPlay = CreateAudioPlay();
    resample->AddObs(audioPlay);

    play->demux = demux;
    play->vDecode = vDecode;
    play->aDecode = aDecode;
    play->resample = resample;
    play->videoView = view;
    play->audioPlay = audioPlay;

    return play;
}
