//
// Created by CC000033 on 2018/8/9.
//

#ifndef XPLAYER_IPLAYERBUILDER_H
#define XPLAYER_IPLAYERBUILDER_H

#include "IPlayer.h"

class IPlayerBuilder {
public:
    virtual IPlayer *BuilderPlayer(unsigned char index = 0);

protected:
    virtual IDemux *CreateDemux() = 0;

    virtual IDecode *CreateDecode() = 0;

    virtual IResample *CreateResample() = 0;

    virtual IVideoView *CreateVideoView() = 0;

    virtual IAudioPlay *CreateAudioPlay() = 0;

    virtual IPushStream *CreatePushStream() = 0;

    virtual IPlayer *CreatePlayer(unsigned char index = 0) = 0;

};


#endif //XPLAYER_IPLAYERBUILDER_H
