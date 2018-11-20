//
// Created by CC000033 on 2018/8/2.
//

#ifndef XPLAYER_XLOG_H
#define XPLAYER_XLOG_H


class XLog {

};

#define XLOG_LEVEL_CLOSE 0
#define XLOG_LEVEL_ERROR 1
#define XLOG_LEVEL_INFO  2
#define XLOG_LEVEL_DEBUG 3

//#define XLOG_LEVEL XLOG_LEVEL_ERROR
//#define XLOG_DETAIL

#ifndef XLOG_TAG
#define XLOG_TAG "XPlay"
#endif

#ifndef XLOG_LEVEL
#define XLOG_LEVEL XLOG_LEVEL_DEBUG
#endif

#ifdef ANDROID
#include <android/log.h>

#if XLOG_LEVEL >= XLOG_LEVEL_DEBUG
#define XLOGD(...) __android_log_print(ANDROID_LOG_DEBUG,XLOG_TAG,__VA_ARGS__)
#else
#define XLOGD(...)
#endif

#if XLOG_LEVEL >= XLOG_LEVEL_INFO
#define XLOGI(...) __android_log_print(ANDROID_LOG_INFO,XLOG_TAG,__VA_ARGS__)
#else
#define XLOGI(...)
#endif

#if XLOG_LEVEL >= XLOG_LEVEL_ERROR
#define XLOGE(...) __android_log_print(ANDROID_LOG_ERROR,XLOG_TAG,__VA_ARGS__)
#else
#define XLOGE(...)
#endif

#else
//在其他的系统上面也能打印,比如ios
#include <stdio.h>

#ifndef XLOG_DETAIL
#define PRINT(format,...) printf("%s: "format,XLOG_TAG,##__VA_ARGS__);
#else
#define PRINT(format,...) printf("%s %s: [%s,%s,%d]: %s: " format,__DATE__,__TIME__,__FILE__,__FUNCTION__,__LINE__,XLOG_TAG,##__VA_ARGS__);
#endif

#if XLOG_LEVEL >= XLOG_LEVEL_DEBUG
#define XLOGD(format,...) PRINT(format,##__VA_ARGS__)
#else
#define XLOGD(format,...)
#endif

#if XLOG_LEVEL >= XLOG_LEVEL_INFO
#define XLOGI(format,...) PRINT(format,##__VA_ARGS__)
#else
#define XLOGI(format,...)
#endif

#if XLOG_LEVEL >= XLOG_LEVEL_ERROR
#define XLOGE(format,...) PRINT(format,##__VA_ARGS__)
#else
#define XLOGE(format,...)
#endif

//#define XLOGD(format, ...) printf(format,##__VA_ARGS__)
//#define XLOGI(format, ...) printf(format,##__VA_ARGS__)
//#define XLOGE(format, ...) printf(format,##__VA_ARGS__)

#endif


#endif //XPLAYER_XLOG_H
