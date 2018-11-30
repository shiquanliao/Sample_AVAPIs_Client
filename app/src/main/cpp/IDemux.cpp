//
// Created by CC000033 on 2018/8/2.
//

#include "IDemux.h"
#include "XLog.h"
bool IDemux::isReStarting = false;
void IDemux::Main() {
    while (!isExit ) {
        if (isReStarting){
//            XSleep(1);
            continue;
        }
        XData d;
        if (!isP2P){
            d = Read();
        }
        if (d.size > 0)
            Notify(d);
        else
            XSleep(2);
    }
}

void IDemux::Update(XData data) {
    IObserver::Update(data);
}

bool IDemux::P2POpen() {
    isP2P = true;
    return true;
}





