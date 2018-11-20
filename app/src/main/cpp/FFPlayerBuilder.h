//
// Created by CC000033 on 2018/8/9.
//

#ifndef XPLAYER_FFPLAYERBUILDER_H
#define XPLAYER_FFPLAYERBUILDER_H


#include "IPlayerBuilder.h"

class FFPlayerBuilder : public IPlayerBuilder {
public:
    static void InitHard(void *vm);

    static FFPlayerBuilder *Get();

    IDemux *CreateDemux() override;

    IPushStream *CreatePushStream() override;

protected:
    FFPlayerBuilder() {};

    IDecode *CreateDecode() override;

    IResample *CreateResample() override;

    IVideoView *CreateVideoView() override;

    IAudioPlay *CreateAudioPlay() override;

    IPlayer *CreatePlayer(unsigned char index) override;



};


#endif //XPLAYER_FFPLAYERBUILDER_H
