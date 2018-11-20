//
// Created by CC000033 on 2018/8/3.
//

#ifndef XPLAYER_FFDECODE_H
#define XPLAYER_FFDECODE_H


#include "IDecode.h"

struct AVCodecContext;
struct AVFrame;
struct SwsContext;

class FFDecode : public IDecode {
public:
    static void InitHard(void *vm);

    bool Open(XParameter para, bool isHard = false) override;

    bool P2POpen(int i) override;

    void Close() override;

    //future模型 发送数据到线程解码
    bool SendPacket(XData pkt) override;

    //从线程中获取解码结果，再次调用会复用上次空间，线程不安全
    XData RecvFrame() override;

protected:

    void DataCopy(AVFrame *frame);

    AVCodecContext *codec = 0;
    AVFrame *frame = 0;
    AVFrame *pFrameYUV = 0;
    struct SwsContext *img_convert_ctx;
    //可以理解成缓冲数组
    uint8_t *out_buffer;
    std::mutex mutex;
};


#endif //XPLAYER_FFDECODE_H
