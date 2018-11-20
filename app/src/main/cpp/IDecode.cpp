//
// Created by CC000033 on 2018/8/3.
//

#include "IDecode.h"
#include "XLog.h"

void IDecode::Update(XData data) {
    if (data.type != AVPACKET_TYPE) return;  //不是发给音频或者视频的数据  而是发给推流的数据
    if (data.isAudio != isAudio) {
        return;
    }
//    XLOGE("decode packet size is %d ",packs.size());

    if (!isExit) {
        //阻塞
        packsMutex.lock();
        if (packs.size() < maxList) {
            //生产者
            packs.push_back(data);
            packsMutex.unlock();
        }
    }

}

void IDecode::Main() {

    while (!isExit) {
//        XLOGE(" loop ------****************---------");
//        XLOGE(" isExit is  %d",isExit);

        packsMutex.lock();
        // 判断音视频同步
        if (!isAudio && synPts > 0 && isHaveAudio) {
            if (synPts < pts) {
                packsMutex.unlock();
                XSleep(1);
                XLOGE(" synPts is %d, pts is %d",synPts,pts);
                continue;
            }
        }


        if (packs.empty()) {
            packsMutex.unlock();
            XSleep(1);
//            XLOGE(" loop ------22222---------");
            continue;
        }
        // 取出packet 消费者
        bool bIsInput = false;
        XData pack = packs.front();
        packs.pop_front();  //把头部的数据从列表中删除

        if (this->SendPacket(pack)) {
//        do {
//            XLOGE(" loop ------33333---------");
//            bIsInput = this->SendPacket(pack);
            do {
//                XLOGE(" loop ------44444---------");
                // 获取解码数据
                XData frame = RecvFrame();
                // XLOGE("frame %d",frame.size);
                if (!frame.data) break;
                pts = frame.pts;
                // XLOGE("RecvFrame %d", frame.size);
                // 发送数据给观察者(视频播放观察者,音频播放器观察者)
                this->Notify(frame);
            } while (!isExit);
        }
        pack.Drop();
        packsMutex.unlock();
    }
}

void IDecode::Clear() {
    packsMutex.lock();
    while (!packs.empty()) {
        packs.front().Drop();
        packs.pop_front();
    }
    pts = 0;
    synPts = 0;
    packsMutex.unlock();
}
