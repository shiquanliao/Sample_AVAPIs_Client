//
// Created by CC000033 on 2018/8/6.
//

#ifndef XPLAYER_XEGL_H
#define XPLAYER_XEGL_H


class XEGL {
public:
    virtual bool Init(void *win) = 0;

    virtual void Close() = 0;

    virtual void Draw() = 0;

    static XEGL *Get();

protected:
    XEGL() {}
};


#endif //XPLAYER_XEGL_H
