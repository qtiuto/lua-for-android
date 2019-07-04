//
// Created by Karven on 2018/8/24.
//

#ifndef LUADROID_MACROS_H
#define LUADROID_MACROS_H

#include <android/log.h>
#ifdef __cplusplus
#endif

#define likely(x)    __builtin_expect(!!(x),1)
#define unlikely(x) __builtin_expect((x),0)
#ifdef NDEBUG
#define LOGE(msg, ...)
#define LOGV(msg, ...)
#define LOGI(msg, ...)
#define LOGW(msg, ...)
#else
#define LOGE(msg, ...) __android_log_print(ANDROID_LOG_ERROR,"Lua",msg,##__VA_ARGS__)
#define LOGV(msg, ...) __android_log_print(ANDROID_LOG_VERBOSE,"Lua",msg,##__VA_ARGS__)
#define LOGI(msg, ...) __android_log_print(ANDROID_LOG_INFO,"Lua",msg,##__VA_ARGS__)
#define LOGW(msg, ...) __android_log_print(ANDROID_LOG_WARN,"Lua",msg,##__VA_ARGS__)
#endif

#endif //LUADROID_MACROS_H
