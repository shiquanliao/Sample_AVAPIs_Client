//
// Created by CC000033 on 2018/8/6.
//

#ifndef XPLAYER_GLVIDEOVIEW_H
#define XPLAYER_GLVIDEOVIEW_H

#include "XData.h"
#include "IVideoView.h"

class XTexture;

class GLVideoView : public IVideoView {
public:
    virtual void SetRender(void *win);

    virtual void Render(XData data);

    void Close() override;

    void SetShaderVertex(float widthRatioForScreen, float heightRationForScreen, float offset,
                         float screenRatio) override;

protected:
    void *view = 0;
    XTexture *txt = 0;
    std::mutex mutex;

};


#endif //XPLAYER_GLVIDEOVIEW_H
