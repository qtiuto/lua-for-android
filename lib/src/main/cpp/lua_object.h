//
// Created by Karven on 2018/1/27.
//

#ifndef LUADROID_LUAOBJECT_H
#define LUADROID_LUAOBJECT_H

#include <cstdint>
#include <jni.h>
#include "lua.h"
#include "java_object.h"
#include "base_func.h"
#include "Vector.h"

enum LUA_TYPE {
    T_NIL,
    T_BOOLEAN,
    T_INTEGER,
    T_FLOAT,
    T_STRING,
    T_OBJECT,
    T_FUNCTION,
    T_TABLE,
    T_CHAR//For conversion use only
};
enum EXTRA_LUA_TYPE{
    T_LIGHT_USER_DATA=T_TABLE+1,
    T_USER_DATA,
    T_JAVA_TYPE
};

class FuncInfo;

class LazyTable;

struct ValidLuaObject {
    LUA_TYPE type:16;
    bool shouldRelease:16;

    union {
        uint8_t isTrue;
        char16_t character;
        int64_t integer=0;
        lua_Number number;
        const char *string;
        JavaObject *objectRef;
        BaseFunction *func;
        LazyTable *lazyTable;
        void *userdata;
    };

    ValidLuaObject():type(T_NIL),shouldRelease(false){}

    ValidLuaObject(ValidLuaObject &&object) : type(object.type),shouldRelease(object.shouldRelease), integer(object.integer) {
        object.type = T_NIL;
    }

    ValidLuaObject(const ValidLuaObject &) = delete;

    ValidLuaObject &operator=(ValidLuaObject &&other) {
        if(unlikely(type!=T_NIL))ValidLuaObject::~ValidLuaObject();
        type = other.type;
        shouldRelease=other.shouldRelease;
        integer = other.integer;
        other.type = T_NIL;
        return *this;
    }

    const char *typeName() const {
        switch (type) {
            case T_NIL:
                return "nil";
            case T_BOOLEAN:
                return "boolean";
            case T_INTEGER:
                return "integer";
            case T_FLOAT:
                return "float";
            case T_OBJECT:
                return "object";
            case T_STRING:
                return "string";
            case T_FUNCTION:
                return "function";
            case T_TABLE:
                return "table";
            case T_CHAR:
                return "char";
        }
        return "";
    }

    inline ~ValidLuaObject();
};


template<typename Tp>
class LuaTable {
    using Table= Vector<std::pair<Tp, Tp>> ;
    Table table;
    bool isDeleting= false;
public:
    LuaTable *metaTable = nullptr;

    Table &get() {
        return table;
    }

    void free(){
        if(isDeleting)
            return;
        isDeleting = true;
        delete this;
    }

    ~LuaTable(){
        LuaTable* m=metaTable;
        metaTable= nullptr;
        Table(std::move(table));
        if(m) m->free();

    }
};

inline void nop(){}

class ThreadContext;

class LazyTable {
    const int index;
    lua_State *const L;
    LuaTable<ValidLuaObject> *table = nullptr;
public:
    LazyTable(int index, lua_State *L) : index(index > 0 ? index : lua_gettop(L) + index + 1),
                                         L(L) {}

    LuaTable<ValidLuaObject> *getTable(ThreadContext *context);

    jobject asInterface(ThreadContext *context, JavaType *type);

    bool isInterface() {
        lua_pushnil(L);
        while (lua_next(L, index)) {
            if (!lua_isstring(L, -2) || (!lua_isfunction(L, -1) && !lua_istable(L, -1))) {
                lua_pop(L, 2);
                return false;
            }
            lua_pop(L, 1);
        }
        return true;
    }

    ~LazyTable() {
        table?table->free(): nop();
    }
};



ValidLuaObject::~ValidLuaObject() {
    if (type == T_FUNCTION){
        delete func;
    } else if (type == T_TABLE){
        delete lazyTable;
    }
}
class UserData;
struct CrossThreadLuaObject {
    int type = T_NIL;
    union {
        uint8_t isTrue;
        lua_Integer integer = 0;
        lua_Number number;
        const char *string;
        JavaObject *objectRef;
        FuncInfo *func;
        LuaTable<CrossThreadLuaObject> *table;
        void *lightData;
        UserData *userData;
        JavaType* javaType;
    };

    CrossThreadLuaObject()= default;

    CrossThreadLuaObject(const CrossThreadLuaObject &luaObject) = delete;

    CrossThreadLuaObject &operator=(const CrossThreadLuaObject &object)= delete;

    CrossThreadLuaObject(CrossThreadLuaObject &&luaObject) : type(luaObject.type),
                                                             integer(luaObject.integer) {
        luaObject.type=T_NIL;
        luaObject.integer = 0;//clear
    }

    CrossThreadLuaObject &operator=(CrossThreadLuaObject &&object) {
        type = object.type;
        integer = object.integer;
        object.type=T_NIL;
        object.integer = 0;//clear holder;
        return *this;
    }

    ~CrossThreadLuaObject();
};

class UserData {

    bool isDeleting= false;
public:
    Array<char, size_t> data;
    LuaTable<CrossThreadLuaObject> *metaTable = nullptr;

    UserData(size_t len) : data(len) {

    }
    void free(){
        if(isDeleting)
            return;
        isDeleting = true;
        delete this;
    }
    ~UserData() {
        auto m=metaTable;
        metaTable= nullptr;
        if(m)m->free();
    }
};

inline CrossThreadLuaObject::~CrossThreadLuaObject() {
    if (type == T_STRING) {
        delete[] string;
    } else if (type == T_OBJECT) {
        AutoJNIEnv()->DeleteGlobalRef(objectRef->object);
        delete objectRef;
    } else if (type == T_TABLE) {
        table->free();
    } else if (type == T_USER_DATA) {
        userData->free();
    }
}
#endif //LUADROID_LUAOBJECT_H
