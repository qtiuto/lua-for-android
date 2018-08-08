
#ifndef LUADROID_JAVA_METHOD_H
#define LUADROID_JAVA_METHOD_H

#include "java_type.h"

#define FLAG_INDEX lua_upvalueindex(1)
#define OBJ_INDEX lua_upvalueindex(2)
#define NAME_INDEX lua_upvalueindex(3)
struct MemberFlag {
    bool isStatic;
    union {
        bool isField;
        bool isNotOnlyMethod;
    };
    bool isDuplicatedField;
};
#endif //LUADROID_JAVA_METHOD_H
