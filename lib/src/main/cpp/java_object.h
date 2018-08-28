//
// Created by Karven on 2018/1/27.
//

#ifndef LUADROID_JAVA_OBJECT_H
#define LUADROID_JAVA_OBJECT_H

#include "common.h"
#include "AutoJNIEnv.h"

class JavaType;

struct JavaObject {
    jobject object;
    JavaType *type;

    static int objectGc(lua_State *L) {//gc function may run in other thread so we set a upvalue
        JavaObject *ref = (JavaObject *) lua_touserdata(L, -1);
        AutoJNIEnv()->DeleteGlobalRef(ref->object);
        return 0;
    }
};

#endif //LUADROID_JAVA_OBJECT_H
