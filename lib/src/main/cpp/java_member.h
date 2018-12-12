
#ifndef LUADROID_JAVA_METHOD_H
#define LUADROID_JAVA_METHOD_H

#include "java_type.h"

#define FLAG_INDEX lua_upvalueindex(1)
#define OBJ_INDEX lua_upvalueindex(2)
struct Member;
struct MemberFlag {
    const Member* member;
    ThreadContext* context;
    bool isStatic;
    union {
        bool isField;
        bool isNotOnlyMethod;
    };
    bool isDuplicatedField;
};
#endif //LUADROID_JAVA_METHOD_H
