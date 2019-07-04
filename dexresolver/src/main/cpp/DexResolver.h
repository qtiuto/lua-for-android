

#ifndef LUADROID_DEXRESOLVER_H
#define LUADROID_DEXRESOLVER_H

#include <jni.h>
namespace DexResolver{
    void init();
    jobjectArray getAllBootClasses(JNIEnv *env,jclass);
    jobjectArray getClassList(JNIEnv *env, jclass, jobject cookie);
}
#endif //LUADROID_DEXRESOLVER_H
