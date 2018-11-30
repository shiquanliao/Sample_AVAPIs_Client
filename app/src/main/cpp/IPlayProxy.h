//
// Created by CC000033 on 2018/8/9.
//

#ifndef XPLAYER_IPLAYPROXY_H
#define XPLAYER_IPLAYPROXY_H


#include <mutex>
#include "IPlayer.h"

class IPlayProxy : public IPlayer {

public:
    static IPlayProxy *Get();

    void restartDemux() override;

    void Init(void *vm = 0);

    virtual void Close();

    bool Open(const char *path) override;

    bool Open(XParameter para) override;

    void InitView(void *win) override;

    bool Start() override;

    void StartPushStream(const char *path, unsigned char i) override;

    void StopPushStream() override;

    void SetShaderVertex(float widthRatioForScreen, float heightRationForScreen,  float offset,float screenRatio) override;

    void Destroy() override;

    void Register(void *xListener) override;


    void SendMsgToJava(int code, const char *msg) override;

    void setDemuxVideoData(u_int8_t *data, int size, unsigned int pts) override;


    void setDemuxAudioData(u_int8_t *data, int size, unsigned int pts) override;

    void StartAudioPlay() override;

protected:
    IPlayProxy() {};
    IPlayer *player = 0;
    std::mutex mutex;
};


#endif //XPLAYER_IPLAYPROXY_H
