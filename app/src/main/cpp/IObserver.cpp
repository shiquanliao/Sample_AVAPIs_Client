//
// Created by CC000033 on 2018/8/3.
//

#include "IObserver.h"
#include "IPlayProxy.h"

//主体函数 添加观察者
void IObserver::AddObs(IObserver *observer) {
    if (!observer) return;
    mux.lock();
    obss.push_back(observer);
    mux.unlock();
}

//删除观察者(线程安全)
void IObserver::RemoveObs() {
    mux.lock();
    obss.pop_back();
    mux.unlock();
}

//通知所有观察者
void IObserver::Notify(XData data) {
    mux.lock();
    for (int i = 0; i < obss.size(); ++i) {
        obss[i]->Update(data);
    }
    mux.unlock();
}


void IObserver::SendMessage(int code, const char *msg) {
    IPlayProxy::Get()->SendMsgToJava(code, msg);
}




