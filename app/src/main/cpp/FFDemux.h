//
// Created by CC000033 on 2018/8/2.
//

#ifndef XPLAYER_FFDEMUX_H
#define XPLAYER_FFDEMUX_H


#include <string>
#include <list>
#include "XData.h"
#include "IDemux.h"
#include "FFPushStream.h"
#include "XListener.h"
#include <future>
#include <jni.h>

#define RESTART_TIME 2

struct AVFormatContext;
struct AVOutputFormat;
// 回调函数的参数，用了时间
typedef struct {
    time_t lasttime;
} Runner;


static int time_out = 0;

class FFDemux : public IDemux {
public:

    FFDemux();

    //打开文件，或者流媒体 rmtp http rtsp
    virtual bool Open(const char *url) override;

    void Close() override;

    //获取视频参数
    virtual XParameter GetVPara() override;

    //获取视频参数
    virtual bool P2POpen() override;

    //获取音频参数
    XParameter GetAPara() override;

    //获取上下文
    AVFormatContext *GetAVFormatContext() override;

    //读取一帧数据，数据由调用者清理
    virtual XData Read() override;

    //AVReadFrame超时回调函数
    static int AVInterruptCallBackFun(void *ctx);

    static void reStart();

    void SetP2PData(u_int8_t *data, int size, int pts) override;


private:
    //输入上下文
    AVFormatContext *ic = 0; // 只有c++11以上,并且只有无参构造函数时才生效
    //输出上下文
    std::mutex mutex;
    Runner input_runner;
    long unmber = 0;
    static FFDemux *m_gFFDemux;



};


#endif //XPLAYER_FFDEMUX_H
