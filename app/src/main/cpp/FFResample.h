//
// Created by CC000033 on 2018/8/6.
//

#ifndef XPLAYER_FFRESAMPLE_H
#define XPLAYER_FFRESAMPLE_H


#include "IResample.h"
struct SwrContext;

class FFResample : public IResample {
public:
    bool Open(XParameter in, XParameter out) override;

    void Close() override;

    XData Resample(XData indata) override;

protected:
    SwrContext *actx = 0;
    std::mutex mutex;
};


#endif //XPLAYER_FFRESAMPLE_H
