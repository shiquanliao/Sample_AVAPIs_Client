package com.common.jniFun;

import com.tutk.sample.AVAPI.Client;

/**
 * control Play
 */
public class PlayControl {
    static {
        System.loadLibrary("native-lib");
    }

    private static PlayControl sPlayControl = null;

    public static final int VIDEODATA = 0;
    public static final int AUDIODATA = 1;


    /**
     * Instance
     *
     * @return
     */
    public static PlayControl getInstance() {
        if (sPlayControl == null) {
            sPlayControl = new PlayControl();
        }
        return sPlayControl;
    }


    private PlayControl() {

    }

    private static native void setVertexSize(float widthRatioForScreen, float heightRationForScreen, float offset, float screenRatio);

    /**
     * set VertexAttribArray for shader
     * self-adaption by video aspect ratio
     *
     * @param widthRatioForScreen   ratio for view(width)/Screen
     * @param heightRationForScreen default is 1.0f
     * @param offset                Y offset
     * @param screenRatio           ratio for screen(width) / height
     */
    public static void setViewSize(float widthRatioForScreen, float heightRationForScreen, float offset, float screenRatio) {
        setVertexSize(widthRatioForScreen, heightRationForScreen, offset, screenRatio);
    }

    public static void setViewSize(float widthRatioForScreen, float offset, float screenRatio) {
        setVertexSize(widthRatioForScreen, -10.0f, offset, screenRatio);
    }

    /**
     * push stream to RTMP OR write stemp to file
     *
     * @param url          example: "rtmp//xxxxxx" OR  "/sdcard/xxx/xxxx.flv"
     *                     Note: only support flv format
     * @param isPushToRTMP True is push stream to RTMP path , False is write to file
     */
    public static native void startPushStream(String url, boolean isPushToRTMP);


    /**
     * stop push or write
     */
    public static native void stopPushStream();


    /**
     * when activity is destroy, please call this func
     */
    public static native void destroy();


    /**
     * P2P model ---  set video data to c++ api
     *
     * @param videoData
     * @param size
     * @param frameInfo
     */
    public static native void setFrameVideoData(byte[] videoData, int size, byte[] frameInfo);

    /**
     * P2P model ---  set audio data to c++ api
     *  @param audioData
     * @param ret
     * @param frameInfo
     */
    public static native void setFrameAudioData(byte[] audioData, int ret, byte[] frameInfo);


    /**
     * normal Treadh
     */
    public native void normalThread();

    /**
     * mutex Thread
     */
    public native void mutexThread();


    /**
     * callback Thread
     */
    public native void callbackThread();

    private OnErrorListener onErrorListener;

    public void setOnErrorListener(OnErrorListener onErrorListener) {
        this.onErrorListener = onErrorListener;
    }


    //Jni调用此方法，把结果返回到java层

    /**
     * c++ or c call this func and return result to java func
     *
     * @param code message type
     * @param msg  message content
     */
    public void onError(int code, String msg) {
        if (onErrorListener != null) {
            onErrorListener.onError(code, msg);
        }
    }

    public void audioState(boolean openAudio) {
        Client.setOpenAudio(openAudio);
    }

    public interface OnErrorListener {
        void onError(int code, String msg);
    }


}
