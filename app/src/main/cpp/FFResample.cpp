//
// Created by CC000033 on 2018/8/6.
//

#include "FFResample.h"
#include "XLog.h"

extern "C"
{
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}


bool FFResample::Open(XParameter in, XParameter out) {
    Close();
    mutex.lock();
    //音频重采样上下文初始化
    actx = swr_alloc();
    actx = swr_alloc_set_opts(actx,
                              av_get_default_channel_layout(out.channels),
                              AV_SAMPLE_FMT_S16, out.sample_rate,
                              av_get_default_channel_layout(in.para->channels),
                              (AVSampleFormat) in.para->format, in.para->sample_rate,
                              0, 0);
//    actx = swr_alloc_set_opts(actx,
//                              av_get_default_channel_layout(out.channels),
//                              AV_SAMPLE_FMT_S16, out.sample_rate,
//                              av_get_default_channel_layout(1),
//                              (AVSampleFormat) 8, 8000,
//                              0, 0);

    int re = swr_init(actx);
    if (re != 0) {
        mutex.unlock();
        XLOGE("swr_init failed!");
        return false;
    } else {
        XLOGI("swr_init success!");
    }
    outChannels = out.channels;
    outFormat = AV_SAMPLE_FMT_S16;

    mutex.unlock();
    return true;
}

void FFResample::Close() {
    mutex.lock();
    if (actx) {
        swr_free(&actx);
    }
    mutex.unlock();

}

XData FFResample::Resample(XData indata) {
    if (indata.size <= 0 || !indata.data) return XData();
    mutex.lock();
    if (!actx) {
        mutex.unlock();
        return XData();
    }
    //XLOGE("indata size is %d",indata.size);
    AVFrame *frame = (AVFrame *) indata.data;



    //输出空间的分配
    XData out;
    int outsize =
            outChannels * frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat) outFormat);
    if (outsize <= 0) {
        mutex.unlock();
        return XData();
    }
    out.Alloc(outsize);
    uint8_t *outArr[2] = {0};
    outArr[0] = out.data;
    int len = swr_convert(actx, outArr, frame->nb_samples, (const uint8_t **) frame->data,
                          frame->nb_samples);
    if (len <= 0) {
        mutex.unlock();
        out.Drop();
        return XData();
    }
    out.pts = indata.pts;

//    FILE *f = fopen("/sdcard/audiodata_after.pcm", "a+b");
//    fwrite(out.data, 1, outsize, f);
//    fclose(f);
    //XLOGE("swr_convert success = %d", len);

    mutex.unlock();
    return out;

}
