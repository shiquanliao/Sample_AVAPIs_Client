//
// Created by CC000033 on 2018/8/2.
//

#include "XThread.h"
#include "XLog.h"
#include <thread>

using namespace std;

void XSleep(int mis) {
    chrono::milliseconds du(mis);
    this_thread::sleep_for(du);
}


//启动线程
bool XThread::Start() {
    isExit = false;
    thread td(&XThread::ThreadMain, this);
    td.detach();

    return true;
}

//通过控制isExit安全停止线程（不一定成功）
void XThread::Stop() {

    isExit = true;
//    XLOGE(" isExit is  %d",isExit);
    for (int i = 0; i < 2200; ++i) {
        if (!isRuning) {
            XLOGI("停止--%s--线程成功",name);
            return;
        }
        XSleep(1);
    }
    XLOGI("停止--%s--线程超时",name);
}

void XThread::ThreadMain() {
    isRuning = true;
    XLOGI("--%s--线程函数进入",name);
    Main();
    XLOGI("--%s--线程函数退出",name);
    isRuning = false;

}
