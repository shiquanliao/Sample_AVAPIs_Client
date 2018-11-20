//
// Created by CC000033 on 2018/10/17.
//

#ifndef JINGZIAPP_IPUSHSTREAM_H
#define JINGZIAPP_IPUSHSTREAM_H


#define  DEFAULT_FRAME_NUM 4

#include <list>
#include "IObserver.h"

struct AVFormatContext;

class IPushStream : public IObserver {
public:
    // 初始化推流
    virtual bool
    Init(AVFormatContext *ctx, const char *fileName, unsigned char isPushStream) = 0;

    virtual void Close() = 0;

    virtual bool StartPushStream(XData data) = 0;
    virtual bool StopPushStream() = 0;

    virtual void Clear();

    //由主体notify的数据 阻塞
    virtual void Update(XData data);

    //最大的队列缓冲
    int maxList = 100;

private:
public:
    virtual void Main() override;

    //读取缓冲
    std::list<XData> frameList;
    std::mutex frameMutex;

};


#endif //JINGZIAPP_IPUSHSTREAM_H
