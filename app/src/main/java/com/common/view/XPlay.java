package com.common.view;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;


import com.common.jniFun.MyHandler;
import com.common.jniFun.PlayControl;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class XPlay extends GLSurfaceView implements SurfaceHolder.Callback,GLSurfaceView.Renderer {
    private String openUrl;
    private MyHandler handler;
    PlayControl playControl;
    private static final String TAG = "Class -- XPlay: ";

    public XPlay(Context context) {
//        super(context);
//        // android 8.0 需要设置
//        setRenderer(this);
        this(context, null);
    }

    public XPlay(Context context, AttributeSet attrs) {
        super(context, attrs);
        // android 8.0 需要设置
        setRenderer(this);
        playControl = new PlayControl();
        Log.d(TAG,"XPlay()");

    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG,"surfaceCreated()");

        playControl.setOnErrorListener((code, msg) -> {
            Log.e(TAG , "---- from Jni  code: " + code + ", msg: " + msg);
            Message message = Message.obtain();
            message.what = code;
            message.obj = msg;
            handler.sendMessage(message);
        });
        //初始化opengl egl 显示
        InitView(holder.getSurface());
        new Thread() {
            @Override
            public void run() {
                 boolean status = Open(openUrl);
                if (!status) { // 打开视频流失败
                    //发送到主线程执行
                    Message msg = Message.obtain();
                    msg.what = 100;
                    handler.sendMessage(msg);
                } else {
                    playControl.callbackThread();
                }
            }
        }.start();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        Log.d(TAG,"surfaceChanged()");
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG,"surfaceDestroyed()");
//        PlayControl.stopPushStream();

    }

    public native void InitView(Object surface);

    /**
     * open XPlay
     *
     * @param url ---- play path
     *            example: path = "rtmp://xxxxxx"  OR  path = "rtsp://xxxxxx"
     */
    public native boolean Open(String url);

    public void setUrl(String url,MyHandler handler) {
        this.openUrl = url;
        this.handler = handler;
    }
    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        Log.d(TAG,"onSurfaceCreated()");
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int i, int i1) {
        Log.d(TAG,"onSurfaceChanged()");
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        Log.d(TAG,"onDrawFrame()");
    }

    @Override
    public void onResume() {
        Log.d(TAG,"执行onResume()");
        super.onResume();
    }
}
