

#ifndef LUADROID_BOOTDEXRESOLVER_H
#define LUADROID_BOOTDEXRESOLVER_H

#include <jni.h>
namespace DexResolver{
    void init();
    jobjectArray getAllBootClasses(JNIEnv *env,jclass);
}
#endif //LUADROID_BOOTDEXRESOLVER_H
