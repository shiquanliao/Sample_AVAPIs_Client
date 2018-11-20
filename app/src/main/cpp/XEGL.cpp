//
// Created by CC000033 on 2018/8/6.
//

#include <android/native_window.h>
#include <EGL/egl.h>
#include <mutex>
#include "XEGL.h"
#include "XLog.h"

class CXEGL : public XEGL {
public:
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
    std::mutex mutex;

    void Close() override {

        mutex.lock();
        if (display == EGL_NO_DISPLAY) {
            mutex.unlock();
            return;
        }
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (surface != EGL_NO_SURFACE)
            eglDestroySurface(display, surface);
        if (context != EGL_NO_CONTEXT)
            eglDestroyContext(display, context);

        eglTerminate(display);

        display = EGL_NO_DISPLAY;
        surface = EGL_NO_SURFACE;
        context = EGL_NO_CONTEXT;
        mutex.unlock();
    }

    void Draw() override {
        mutex.lock();
        if (display == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE) {
            mutex.unlock();
            return;
        }
        eglSwapBuffers(display, surface);
        mutex.unlock();
    }

    bool Init(void *win) override {
        ANativeWindow *nwin = static_cast<ANativeWindow *>(win);
        Close();
        //初始化EGL
        mutex.lock();
        //1 获取EGLDisplay对象 显示设备
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY) {
            mutex.unlock();
            XLOGE("eglGetDisplay failed!");
            return false;
        }
        XLOGE("eglGetDisplay success!");

        //2 初始化Display
        if (EGL_TRUE != eglInitialize(display, 0, 0)) {
            mutex.unlock();
            XLOGE("eglInitialize failed!");
            return false;
        }
        XLOGE("eglInitialize success!");

        //3 获取配置并创建surface
        EGLint configSpec[] = {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_NONE
        };
        EGLConfig config = 0;
        EGLint numConfigs = 0;
        if (EGL_TRUE != eglChooseConfig(display, configSpec, &config, 1, &numConfigs)) {
            mutex.unlock();
            XLOGE("eglChooseConfig failed!");
            return false;
        }
        XLOGE("eglChooseConfig success!");

        // eglCreatePbufferSurface 是创建一个离线的Surface
//        surface = eglCreatePbufferSurface(display,config,NULL);
        surface = eglCreateWindowSurface(display, config, nwin, NULL);

        //4 创建并打开EGL上下文
        const EGLint ctxAttr[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
        context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctxAttr);
        if (context == EGL_NO_CONTEXT) {
            mutex.unlock();
            XLOGE("eglCreateContext failed!");
            return false;
        }
        XLOGE("eglCreateContext success!");

        //为当前线程绑定Context与Surface,这样opengl才能工作
        if (EGL_TRUE != eglMakeCurrent(display, surface, surface, context)) {
            mutex.unlock();
            XLOGE("eglMakeCurrent failed!");
            return false;
        }
        XLOGE("eglMakeCurrent success!");

        mutex.unlock();
        return true;
    }
};


XEGL *XEGL::Get() {
    static CXEGL egl;
    return &egl;
}
