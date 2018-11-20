//
// Created by CC000033 on 2018/9/12.
//

#include "FFPushStream.h"
#include "XLog.h"
#include "FFDemux.h"

extern "C" {
#include <libavformat/avformat.h>
}


void FFPushStream::Close() {
    IPushStream::Clear();
    mutex.lock();
    if (mOutContext) {
        avio_close(mOutContext->pb);
        avformat_free_context(mOutContext);
        mOutContext = NULL;
    }
    findIFrame = false;
    mutex.unlock();
}


bool FFPushStream::Init(AVFormatContext *ctx, const char *fileName, unsigned char isPushStream) {
    Close();
    mutex.lock();
    this->isPushStream = isPushStream;

//    const char *temp = "rtmp://212.64.34.165:10085/hls/8lm3E7xig?sign=U_m3EnbiRz";
    // char *temp = const_cast<char *>("rtmp://demo.easydss.com:10085/hls/r1_fCIkdQ?sign=H1euzRLydQ");
//    char *temp = const_cast<char *>("rtmp://a.rtmp.youtube.com/live2/dwh0-9dws-wpee-78uk");
//    if (isPushStream) {
//        fileName = temp;
//    }
    //创建输出上下文
    int re = 0;
    re = avformat_alloc_output_context2(&mOutContext, nullptr, "flv", fileName);
    if (re < 0) {
        mutex.unlock();
        char buff[1024] = {0};
        av_strerror(re, buff, sizeof(buff));
        XLOGE("avformat_alloc_output_context2 failed ---  %s ", buff);
        return false;
    }
    re = avio_open2(&mOutContext->pb, fileName, AVIO_FLAG_READ_WRITE, nullptr, nullptr);
    if (re < 0) {
        mutex.unlock();
        char buff[1024] = {0};
        av_strerror(re, buff, sizeof(buff));
        XLOGE("avio_open2 failed ---  %s ", buff);
        return false;
    }

//    if (isPushStream) {
//        ofmt = outputContext->oformat;
//    }

    for (int i = 0; i < ctx->nb_streams; ++i) {
        //根据输入流创建输出流
        AVStream *in_stream = ctx->streams[i];
        AVStream *stream = avformat_new_stream(mOutContext, in_stream->codec->codec);
        //将输出流的编码信息复制到输入流
        re = avcodec_copy_context(stream->codec, ctx->streams[i]->codec);
        if (re < 0) {
            char buff[1024] = {0};
            av_strerror(re, buff, sizeof(buff));
            XLOGE("avcodec_copy_context failed ---  %s ", buff);
        }
        stream->codec->codec_tag = 0;
//        if (outputContext->oformat->flags & AVFMT_GLOBALHEADER)
//            stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }

    re = avformat_write_header(mOutContext, nullptr);
    if (re < 0) {
        mutex.unlock();
        char buff[1024] = {0};
        av_strerror(re, buff, sizeof(buff));
        XLOGE("avformat_write_header failed ---  %s ", buff);
        return false;
    }
    XLOGE("-------------path:   %s", fileName);
    XLOGE("----------------- success   output: %s-------------------------- ", fileName);

    mutex.unlock();
    return true;
}


bool FFPushStream::StartPushStream(XData data) {
    mutex.lock();
    int re = 0;
    if (data.size <= 0) return false;

    if (mOutContext) {
        //判断是否I帧再写入文件
        if(!isPushStream && !findIFrame && !data.isAudio){
//            if(((AVPacket *)data.data)->flags == AV_PKT_FLAG_KEY){
//                findIFrame = true;
//                XLOGE("-------------flags:   %s", "关键帧");
//            } else{
//                XLOGE("-------------flags:   %s", "非关键帧");
//                av_packet_unref(reinterpret_cast<AVPacket *>(data.data));
//                mutex.unlock();
//                return false;
//            }
        }
//        XLOGE(" 推送的数据大小: %d", pkt.size);
        re = av_interleaved_write_frame(mOutContext, reinterpret_cast<AVPacket *>(data.data));
        av_packet_unref(reinterpret_cast<AVPacket *>(data.data));
        if (re != 0) {
            char buff[1024] = {0};
            av_strerror(re, buff, sizeof(buff));
            XLOGE("av_interleaved_write_frame failed ---  %s ", buff);
        } else {
//            XLOGE("av_interleaved_write_frame ---  success   pts:  %d", data.pts);
        }
    }
    mutex.unlock();
    return true;
}

bool FFPushStream::StopPushStream() {
    mutex.lock();
    if (isPushStream) {
        mutex.unlock();
        return false;
    }
    //写文件尾
    if (mOutContext != NULL) {
        av_write_trailer(mOutContext);
    }
    findIFrame = false;
    mutex.unlock();
    Clear();
    return true;
}