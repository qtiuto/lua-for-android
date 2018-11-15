//
// Created by Karven on 2018/8/2.
//

#ifndef LUADROID_LOGGERWRAPPER_H
#define LUADROID_LOGGERWRAPPER_H

#include <jni.h>

typedef void(*LoggerCallback)(JNIEnv* env,const char *, bool,void* arg);
typedef void(*Destroyer)(void*);
void requireLogger(LoggerCallback callback, void* arg= nullptr,Destroyer destroyer= nullptr);

void dropLogger();

#endif //LUADROID_LOGGERWRAPPER_H
