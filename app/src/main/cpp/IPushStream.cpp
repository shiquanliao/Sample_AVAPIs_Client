//
// Created by CC000033 on 2018/10/17.
//

#include "IPushStream.h"
#include "XLog.h"


void IPushStream::Update(XData data) {
    if (data.type != AVPACKET_FOR_PUSH) return;
    if (!isExit) {
        //阻塞
        frameMutex.lock();
//        XLOGE(" data -------size is %d",data.size);
//        XLOGE(" frameList --------- size is %d",frameList.size());
        if (frameList.size() < maxList) {
            //生产者
            frameList.push_back(data);
            frameMutex.unlock();
        }
    }

}

void IPushStream::Main() {
    while (!isExit) {
//        XLOGE(" loop ------********pushStream********---------");
        frameMutex.lock();
        if (frameList.empty() || frameList.size() < DEFAULT_FRAME_NUM) { // 先看队列的数据是否有4帧以上
            frameMutex.unlock();
            XSleep(1);
            continue;
        }
        //取出frame  消费者
        XData frame = frameList.front();
        frameList.pop_front(); // 从队列头部删除取出的数据
        this->StartPushStream(frame);
        frameMutex.unlock();
    }
}

void IPushStream::Clear() {
    frameMutex.lock();
    while (!frameList.empty()) {
        frameList.front().Drop();
        frameList.pop_front();
    }
    frameMutex.unlock();
}


