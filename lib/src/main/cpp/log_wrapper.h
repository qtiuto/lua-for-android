//
// Created by Karven on 2018/8/2.
//

#ifndef LUADROID_LOGGERWRAPPER_H
#define LUADROID_LOGGERWRAPPER_H

#include <jni.h>
#include <stdlib.h>

typedef void(*LoggerCallback)(JNIEnv* env,const char *, bool,void* arg);
typedef void(*Destroyer)(void*);
uint requireLogger(LoggerCallback callback, void *arg, Destroyer destroyer);

void dropLogger(unsigned int id);

#endif //LUADROID_LOGGERWRAPPER_H
