//
// Created by CC000033 on 2018/8/6.
//

#ifndef XPLAYER_XSHADER_H
#define XPLAYER_XSHADER_H

#include <mutex>

enum XShaderType {
    XSHADER_YUV420P = 0,    //软解码和虚拟机
    XSHADER_YUVJ420P = 12,  // Y 4  u 1 v 1
    XSHADER_NV12 = 23,      //手机
    XSHADER_NV21 = 24
};

class XShader {
public:
    virtual bool Init(XShaderType type = XSHADER_YUV420P);

    virtual void Close();

    //获取材质并映射到内存
    virtual void
    GetTexture(unsigned int index, int width, int height, unsigned char *buf, bool isa = false);

    virtual void Draw();

    virtual void SetShaderVertex(float widthRatioForScreen, float heightRationForScreen, float offset);

protected:
    unsigned int vsh = 0;
    unsigned int fsh = 0;
    unsigned int program = 0;
    unsigned int texts[100] = {0};
    float width = 1.0f;
    float height = 1.0f;
    float offset = 0.0f;
    std::mutex mutex;

};


#endif //XPLAYER_XSHADER_H
