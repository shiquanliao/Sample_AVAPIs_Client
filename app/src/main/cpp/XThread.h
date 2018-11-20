//
// Created by CC000033 on 2018/8/2.
//

#ifndef XPLAYER_XTHREAD_H
#define XPLAYER_XTHREAD_H

//sleep 毫秒
void XSleep(int mis);

// c++ 11 线程库
class XThread {
public:
    //启动线程
    virtual bool Start();

    //通过控制isExit安全停止线程（不一定成功）
    virtual void Stop();

    //入口主函数
    virtual void Main() {}

    const char *name = 0;
protected:
    bool isExit = false;
    bool isRuning = false;

private:
    void ThreadMain();
};


#endif //XPLAYER_XTHREAD_H
