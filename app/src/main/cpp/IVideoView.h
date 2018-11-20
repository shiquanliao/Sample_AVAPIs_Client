//
// Created by CC000033 on 2018/8/6.
//

#ifndef XPLAYER_IVIDEOVIEW_H
#define XPLAYER_IVIDEOVIEW_H


#include "IObserver.h"

class IVideoView : public IObserver {
public:
    virtual void SetRender(void *win) = 0;

    virtual void Render(XData data) = 0;

    virtual void Close() = 0;

    virtual void Update(XData data);

    virtual void
    SetShaderVertex(float widthRatioForScreen, float heightRationForScreen, float offset,
                    float screenRatio) {}

protected:
    float width = 1.0f; // 屏幕宽 -- 用来在shader中设置vertex属性
    float height = 1.0f; // 屏幕高 -- 用来在shader中设置vertex属性
    float screenRatio = 1.0f; // 屏幕高比例
    float offset = 0.0f;

};


#endif //XPLAYER_IVIDEOVIEW_H
