
#ifndef LUADROID_INT64_SUPPORT_H
#define LUADROID_INT64_SUPPORT_H

#include "lua.hpp"

class Integer64 {
public:
    static constexpr const char *LIB_NAME = "__int64";

    Integer64(int64_t val) : m_val(val) {}

    static void RegisterTo(lua_State *L) {
        if (!luaL_newmetatable(L, LIB_NAME))
            return;


        lua_pushstring(L, "__add");
        lua_pushcfunction(L, i64Add);
        lua_rawset(L, -3);

        lua_pushstring(L, "__sub");
        lua_pushcfunction(L, i64Sub);
        lua_rawset(L, -3);

        lua_pushstring(L, "__div");
        lua_pushcfunction(L, i64Div);
        lua_rawset(L, -3);

        lua_pushstring(L, "__mul");
        lua_pushcfunction(L, i64Mul);
        lua_rawset(L, -3);

        lua_pushstring(L, "__mod");
        lua_pushcfunction(L, i64Mod);
        lua_rawset(L, -3);

        lua_pushstring(L, "__eq");
        lua_pushcfunction(L, i64Eq);
        lua_rawset(L, -3);

        lua_pushstring(L, "__lt");
        lua_pushcfunction(L, i64Lt);
        lua_rawset(L, -3);

        lua_pushstring(L, "__le");
        lua_pushcfunction(L, i64Le);
        lua_rawset(L, -3);

        lua_pushstring(L, "__tostring");
        lua_pushcfunction(L, i64toString);
        lua_rawset(L, -3);
        const struct luaL_Reg i64Lib[] = {
                {"new", newI64},
                {NULL, NULL},
        };
#if LUA_VERSION_NUM >= 502
        lua_createtable(L, 0, 1);
        luaL_setfuncs(L, i64Lib, 0);
        lua_setfield(L, -1, "__index");
        luaL_requiref(L, "i64", luaGetJava,/*glb*/true);
#else
        luaL_register(L,"i64",i64Lib);
#endif
        lua_pop(L, 1);
    }

    static void pushLong(lua_State *L, int64_t val) {
        void *buf = lua_newuserdata(L, sizeof(Integer64));
        new(buf)Integer64(val);
        Integer64::setmetatable(L);
    }

private:
    static int newI64(lua_State *L) {
        Integer64 *tmp = (Integer64 *) lua_touserdata(L, 1);
        long val = tmp ? tmp->m_val : static_cast<long>(lua_tonumber(L, 1));
        void *buf = lua_newuserdata(L, sizeof(Integer64));
        new(buf)Integer64(val);
        Integer64::setmetatable(L);
        return 1;
    }


    static int luaGetJava(lua_State *L) {
        lua_newuserdata(L, 1);
        luaL_setmetatable(L, LIB_NAME);
        return 1;
    }

    static void setmetatable(lua_State *L) {
        luaL_getmetatable(L, LIB_NAME);
        lua_setmetatable(L, -2);
    }


#ifndef I64_RELA
#define I64_RELA(OP)\
    Integer64 *i64self  = (Integer64 *)lua_touserdata(L,1);\
    Integer64 *i64other = (Integer64 *)lua_touserdata(L,2);\
    lua_pushboolean(L,i64self->m_val OP i64other->m_val);\
    return 1;
#endif

    static int i64Le(lua_State *L) {
        I64_RELA(<=);
    }

    static int i64Lt(lua_State *L) {
        I64_RELA(<);
    }

    static int i64Eq(lua_State *L) {
        I64_RELA(==);
    }

#ifndef I64_MATH
#define I64_MATH(OP)\
    Integer64 *i64self  = (Integer64 *)lua_touserdata(L,1);\
    Integer64 *i64other = (Integer64 *)lua_touserdata(L,2);\
    Integer64 tmp(0);\
    if(!i64other)\
    {\
        tmp.m_val = lua_tonumber(L,2);\
        i64other = &tmp;\
    }\
    if(!i64self)\
    {\
        long num = lua_tonumber(L,1);\
        size_t nbytes = sizeof(Integer64);\
        i64self = (Integer64*)lua_newuserdata(L, nbytes);\
        new(i64self)Integer64(num);\
        setmetatable(L);\
        i64self->m_val OP##= i64other->m_val;\
    }else\
    {\
        i64self->m_val OP##= i64other->m_val;\
        lua_pushvalue(L,1);\
    }\
    return 1;
#endif

    static int i64Mod(lua_State *L) {
        I64_MATH(%);
    }

    static int i64Div(lua_State *L) {
        I64_MATH(/);
    }

    static int i64Mul(lua_State *L) {
        I64_MATH(*);
    }

    static int i64Add(lua_State *L) {
        I64_MATH(+);
    }

    static int i64Sub(lua_State *L) {
        I64_MATH(-);
    }

    static int i64toString(lua_State *L) {
        Integer64 *i64self = (Integer64 *) lua_touserdata(L, 1);
        luaL_argcheck(L, i64self != NULL, 1, "userdata expected");
        char temp[64];
        sprintf(temp, "%lld", static_cast<long long int>(i64self->m_val));
        lua_pushstring(L, temp);
        return 1;
    }

public:
    int64_t m_val;
};

#endif //LUADROID_INT64_SUPPORT_H
