//
// Created by CC000033 on 2018/8/3.
//

#include "IDecode.h"
#include "XLog.h"

void IDecode::Update(XData pkt) {
    if (pkt.type != AVPACKET_TYPE) return;  //不是发给音频或者视频的数据  而是发给推流的数据
    if (pkt.isAudio != isAudio) {
        return;
    }
    if (pkt.isAudio) {
//        XLOGE("audio  --- decode packet size is %d, data size is %d ", packs.size(),pkt.size);
    } else {
//        XLOGE("video --- decode packet size is %d ", packs.size());
    }


    if (!isExit) {
        //阻塞
        packsMutex.lock();
        if (packs.size() < maxList) {
            //生产者
            packs.push_back(pkt);
//            if (pkt.isAudio) {
//                XLOGE("压完之后 audio --- decode packet size is %d ", packs.size());
//            } else {
////                XLOGE("压完之后 video --- decode packet size is %d ", packs.size());
//            }

            packsMutex.unlock();
        }
    }

//    while (!isExit) {
//        //阻塞
//        packsMutex.lock();
//        if (packs.size() < maxList) {
//            //生产者
//            packs.push_back(pkt);
//            packsMutex.unlock();
//            break;
//        }
//        packsMutex.unlock();
//        XSleep(1);
//    }

}

void IDecode::Main() {

    while (!isExit) {
//        XLOGE(" loop ------****************---------");

        packsMutex.lock();
        // 判断音视频同步
//        if (!isAudio && synPts > 0 && isHaveAudio) {
//            if (synPts < pts) {
//                packsMutex.unlock();
//                XSleep(1);
//                XLOGE(" synPts is %d, pts is %d", synPts, pts);
//                continue;
//            }
//        }


        if (packs.empty()) {
            packsMutex.unlock();
            XSleep(1);
            continue;
        }
        // 取出packet 消费者
        XData pack = packs.front();
        packs.pop_front();  //把头部的数据从列表中删除

        if (this->SendPacket(pack)) {
            while (!isExit) {
                // 获取解码数据
                XData frame = RecvFrame();
                if (!frame.data) break;
//                XLOGE("frame %d", frame.size);
                if (frame.isAudio) {
//                    XLOGE("audio --- frame size is %d ", frame.size);
                }
                pts = frame.pts;
                // 发送数据给观察者(视频播放观察者,音频播放器观察者)
                this->Notify(frame);

            }
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
