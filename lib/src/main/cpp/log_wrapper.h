//
// Created by Karven on 2018/8/2.
//

#ifndef LUADROID_LOGGERWRAPPER_H
#define LUADROID_LOGGERWRAPPER_H

#include <jni.h>
#include <stdlib.h>

typedef void(*LoggerCallback)(JNIEnv* env,const char *, bool,void* arg);
typedef void(*Destroyer)(void*);
intptr_t requireLogger(LoggerCallback callback, void *arg, Destroyer destroyer);

void dropLogger(intptr_t id);

#endif //LUADROID_LOGGERWRAPPER_H
