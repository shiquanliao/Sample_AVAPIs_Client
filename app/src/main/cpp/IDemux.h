//
// Created by CC000033 on 2018/8/2.
//

#ifndef XPLAYER_IDEMUX_H
#define XPLAYER_IDEMUX_H


#include "XData.h"
#include "IObserver.h"
#include "XParameter.h"

struct AVFormatContext;

class IDemux : public IObserver {
public:
    //打开文件，或者流媒体 rmtp http rtsp
    virtual bool Open(const char *url) = 0;

    virtual AVFormatContext *GetAVFormatContext() = 0;

    virtual void Close() = 0;

    //获取视频参数
    virtual XParameter GetVPara() = 0;
    virtual bool P2POpen() = 0;

    //获取音频参数
    virtual XParameter GetAPara() = 0;

    //读取一帧数据，数据由调用者清理
    virtual XData Read() = 0;

    //读取P2P一帧视频数据
    virtual void SetP2PVideoData(u_int8_t *data, int size, int pts) = 0;

    //读取P2P一帧音频数据
    virtual void SetP2PAudioData(u_int8_t *data, int size, int pts) = 0;


    //总时长（毫秒）
    int totalMs = 0;

    //判断当前是否需要通知推流对象
    bool isStopPush = true;

    static bool isP2P;

protected:
    virtual void Main() override;

    virtual void Update(XData data) override;

    static bool isReStarting;


//    XData pushStreamData = {0};
};


#endif //XPLAYER_IDEMUX_H
