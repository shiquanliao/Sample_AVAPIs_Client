//
// Created by CC000033 on 2018/8/3.
//

#ifndef XPLAYER_IOBSERVER_H
#define XPLAYER_IOBSERVER_H


#include "XThread.h"
#include "XData.h"
#include "XListener.h"
#include <vector>
#include <mutex>


class IObserver : public XThread {
public:
    //观察者接收数据函数
    virtual void Update(XData data) {}

    //主体函数 添加观察者(线程安全)
    void AddObs(IObserver *observer);

    //删除观察者(线程安全)
    void RemoveObs();

    //通知所有观察者(线程安全)
    void Notify(XData data);

    static void SendMessage(int code, const char *msg);

protected:
    std::vector<IObserver *> obss;

    std::mutex mux;
};


#endif //PLAYER_IOBSERVER_H
