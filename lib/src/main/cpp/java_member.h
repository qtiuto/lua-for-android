
#ifndef LUADROID_JAVA_METHOD_H
#define LUADROID_JAVA_METHOD_H

#include "java_type.h"

#define FLAG_INDEX lua_upvalueindex(1)
struct Member;
struct MemberInfo {
    const Member* member;
    ThreadContext* context;
    union {
        JavaObject* object;
        JavaType* type;
    };
    bool isStatic;
    union {
        bool isField;
        bool isNotOnlyMethod;
    };
    bool isDuplicatedField;
};
#endif //LUADROID_JAVA_METHOD_H
