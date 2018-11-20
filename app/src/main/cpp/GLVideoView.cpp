//
// Created by CC000033 on 2018/8/6.
//

#include "GLVideoView.h"
#include "XTexture.h"

void GLVideoView::SetRender(void *win) {
    view = win;
}

void GLVideoView::Render(XData data) {
    if (!view) return;
    if (!txt) {
        txt = XTexture::Create();

        // 设置shader显示比例
        if (height == -10.0f) {
            float temp = 2 * width * (float) data.height / (float) data.width;
            height = (1.0f - temp * screenRatio);
        } else {
            height = 1.0f - 2 * height;
        }
        width = -1.0f + 2 * width;
        txt->SetShaderVertex(width, height,offset);
        txt->Init(view, static_cast<XTextureType>(data.format));
    }
    txt->Draw(data.datas, data.width, data.height);


}

void GLVideoView::Close() {
    mutex.lock();
    if (txt) {
        txt->Drop();
        txt = 0;
    }
    mutex.unlock();
}

void
GLVideoView::SetShaderVertex(float widthRatioForScreen, float heightRationForScreen, float offset,
                             float screenRatio) {
    mutex.lock();
    // 设置宽高
    width = widthRatioForScreen;
    height = heightRationForScreen;
    this->screenRatio = screenRatio;
    this->offset = offset;

    mutex.unlock();
}
