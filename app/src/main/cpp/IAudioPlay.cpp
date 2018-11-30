//
// Created by CC000033 on 2018/8/6.
//

#include "IAudioPlay.h"
#include "XLog.h"

XData IAudioPlay::GetData() {
    XData d;

    isRuning = true;
    while (!isExit) {
        framesMutex.lock();
//        XLOGE("出栈 ---- 当前栈的大小是：%d",frames.size());
        if (isFirst) {
            if (frames.size() > 5) {
                isFirst = false;
            }

        } else {
            if (!frames.empty()) {
                //有数据返回
                d = frames.front();
                frames.pop_front();
                framesMutex.unlock();
                pts = d.pts;
                return d;
            }
        }

        framesMutex.unlock();
        XSleep(1);
    }

    isRuning = false;
    //未获取数据
    return d;
}

void IAudioPlay::Update(XData data) {
    //XLOGE("IAudioPlay::Update %d", data.size);
    //压入缓冲队列
    if (data.size <= 0 || !data.data) return;
//    XLOGE("压栈1 ---- 当前栈的大小是：%d",frames.size());
    while (!isExit) {
        framesMutex.lock();
//        XLOGE("压栈2 ---- 当前栈的大小是：%d  数据包大小 %d",frames.size(),data.size);
        if (frames.size() > maxFrame) {
            framesMutex.unlock();
            XSleep(1);
            continue;
        }
        frames.push_back(data);
        framesMutex.unlock();
        break;
    }
}

void IAudioPlay::Clear() {
    framesMutex.lock();
    while (!frames.empty()) {
        frames.front().Drop();
        frames.pop_front();
    }
    framesMutex.unlock();

}
