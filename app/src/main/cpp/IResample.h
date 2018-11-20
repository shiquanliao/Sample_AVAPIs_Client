//
// Created by CC000033 on 2018/8/6.
//

#ifndef XPLAYER_IRESAMPLE_H
#define XPLAYER_IRESAMPLE_H


#include "IObserver.h"
#include "XParameter.h"

class IResample : public IObserver {
public:
    virtual bool Open(XParameter in, XParameter out = XParameter()) = 0;

    virtual void Close() = 0;

    virtual XData Resample(XData indata) = 0;

    virtual void Update(XData data);

    int outChannels = 2;
    int outFormat = 1;
};


#endif //XPLAYER_IRESAMPLE_H
