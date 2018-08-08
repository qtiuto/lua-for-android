

#ifndef LUADROID_AUTOJNIENV_H
#define LUADROID_AUTOJNIENV_H

#include <jni.h>
#include "common.h"
#include "TJNIEnv.h"

class AutoJNIEnv {
    TJNIEnv *env;
public:
    AutoJNIEnv() {
        int status = vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_4);
        if (status != JNI_OK) abort();
    }

    AutoJNIEnv(const AutoJNIEnv &) = delete;

    AutoJNIEnv(AutoJNIEnv &&other) : env(other.env) {
        other.env = nullptr;
    }

    TJNIEnv *operator->() { return env; }

    operator TJNIEnv *() { return env; }

    AutoJNIEnv &operator=(const AutoJNIEnv &other)= delete;
};

#endif //LUADROID_AUTOJNIENV_H
