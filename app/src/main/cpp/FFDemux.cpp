//
// Created by CC000033 on 2018/8/2.
//

#include "FFDemux.h"
#include "XLog.h"
#include "FFPushStream.h"
#include "IPlayProxy.h"
#include <thread>
#include <jni.h>

extern "C"
{
#include <libavformat/avformat.h>
}

//分数转为浮点数
static double r2d(AVRational r) {
    return r.num == 0 || r.den == 0 ? 0. : (double) r.num / (double) r.den;
}


static char rtspUrl[256] = {0};


//打开文件，或者流媒体 rmtp http rtsp
bool FFDemux::Open(const char *url) {
    Close();
    mutex.lock();
    strncpy(rtspUrl, url, strlen(url));
    XLOGI("open file %s begin", url);

    //使用TCP连接打开RTSP，设置最大延迟时间
    AVDictionary *avdic = NULL;
    char option_key[] = "rtsp_transport";
    char option_value[] = "tcp";
    av_dict_set(&avdic, option_key, option_value, 0);
    char option_key1[] = "bsf:a";
    char option_value1[] = "aac_adtstoasc";
    av_dict_set(&avdic, option_key1, option_value1, 0);
    char option_key2[] = "max_delay";
    char option_value2[] = "3000000";
    av_dict_set(&avdic, option_key2, option_value2, 0);

//    char option_key3[] = "buffer_size";
//    char option_value3[] = "10000000";
//    av_dict_set(&avdic, option_key3, option_value3, 0);
//    char option_key4[] = "recv_buffer_size";
//    char option_value4[] = "10000000";
//    av_dict_set(&avdic, option_key4, option_value4, 0);
//    char option_key6[] = "timeout";
//    char option_value6[] = "2000000";
//    av_dict_set(&avdic, option_key6, option_value6, 0);

    char option_key5[] = "stimeout";
    char option_value5[] = "2000000";
    av_dict_set(&avdic, option_key5, option_value5, 0);

    //这2项是为了配置 --- avformat_find_stream_info()
    av_dict_set(&avdic, "probesize", "2048", 0);
    av_dict_set(&avdic, "max_analyze_duration", "1000", 0);

    input_runner = {0};

    ic = avformat_alloc_context();
    ic->interrupt_callback.callback = AVInterruptCallBackFun;
    ic->interrupt_callback.opaque = &input_runner;
    // 调用之前初始化时间
//    input_runner.lasttime = time(NULL);

    //添加重连机制
    int connectTime = 1;
    if (isReStarting) {
        connectTime = RESTART_TIME; //设置为2次重试机会
//        m_gFFDemux->SendMessage(110, "Test ------------");


    }
    int re = 0;
    for (int i = 0; i < connectTime; i++) {
        re = avformat_open_input(&ic, url, 0, &avdic);
        if (re != 0) {
            char buff[1024] = {0};
            av_strerror(re, buff, sizeof(buff));
            if (isReStarting) {
                if (ic) { // 先释放
                    avformat_close_input(&ic);
                }
                //开启子线程，并把WlListener指针传递到子线程中

                m_gFFDemux->SendMessage(110 + i, "Test ------------");

                XLOGE("第%d重新连接 FFDemux open %s failed! beacuse: %s", i, url, buff);
            } else {
                XLOGE("FFDemux open %s failed! beacuse: %s", url, buff);

            }
            if (i == connectTime - 1) { // 最后一次连接 可能是 第一次打开 ,也可能是重连的最后一次 不执行后面的代码
                mutex.unlock();
                return false;
            }
        } else {
            XLOGE("FFDemux open %s success!", url);
            if (isReStarting) {
//                m_gFFDemux->SendMessage(120, "Test ------------");
            }
            break;
        }
    }

    //读取文件信息
    re = avformat_find_stream_info(ic, 0);
    if (re != 0) {
        mutex.unlock();
        char buff[1024] = {0};
        av_strerror(re, buff, sizeof(buff));
        XLOGE("avformat_find_stream_info %s failed!", url);
        return false;
    }
    this->totalMs = ic->duration / (AV_TIME_BASE / 1000);
    isReStarting = false;

    mutex.unlock();
    XLOGI("total ms = %d !", totalMs);

    GetVPara();
    GetAPara();
    return true;
}

void FFDemux::Close() {
    mutex.lock();
    if (ic) {
        avformat_close_input(&ic);
    }
    mutex.unlock();
}

//读取一帧数据，数据由调用者清理
XData FFDemux::Read() {
    mux.lock();
    if (!ic) {
        mux.unlock();
        return XData();
    }
    if (isReStarting) {
        mux.unlock();
        return XData();
    }
    XData data;
    AVPacket *pkt = av_packet_alloc();
    // 调用之前初始化时间
    input_runner.lasttime = time(NULL);
    int re = av_read_frame(ic, pkt);
    if (re != 0) { // 读取帧失败
        av_packet_free(&pkt);
        mux.unlock();
        XLOGE("av_read_frame failed");
        return XData();
    }
//    XLOGI("pack size is %d , pts is %lld ", pkt->size, pkt->pts);
    data.data = reinterpret_cast<unsigned char *>(pkt);
    data.size = pkt->size;
    if (pkt->stream_index == AVMEDIA_TYPE_AUDIO) {
        data.isAudio = true;
        FILE *f = fopen("/sdcard/audiodata.aac", "a+b");
        fwrite(pkt->data, 1, pkt->size, f);
        fclose(f);
//        XLOGE("aac ------- success = %d", pkt->size);
    } else if (pkt->stream_index == AVMEDIA_TYPE_VIDEO) {
        data.isAudio = false;
    } else { //销毁数据, 防止内存泄漏
        av_packet_free(&pkt);
        mux.unlock();
        return XData();
    }

    //转换pts
    pkt->pts = pkt->pts * (1000 * r2d(ic->streams[pkt->stream_index]->time_base));
    pkt->dts = pkt->dts * (1000 * r2d(ic->streams[pkt->stream_index]->time_base));
    data.pts = (int) pkt->pts;
//    XLOGE("demux pts %d", data.pts);

    // 推流
    XData pushData;
    if (!isStopPush) {
        AVPacket *outPkt = av_packet_alloc();
        av_packet_ref(outPkt, pkt);

        pushData.data = reinterpret_cast<unsigned char *>(outPkt);
        pushData.size = outPkt->size;
        pushData.type = AVPACKET_FOR_PUSH;
        pushData.isAudio = data.isAudio;
    }

    mux.unlock();
    if (!isStopPush)
        Notify(pushData);
    return data;
}

void FFDemux::SetP2PVideoData(u_int8_t *data, int size, int pts) {
    mux.lock();
    XData frameData;
    AVPacket *p2pData = av_packet_alloc();
    av_init_packet(p2pData);
    p2pData->data = data;
    p2pData->pts = pts;
    p2pData->dts = pts;
    p2pData->size = size;

    frameData.data = reinterpret_cast<unsigned char *>(p2pData);
    frameData.size = size;
    frameData.isAudio = false;
    mux.unlock();
    Notify(frameData);
}


void FFDemux::SetP2PAudioData(u_int8_t *data, int size, int pts) {
    mux.lock();
    XData frameData;
    AVPacket *p2pData = av_packet_alloc();
    av_init_packet(p2pData);
    p2pData->data = data;
    p2pData->pts = pts;
    p2pData->dts = pts;
    p2pData->size = size;

    frameData.data = reinterpret_cast<unsigned char *>(p2pData);
    frameData.size = size; //随机输入的
    frameData.isAudio = true;


    mux.unlock();
    Notify(frameData);
}

XParameter FFDemux::GetVPara() {
    mux.lock();
    if (!ic) {
        mux.unlock();
        XLOGE("GetVPara failed! ic is NULL！");
        return XParameter();
    }
    //获取了视频流索引
    int re = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, 0, 0);
    if (re < 0) {
        mux.unlock();
        XLOGE("av_find_best_stream failed!");
        return XParameter();
    }
    XParameter para;
    para.para = ic->streams[re]->codecpar;
    mux.unlock();
    return para;
}


XParameter FFDemux::GetAPara() {
    mux.lock();
    if (!ic) {
        mux.unlock();
        XLOGE("GetVPara failed! ic is NULL！");
        return XParameter();
    }
    //获取了音频流索引
    int re = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, 0, 0);
    if (re < 0) {
        mux.unlock();
        XLOGE("av_find_best_stream failed!");
        return XParameter();
    }
    XParameter para;
    para.para = ic->streams[re]->codecpar;
    para.channels = ic->streams[re]->codecpar->channels;
    para.sample_rate = ic->streams[re]->codecpar->sample_rate;
    mux.unlock();
    return para;
}


int FFDemux::AVInterruptCallBackFun(void *ctx) {
    Runner *r = (Runner *) ctx;
    if (r->lasttime > 0) {

        //-------------test---------------
        /*  //执行回调
          m_gFFDemux->unmber++;
          if (m_gFFDemux->unmber >1000 && m_gFFDemux->unmber % 30000 == 0) {
              m_gFFDemux->isReStarting = true;
              XLOGE(" m_gFFDemux->unmber is %d",m_gFFDemux->unmber);
              std::thread td(reStart);
              td.detach();
          }*/
        if (time(NULL) - r->lasttime > 2) {
            // 等待超过2s则中断
            if (m_gFFDemux->isReStarting)
                return 1;
            m_gFFDemux->isReStarting = true;
            XLOGE("等待超时 isExit is %d", m_gFFDemux->isExit);
            std::thread td(reStart);
            td.detach();
            XLOGE("执行完毕");
            return 1;
        }
    }
//    XLOGE("连接成功");
    return 0;
}

void FFDemux::reStart() {
//    const char *temp = "rtsp://192.168.154.153/main";
    IPlayProxy::Get()->restartDemux();
    m_gFFDemux->Open(rtspUrl);

}

FFDemux *FFDemux::m_gFFDemux;

FFDemux::FFDemux() {
    m_gFFDemux = this;
}

AVFormatContext *FFDemux::GetAVFormatContext() {
    if (ic) return ic;
    return nullptr;
}















