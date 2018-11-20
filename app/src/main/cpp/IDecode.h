//
// Created by CC000033 on 2018/8/3.
//

#ifndef XPLAYER_IDECODE_H
#define XPLAYER_IDECODE_H

#include "XParameter.h"
#include "IObserver.h"
#include "list"

//解码接口，支持硬解码
class IDecode : public IObserver {
public:
    //打开解码器
    virtual bool Open(XParameter para, bool isHard = false) = 0;
    virtual bool P2POpen(int i) = 0;

    virtual void Close() = 0;

    virtual void Clear();

    //future模型 发送数据到线程解码
    virtual bool SendPacket(XData pkt) = 0;

    //从线程中获取解码结果  再次调用会复用上次空间，线程不安全
    virtual XData RecvFrame() = 0;

    //由主体notify的数据 阻塞
    virtual void Update(XData data);

    bool isAudio = true; // 默认必须是true

    //最大的队列缓冲
    int maxList = 100;

    //同步时间, 再次打开文件数据要清理
    int synPts = 0;
    //当前视频播放的pts
    int pts = 0;

    // 判断当前是否有音频
    int isHaveAudio = true;


protected:
    virtual void Main() override ;
    //读取缓冲
    std::list<XData> packs;
    std::mutex packsMutex;

};


#endif //XPLAYER_IDECODE_H
