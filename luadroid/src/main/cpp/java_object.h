//
// Created by Karven on 2018/1/27.
//

#ifndef LUADROID_JAVA_OBJECT_H
#define LUADROID_JAVA_OBJECT_H

#include "common.h"
#include "AutoJNIEnv.h"

#define JAVA_OBJECT "java_object"
#define JAVA_RETHROW "java_rethrow"

class JavaType;

struct JavaObject {
    jobject object;
    JavaType *type;

    static int objectGc(lua_State *L) {
        JavaObject *ref = (JavaObject *) lua_touserdata(L, -1);
        AutoJNIEnv()->DeleteGlobalRef(ref->object);
        return 0;
    }
};

#endif //LUADROID_JAVA_OBJECT_H
