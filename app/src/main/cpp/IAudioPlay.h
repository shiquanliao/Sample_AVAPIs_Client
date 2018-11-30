//
// Created by CC000033 on 2018/8/6.
//

#ifndef XPLAYER_IAUDIOPLAY_H
#define XPLAYER_IAUDIOPLAY_H


#include <list>
#include "IObserver.h"
#include "XParameter.h"

class IAudioPlay : public IObserver {
public:
    //缓冲满后阻塞
    virtual void Update(XData data);

    //获取缓冲数据，如没有则阻塞
    virtual XData GetData();

    virtual bool StartPlay(XParameter out) = 0;

    virtual void Close() = 0;

    virtual void Clear();

    //最大缓冲
    int maxFrame = 500;
    int pts = 0;
    bool isFirst = true;
protected:
    std::list<XData> frames;
    std::mutex framesMutex;
};


#endif //XPLAYER_IAUDIOPLAY_H
