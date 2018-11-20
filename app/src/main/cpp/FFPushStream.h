//
// Created by CC000033 on 2018/9/12.
//

#ifndef JINGZIAPP_FFPUSHSTREAM_H
#define JINGZIAPP_FFPUSHSTREAM_H


#include "IPushStream.h"

struct AVFormatContext;

class FFPushStream: public IPushStream {
public:

    bool Init(AVFormatContext *ctx, const char *fileName, unsigned char isPushStream) override;

    void Close() override;

    bool StartPushStream(XData data) override;

    bool StopPushStream() override;


protected:
    AVFormatContext *mOutContext;
    //判断是推流还是写文件
    bool isPushStream;
    std::mutex mutex;
    bool findIFrame = false;//是否找到I帧，默认false

};


#endif //JINGZIAPP_FFPUSHSTREAM_H
