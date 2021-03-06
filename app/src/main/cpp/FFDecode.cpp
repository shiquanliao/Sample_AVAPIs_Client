//
// Created by CC000033 on 2018/8/3.
//


#include <string>
#include "FFDecode.h"
#include "XLog.h"

extern "C"
{
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavcodec/jni.h>
#include <libswscale/swscale.h>
}


void make_dsi(unsigned int sampling_frequency_index, unsigned int channel_configuration,
              unsigned char *dsi) {
    unsigned int object_type = 2; // AAC LC by default
    dsi[0] = static_cast<unsigned char>((object_type << 3) | (sampling_frequency_index >> 1));
    dsi[1] = static_cast<unsigned char>(((sampling_frequency_index & 1) << 7) | (channel_configuration << 3));

}

int get_sr_index(unsigned int sampling_frequency) {
    switch (sampling_frequency) {
        case 96000:
            return 0;
        case 88200:
            return 1;
        case 64000:
            return 2;
        case 48000:
            return 3;
        case 44100:
            return 4;
        case 32000:
            return 5;
        case 24000:
            return 6;
        case 8000:
            return 11;
        default:
            return 0;
    }
}



void FFDecode::InitHard(void *vm) {
    av_jni_set_java_vm(vm, 0);
}

bool FFDecode::P2POpen(int i) {

    Close();
    //1 查找解码器
    AVCodecID type;
    if (i == 0) {
        type = AV_CODEC_ID_H264;
    } else {
        type = AV_CODEC_ID_AAC;
    }
    AVCodec *cd = avcodec_find_decoder(type);
    if (!cd) {
        XLOGE("P2P avcodec_find_decoder %d failed -- %d");
        return false;
    }
    mux.lock();
    //2 创建解码上下文
    codec = avcodec_alloc_context3(cd);
    codec->thread_count = 8;

//    unsigned char dsi[2];
//    make_dsi((unsigned int) get_sr_index((unsigned int) 8000), (unsigned int) 1, dsi);
//    codec->extradata = (uint8_t *) dsi;
//    codec->extradata_size = 2;


            //3 打开解码器
    int re = avcodec_open2(codec, 0, 0);
    if (re != 0) {
        mux.unlock();
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        XLOGE("avcodec_open2 failed because : %s", buf);
        return false;
    }
    if (codec->codec_type == AVMEDIA_TYPE_AUDIO) {
        this->isAudio = true;
    } else {
        this->isAudio = false;
    }
    mux.unlock();
    XLOGE("P2P avcodec_open2 success");
    return true;
}


bool FFDecode::Open(XParameter para, bool isHard) {
    Close();
    if (!para.para) return false;
    AVCodecParameters *p = para.para; // 弄清楚从哪里过来

    //1 查找解码器
    AVCodec *cd = avcodec_find_decoder(p->codec_id);
//    if (isHard) {
//        cd = avcodec_find_decoder_by_name("h264_mediacodec");
//    }
    if (!cd) {
        XLOGE("avcodec_find_decoder %d failed -- %d", p->codec_id, isHard);
        return false;
    }
    XLOGI("avcodec_find_decoder success! --- %d", isHard);

    mux.lock();
    //2 创建解码上下文，并复制参数
    codec = avcodec_alloc_context3(cd);
    avcodec_parameters_to_context(codec, p);

    codec->thread_count = 8;
    //3 打开解码器
    int re = avcodec_open2(codec, 0, 0);
    if (re != 0) {
        mux.unlock();
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        XLOGE("avcodec_open2 failed because : %s", buf);
        return false;
    }

    if (codec->codec_type == AVMEDIA_TYPE_AUDIO) {
        this->isAudio = true;
    } else {
        this->isAudio = false;
    }

    mux.unlock();
    XLOGE("avcodec_open2 success");
    return true;
}

void FFDecode::Close() {
    IDecode::Clear();
    mutex.lock();
    pts = 0;
    if (pFrameYUV) {
        av_frame_free(&pFrameYUV);
    }
    if (frame) {
        av_frame_free(&frame);
    }
    if (img_convert_ctx) {
        sws_freeContext(img_convert_ctx);
        img_convert_ctx = NULL;
    }

    if (codec) {
        avcodec_close(codec);
        avcodec_free_context(&codec);
    }
    mutex.unlock();
}

bool FFDecode::SendPacket(XData pkt) {
    if (pkt.size <= 0 || !pkt.data) {
        return false;
    }
    mux.lock();
    if (!codec) {
        mux.unlock();
        return false;
    }
    int re = avcodec_send_packet(codec, (AVPacket *) pkt.data);
    mux.unlock();
    if (re != 0) {
        XLOGE("decode msg:[re is %d, --  %s].", re, av_err2str(re));
        return false;
    }
//    XLOGE("avcodec_send_packet  success!!!!!!");
    return true;
}

//从线程中获取解码结果
XData FFDecode::RecvFrame() {
    mux.lock();
    if (!codec) {
        mux.unlock();
        return XData();
    }
    if (!frame) {
        frame = av_frame_alloc();
    }
    if (!pFrameYUV) {
        pFrameYUV = av_frame_alloc();
    }

    int re = avcodec_receive_frame(codec, frame);
    if (re != 0) {
        mux.unlock();
//        XLOGE("avcodec_receive_frame failed because : %s", av_err2str(re));
        return XData();
    }
    XData d;
    d.data = (unsigned char *) frame;
    if (codec->codec_type == AVMEDIA_TYPE_VIDEO) {
        d.size = (frame->linesize[0] + frame->linesize[1] + frame->linesize[2]) * frame->height;
        d.width = frame->width;
        d.height = frame->height;
        d.isAudio = false;
        DataCopy(frame);
        memcpy(d.datas, pFrameYUV->data, sizeof(d.datas));
//        XLOGE("视频解码数据pts:  %d",(int)frame->pts);
    } else {
        //样本字节数 * 单通道样本数 * 通道数
        d.size = av_get_bytes_per_sample((AVSampleFormat) frame->format) * frame->nb_samples *
                 frame->channel_layout;
        d.isAudio = true;
        memcpy(d.datas, frame->data, sizeof(d.datas));
//        XLOGE("音频解码数据pts:  %d",(int)frame->pts);

    }
    d.format = frame->format;

    d.pts = frame->pts;

    pts = d.pts;
    mux.unlock();
//    XLOGE("avcodec_receive_frame  success!!!!!!");
    return d;
}

void FFDecode::DataCopy(AVFrame *m_pYUVFrame) {

    if (!img_convert_ctx) {
        out_buffer = (uint8_t *) av_malloc(
                avpicture_get_size(AV_PIX_FMT_YUV420P, codec->width, codec->height));
        //设置图像内容
        avpicture_fill((AVPicture *) pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, codec->width,
                       codec->height);

        img_convert_ctx = sws_getContext(codec->width, codec->height, codec->pix_fmt,
                                         codec->width, codec->height, AV_PIX_FMT_YUV420P,
                                         SWS_BICUBIC, NULL, NULL, NULL);

    }

    sws_scale(img_convert_ctx, (const uint8_t *const *) m_pYUVFrame->data, m_pYUVFrame->linesize, 0,
              codec->height,
              pFrameYUV->data, pFrameYUV->linesize);

}




