//
// Created by CC000033 on 2018/8/9.
//

#ifndef XPLAYER_IPLAYER_H
#define XPLAYER_IPLAYER_H


#include <mutex>
#include "XThread.h"
#include "XParameter.h"
#include "XListener.h"

class IDemux;

class IDecode;

class IAudioPlay;

class IVideoView;

class IResample;

class IPushStream;



typedef struct {
    XListener *xListener;
    int code;
    const char *msg;
} CallData;

class IPlayer : public XThread {
public:
    static IPlayer *Get(unsigned char index = 0);

    virtual bool Open(const char *path);

    virtual bool Open(int type);

    virtual void Close();

    virtual void InitView(void *win);

    bool Start() override;

    virtual void StartPushStream(const char *path, unsigned char i);

    virtual void StopPushStream();

    virtual void Destroy();

    virtual void SetShaderVertex(float widthRatioForScreen, float heightRationForScreen,float offset, float screenRatio);

    virtual void restartDemux();

    // 注册回调函数
    virtual void Register(void *xListener);

    virtual void SendMsgToJava(int code, const char *msg);

    virtual void setDemuxData(u_int8_t *data, int size, int pts);


    //是否视频硬解码
    bool isHardDecode = true;

    //音频输出参数配置
    XParameter outPara;

    IDemux *demux = 0;
    IDecode *vDecode = 0;
    IDecode *aDecode = 0;
    IResample *resample = 0;
    IAudioPlay *audioPlay = 0;
    IVideoView *videoView = 0;
    IPushStream *pushStream = 0;

    XListener *mListener = 0; //监听函数

protected:
    //用作音视频同步
    void Main() override;
    std::mutex mutex;

    IPlayer();

};


#endif //XPLAYER_IPLAYER_H
