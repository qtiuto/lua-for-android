
#include "luadroid.h"
#include "SpinLock.h"
#include "myarray.h"
#include "java_type.h"
#include "utf8.h"
#include "java_member.h"
#include "lua_object.h"
#include "log_wrapper.h"
#include "lfs.h"
#include <unordered_map>
#include <algorithm>
#include <FakeVector.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <setjmp.h>
#include <lua.h>
#include <dex/DexResolver.h>

#if LUA_VERSION_NUM < 503
#include "int64_support.h"
#endif
#define TopErrorHandle(fmt, ...)  \
    ({lua_pushfstring(L,fmt,##__VA_ARGS__);\
    goto __ErrorHandle;})
#define SetErrorJMP()({if(false){\
__ErrorHandle:\
        lua_error(L);\
return 0;\
}})
#define luaL_isstring(L, i) (lua_type(L,i)==LUA_TSTRING)

static int javaType(lua_State *L)noexcept;

static int javaInstanceOf(lua_State *L)noexcept;

static int javaNew(lua_State *L)noexcept;

static int javaNewArray(lua_State *L)noexcept;

static int javaImport(lua_State *L)noexcept;

static int javaUsing(lua_State*L);

static int javaIterate(lua_State*L);

static int javaCharValue(lua_State *L)noexcept;

static int javaCharString(lua_State *L)noexcept;

static int javaToJavaObject(lua_State *L)noexcept;

static int javaProxy(lua_State *L)noexcept;

static int javaSync(lua_State *L)noexcept;

static int javaThrow(lua_State *L)noexcept;

static int javaTry(lua_State *L)noexcept;

static int javaUnBox(lua_State *L)noexcept;

static int javaPut(lua_State *L)noexcept;

static int javaGet(lua_State *L)noexcept;

static int concatString(lua_State *L)noexcept;

static int objectEquals(lua_State *L)noexcept;

static int javaTypeToString(lua_State *L) noexcept;

static int javaObjectToString(lua_State *L)noexcept;

static int getFieldOrMethod(lua_State *L)noexcept;

static int setFieldOrArray(lua_State *L)noexcept;

static int setField(lua_State *L)noexcept;

static int getField(lua_State *L)noexcept;

static int callMethod(lua_State *L)noexcept;

static ThreadContext *getContext(lua_State *L);

static FuncInfo *saveLuaFunction(lua_State *L, ThreadContext *context, int funcIdx);

static int newArray(lua_State *L, int start, ThreadContext *context, JavaType *type);

static bool parseLuaObject(lua_State *L, ThreadContext *context, int idx,
                           ValidLuaObject &luaObject);

static bool parseCrossThreadLuaObject(lua_State *L, ThreadContext *infcontext, int idx,
                                      CrossThreadLuaObject &luaObject);

static bool pushLuaObject(TJNIEnv *env, lua_State *L, ScriptContext *context,
                          const CrossThreadLuaObject &luaObject);

static inline void pushJavaType(lua_State *L,JavaType* type);

static void
loadLuaFunction(TJNIEnv *env, lua_State *L, const FuncInfo *info, ScriptContext *context);

static void checkLuaType(TJNIEnv *env, lua_State *L, JavaType *expected, ValidLuaObject &luaObject);

extern bool changeClassName(String &className);

static void pushMember(ThreadContext *context, lua_State *L, bool isStatic, int fieldCount, bool isMethod);

static void
pushArrayElement(lua_State *L, ThreadContext *context, const JavaObject *obj,
                 JavaType *component);

static void
readArguments(lua_State *L, ThreadContext *context, FakeVector<JavaType *> &types, FakeVector<ValidLuaObject> &objects,
              int start, int end);

static void parseTryTable(lua_State *L);

static void recordLuaError(ThreadContext *context, lua_State *L, int ret);

static int getObjectLength(lua_State *L);

static LocalFunctionInfo *saveLocalFunction(lua_State *L, int i);

static void pushMetaTable(TJNIEnv *env, lua_State *L, ScriptContext *context,
                          LuaTable<CrossThreadLuaObject> *metaTable);

static bool
readProxyMethods(lua_State *L, ThreadContext *context, Vector<JavaType *> &interfaces,
                 JavaType *main, Vector<std::unique_ptr<BaseFunction>> &luaFuncs,
                 Vector<JObject> &agentMethods);

extern "C" {
jlong compile(TJNIEnv *env, jclass thisClass, jlong ptr, jstring script, jboolean isFile);
jlong nativeOpen(TJNIEnv *env, jobject object, jboolean importAll);
void registerLogger(TJNIEnv *, jclass, jlong ptr, jobject out, jobject err);
void nativeClose(JNIEnv *env, jclass thisClass, jlong ptr);
void nativeClean(JNIEnv *env, jclass thisClass, jlong ptr);
void releaseFunc(JNIEnv *env, jclass thisClass, jlong ptr);
jint getClassType(TJNIEnv * env, jclass, jlong ptr,jclass clz);
void addJavaObject(TJNIEnv *env, jclass thisClass, jlong ptr, jstring _name, jobject obj,
                   jboolean local);
void addJavaMethod(TJNIEnv *env, jclass thisClass, jlong ptr, jstring jname, jstring method,
                   jobject jinst, jboolean local);
jobjectArray runScript(TJNIEnv *env, jclass thisClass, jlong ptr, jobject script,
                       jboolean isFile,
                       jobjectArray args);
jobject constructChild(TJNIEnv *env, jclass thisClass, jlong ptr, jclass target,
                       jlong nativeInfo);
jobject invokeLuaFunction(TJNIEnv *env, jclass thisClass, jlong ptr,
                          jboolean isInterface,
                          jlong funcRef, jobject proxy, jintArray argTypes,
                          jobjectArray args);
}

static const luaL_Reg javaInterfaces[] =
        {{"type",       javaType},
         {"instanceof", javaInstanceOf},
         {"new",        javaNew},
         {"newArray",   javaNewArray},
         {"import",     javaImport},
         {"using",javaUsing},
         {"proxy",      javaProxy},
         {"sync",       javaSync},
         {"object",     javaToJavaObject},
         {"charString",
                        javaCharString},
         {"charValue",  javaCharValue},
         {"throw",      javaThrow},
         {"try",        javaTry},
         {"unbox",      javaUnBox},
         {nullptr,      nullptr}};

static const JNINativeMethod nativeMethods[] =
        {{"nativeOpen",        "(ZZ)J",                            (void *) nativeOpen},
         {"nativeClose",       "(J)V",                             (void *) nativeClose},
         {"nativeClean",       "(J)V",                             (void *) nativeClean},
         {"registerLogger",    "(JLjava/io/OutputStream;"
                                       "Ljava/io/OutputStream;)V", (void *) registerLogger},
         {"compile",           "(JLjava/lang/String;Z)J",          (void *) compile},
         {"runScript",         "(JLjava/lang/Object;Z"
                                       "[Ljava/lang/Object;"
                                       ")[Ljava/lang/Object;",     (void *) runScript},
         {"addObject",         "(JLjava/lang/String;"
                                       "Ljava/lang/Object;Z)V",    (void *) addJavaObject},
         {"addMethod",         "(JLjava/lang/String;"
                                       "Ljava/lang/String;"
                                       "Ljava/lang/Object;Z)V",    (void *) addJavaMethod},
         {"constructChild",    "(JLjava/lang/Class;J)"
                                       "Ljava/lang/Object;",       (void *) constructChild},
         {"releaseFunc",       "(J)V",                             (void *) releaseFunc},
         {"getClassType",      "(JLjava/lang/Class;)I",            (void *) getClassType},
         {"invokeLuaFunction", "(JZJLjava/lang/Object;"
                                       "[I[Ljava/lang/Object;)"
                                       "Ljava/lang/Object;", (void *) invokeLuaFunction}};

JavaVM *vm;
jclass stringType;
jclass classType;
jclass throwableType;
jclass contextClass;
jclass loaderClass;
jmethodID objectHash;
jmethodID classGetName;
jmethodID objectToString;
static __thread jmp_buf errorJmp;

#define JAVA_OBJECT "java_object"
#define JAVA_TYPE "java_type"
#define JAVA_CONTEXT "java_context"
class RegisterKey{};
static const RegisterKey* OBJECT_KEY= reinterpret_cast<const RegisterKey *>(javaInterfaces);
static const RegisterKey* TYPE_KEY=OBJECT_KEY+1;


#if  LUA_VERSION_NUM == 502
static inline int64_t lua_tointegerx(lua_State* L,int index,int* isnum){
    double num=lua_tonumberx(L,index,isnum);
    int64_t ret=(int64_t)num;
    *isnum=*isnum&&ret==num;
    return ret;

}
#endif
#if LUA_VERSION_NUM < 502
static void luaL_requiref(lua_State *L, const char *modname,
                              lua_CFunction openf, int glb) {
    luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE);
    lua_getfield(L, -1, modname);  /* LOADED[modname] */
    if (!lua_toboolean(L, -1)) {  /* package not already loaded? */
        lua_pop(L, 1);  /* remove field */
        lua_pushcfunction(L, openf);
        lua_pushstring(L, modname);  /* argument to open function */
        lua_call(L, 1, 1);  /* call 'openf' to open module */
        lua_pushvalue(L, -1);  /* make copy of module (call result) */
        lua_setfield(L, -3, modname);  /* LOADED[modname] = module */
    }
    lua_remove(L, -2);  /* remove LOADED table */
    if (glb) {
        lua_pushvalue(L, -1);  /* copy of module */
        lua_setglobal(L, modname);  /* _G[modname] = module */
    }
}

static void luaL_setfuncs(lua_State *L, const luaL_Reg *l, int nup) {
    luaL_checkstack(L, nup, "too many upvalues");
    for (; l->name != NULL; l++) {  /* fill the table with given functions */
        int i;
        for (i = 0; i < nup; i++)  /* copy upvalues to the top */
            lua_pushvalue(L, -nup);
        lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
        lua_setfield(L, -(nup + 2), l->name);
    }
    lua_pop(L, nup);  /* remove upvalues */
}

static inline int lua_rawgetp(lua_State* L,int index,const void* p){
    lua_pushlightuserdata(L,(void*)p);
    lua_rawget(L,index);
    return 0;
}
static inline void lua_rawsetp(lua_State* L,int index,const void* p){
    lua_pushlightuserdata(L,(void*)p);
    lua_pushvalue(L,-2);
    lua_rawset(L,index);
    lua_pop(L,1);
}
static inline size_t lua_rawlen(lua_State*L,int index){
    return lua_objlen(L,index);
}

#if LUAJIT_VERSION_NUM<20100 //beta3 only

static inline int64_t lua_tointegerx(lua_State* L,int index,int* isnum){
    double num=lua_tonumber(L,index);
    int64_t ret=(int64_t)num;
    *isnum=lua_isnumber(L,index)&&ret==num;
    return ret;
}
#endif

static const char *luaL_tolstring (lua_State *L, int idx, size_t *len) {
    if (luaL_callmeta(L, idx, "__tostring")) {  /* metafield? */
        if (!lua_isstring(L, -1))
            luaL_error(L, "'__tostring' must return a string");
    }
    else {
        switch (lua_type(L, idx)) {
            case LUA_TNUMBER: {
                double floatVal=lua_tonumber(L, idx);
                int64_t integer= int64_t(floatVal);
                if (floatVal==integer)
                    lua_pushfstring(L, "%I",integer);
                else
                    lua_pushfstring(L, "%f", floatVal);
                break;
            }
            case LUA_TSTRING:
                lua_pushvalue(L, idx);
                break;
            case LUA_TBOOLEAN:
                lua_pushstring(L, (lua_toboolean(L, idx) ? "true" : "false"));
                break;
            case LUA_TNIL:
                lua_pushliteral(L, "nil");
                break;
            default: {
                int tt = luaL_getmetafield(L, idx, "__name");  /* try name */
                const char *kind = (tt == LUA_TSTRING) ? lua_tostring(L, -1) :
                                   luaL_typename(L, idx);
                lua_pushfstring(L, "%s: %p", kind, lua_topointer(L, idx));
                if (tt != LUA_TNIL)
                    lua_remove(L, -2);  /* remove '__name' */
                break;
            }
        }
    }
    return lua_tolstring(L, -1, len);
}
#endif


static bool testType(lua_State *L, int objIndex, const RegisterKey *key) {
    if (!lua_getmetatable(L, objIndex)) return false;
    lua_rawgetp(L,LUA_REGISTRYINDEX, key);
    bool ret = lua_rawequal(L, -1, -2) != 0;
    lua_pop(L, 2);
    return ret;
}

static void* testUData(lua_State* L,int ud,const RegisterKey* key){
    void *p = lua_touserdata(L, ud);
    if (p != NULL) {  /* value is a userdata? */
        if (lua_getmetatable(L, ud)) {  /* does it have a metatable? */
            lua_rawgetp(L,LUA_REGISTRYINDEX, key);  /* get correct metatable */
            if (!lua_rawequal(L, -1, -2))  /* not the same? */
                p = NULL;  /* value is a userdata with wrong metatable */
            lua_pop(L, 2);  /* remove both metatables */
            return p;
        }
    }
    return NULL;  /* value is not a userdata with a metatable */
}
static bool isJavaTypeOrObject(lua_State*L,int index){
    return lua_rawlen(L,index)== sizeof(void *);
}

static JavaObject* checkJavaObject(lua_State* L,int idx){
    JavaObject* ret= static_cast<JavaObject *>(testUData(L, idx, OBJECT_KEY));
    if(unlikely(ret== nullptr))
        luaL_error(L,"Expected " JAVA_OBJECT ",but got %s",luaL_tolstring(L,idx,NULL));
    return ret;
}
static JavaType* checkJavaType(lua_State* L,int idx){
    JavaType** ret= static_cast<JavaType **>(testUData(L, idx, TYPE_KEY));
    if(unlikely(ret== nullptr))
        luaL_error(L,"Expected " JAVA_TYPE ",but got %s",luaL_tolstring(L,idx,NULL));
    return *ret;
}
static void setMetaTable(lua_State* L,const RegisterKey* key){
    lua_rawgetp(L,LUA_REGISTRYINDEX,key);
    lua_setmetatable(L,-2);
}

static bool newMetaTable(lua_State*L,const RegisterKey* key,const char* tname){
    lua_rawgetp(L,LUA_REGISTRYINDEX,key);
    if(!lua_isnil(L,-1)) return false;
    lua_pop(L,1);
    lua_createtable(L, 0, 2);  /* create metatable */
    if(tname){
        lua_pushstring(L, tname);
        lua_setfield(L, -2, "__name");  /* metatable.__name = tname */
    }
    lua_pushvalue(L, -1);
    lua_rawsetp(L, LUA_REGISTRYINDEX, key);
    return true;
}

static JavaObject *pushJavaObject(lua_State *L, TJNIEnv *env, ScriptContext *context, jobject obj, JavaType *given) {
#ifndef NDEBUG
    if(obj== nullptr){
        LOGE("Error put object");
        return nullptr;
    }
#endif
    JavaObject *objectRef = (JavaObject *) lua_newuserdata(L, sizeof(JavaObject));
    objectRef->object = env->NewGlobalRef(obj);
    objectRef->type = given?given:context->ensureType(env, (JClass) env->GetObjectClass(obj));
    setMetaTable(L, OBJECT_KEY);
    return objectRef;
}
static inline  JavaObject *pushJavaObject(lua_State *L, ThreadContext *context, jobject obj){
    return pushJavaObject(L, context->env, context->scriptContext, obj, nullptr);
}

#include <sstream>

static String traceback(lua_State *L, int level) {
    lua_Debug ar;
    std::stringstream ret;
    ret<< "\nstack traceback:";
    bool isOk= false;
    while (lua_getstack(L, level++, &ar)) {
        lua_getinfo(L, "Slnt", &ar);
        if (ar.currentline > 0){
            ret<<"\n\tat line " <<ar.currentline/*<<':'*/;
            if (*ar.namewhat != '\0')  /* is there a name from code? */
                ret<<" in " << ar.namewhat<< ar.name;  /* use it */
            else if (*ar.what == 'm')  /* main? */
                ret<<" in main chunk";
            else if (*ar.what != 'C')  /* for Lua functions, use <file:line> */
                ret<<" in function <"<< ar.short_src<<':'<<ar.linedefined<<'>';
            if (ar.istailcall)
                ret<< "\n\t(...tail calls...)";
            isOk=true;
        }
    }
    if(isOk)
    return ret.str();
    return String();
}
static void pushJavaException(lua_State*L,ThreadContext* context){
    context->setPendingException(traceback(L,0));
    pushJavaObject(L,context,JObject(context->env,context->transferJavaError()));
}

inline void throwJavaError(lua_State *L, ThreadContext *context) {
    pushJavaException(L,context);
    lua_error(L);
}

static inline bool isThrowableObject(lua_State *L,ThreadContext* context){
    return testUData(L,-1,OBJECT_KEY)&& static_cast<JavaObject*>(
            lua_touserdata(L,-1))->type->isThrowable(context->env);
}

static int luaPCallHandler(lua_State *L) {
    ThreadContext* context=getContext(L);
    if(isThrowableObject(L,context))
        return 1;
    if(context->hasErrorPending()){
        pushJavaException(L,context);
        return 1;
    }
    const char *s = luaL_tolstring(L, -1, nullptr);
    luaL_traceback(L, L, s, 1);
    return 1;
}

static inline void pushErrorHandler(lua_State *L,ThreadContext* context){
    lua_pushlightuserdata(L,context);
    lua_pushcclosure(L,luaPCallHandler,1);
}

static int luaFullGC(lua_State *L) {
    lua_gc(L, LUA_GCCOLLECT, 0);
    return 0;
}

static int luaGetJava(lua_State *L) {
    int size = sizeof(javaInterfaces) / sizeof(luaL_Reg) - 1;
    lua_createtable(L, 0, size);
    lua_getfield(L,LUA_REGISTRYINDEX,JAVA_CONTEXT);
    luaL_setfuncs(L, javaInterfaces, 1);
    return 1;
}

static int luaPanic(lua_State *L) {
    const char *s = lua_tostring(L, -1);
    if (s) {
        luaL_traceback(L, L, s, 1);
        s = lua_tostring(L, -1);
    } else s = "Unexpected lua error";
    lua_getfield(L,LUA_REGISTRYINDEX,JAVA_CONTEXT);
    ThreadContext* context= static_cast<ThreadContext *>(lua_touserdata(L, -1));
    lua_pop(L,1);
    context->setPendingException(s);
    _longjmp(errorJmp, -1);
    return 0;
}


void ScriptContext::config(lua_State *L) {
    lua_atpanic(L, luaPanic);
    ThreadContext* context=getThreadContext();
    if(!context->env) context->env=AutoJNIEnv();
    lua_pushlightuserdata(L, context);//for panic and lib init
    lua_setfield(L, LUA_REGISTRYINDEX, JAVA_CONTEXT);
    int top = lua_gettop(L);
    luaL_requiref(L, "java", luaGetJava,/*glb*/true);
#if LUA_VERSION_NUM < 503
    luaL_requiref(L, Integer64::LIB_NAME, Integer64::RegisterTo,/*glb*/true);
#endif
    if (importAll) {
        const luaL_Reg *l = javaInterfaces;
        for (; l->name != NULL; l++) {
            lua_pushlightuserdata(L,context);
            lua_pushcclosure(L, l->func,1);
            lua_setglobal(L, l->name);
        }
        lua_pushlightuserdata(L,context);
        lua_pushcclosure(L, javaType,1);
        lua_setglobal(L, "Type");
    }
    {
        *(ThreadContext**)lua_newuserdata(L, sizeof(void*))=context;
        lua_createtable(L,0,3);
        lua_pushstring(L, "Can't change java metatable");
        lua_setfield(L, -2, "__metatable");
        lua_pushcfunction(L, javaPut);
        lua_setfield(L, -2, "__newindex");
        lua_pushcfunction(L, javaGet);
        lua_setfield(L, -2, "__index");
        lua_setmetatable(L,-2);
        lua_setglobal(L,"cross");
    }

    if (newMetaTable(L,TYPE_KEY, JAVA_TYPE)) {
        int index = lua_gettop(L);
        lua_pushstring(L, "Can't change java metatable");
        lua_setfield(L, index, "__metatable");
        lua_pushlightuserdata(L,context);
        lua_pushcclosure(L, setFieldOrArray,1);
        lua_setfield(L, index, "__newindex");
        lua_pushlightuserdata(L,context);
        lua_pushcclosure(L, getFieldOrMethod,1);
        lua_setfield(L, index, "__index");
        lua_pushlightuserdata(L,context);
        lua_pushcclosure(L, javaNew,1);
        lua_setfield(L, index, "__call");
        lua_pushlightuserdata(L,context);
        lua_pushcclosure(L, javaTypeToString,1);
        lua_setfield(L, index, "__tostring");
    }
    if (newMetaTable(L,OBJECT_KEY, JAVA_OBJECT)) {
        int index = lua_gettop(L);
        lua_pushstring(L, "Can't change java metatable");
        lua_setfield(L, index, "__metatable");
        lua_pushlightuserdata(L,context);
        lua_pushcclosure(L, setFieldOrArray,1);
        lua_setfield(L, index, "__newindex");
        lua_pushlightuserdata(L,context);
        lua_pushcclosure(L, getFieldOrMethod,1);
        lua_setfield(L, index, "__index");
        lua_pushlightuserdata(L,context);
        lua_pushcclosure(L, objectEquals,1);
        lua_setfield(L, index, "__eq");
        lua_pushlightuserdata(L,context);
        lua_pushcclosure(L, javaObjectToString,1);
        lua_setfield(L, index, "__tostring");
        lua_pushlightuserdata(L,context);
        lua_pushcclosure(L, javaIterate,1);
        lua_setfield(L, index, "__pairs");
        lua_pushlightuserdata(L,context);
        lua_pushcclosure(L, getObjectLength,1);
        lua_setfield(L, index, "__len");
        //concat is simple
        lua_pushcfunction(L, concatString);
        lua_setfield(L, index, "__concat");
        //gc function may run in other thread so we don't set a upvalue
        lua_pushcfunction(L, JavaObject::objectGc);
        lua_setfield(L, index, "__gc");
    }
    for (auto &pair:addedMap) {
        pushAddedObject(context->env, L, pair.first.data(), pair.second.first.data(), pair.second.second);
    }
    pushJavaType(L,context->ensureType("com.oslorde.luadroid.ClassBuilder"));
    lua_pushvalue(L, -1);
    lua_setglobal(L, "ClassBuilder");
    lua_settop(L, top);

    luaL_openlibs(L);

    luaL_requiref(L,LFS_LIBNAME,luaopen_lfs, true);
}

static inline void pushJavaType(lua_State *L,JavaType* type){
    *((JavaType **) lua_newuserdata(L, sizeof(JavaType *))) = type;
    setMetaTable(L, TYPE_KEY);
}

static inline ThreadContext *getContext(lua_State *L) {
    return  (ThreadContext *) lua_touserdata(L, lua_upvalueindex(1));
}

static bool parseLuaObject(lua_State *L, ThreadContext *context, int idx,
                           ValidLuaObject &luaObject) {
    switch (lua_type(L, idx)) {
        case LUA_TNIL:
            luaObject.type = T_NIL;
            break;
        case LUA_TBOOLEAN: {
            luaObject.type = T_BOOLEAN;
            luaObject.isTrue = (jboolean) lua_toboolean(L, idx);
            break;
        }
        case LUA_TSTRING: {
            luaObject.type = T_STRING;
            luaObject.string = lua_tostring(L, idx);
            break;
        }
        case LUA_TFUNCTION: {
            luaObject.type = T_FUNCTION;
            if (context->isLocalFunction()) {
                luaObject.func = saveLocalFunction(L, idx);
            } else {
                luaObject.func = saveLuaFunction(L, context, idx);
            }
            break;
        }
        case LUA_TNUMBER: {
#if LUA_VERSION_NUM >= 503
            int isInt;
            lua_Integer intValue = lua_tointegerx(L, idx, &isInt);
            if (isInt) {
                luaObject.type = T_INTEGER;
                luaObject.integer = intValue;
            } else {
                luaObject.type = T_FLOAT;
                luaObject.number = lua_tonumber(L, idx);
            }
#else
            lua_Number number=lua_tonumber(L,idx);
            int64_t intVal=(int64_t)number;
            if(number==intVal){
                luaObject.type=T_INTEGER;
                luaObject.integer=intVal;
            } else{
                luaObject.type=T_FLOAT;
                luaObject.number=number;
            }
#endif
            break;
        }
        case LUA_TLIGHTUSERDATA: {
            luaObject.type = T_INTEGER;
            luaObject.userdata = lua_touserdata(L, idx);
            break;
        }
        case LUA_TUSERDATA:
            if (testUData(L, idx, OBJECT_KEY)) {
                luaObject.type = T_OBJECT;
                luaObject.objectRef = (JavaObject *) lua_touserdata(L, idx);
            }
#if LUA_VERSION_NUM < 503
        else if(luaL_testudata(L,idx,Integer64::LIB_NAME)){
            luaObject.type=T_INTEGER;
            luaObject.integer=((Integer64*)lua_touserdata(L,idx))->m_val;
        }
#endif
            break;
        case LUA_TTABLE: {
            luaObject.type = T_TABLE;
            luaObject.lazyTable = new LazyTable(idx, L);
            break;
        }
        default:
            return false;
    }
    return true;
}

static bool
checkLuaTypeNoThrow(TJNIEnv *env, lua_State *L, JavaType *expected, ValidLuaObject &luaObject) {
    if (expected == nullptr) return false;
    if (expected->isInteger()) {
        if (luaObject.type != T_INTEGER){
            if(luaObject.type==T_FLOAT){
                luaObject.type=T_INTEGER;
                luaObject.integer= int64_t(luaObject.number);
            } else if(luaObject.type==T_OBJECT){
                JavaType* type=luaObject.objectRef->type;
                if(!expected->canAcceptBoxedNumber(type)) goto bail;
                luaObject.type=T_INTEGER;
                luaObject.integer=env->CallLongMethod(luaObject.objectRef->object,longValue);
            } else goto bail;
        }
    } else if (expected->isFloat()) {
        if (luaObject.type != T_FLOAT){
            if(luaObject.type==T_INTEGER){
                luaObject.type=T_FLOAT;
                luaObject.number=luaObject.integer;
            } else if(luaObject.type==T_OBJECT){
                JavaType* type=luaObject.objectRef->type;
                if(!expected->canAcceptBoxedNumber(type)) goto bail;
                luaObject.type=T_FLOAT;
                luaObject.number=env->CallDoubleMethod(luaObject.objectRef->object,doubleValue);
            } else goto bail;
        }
    } else if (expected->isChar()) {
        if (luaObject.type == T_STRING){
            if (strlen8to16(luaObject.string) != 1) {
                lua_pushstring(L, "String length for char type must be 1");
                return true;
            } else{
                luaObject.type=T_CHAR;
                strcpy8to16(&luaObject.character,luaObject.string,NULL);
            }
        } else if(luaObject.type==T_INTEGER){
            if(luaObject.integer<0||luaObject.integer>65535){
                lua_pushstring(L, "The integer is too large for char value");
                return true;
            }else luaObject.type=T_CHAR;
        } else if(luaObject.type==T_OBJECT&&luaObject.objectRef->type->isBoxedChar()){
            luaObject.type=T_CHAR;
            luaObject.character=env->CallCharMethod(luaObject.objectRef->object,charValue);
            return false;
        } else goto bail;

    } else if (expected->isBool()) {
        if (luaObject.type != T_BOOLEAN){
            if(luaObject.type==T_OBJECT&&luaObject.objectRef->type->isBoxedChar()){
                luaObject.type=T_BOOLEAN;
                luaObject.isTrue=env->CallBooleanMethod(luaObject.objectRef->object,booleanValue);
                return false;
            } else goto bail;
        }
    } else {
        if (luaObject.type == T_NIL) return false;
        if (luaObject.type == T_OBJECT) {
            if (luaObject.objectRef->type == expected ||
                env->IsInstanceOf(luaObject.objectRef->object, expected->getType()))
                return false;
            else {
                lua_pushfstring(L, "Incompatible object,expected:%s,received:%s",
                                expected->name(env).str(), luaObject.objectRef->type->name(env).str());
                return true;
            }
        }
        if (expected->isStringAssignable(env) && luaObject.type == T_STRING) return false;
        if (luaObject.type == T_FUNCTION) {
            if (expected->isSingleInterface(env))return false;
            forceRelease(luaObject);
            lua_pushstring(L, "Can't convert function to a class that is not a single interface");
            return true;
        }
        if (luaObject.type == T_TABLE) {
            if (expected->isTableType(env) ||
                (expected->isInterface(env) && luaObject.lazyTable->isInterface()))
                return false;
        }
        if(luaObject.type==T_FLOAT){
            if(expected->isBoxedFloat()){
                return false;
            }
        } else if(luaObject.type==T_INTEGER){
            if(expected->isBoxedInteger()||expected->isBoxedFloat())
                return false;
        }
        if(expected->isBoxedChar()&&luaObject.type == T_STRING)
            return false;
        if(expected->isBoxedBool()&&luaObject.type == T_BOOLEAN)
            return false;
        goto bail;
    }
    return false;
    bail:
    lua_pushfstring(L, "Expected type is %s,but receive %s",
                    JString(env->CallObjectMethod(expected->getType(), classGetName)).str(),
                    luaObject.typeName());
    return true;
}

void checkLuaType(TJNIEnv *env, lua_State *L, JavaType *expected, ValidLuaObject &luaObject) {
    if (likely(expected == nullptr)) return;
    if (checkLuaTypeNoThrow(env, L, expected, luaObject)) {
        forceRelease(luaObject);
        lua_error(L);
    }
}

void readArguments(lua_State *L, ThreadContext *context, FakeVector<JavaType *> &types, FakeVector<ValidLuaObject> &objects,
                   int start, int end) {
    for (int i = start; i <= end; ++i) {
        JavaType **typeRef = (JavaType **) testUData(L, i, TYPE_KEY);
        bool noType = typeRef == nullptr;
        JavaType *paramType = noType ? nullptr : *typeRef;
        types.asVector().push_back(paramType);
        if (!noType) i++;
        ValidLuaObject luaObject;
        if (!parseLuaObject(L, context, i, luaObject)) {
            forceRelease(types, objects);
            luaL_error(L, "Arg unexpected for array");
        }
        if (checkLuaTypeNoThrow(context->env, L, paramType, luaObject)) {
            forceRelease(luaObject);
            objects.release();
            lua_error(L);
        }
        objects.asVector().push_back(std::move(luaObject));
    }
}


void pushArrayElement(lua_State *L, ThreadContext *context, const JavaObject *obj,
                      JavaType *componentType) {
    int isnum;
    jlong index = lua_tointegerx(L, 2, &isnum);
    if (unlikely(!isnum)) luaL_error(L, "Invalid index for an array");
    if (unlikely(index > INT32_MAX))luaL_error(L, "Index out of range:%d", index);
    auto  env=context->env;
    switch (componentType->getTypeID()){
#define PushChar(c) ({ \
        char s[4];\
        strncpy16to8(s,(const char16_t *) &c, 1);\
        lua_pushstring(L,s);\
        })
#define ArrayGet(typeID,jtype, jname, TYPE)\
            case JavaType::typeID:{\
                j##jtype buf;\
                env->Get##jname##ArrayRegion((j##jtype##Array) obj->object, (jsize) index, 1, &buf);\
                lua_push##TYPE(L,buf);\
            }
#define IntArrayGet(typeID,jtype, jname) ArrayGet(typeID,jtype,jname,integer)
#define FloatArrayGet(typeID,jtype, jname) ArrayGet(typeID,jtype,jname,number)
        IntArrayGet(BYTE,byte, Byte)
        IntArrayGet(SHORT,short, Short)
        IntArrayGet(INT,int, Int)
        IntArrayGet(LONG,long, Long)
        FloatArrayGet(FLOAT,float, Float)
        FloatArrayGet(DOUBLE,double, Double)
        ArrayGet(BOOLEAN,boolean, Boolean, boolean)
        case JavaType::CHAR: {
            jchar buf;
            env->GetCharArrayRegion((jcharArray) obj->object, (jsize) index, 1, &buf);
            PushChar(buf);
        }
        default: {
            auto ele = env->GetObjectArrayElement((jobjectArray) obj->object, (jsize) index);
            if(ele== nullptr)
                lua_pushnil(L);
            else pushJavaObject(L, context, ele);
        }
    }
    HOLD_JAVA_EXCEPTION(context,{throwJavaError(L,context);});
}

static void pushRawMethod(ThreadContext *context,lua_State *L, bool isStatic) {
    MemberFlag *member = (MemberFlag *) lua_newuserdata(L, sizeof(MemberFlag));
    member->isStatic = isStatic;
    member->isNotOnlyMethod = false;
    member->isField = false;
    member->isDuplicatedField = false;
    member->context=context;
    lua_pushvalue(L, -3);//obj
    lua_pushvalue(L, -3);//name
    lua_pushcclosure(L, callMethod, 3);
}

void pushMember(ThreadContext *context, lua_State *L, bool isStatic, int fieldCount, bool isMethod) {
    if (fieldCount == 0 && isMethod) {
        pushRawMethod(context,L, isStatic);
        return;
    }
    lua_newuserdata(L, 0);//value represent the java member
    lua_createtable(L, 0, 3);//metatable;
    int tableIndex = lua_gettop(L);
    int nameIndex = tableIndex - 2;
    int tOIndex = tableIndex - 3;//class or the obj
    MemberFlag *member = (MemberFlag *) lua_newuserdata(L, sizeof(MemberFlag));
    member->isStatic = isStatic;
    member->isNotOnlyMethod = true;
    member->isField = true;
    member->isDuplicatedField = fieldCount > 1;
    int flagIndex = lua_gettop(L);
    if (fieldCount > 0) {
        lua_pushstring(L, "__index");
        lua_pushvalue(L, flagIndex);
        lua_pushvalue(L, tOIndex);
        lua_pushvalue(L, nameIndex);
        lua_pushcclosure(L, getField, 3);
        lua_rawset(L, tableIndex);

        lua_pushstring(L, "__newindex");
        lua_pushvalue(L, flagIndex);
        lua_pushvalue(L, tOIndex);
        lua_pushvalue(L, nameIndex);
        lua_pushcclosure(L, setField, 3);
        lua_rawset(L, tableIndex);
    }

    if (isMethod) {
        lua_pushstring(L, "__call");
        lua_pushvalue(L, flagIndex);
        lua_pushvalue(L, tOIndex);
        lua_pushvalue(L, nameIndex);
        lua_pushcclosure(L, callMethod, 3);
        lua_rawset(L, tableIndex);
    }

    lua_pop(L, 1);//pop flag

    //to improve performance,comment out below code
    /*lua_pushstring(L,"__metatable");
    lua_pushboolean(L,0);
    lua_rawset(L,tableIndex);*/

    lua_setmetatable(L, -2);
}


void pushMetaTable(TJNIEnv *env, lua_State *L, ScriptContext *context,
                   LuaTable<CrossThreadLuaObject> *metaTable) {
    CrossThreadLuaObject key;
    key.table = metaTable;
    key.type = T_TABLE;
    pushLuaObject(env, L, context, key);
    key.type = T_NIL;
    lua_setmetatable(L, -2);
}

bool parseCrossThreadLuaObject(lua_State *L, ThreadContext *context, int idx,
                               CrossThreadLuaObject &luaObject) {

    switch (lua_type(L, idx)) {
        case LUA_TNIL:
            luaObject.type = T_NIL;
            break;
        case LUA_TBOOLEAN: {
            luaObject.type = T_BOOLEAN;
            luaObject.isTrue = (jboolean) lua_toboolean(L, idx);
            break;
        }
        case LUA_TSTRING: {
            luaObject.type = T_STRING;
            size_t len;
            auto src = lua_tolstring(L, idx, &len);
            char *dest = new char[len];
            strcpy(dest, src);
            luaObject.string = dest;
            break;
        }
        case LUA_TFUNCTION: {
            luaObject.type = T_FUNCTION;
            luaObject.func = saveLuaFunction(L, context, idx);
            break;
        }
        case LUA_TNUMBER: {
#if LUA_VERSION_NUM >= 503
            int isInt;
            lua_Integer intValue = lua_tointegerx(L, idx, &isInt);
            if (isInt) {
                luaObject.type = T_INTEGER;
                luaObject.integer = intValue;
            } else {
                luaObject.type = T_FLOAT;
                luaObject.number = lua_tonumber(L, idx);
            }
#else
            lua_Number number=lua_tonumber(L,idx);
            int64_t intVal=(int64_t)number;
            if(number==intVal){
                luaObject.type=T_INTEGER;
                luaObject.integer=intVal;
            } else{
                luaObject.type=T_FLOAT;
                luaObject.number=number;
            }
#endif
            break;
        }
        case LUA_TLIGHTUSERDATA: {
            luaObject.type = T_LIGHT_USER_DATA;
            luaObject.lightData = lua_touserdata(L, idx);
            break;
        }
        case LUA_TUSERDATA:
            if (testUData(L, idx, OBJECT_KEY)) {
                luaObject.type = T_OBJECT;
                JavaObject *orig = (JavaObject *) lua_touserdata(L, idx);
                luaObject.objectRef = new JavaObject;
                luaObject.objectRef->type = orig->type;
                luaObject.objectRef->object = context->env->NewGlobalRef(orig->object);
            }else if(testUData(L,idx,TYPE_KEY)){
                luaObject.type = T_JAVA_TYPE;
                luaObject.javaType=*(JavaType**) lua_touserdata(L,idx);
            }
#if LUA_VERSION_NUM < 503
                else if(luaL_testudata(L,idx,Integer64::LIB_NAME)){
                luaObject.type=T_INTEGER;
                luaObject.integer=((Integer64*)lua_touserdata(L,idx))->m_val;
            }
#endif

            else {
                lua_pushvalue(L, idx);
                lua_rawget(L, LUA_REGISTRYINDEX);
                UserData *data = static_cast<UserData *>(lua_touserdata(L, -1));
                lua_pop(L,1);
                if (data == nullptr) {
                    size_t len = lua_rawlen(L, idx);
                    data = new UserData(len);
                    lua_pushvalue(L,idx);
                    lua_pushlightuserdata(L,data);
                    lua_rawset(L,LUA_REGISTRYINDEX);
                    int hasMeta = lua_getmetatable(L, idx);
                    if (hasMeta && lua_istable(L, -1)) {
                        CrossThreadLuaObject object;
                        lua_getfield(L,-1,"__gc");
                        bool ok=lua_isnil(L,-1);
                        lua_pop(L,1);
                        ok=ok&&parseCrossThreadLuaObject(L, context, lua_gettop(L), object);
                        lua_pop(L, 1);
                        if (ok) {
                            data->metaTable = object.table;
                            object.type = T_NIL;
                            object.table = nullptr;
                        }
                        if (!ok) {
                            delete data;
                            lua_pushvalue(L,idx);
                            lua_pushnil(L);
                            lua_rawset(L,LUA_REGISTRYINDEX);
                            return false;
                        }
                    }
                    void *orig = lua_touserdata(L, idx);
                    memcpy(&data->data[0], orig, len);
                    luaObject.userData = data;
                    lua_pushvalue(L,idx);
                    lua_pushnil(L);
                    lua_rawset(L,LUA_REGISTRYINDEX);
                } else {
                    luaObject.userData = data;
                }
                luaObject.type = T_USER_DATA;
            }
            break;
        case LUA_TTABLE: {
            lua_pushvalue(L, idx);
            lua_rawget(L, LUA_REGISTRYINDEX);
            LuaTable<CrossThreadLuaObject> *luaTable = (LuaTable<CrossThreadLuaObject> *)(lua_touserdata(L, -1));
            lua_pop(L,1);
            if (luaTable == nullptr) {
                luaTable = new LuaTable<CrossThreadLuaObject>();
                lua_pushvalue(L,idx);
                lua_pushlightuserdata(L,luaTable);
                lua_rawset(L,LUA_REGISTRYINDEX);
                luaTable->get().reserve(int(lua_rawlen(L, idx)));
                lua_pushnil(L);
                while (lua_next(L, idx)) {
                    CrossThreadLuaObject key;
                    CrossThreadLuaObject value;
                    bool ok = parseCrossThreadLuaObject(L, context, lua_gettop(L)-1, key) &&
                              parseCrossThreadLuaObject(L, context, lua_gettop(L), value);
                    lua_pop(L, 1);
                    if (ok)
                        luaTable->get().push_back({std::move(key), std::move(value)});
                    else {
                        delete luaTable;
                        lua_pushvalue(L,idx);
                        lua_pushnil(L);
                        lua_rawset(L,LUA_REGISTRYINDEX);
                        return false;
                    }
                }

                int hasMeta = lua_getmetatable(L, idx);
                if (hasMeta) {
                    CrossThreadLuaObject object;
                    lua_getfield(L,-1,"__gc");
                    bool ok=lua_isnil(L,-1);
                    lua_pop(L,1);
                    ok=ok&&parseCrossThreadLuaObject(L, context, lua_gettop(L), object);
                    lua_pop(L, 1);
                    if (ok) {
                        luaTable->metaTable = object.table;
                        object.type = T_NIL;
                        object.table = nullptr;
                    } else {
                        delete luaTable;
                        lua_pushvalue(L,idx);
                        lua_pushnil(L);
                        lua_rawset(L,LUA_REGISTRYINDEX);
                        return false;
                    }
                }
                luaObject.table = luaTable;
                lua_pushvalue(L,idx);
                lua_pushnil(L);
                lua_rawset(L,LUA_REGISTRYINDEX);
            } else {
                luaObject.table = luaTable;
            }
            luaObject.type = T_TABLE;
            break;
        }
        default:
            return false;
    }
    return true;
}

bool pushLuaObject(TJNIEnv* env,lua_State *L, ScriptContext *context,
                   const CrossThreadLuaObject &luaObject) {
    switch (luaObject.type) {
        case T_NIL:
            return false;
        case T_BOOLEAN:
            lua_pushboolean(L, luaObject.isTrue);
            break;
        case T_FLOAT:
            lua_pushnumber(L, luaObject.number);
            break;
        case T_INTEGER:
#if LUA_VERSION_NUM >= 503
            lua_pushinteger(L, luaObject.integer);
#else
        if(int64_t(double(luaObject.integer))!=luaObject.integer){
            Integer64::pushLong(L,luaObject.integer);
        } else lua_pushnumber(L,luaObject.integer);
#endif

            break;
        case T_STRING:
            lua_pushstring(L, luaObject.string);
            break;
        case T_OBJECT:
            pushJavaObject(L, env, context, luaObject.objectRef->object, nullptr);
            break;
        case T_FUNCTION:
            loadLuaFunction(env, L, luaObject.func, context);
            break;
        case T_TABLE:
            lua_rawgetp(L, LUA_REGISTRYINDEX,luaObject.table);
            if (lua_isnil(L, -1)) {
                lua_pop(L, -1);
                lua_createtable(L, 0, luaObject.table->get().size());
                lua_pushlightuserdata(L, luaObject.table);
                lua_pushvalue(L, -2);
                lua_rawset(L, LUA_REGISTRYINDEX);
                for (const auto &pair:luaObject.table->get()) {
                    const CrossThreadLuaObject &key = pair.first;
                    pushLuaObject(env, L, context, key);
                    const CrossThreadLuaObject &value = pair.second;
                    pushLuaObject(env, L, context, value);
                    lua_rawset(L, -3);
                }
                if (luaObject.table->metaTable != nullptr) {
                    pushMetaTable(env, L, context, luaObject.table->metaTable);
                }
                lua_pushlightuserdata(L, luaObject.table);
                lua_pushnil(L);
                lua_rawset(L, LUA_REGISTRYINDEX);
            }
            break;
        case T_LIGHT_USER_DATA:
            lua_pushlightuserdata(L, luaObject.lightData);
            break;
        case T_USER_DATA: {
            lua_rawgetp(L, LUA_REGISTRYINDEX,luaObject.userData);
            if (lua_isnil(L, -1)) {
                lua_pop(L, -1);
                size_t len = luaObject.userData->data.size();
                void *orig = lua_newuserdata(L, len);;
                memcpy(orig, &luaObject.userData->data[0], len);
                if (luaObject.userData->metaTable != nullptr) {
                    lua_pushlightuserdata(L, luaObject.userData);
                    lua_pushvalue(L, -2);
                    lua_rawset(L, LUA_REGISTRYINDEX);
                    pushMetaTable(env, L, context, luaObject.userData->metaTable);
                    lua_pushlightuserdata(L, luaObject.userData);
                    lua_pushnil(L);
                    lua_rawset(L, LUA_REGISTRYINDEX);
                }
            }
            break;
        }
        case T_JAVA_TYPE:
            pushJavaType(L,luaObject.javaType);
            break;
        default:
            return false;
    }
    return true;
}

void recordLuaError(ThreadContext *context, lua_State *L, int ret) {
    switch (ret) {
        case LUA_ERRERR:
        case LUA_ERRRUN:
#if LUA_VERSION_NUM > 501
        case LUA_ERRGCMM:
#endif
        case LUA_ERRSYNTAX:
            if (!isThrowableObject(L,context))
                  context->setPendingException(luaL_tolstring(L, -1, nullptr));
            else context->setPendingException((jthrowable)static_cast<JavaObject*>(lua_touserdata(L,-1))->object);
            lua_pop(L, 1);
            break;
        case LUA_ERRMEM:
            context->setPendingException("Lua memory error");
            break;
        case -1:
            context->setPendingException( "Unknown error");
        default:
            break;
    }
}

int javaType(lua_State *L) {
    ThreadContext *context = getContext(L);
    auto env=context->env;
    int count = lua_gettop(L);
    for (int i = 1; i <= count; ++i) {
        JavaType *type;
        if (luaL_isstring(L, i)) {
            type = context->ensureType(lua_tostring(L, i));
            if(type== nullptr)
                goto Error;
        } else {
            JavaObject *objectRef = (JavaObject *) testUData(L, i, OBJECT_KEY);
            if (objectRef != nullptr) {
                if (env->IsInstanceOf(objectRef->object, classType)) {
                    type = context->scriptContext->ensureType(env, (jclass) objectRef->object);
                    goto Ok;
                } else if (env->IsInstanceOf(objectRef->object, stringType)) {
                    type = context->ensureType(JString(env, (jstring) objectRef->object));
                    if(type!= nullptr)
                        goto Ok;
                }
            }
            goto Error;
        }
        Ok:
        pushJavaType(L,type);
        continue;//never reach
        Error:
        luaL_error(L, "Invalid type=%s", luaL_tolstring(L, i,NULL));
    }
    return count;
}

static int proxyByTable(lua_State *L) {
    if (!lua_istable(L, 1)) return 0;
    ThreadContext *context = getContext(L);
    SetErrorJMP();
    Vector<JavaType *> interfaces;
    lua_getfield(L, 1, "super");
    JavaType **typeRef;
    JavaType *main = nullptr;
    JavaObject* superObject= nullptr;
    if ((typeRef = (JavaType **) testUData(L, -1, TYPE_KEY)) != nullptr)
        main = *typeRef;
    else if((superObject=(JavaObject*)testUData(L,-1,OBJECT_KEY))!= nullptr){
        main=superObject->type;
    }
    lua_getfield(L, 1, "interfaces");
    if (lua_istable(L, -1)) {
        lua_pushnil(L);
        while (lua_next(L, -2)) {
            if ((typeRef = (JavaType **) testUData(L, -1, TYPE_KEY)) != nullptr)
                interfaces.push_back(*typeRef);
            lua_pop(L, 1);
        }
    }
    if (main == nullptr) {
        if (interfaces.size() == 0) {
            lua_pushstring(L,"No class or interfaces");
            goto __ErrorHandle;
        } else {
            main = *interfaces.begin();
            interfaces.erase(interfaces.begin());
        }
    }
    auto env=context->env;
    Vector<std::unique_ptr<BaseFunction>> luaFuncs;
    Vector<JObject> agentMethods;
    lua_getfield(L, 1, "methods");
    if (lua_istable(L, -1)) {
        if (!readProxyMethods(L, context, interfaces, main, luaFuncs,
                              agentMethods)) {
            goto __ErrorHandle;
        }
    } else if (lua_isfunction(L, -1)) {
        JObject single(main->getSingleInterface(env));
        if (unlikely(single == nullptr)) TopErrorHandle("All methods should be specified");
        agentMethods.push_back(std::move(single));
        if (context->isLocalFunction()) {
            luaFuncs.emplace_back(saveLocalFunction(L, -1));
        } else {
            luaFuncs.emplace_back(saveLuaFunction(L, context, -1));
        }

    }
    lua_getfield(L, 1, "shared");
    bool shared;
    if (lua_isnil(L, -1)) shared = false;
    else if (lua_isboolean(L, -1)) shared = lua_toboolean(L, -1) != 0;
    else if (lua_isnumber(L, -1)) shared = lua_tointeger(L, -1) != 0;
    else shared = true;
    jobject proxy;
    if(superObject){
       proxy = context->proxy( main, &interfaces, agentMethods, luaFuncs, shared,
                                     0,superObject->object);
    } else{
        Vector<ValidLuaObject> constructArgs;
        Vector<JavaType *> argTypes;
        lua_getfield(L, 1, "args");
        if (lua_istable(L, -1)) {
            lua_pushnil(L);
            JavaType *type = nullptr;
            while (lua_next(L, -2)) {
                typeRef = (JavaType **) testUData(L, -1, TYPE_KEY);
                if (typeRef != nullptr) {
                    type = *typeRef;
                } else {
                    ValidLuaObject luaObject;
                    if (!parseLuaObject(L, context, -1, luaObject)) {
                        lua_pushstring(L,"Arg type not support");
                        goto __ErrorHandle;
                    }
                    argTypes.push_back(type);
                    constructArgs.push_back(std::move(luaObject));
                    type= nullptr;
                }

            }
        }
        void *constructInfo[] = {&constructArgs, &argTypes};
        proxy = context->proxy( main, &interfaces, agentMethods, luaFuncs, shared,
                               (long) &constructInfo);
    }
    if (proxy == INVALID_OBJECT) {
        pushJavaException(L,context);
        goto __ErrorHandle;
    }
    for (auto &ptr:luaFuncs) {
        ptr.release();
    }
    pushJavaObject( L, context, JObject(env, proxy));
    return 1;
}

bool
readProxyMethods(lua_State *L, ThreadContext *context, Vector<JavaType *> &interfaces,
                 JavaType *main, Vector<std::unique_ptr<BaseFunction>> &luaFuncs,
                 Vector<JObject> &agentMethods) {
    int expectedLen = int(lua_rawlen(L, -1));
    luaFuncs.reserve(expectedLen);
    agentMethods.reserve(expectedLen);
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        if (!lua_isstring(L, -2)) {
            TopErrorHandle("Not a string for method name=%s", luaL_tolstring(L, -2, NULL));
        }
        FakeString methodName(lua_tostring(L, -2));
        if (strcmp(methodName.data(), "<init>") == 0) {
            TopErrorHandle("Constructor can't be override");
        }
        if (lua_istable(L, -1)) {
            Vector<JavaType *> paramTypes;
            lua_pushnil(L);
            while (lua_next(L, -2)) {
                JavaType **typeRef;
                if ((typeRef = (JavaType **) testUData(L, -1, TYPE_KEY)) != nullptr)
                    paramTypes.push_back(*typeRef);
                else if (lua_isfunction(L, -1)) {
                    JavaType *matchType = main;
                    auto info = main->findMethod(context->env,methodName, false, paramTypes, nullptr);
                    if (info == nullptr) {
                        for (auto interface:interfaces) {
                            info = interface->findMethod(context->env,methodName, false, paramTypes, nullptr);
                            if (info != nullptr) {
                                matchType = interface;
                                break;
                            }
                        }
                    }
                    if (info == nullptr) {
                        TopErrorHandle("Can't find matched method "
                                               "for the method:%s", methodName.c_str());
                    }
                    agentMethods.push_back(context->env->ToReflectedMethod(
                            matchType->getType(), info->id, JNI_FALSE));
                    BaseFunction *func;
                    if (context->isLocalFunction())
                        func = saveLocalFunction(L, -1);
                    else
                        func = saveLuaFunction(L, context, -1);
                    func->javaRefCount++;
                    luaFuncs.emplace_back(func);
                    paramTypes.clear();
                }
                lua_pop(L, 1);
            }
        } else if (lua_isfunction(L, -1)) {//proxy for all methods
            BaseFunction *func;
            if (context->isLocalFunction()) {
                func = saveLocalFunction(L, -1);
            } else {
                func = saveLuaFunction(L, context, -1);
            }
            auto all = main->findAllObjectMethod(context->env,methodName);
            bool found = false;
            if (all) {
                for (auto &&info:*all) {
                    func->javaRefCount++;
                    agentMethods.push_back(context->env->ToReflectedMethod(
                            main->getType(), info.id, JNI_FALSE));
                    luaFuncs.emplace_back(func);
                    found = true;
                }
            }
            for (auto interface:interfaces) {
                all = main->findAllObjectMethod(context->env,methodName);
                if (!all) continue;
                for (auto &&info:*all) {
                    func->javaRefCount++;
                    agentMethods.push_back(context->env->ToReflectedMethod(
                            interface->getType(), info.id, JNI_FALSE));
                    luaFuncs.emplace_back(func);
                    found = true;
                }
            }
            if (!found) {
                TopErrorHandle("No method named %s", methodName);
            }
        }
        lua_pop(L, 1);
    }
    return true;
    __ErrorHandle:
    return false;
};

int javaProxy(lua_State *L) {
    if (lua_gettop(L) == 1)
        return proxyByTable(L);
    ThreadContext *context = getContext(L);
    JavaType **typeRef;
    JavaType *main;
    JavaObject* superObject;
    if((superObject=(JavaObject*)testUData(L,1,OBJECT_KEY))!= nullptr){
        main=superObject->type;
    } else{
        main = checkJavaType(L,1);
    }
    SetErrorJMP();

    int top = lua_gettop(L);
    Vector<JavaType *> interfaces;
    int i;
    for (i = 2; i < top; ++i) {
        if ((typeRef = (JavaType **) testUData(L, i, TYPE_KEY)) != nullptr)
            interfaces.push_back(*typeRef);
        else break;
    }
    auto env=context->env;
    Vector<JavaType *> curMethodTypes;
    const char *curMethod = nullptr;
    Vector<JObject> agentMethods;
    Vector<std::unique_ptr<BaseFunction>> luaFuncs;
    bool isLocalFunction = context->isLocalFunction();
    for (int j = i; j <= top; ++j) {
        if (luaL_isstring(L, j)) {
            curMethod = lua_tostring(L, j);
            if (strcmp(curMethod, "<init>") == 0) {
                TopErrorHandle("Constructor can't be override");
            }
        } else if ((typeRef = (JavaType **) testUData(L, j, TYPE_KEY)) != nullptr) {
            curMethodTypes.push_back(*typeRef);
        } else if (lua_isfunction(L, j)) {
            if (curMethod != nullptr) {
                JavaType *matchType = main;
                auto info = main->findMethod(env,FakeString(curMethod), false, curMethodTypes, nullptr);
                if (info == nullptr) {
                    for (auto interface:interfaces) {
                        info = interface->findMethod(env,FakeString(curMethod), false, curMethodTypes,
                                                     nullptr);
                        if (info != nullptr) {
                            matchType = interface;
                            break;
                        }
                    }
                }
                if (unlikely(info == nullptr)) {
                    TopErrorHandle("Can't find matched method "
                                           "for the No.%d method:%s", agentMethods.size(),
                                   curMethod);
                } else
                    agentMethods.push_back(
                            env->ToReflectedMethod(matchType->getType(), info->id, JNI_FALSE));
            } else if (likely(curMethodTypes.size() == 0)) {
                JObject single(main->getSingleInterface(env));
                if (unlikely(single == nullptr)) TopErrorHandle("methods should be specified");
                agentMethods.push_back(std::move(single));
            } else {
                TopErrorHandle("Lambda class should not specify types");
            }
            BaseFunction *function;
            if (isLocalFunction) {
                function = saveLocalFunction(L, j);
            } else {
                function = saveLuaFunction(L, context, j);
            }
            function->javaRefCount++;
            luaFuncs.emplace_back(function);
            curMethodTypes.clear();
            i = j + 1;//add stack index on success;
            if(!curMethod) break;
        } else
            break;
    }
    if (luaFuncs.size() == 0) {
        TopErrorHandle("No proxy method");
    }

    if (context->hasErrorPending()){
        pushJavaException(L,context);
        goto __ErrorHandle;
    }
    bool shared = false;
    if (i <= top)
        if (lua_isboolean(L, i)) {
            shared = lua_toboolean(L, i) == 1;
            ++i;
        }
    jobject proxy;
    if(superObject){
        proxy = context->proxy(main, &interfaces, agentMethods, luaFuncs, shared,
                               0,superObject->object);
    } else{
        Vector<ValidLuaObject> constructArgs;
        Vector<JavaType *> argTypes;
        for (; i <= top; ++i) {
            JavaType *type = nullptr;
            typeRef = (JavaType **) testUData(L, i, TYPE_KEY);
            if (typeRef != nullptr) {
                type = *typeRef;
                if (++i > top) {
                    TopErrorHandle("no arg found for the No.%d type", argTypes.size());
                }
            }
            argTypes.push_back(type);
            ValidLuaObject luaObject;
            if (!parseLuaObject(L, context, i, luaObject)) {
                lua_pushstring(L, "Arg type not support");
                goto __ErrorHandle;
            }
            constructArgs.push_back(std::move(luaObject));
        }
        void *constructInfo[] = {&constructArgs, &argTypes};
        proxy = context->proxy( main, &interfaces, agentMethods, luaFuncs, shared,
                               (long) &constructInfo);
    }
    if (proxy == INVALID_OBJECT) {
        pushJavaException(L,context);
        goto __ErrorHandle;
    }
    for (auto &ptr:luaFuncs) {
        ptr.release();
    }
    pushJavaObject( L, context, JObject(env, proxy));
    return 1;

}
jobject constructChild(TJNIEnv *env, jclass, jlong ptr, jclass target,
                       jlong nativeInfo) {
    ScriptContext *context = (ScriptContext *) ptr;
    ThreadContext* threadContext= context->getThreadContext();
    JavaType *type = context->ensureType(env, target);
    void **constructInfo = (void **) nativeInfo;
    Vector<ValidLuaObject> *constructArgs = (Vector<ValidLuaObject> *) constructInfo[0];
    Vector<JavaType *> *argTypes = (Vector<JavaType *> *) constructInfo[1];
    jobject ret = type->newObject(threadContext,*argTypes, *constructArgs);
    if (threadContext->hasErrorPending())
        threadContext->throwToJava();
    return ret;
}

LocalFunctionInfo *saveLocalFunction(lua_State *L, int i) {
    LocalFunctionInfo *info = new LocalFunctionInfo(L);
    lua_pushlightuserdata(L, info);
    lua_pushvalue(L, i);
    lua_settable(L, LUA_REGISTRYINDEX);
    return info;
}

static bool qualifyJavaName(String& name){
    auto i=name.length();
    char* s=&name[0];
    for(;i!=0;){
        auto c=s[--i];
        if(c=='$'){
            s[i]='_';
        }else if(c=='-'||int8_t (c)<0){
            return false;
        }
    }
    return true;
}

int javaUsing(lua_State*L){
    ThreadContext *context = getContext(L);
    TJNIEnv *env = context->env;
    Import *import = context->getImport();
    if(lua_isstring(L, 1)){
        const char* pack=lua_tostring(L,1);
        static jmethodID importAll= env->GetMethodID(contextClass, "importAll", "(Ljava/lang/String;)[Ljava/lang/String;");
        JString jpack= env->NewStringUTF(pack);
        JObjectArray classes(env->CallObjectMethod(context->scriptContext->javaRef, importAll, jpack.get()));
        HOLD_JAVA_EXCEPTION(context,{ return 0;});
        int length=env->GetArrayLength(classes);
        String importPack=pack;
        if(pack[0])importPack+='.';
        int packLen=(int)importPack.size();
        if(length!=0){
            pack= import->packages.insert(importPack).first->data();
        }
        for (; length!=0; ) {
            JString cl(env->GetObjectArrayElement(classes,--length));
            String name(cl.str()+packLen);
            JavaType* type;
            auto &&iter = import->stubbed.find(name);
            if(iter==import->stubbed.end()){
                 auto &&clazz = context->findClass(cl.str());
                 if(clazz==nullptr)
                     continue;
                 type= context->scriptContext->ensureType(env, clazz);
                 import->stubbed[name]={type, pack};
            } else {
                type=iter->second.type;
#ifndef NDEBUG
                if(strcmp(iter->second.pack,pack)!=0){
                    printf("name %s overloaded for new %s and old %s\n",name.data(),pack,iter->second.pack);
                }
#endif
            }
            if(!qualifyJavaName(name))continue;
            lua_getglobal(L,name.data());
            JavaType* existed= (JavaType *)lua_touserdata(L, -1);
            lua_pop(L,1);
            if(existed!=type){
                pushJavaType(L,type);
                lua_setglobal(L,name.data());
            }
        }
        
    } else {
        JavaObject * loader=checkJavaObject(L,1);
        if(loaderClass== nullptr)
            loaderClass= static_cast<jclass>(env->NewGlobalRef(env->FindClass("java/lang/ClassLoader")));
        if(env->IsInstanceOf(loader->object,loaderClass)){
            static jmethodID  loadClassLoader=env->GetMethodID(contextClass,"loadClassLoader","(Ljava/lang/ClassLoader;)V");
            env->CallVoidMethod(context->scriptContext->javaRef,loadClassLoader,loader->object);
            import->externalLoaders.push_back(env->NewGlobalRef(loader->object));
        }
    }
    return 0;
}

int javaImport(lua_State *L) {
    ThreadContext *context = getContext(L);
    SetErrorJMP();
    auto env=context->env;
    if (!luaL_isstring(L, -1)) luaL_error(L, "Should pass a string for import");
    auto&& s=lua_tostring(L, -1);
    Import *import = context->getImport();
    if(s[0]=='[') TopErrorHandle("Don't import array type!");
    const char* separate = strrchr(s,'.');
    String pack;
    if (separate!= nullptr) {
        auto len = separate - s + 1;
        pack.resize(len);
        memcpy(&pack[0],s,len);
    }
    size_t len=strlen(s);
    if (s[len-1]=='*'&&separate==s+len-2) {//ends with .*
        import->packages.emplace(std::move(pack));
    } else {
        String name=separate?separate+1:s;
        auto &&iter = import->stubbed.find(name);
        if (iter != import->stubbed.end()) {
            const char *prevName = iter->second.pack;
            if (strcmp(prevName, pack.data()) != 0&&strcmp(prevName, "java.lang.") != 0) {
                TopErrorHandle("Only single import is allowed for name: %s"
                                       ",with previous class: %s", name.c_str(), prevName);
            }
            pushJavaType(L,iter->second.type);
            goto RET;
        }
        {
            JClass c(context->findClass(s));
            if(c== nullptr){  TopErrorHandle(" Type:%s not found", s); }
            auto type = context->scriptContext->ensureType(env, c);
            Import::TypeInfo info={type};
            auto&& existedPack=import->packages.find(pack);
            if(existedPack==import->packages.end()){
                info.cachePack=std::move(pack);
                info.pack=info.cachePack.data();
            } else info.pack=existedPack->data();
            import->stubbed[name]=std::move(info);
            pushJavaType(L,type);
        }
        RET:
        if(qualifyJavaName(name)){
            lua_pushvalue(L, -1);
            lua_setglobal(L, name.data());
        }
        return 1;
    }
    return 0;
}

int javaCharValue(lua_State *L) {
    if (!luaL_isstring(L, -1)) {
        luaL_error(L, "Not a string but %s", luaL_typename(L, lua_type(L, -1)));
    }
    const char *s = lua_tostring(L, -1);
    if (strlen8to16(s) == 1) {
        char16_t ret;
        strcpy8to16(&ret, s, nullptr);
        lua_pushinteger(L, ret);
    } else luaL_error(L, "the string has more than one char:%s", s);
    return 1;
}

int javaCharString(lua_State *L) {
    int isnum;
    jlong v = lua_tointegerx(L, -1, &isnum);
    if (!isnum)
        luaL_error(L, "Not a integer");
    if (v > UINT16_MAX) luaL_error(L, "Not a char value");
    char16_t s = (char16_t) v;
    char ret[4];
    strncpy16to8(ret,&s, 1);
    lua_pushstring(L, ret);
    return 1;
}

int newArray(lua_State *L, int index, ThreadContext *context, JavaType *type) {
    jlong size = 0;
    if (type->isVoid()) {
        luaL_error(L, "Type Error:array for void.class can't be created");
    }
    int isnum;
    size = lua_tointegerx(L, index++, &isnum);
    if (!isnum) {
        luaL_error(L, "Type Error: not a integer but %s", luaL_typename(L, index));
    } else if (size > INT32_MAX || size < -1) {
        luaL_error(L, "integer overflowed");
    }
    int top = lua_gettop(L);
    if (size!=-1&&top - index > size) {
        luaL_error(L, "%d elements is too many for an array of size %d", top - index, size);
    }
    Vector<ValidLuaObject> elements;
    elements.reserve(top - index + 1);
    for (; index <= top; ++index) {
        ValidLuaObject object;
        if (!parseLuaObject( L, context, index, object)) {
            forceRelease(elements);
            luaL_error(L, "Arg unexpected for array");
        }
        if(checkLuaTypeNoThrow(context->env, L, type, object)){
            forceRelease(elements,object);
            lua_error(L);
        }
        elements.push_back(std::move(object));
    }
    if(size==-1) size=elements.size();
    JArray ret(context->env, type->newArray(context,(jint) size, elements));
    if (ret == nullptr) {
        forceRelease(elements);
        throwJavaError(L,context);
    }
    pushJavaObject(L, context, ret);
    return 1;
}
int javaNew(lua_State *L) {
    ThreadContext *context = getContext(L);
    JavaType *type = checkJavaType(L,1);
    auto env=context->env;
    auto component = type->getComponentType(env);
    if (component != nullptr) {
        return newArray(L, 2, context, component);
    } else if(!type->isPrimitive()){
        int top=lua_gettop(L);
        auto expectedSize = top - 1;
        JavaType* arr1[expectedSize];
        ValidLuaObject arr2[expectedSize];
        FakeVector<JavaType *> types(arr1,expectedSize);
        FakeVector<ValidLuaObject> objects(arr2,expectedSize);
        readArguments(L, context, types, objects, 2, top);
        JObject obj = JObject(env, type->newObject(context,types, objects));
        if (context->hasErrorPending()) {
            forceRelease(obj);
            types.release();
            throwJavaError(L,context);
        }
        pushJavaObject(L, env, context->scriptContext, obj.get(), type);
    } else
        luaL_error(L,"Primitive type can't make a new instance");
    return 1;
}

static int javaUnBox(lua_State* L){
    ThreadContext *context = getContext(L);
    JavaObject* object= checkJavaObject(L,1);
    switch (object->type->getTypeID()){
        case JavaType::BOX_BOOLEAN:
            lua_pushboolean(L,context->env->CallBooleanMethod(object->object,booleanValue));
            break;
        case JavaType::BOX_BYTE:
        case JavaType::BOX_SHORT:
        case JavaType::BOX_INT:
        case JavaType::BOX_LONG:
            lua_pushinteger(L,context->env->CallLongMethod(object->object,longValue));
            break;
        case JavaType::BOX_FLOAT:
        case JavaType::BOX_DOUBLE:
            lua_pushnumber(L,context->env->CallDoubleMethod(object->object,doubleValue));
            break;
        case JavaType::BOX_CHAR: {
            jchar c = context->env->CallCharMethod(object->object, charValue);
            char s[4] ;
            strncpy16to8(s,(const char16_t *) &c, 1);
            lua_pushstring(L, s);
            break;
        }
        default:
            break;
    }
    return 1;
}

static int javaNext(lua_State* L){
    ThreadContext *context = getContext(L);
    JavaObject* object= static_cast<JavaObject *>(lua_touserdata(L, 1));
    TJNIEnv *env = context->env;
    static jmethodID hasNext= env->GetMethodID(object->type->getType(),"hasNext","()Z");
    static jmethodID nextEntry= env->GetMethodID(object->type->getType(),"nextEntry","()[Ljava/lang/Object;");
    jboolean hasMore=env->CallBooleanMethod(object->object,hasNext);
    HOLD_JAVA_EXCEPTION(context,{ throwJavaError(L,context);});
    if(!hasMore){
        lua_pushnil(L);
        return 1;
    }
    JObjectArray next(env->CallObjectMethod(object->object,nextEntry));
    HOLD_JAVA_EXCEPTION(context,{throwJavaError(L, context);});
    int len=env->GetArrayLength(next);
    if(len==1){
        int64_t key=lua_tointeger(L,2);
        JObject value=env->GetObjectArrayElement(next,0);
        lua_pushinteger(L,++key);
        if(value== nullptr)lua_pushnil(L);
        else pushJavaObject(L,context,value);
    } else{
        JObject key=env->GetObjectArrayElement(next,0);
        JObject value=env->GetObjectArrayElement(next,1);
        if(key== nullptr)
            lua_pushboolean(L,0);
        else pushJavaObject(L,context,key);
        if(value== nullptr)lua_pushnil(L);
        else pushJavaObject(L,context,value);
    }
    return 2;
}



int javaIterate(lua_State* L){
    ThreadContext *context = getContext(L);
    auto env=context->env;
    static jmethodID  iterate=env->GetMethodID(contextClass,"iterate","(Ljava/lang/Object;)Lcom/oslorde/luadroid/MapIterator;");
    JavaObject* object= checkJavaObject(L,1);
    JObject iterator=env->CallObjectMethod(context->scriptContext->javaRef,iterate,object->object);
    HOLD_JAVA_EXCEPTION(context,{});
    if(iterator==nullptr){
        luaL_error(L,"Bad argument for iterator:%s",luaL_tolstring(L,1, nullptr));
    }
    lua_pushlightuserdata(L,context);
    lua_pushcclosure(L,javaNext,1);
    pushJavaObject(L,context,iterator);
    lua_pushinteger(L,0);
    return 3;
}

int javaNewArray(lua_State *L) {
    ThreadContext *context = getContext(L);
    JavaType *type = checkJavaType(L,1);
    return newArray(L, 2, context, type);
}

int javaToJavaObject(lua_State *L) {
    ThreadContext *context = getContext(L);
    auto env=context->env;
    int top=lua_gettop(L);
    auto expectedSize = top - 1;
    JavaType* arr1[expectedSize];
    ValidLuaObject arr2[expectedSize];
    FakeVector<JavaType *> types(arr1,expectedSize);
    FakeVector<ValidLuaObject> luaObjects(arr2,expectedSize);
    readArguments(L, context, types, luaObjects, 1, 0);
    int len=luaObjects.asVector().size();
    int i;
    for (i = 0; i < len; ++i) {
        JavaType* type=types.asVector()[i];
        if(type== nullptr){
            jobject obj=context->luaObjectToJObject(luaObjects.asVector()[i]);
            if(likely(obj!=INVALID_OBJECT))
                pushJavaObject(L,context,JObject(env,obj));
            continue;
        } else {
            if(type->isPrimitive()){
                type=type->toBoxedType();
            }
            jvalue v=context->luaObjectToJValue(luaObjects.asVector()[i],type);
            if(likely(v.l!=INVALID_OBJECT)){
                pushJavaObject(L,context,v.l);
                cleanArg(env,v.l,luaObjects.asVector()[i].shouldRelease);
            }
        }
    }

    return i;

}

int javaInstanceOf(lua_State *L) {
    JavaObject *objectRef = checkJavaObject(L,1);
    JavaType *typeRef = checkJavaType(L,2);
    lua_pushboolean(L, getContext(L)->env->IsInstanceOf(objectRef->object, typeRef->getType()));
    return 1;
}
#if LUA_VERSION_NUM >=503


static int lockGC(lua_State*L){
    JavaObject* objectRef=(JavaObject*)lua_touserdata(L,lua_upvalueindex(1));
    if(*static_cast<bool *>(lua_touserdata(L, 1))){
        LOGE("Lock unlocked");
        AutoJNIEnv()->MonitorExit(objectRef->object);
    }
    return 0;
}
static int javaSyncContinue(lua_State* L, int ret,lua_KContext kContext){
    JavaObject *objectRef =  checkJavaObject(L,1);
    auto context=(ThreadContext*) kContext;
    auto env=context->env;
    env->MonitorExit(objectRef->object);
    *(bool *)lua_touserdata(L,3)=false;
    HOLD_JAVA_EXCEPTION(context, {throwJavaError(L,context);});
    if(unlikely(ret!=LUA_OK&&ret!=LUA_YIELD)){
        lua_error(L);
    }
    return 0;
}

int javaSync(lua_State* L){
    JavaObject *objectRef =  checkJavaObject(L,1);
    auto context=getContext(L);
    auto env=context->env;
    env->MonitorEnter(objectRef->object);
    HOLD_JAVA_EXCEPTION(context, {throwJavaError(L,context);});
    *(bool *)lua_newuserdata(L, sizeof(bool))=true;
    lua_createtable(L,0,1);
    lua_pushstring(L,"__gc");
    lua_pushvalue(L,1);
    lua_pushcclosure(L,lockGC,1);
    lua_rawset(L,-3);
    lua_setmetatable(L,-2);
    // when the stack destroyed, release the object in never returned yield call
    pushErrorHandler(L,context);
    lua_pushvalue(L,2);
    return javaSyncContinue(L,lua_pcallk(L, 0, 0, -2,(intptr_t)context,javaSyncContinue),(intptr_t)context) ;
}
#else
int javaSync(lua_State *L) {
    JavaObject *objectRef =  checkJavaObject(L,1);
    auto context=getContext(L);
    auto env=context->env;
    env->MonitorEnter(objectRef->object);
    HOLD_JAVA_EXCEPTION(context, {throwJavaError(L,context);});
    pushErrorHandler(L,context);
    lua_pushvalue(L,2);
    int ret;
    ret = lua_pcall(L, 0, 0, -2);
    env->MonitorExit(objectRef->object);
    HOLD_JAVA_EXCEPTION(context, {throwJavaError(L,context);});
    if(unlikely(ret!=LUA_OK)){
        lua_error(L);
    }
    return 0;
}
#endif

int javaThrow(lua_State *L) {
    JavaObject *objectRef =  (JavaObject*)testUData(L,1,OBJECT_KEY);
    ThreadContext *context = getContext(L);
    if (objectRef== nullptr||!context->env->IsInstanceOf(objectRef->object, throwableType))
        context->setPendingException(luaL_tolstring(L,1,NULL));
    else context->setPendingException((jthrowable) objectRef->object);
    throwJavaError(L,context);
    return 0;
}

void parseTryTable(lua_State *L) {
    lua_pushinteger(L, 1);
    lua_gettable(L, 1);
    lua_getfield(L, 1, "catch");
    int catchIndex = lua_gettop(L);
    if (lua_isfunction(L, catchIndex)) {
        lua_pushstring(L, "all");
        lua_pushvalue(L, catchIndex);
    } else if (lua_istable(L, catchIndex)) {
        lua_pushnil(L);
        while (lua_next(L, catchIndex)) {
            lua_pushvalue(L, -2);//push key for next
        }
    }
    lua_remove(L, catchIndex);
    lua_getfield(L, 1, "finally");
    if (lua_isnil(L, -1))
        lua_pop(L, 1);
    lua_remove(L, 1);//remove table
}

struct TryInfo{
    bool noCatch;
    int finallyIndex;
    int catchAllIndex;
    int handlerIndex;
    int top;
    ThreadContext* context;
};
int finallyContinue(lua_State*L,int,lua_KContext shouldTrow){
    if(shouldTrow)
        lua_error(L);
    return 0;
}
int catchContinue(lua_State* L,int code,lua_KContext finallyIndex){
    if(code==LUA_YIELD)
        code=LUA_OK;
    if(finallyIndex>0){
        lua_pushvalue(L,(int)finallyIndex);
        lua_callk(L,0,0,code,finallyContinue);
    }
    if(code!=LUA_OK){
        lua_error(L);
    }
    return 0;
}
int javaTryContinue(lua_State*L,int status,lua_KContext tryInfo){
    TryInfo* info=(TryInfo*) tryInfo;
    int finallyIndex = info->finallyIndex;
    if (status != LUA_OK&&status!=LUA_YIELD) {
        auto context=info->context;
        int catchAllIndex=info->catchAllIndex;
        if (!info->noCatch) {
            lua_pushvalue(L,-1);
            recordLuaError(context,L,status);
            auto env=context->env;
            JType<jthrowable> error(env,context->transferJavaError());
            int end=catchAllIndex?catchAllIndex:finallyIndex?finallyIndex:info->top;
            for (int idx = 2; idx < end; idx += 2) {
                JavaType *type = *(JavaType **) lua_touserdata(L, idx);
                if (env->IsInstanceOf(error, type->getType())) {
#define CALL_CATCH_K(idx)\
                    lua_pushvalue(L,(idx)+1);\
                    pushJavaObject(L,context,JObject(std::move(error)));\
                    return catchContinue(L,lua_pcallk(L,1,0,info->handlerIndex,finallyIndex,catchContinue),finallyIndex);

                    CALL_CATCH_K(idx);
                }
            }
            if (catchAllIndex ) {
                CALL_CATCH_K(catchAllIndex);
            }
        }
        if (finallyIndex) {
            lua_pushvalue(L,finallyIndex);
            lua_callk(L, 0, 0,1,finallyContinue);
        }else if(info->noCatch){
            recordLuaError(context,L,status);
            lua_pushinteger(L,status);
            pushJavaObject(L,context,JObject(context->env,context->transferJavaError()));
            return 2;//if no handler,return the error;
        }
        lua_error(L);
    }
    if (finallyIndex){
        lua_pushvalue(L, finallyIndex);
        lua_callk(L, 0, 0,0,finallyContinue);
    } 
    return 0;
};

int javaTry(lua_State *L) {
    ThreadContext *context = getContext(L);
    int top = lua_gettop(L);
    if (top == 0) luaL_error(L, "No args");
    bool noCatch = false;
    int finallyIndex = 0;
    if (top == 1&&lua_istable(L, 1))
        parseTryTable(L);
    if (lua_isnil(L, 1))
        luaL_error(L, "No try body");
    top = lua_gettop(L);
    int catchAllIndex = 0;
    if (top == 1) noCatch = true;
    else if (top == 2) {
        noCatch = true;
        finallyIndex = 2;
    } else {
        int i;
        auto env=context->env;
        for (i = 2; i < top ; i += 2) {
            JavaType* type;
            if ((type=*(JavaType**)testUData(L, i, TYPE_KEY))!= nullptr) {
                if(type->isThrowable(env))
                    continue;
            } else if (luaL_isstring(L, i)) {
                const char *thrType = lua_tostring(L, i);
                if(strcmp(thrType, "all") == 0) {
                    if (catchAllIndex == 0) catchAllIndex = i;
                    else
                        luaL_error(L,"More than one catch all functions");
                }else{
                    type = context->ensureType(thrType);
                    if(type== nullptr||!type->isThrowable(env))
                        luaL_error(L,"Not a exception type: %s",thrType);
                    pushJavaType(L,type);
                    lua_replace(L,i);
                }
            } else {
                luaL_error(L,"Not a exception type:%s",luaL_tolstring(L,i,NULL));
            }
        }
        if (i == top) finallyIndex = top;
    }
    pushErrorHandler(L,context);
    int handlerIndex = lua_gettop(L);
#if LUA_VERSION_NUM >=503
    TryInfo* info=(TryInfo*)lua_newuserdata(L, sizeof(TryInfo));
    info->context=context;
    info->catchAllIndex=catchAllIndex;
    info->finallyIndex=finallyIndex;
    info->handlerIndex=handlerIndex;
    info->top=top;
    info->noCatch=noCatch;
    lua_pushvalue(L, 1);
    return javaTryContinue(L,lua_pcallk(L,0,0,handlerIndex,(intptr_t)info,javaTryContinue),(intptr_t)info);
#else 
    lua_pushvalue(L, 1);
    int ret = lua_pcall(L, 0, 0, handlerIndex);
    if (ret != LUA_OK) {
        if (!noCatch) {
            lua_pushvalue(L,-1);
            recordLuaError(context,L,ret);
            auto env=context->env;
            JType<jthrowable> error(env,context->transferJavaError());
            int end=catchAllIndex?catchAllIndex:finallyIndex?finallyIndex:top;
            for (int idx = 2; idx < end; idx += 2) {
                JavaType *type = *(JavaType **) lua_touserdata(L, idx);
                if (env->IsInstanceOf(error, type->getType())) {
#define CALL_CATCH(index)\
                    lua_pushvalue(L,(index)+1);\
                    pushJavaObject(L,context,JObject(std::move(error)));\
                    int code=lua_pcall(L,1,0,handlerIndex);\
                    if(finallyIndex>0){lua_pushvalue(L,finallyIndex);lua_call(L,0,0);} \
                    if(code!=LUA_OK){\
                        lua_error(L);\
                    }\
                    return 0;
                    CALL_CATCH(idx);
                }
            }
            if (catchAllIndex ) {
                CALL_CATCH(catchAllIndex);
            }
        }
        if (finallyIndex) {
            lua_pushvalue(L,finallyIndex);
            lua_call(L, 0, 0);
        }else if (noCatch){
            recordLuaError(context,L,ret);
            lua_pushinteger(L,ret);
            pushJavaObject(L,context,JObject(context->env,context->transferJavaError()));
            return 2;//if no handler,return the error;
        }
        lua_error(L);
    }
    lua_pop(L,1);
    if (finallyIndex) lua_call(L, 0, 0);
    return 0;
#endif
}

int javaPut(lua_State *L) {
    ThreadContext *context =* (ThreadContext**)lua_touserdata(L,1);
    const char *name = luaL_tolstring(L,2, nullptr);
    CrossThreadLuaObject object;
    if (!parseCrossThreadLuaObject(L, context, 3, object)) {
        const char *s = luaL_tolstring(L, 3, nullptr);
        luaL_error(L, "Invalid object %s with name %s to be pushed to cross thread table", s, name);
    }
    if(object.type==T_NIL){
        context->scriptContext->deleteLuaObject(name);
    } else
        context->scriptContext->saveLuaObject(object, name);
    return 0;
}

int javaGet(lua_State *L) {
    ThreadContext *context =* (ThreadContext**)lua_touserdata(L,1);
    const char *name = luaL_tolstring(L, 2, nullptr);
    CrossThreadLuaObject *object = context->scriptContext->getLuaObject(name);
    if (object == nullptr||!pushLuaObject(context->env, L, context->scriptContext, *object)) {
        lua_pushnil(L);
    }
    return 1;

}

int callMethod(lua_State *L) {
    MemberFlag *flag = (MemberFlag *) lua_touserdata(L, FLAG_INDEX);
    ThreadContext* context=flag->context;
    const char *name = lua_tostring(L, NAME_INDEX);
    SetErrorJMP();
    bool isStatic = flag->isStatic;
    JavaObject *objRef = isStatic ? nullptr : (JavaObject *) lua_touserdata(L, OBJ_INDEX);
    JavaType *type = isStatic ? *(JavaType **) lua_touserdata(L, OBJ_INDEX) : objRef->type;
    int start=1 + flag->isNotOnlyMethod;
    int top=lua_gettop(L);
    auto expectedSize = top - (flag->isNotOnlyMethod);
    JavaType* arr1[expectedSize];
    ValidLuaObject arr2[expectedSize];
    FakeVector<JavaType *> types(arr1,expectedSize);
    FakeVector<ValidLuaObject> objects(arr2,expectedSize);
    readArguments(L, context, types, objects, start,top);
    auto env=context->env;
    auto info = type->findMethod(env,FakeString(name), isStatic, types, &objects.asVector());
    if (unlikely(info == nullptr)) {
        TopErrorHandle("No matched found for the method %s;%s",type->name(env).str(), name);
    }
    int argCount = objects.asVector().size();
    jvalue args[argCount];
    for (int i = argCount - 1; i != -1; --i) {
        ValidLuaObject &object = objects.asVector()[i];
        JavaType::ParameterizedType &tp = info->params[i];
        args[i] = context->luaObjectToJValue(object, tp.rawType,tp.realType);
        if (!tp.rawType->isPrimitive() && args[i].l == INVALID_OBJECT) {
            cleanArgs(args, argCount, objects, env);
            pushJavaException(L,context);
            goto __ErrorHandle;
        }
    }
    int retCount = 1;
    auto returnType=info->returnType.rawType;
#define PushResult(jtype, jname, NAME)\
     case JavaType::jtype:{\
        lua_push##NAME(L,isStatic?env->CallStatic##jname##MethodA(type->getType(),info->id\
        ,args):env->CallNonvirtual##jname##MethodA(objRef->object,objRef->type->getType(),info->id,args));\
        break;}
#define PushFloatResult(jtype, jname) PushResult(jtype,jname,number)
#define PushIntegerResult(jtype, jname) PushResult(jtype,jname,integer)

    switch(returnType->getTypeID()){
        PushResult(BOOLEAN, Boolean, boolean)
        PushIntegerResult(INT, Int)
#if LUA_VERSION_NUM >= 503
        PushIntegerResult(LONG, Long)
#else
        case JavaType::LONG:{
           jlong num=isStatic?env->CallStaticLongMethodA(type->getType(),info->id
           ,args):env->CallNonvirtualLongMethodA(objRef->object,objRef->type->getType(),
           info->id,args);
           if(jlong(double(num))!=num){
               Integer64::pushLong(L,num);
           }else {
               lua_pushnumber(L,num);
           }
           break;
        }
#endif
        PushFloatResult(DOUBLE, Double)
        PushFloatResult(FLOAT, Float)
        PushIntegerResult(BYTE, Byte)
        PushIntegerResult(SHORT, Short)
        case JavaType::CHAR:{
            jchar buf;
            if (isStatic) buf = env->CallStaticCharMethodA(type->getType(), info->id, args);
            else
                buf = env->CallNonvirtualCharMethodA(objRef->object, objRef->type->getType(), info->id, args);
            char s[4] ;
            strncpy16to8(s,(const char16_t *) &buf, 1);
            lua_pushstring(L, s);
            break;
        }
        case JavaType::VOID:
            if (isStatic) env->CallStaticVoidMethodA(type->getType(), info->id, args);
            else
                env->CallNonvirtualVoidMethodA(objRef->object, objRef->type->getType(), info->id, args);
            retCount = 0;
            break;
        default:
            JObject object = isStatic ? env->CallStaticObjectMethodA(type->getType(), info->id, args) :
                             env->CallNonvirtualObjectMethodA(objRef->object, objRef->type->getType(),
                                                              info->id, args);
            if (object == nullptr) lua_pushnil(L); else pushJavaObject(L, context, object);
            break;
    }
    HOLD_JAVA_EXCEPTION(context, {
        pushJavaException(L,context);
        goto __ErrorHandle;
    });
    cleanArgs(args, argCount, objects, env);
    return retCount;
}
static int pushMapValue(lua_State *L,ThreadContext* context,TJNIEnv* env,jobject obj){
    static jmethodID sGet = env->GetMethodID(contextClass, "at", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    ValidLuaObject object;
    parseLuaObject(L,context,2,object);
    JObject  v(env,context->luaObjectToJObject(object));
    auto&& retObj = env->CallObjectMethod(context->scriptContext->javaRef, sGet, obj, v.get());
    HOLD_JAVA_EXCEPTION(context,{throwJavaError(L,context);});
    if(retObj==nullptr)
        lua_pushnil(L);
    else pushJavaObject(L, context, retObj);
    return 1;
}
static int setMapValue(lua_State *L,ThreadContext* context,TJNIEnv* env,jobject obj){
    static jmethodID sSet = env->GetMethodID(contextClass, "set", "(Ljava/lang/Object;Ljava/lang/Object;Ljava/lang/Object;)V");
    ValidLuaObject key;
    parseLuaObject(L,context,2,key);
    ValidLuaObject value;
    parseLuaObject(L,context,3,value);
    JObject  k(env,context->luaObjectToJObject(key));
    jobject  v=context->luaObjectToJObject(value);
    if(v==INVALID_OBJECT){
        forceRelease(key,value,k);
        luaL_error(L,"invalid map value");
    }
    env->CallVoidMethod(context->scriptContext->javaRef,sSet,obj,k.get(),JObject(env,v).get());
    HOLD_JAVA_EXCEPTION(context,{
        throwJavaError(L,context);
    });
    return 0;
}

static int isInstanceOfCall(lua_State* L){
    ThreadContext* context=getContext(L);
    JavaObject* object=(JavaObject*)lua_touserdata(L,lua_upvalueindex(2));
    JavaType* type=checkJavaType(L,1);
    lua_pushboolean(L,context->env->IsInstanceOf(object->object,type->getType()));
    return 1;
}

static int isAssignableFromCall(lua_State* L){
    ThreadContext* context=getContext(L);
    JavaType* sub=(JavaType*)lua_touserdata(L,lua_upvalueindex(2));
    JavaType* type=checkJavaType(L,1);
    lua_pushboolean(L,context->env->IsAssignableFrom(sub->getType(),type->getType()));
    return 1;
}

static int newCall(lua_State* L){
    int len=lua_gettop(L);
    lua_pushvalue(L,lua_upvalueindex(1));
    luaL_getmetafield(L,-1,"__call");
    lua_insert(L,1);
    lua_insert(L,2);
    lua_call(L,len+1,1);
    return 1;
}

int getFieldOrMethod(lua_State *L) {
    bool isStatic = isJavaTypeOrObject(L,1);
    JavaObject *obj = isStatic ? nullptr : (JavaObject*) lua_touserdata(L,1);
    JavaType *type = isStatic ? *(JavaType**) lua_touserdata(L,1) : obj->type;
    ThreadContext* context=getContext(L);
    auto env=context->env;
    if (!isStatic) {
        auto component = obj->type->getComponentType(env);
        if (component != nullptr) {
            if (luaL_isstring(L, 2)) {
                const char *s = lua_tostring(L, 2);
                if (unlikely(strcmp(s, "length") != 0)) {
                    luaL_error(L, "Invalid member for an array:%s", s);
                } else {
                    lua_pushinteger(L, env->GetArrayLength((jarray) obj->object));
                }
            } else {
                pushArrayElement(L, context, obj, component);
            }
            return 1;
        }
    }
    if (unlikely(!luaL_isstring(L, 2))){
        if(!isStatic&&(lua_isnumber(L,2)||testType(L,2,OBJECT_KEY))){
            return pushMapValue(L,context,env,obj->object);
        }
        luaL_error(L, "Invaild type to get a field or method:%s", luaL_typename(L, 2));
    }

    FakeString name(lua_tostring(L, 2));
    auto member=type->ensureMember(env,name,isStatic);
    JavaType::FieldArray* fieldArr=nullptr;
    bool isMethod= false;
    int fieldCount=0;
    if(member){
        fieldArr = &member->fields ;
        isMethod =member->methods.size() != 0 ;
        fieldCount = fieldArr->size();
    }
    if (fieldCount == 1 && !isMethod) {
        auto&& info = fieldArr->at(0);
        JavaType *fieldType = info.type.rawType;
#define GetField(typeID,jtype, jname, TYPE)\
        case JavaType::typeID:{\
            lua_push##TYPE(L,isStatic?env->GetStatic##jname##Field(type->getType()\
                    ,info.id):env->Get##jname##Field(obj->object,info.id));\
            break;\
        }
#define GetIntegerField(typeID,jtype, jname) GetField(typeID,jtype,jname,integer)
#define GetFloatField(typeID,jtype, jname) GetField(typeID,jtype,jname,number)

#if LUA_VERSION_NUM >= 503
#define GetInteger64Field() GetIntegerField(LONG,long,Long)
#else
#define GetInteger64Field()\
    case JavaType::LONG:{\
        jlong v=isStatic?env->GetStaticLongField(type->getType()\
                    ,info.id):env->GetLongField(obj->object,info.id);\
        if(int64_t(double(v))!=v)\
            lua_pushnumber(L,v);\
        else Integer64::pushLong(L,v);\
        break;\
    }
#endif


#define PushField()
        switch(fieldType->getTypeID()){\
            GetIntegerField(INT,int,Int)\
            GetIntegerField(BYTE,byte,Byte)\
            GetInteger64Field()\
            GetIntegerField(SHORT,short,Short)\
            GetFloatField(FLOAT,float,Float)\
            GetFloatField(DOUBLE,double,Double)\
            GetField(BOOLEAN,boolean,Boolean,boolean)\
            case JavaType::CHAR:{\
            jchar c=isStatic?env->GetStaticCharField(type->getType(),info.id):env->GetCharField(obj->object,info.id);\
            PushChar(c);\
            }\
            default:{\
            JObject object=isStatic?env->GetStaticObjectField(type->getType(),info.id):env->GetObjectField(obj->object,info.id);\
            if(object==nullptr) lua_pushnil(L);else pushJavaObject(L,context,object);\
            }}
        PushField();
    } else {
        if (!isMethod && unlikely(fieldCount == 0)) {
            if(strcmp(name,"class")==0){
                pushJavaObject(L,context,type->getType());
                return 1;
            }else if(!isStatic&&strcmp(name,"instanceof")==0){
                lua_pushlightuserdata(L,context);
                lua_pushvalue(L,1);
                lua_pushcclosure(L,isInstanceOfCall,2);
                return 1;
            }else if(isStatic){
                if(strcmp(name,"new")==0){
                    lua_pushvalue(L,1);
                    lua_pushcclosure(L,newCall,1);
                    return 1;
                }else if(strcmp(name,"assignableFrom")==0){
                    lua_pushlightuserdata(L,context);
                    lua_pushlightuserdata(L,type);
                    lua_pushcclosure(L,isAssignableFromCall,2);
                    return 1;
                }
            }
            if(isStatic)
                luaL_error(L, "No static member is named %s in class %s", name.data(),
                            type->name(env).str());
            else{
                auto getter=type->findMockName(env,name, true);
                if(getter){
                    lua_pushstring(L,getter);
                    lua_replace(L,2);
                    pushMember(context,L,false,0,true);
                    lua_call(L,0,1);
                    return 1;
                } else
                    return pushMapValue(L,context,env,obj->object);
            }
            return 0;
        }
        pushMember(context, L, isStatic, fieldCount, isMethod);
    }
    return 1;
}

int getObjectLength(lua_State *L) {
    JavaObject *objRef = (JavaObject *)(lua_touserdata(L, 1));
    ThreadContext *context = getContext(L);
    auto env= context->env;
    if (objRef->type->isArray(env)) lua_pushinteger(L, env->GetArrayLength((jarray) objRef->object));
    else {
        static jmethodID sLength = env->GetMethodID(contextClass, "length", "(Ljava/lang/Object;)I");
        int len = env->CallIntMethod(context->scriptContext->javaRef, sLength, objRef->object);
        HOLD_JAVA_EXCEPTION(context,{ throwJavaError(L,context); });
        lua_pushinteger(L, len);
    }
    return 1;
}

int getField(lua_State *L) {
    MemberFlag *flag = (MemberFlag *) lua_touserdata(L, FLAG_INDEX);

    const char *name = lua_tostring(L, NAME_INDEX);
    if (unlikely(!flag->isField)) {
        luaL_error(L, "The member %s is not a field", name);
    }
    bool isStatic = flag->isStatic;
    JavaObject *obj = isStatic ? nullptr : (JavaObject *) lua_touserdata(L, OBJ_INDEX);
    JavaType *type = isStatic ? *(JavaType **) lua_touserdata(L, OBJ_INDEX) : obj->type;

    JavaType **fieldTypeRef;
    if (unlikely(flag->isDuplicatedField) && (fieldTypeRef = (JavaType **) testUData(L, 2, TYPE_KEY)) ==
                                   nullptr) {
        luaL_error(L, "The class has duplicated field name %s", name);
    }
    auto env=flag->context->env;
    auto&& info = *type->findField(env,FakeString(name), isStatic,
                                flag->isDuplicatedField ? nullptr : *fieldTypeRef);
    JavaType *fieldType = info.type.rawType;
    PushField();
    return 1;
}

int setField(lua_State *L) {
    MemberFlag *flag = (MemberFlag *) lua_touserdata(L, FLAG_INDEX);
    ThreadContext* context=flag->context;

    const char *name = lua_tostring(L, NAME_INDEX);
    if (unlikely(!flag->isField)) {
        luaL_error(L, "The member %s is not a field", name);
    }
    bool isStatic = flag->isStatic;
    JavaObject *objRef = isStatic ? nullptr : (JavaObject *) lua_touserdata(L, OBJ_INDEX);
    JavaType *type = isStatic ? *(JavaType **) lua_touserdata(L, OBJ_INDEX) : objRef->type;

    JavaType **fieldTypeRef;
    if (unlikely(flag->isDuplicatedField) && (fieldTypeRef = (JavaType **)
            testUData(L, 2, TYPE_KEY)) == nullptr) {
        luaL_error(L, "The class has duplicated field name %s", name);
    }
    auto env=context->env;
    auto info = type->findField(env,FakeString(name), isStatic,
                                flag->isDuplicatedField ? nullptr : *fieldTypeRef);
    JavaType *fieldType = info->type.rawType;
    ValidLuaObject luaObject;
    if (unlikely(!parseLuaObject(L, context, 3, luaObject))) {
        luaL_error(L, "Invalid value passed to java as a field with type:%s", luaL_typename(L, 3));
    }
    checkLuaType(env, L, fieldType, luaObject);
#define RawSetField(jname, NAME)({\
        if(isStatic) env->SetStatic##jname##Field(type->getType(),info->id,NAME);\
        else env->Set##jname##Field(objRef->object,info->id,NAME);})
#define SetField(typeID,jtype, jname, NAME)\
    case JavaType::typeID:{\
        RawSetField(jname,(luaObject.NAME));\
        break;\
    }
#define SetIntegerField(typeID,jtype, jname) SetField(typeID,jtype,jname,integer)
#define SetFloatField(typeID,jtype, jname) SetField(typeID,jtype,jname,number)
#define SET_FIELD()
    switch(fieldType->getTypeID()){\
        SetIntegerField(INT,int,Int)\
        SetField(BOOLEAN,boolean,Boolean,isTrue)\
        SetIntegerField(LONG,long,Long)\
        SetFloatField(FLOAT,float,Float)\
        SetFloatField(DOUBLE,double,Double)\
        SetIntegerField(BYTE,byte,Byte)\
        SetIntegerField(SHORT,short,Short)\
        case JavaType::CHAR:{\
            char16_t  s;\
            strcpy8to16(&s,luaObject.string, nullptr);\
            RawSetField(Char,s);\
        }\
        default:{\
            jvalue v= context->luaObjectToJValue(luaObject,fieldType,info->type.realType);\
            RawSetField(Object,v.l);\
            if(luaObject.type==T_FUNCTION||luaObject.type==T_STRING) env->DeleteLocalRef(v.l);\
        }}/*\
        HOLD_JAVA_EXCEPTION(context,{throwJavaError(L,context);});// jni doesn't seem to throw on mismatched type*/

    SET_FIELD()
    return 0;
}

int setFieldOrArray(lua_State *L) {
    bool isStatic = isJavaTypeOrObject(L,1);
    JavaObject *objRef = isStatic ? nullptr : (JavaObject*) lua_touserdata(L,1);
    JavaType *type = isStatic ? *(JavaType**) lua_touserdata(L,1) : objRef->type;
    ThreadContext *context = getContext(L);
    auto env=context->env;
    if (!isStatic) {
        JavaType *component = objRef->type->getComponentType(env);
        if (component != nullptr) {
            type = component;
            int isnum;
            jlong index = lua_tointegerx(L, 2, &isnum);
            if (unlikely(!isnum))
                luaL_error(L, "Invalid Value to set a array:%s", luaL_tolstring(L, 2, nullptr));
            if (index < 0 || index > INT32_MAX) luaL_error(L, "Index out of range:%lld", index);
            ValidLuaObject luaObject;
            parseLuaObject(L, context, 3, luaObject);
            checkLuaType(env, L, type, luaObject);

#define RAW_SET_ARR(jtype, jname, Ref) env->Set##jname##ArrayRegion((j##jtype##Array) objRef->object, (jsize) index, 1,Ref)
#define SET_ARR(typeID,jtype, jname, NAME) \
            case JavaType::typeID:{ j##jtype* ref=(j##jtype*) &luaObject.NAME;RAW_SET_ARR(jtype,jname,ref);break;}
#define SET_INTEGER_ARR(typeID,jtype, jname) SET_ARR(typeID,jtype,jname,integer)
#define SET_FLOAT_ARR(typeID,jtype, jname) SET_ARR(typeID,jtype,jname,number)
            switch (type->getTypeID()){
                SET_INTEGER_ARR(BYTE,byte, Byte)
                SET_INTEGER_ARR(SHORT,short, Short)
                SET_INTEGER_ARR(INT,int, Int)
                SET_INTEGER_ARR(LONG,long, Long)
                SET_FLOAT_ARR(FLOAT,float, Float)
                SET_FLOAT_ARR(DOUBLE,double, Double)
                SET_ARR(BOOLEAN,boolean, Boolean, isTrue)
                case JavaType::CHAR:{
                    char16_t s[strlen8to16(luaObject.string)];
                    strcpy8to16(s,luaObject.string, nullptr);
                    RAW_SET_ARR(char, Char, (jchar*)s);}
                default: {
                    jobject v = context->luaObjectToJValue(luaObject, type).l;
                    if (unlikely(v == INVALID_OBJECT)) lua_error(L);
                    env->SetObjectArrayElement((jobjectArray) objRef->object, (jsize) index, v);
                    cleanArg(env, v, luaObject.shouldRelease);
                }
            }
            HOLD_JAVA_EXCEPTION(context,{throwJavaError(L,context);});
            return 0;
        }
    }
    if (unlikely(!luaL_isstring(L, 2))){
        if(!isStatic&&(lua_isnumber(L,2)||testType(L,2,OBJECT_KEY))){
            setMapValue(L,context,env,objRef->object);
            return 0;
        }
        luaL_error(L, "Invalid index for a field member:%s",
                   luaL_tolstring(L, 2, NULL));
    }

    const FakeString name(lua_tostring(L, 2));
    auto arr=type->ensureField(env,name, isStatic);
    if (arr== nullptr){
        if(!isStatic){
            auto setter=type->findMockName(env,name, false);
            if(setter){
                lua_pushvalue(L,1);
                lua_pushstring(L,setter);
                pushMember(context,L,false,0,true);
                lua_pushvalue(L,3);
                lua_call(L,1,0);
                return 0;
            }
            setMapValue(L,context,env,objRef->object);
            return 0;
        }
        luaL_error(L,"No such field");
    }
    if (arr->size() > 1) luaL_error(L,"The name %s repsents not only one field", name.data());
    auto &&info = arr->at(0);
    JavaType *fieldType = info.type.rawType;
    ValidLuaObject luaObject;
    if (unlikely(!parseLuaObject(L, context, 3, luaObject))) {
        luaL_error(L,"Invalid value passed to java as a field with type:%s",
                       luaL_tolstring(L, 3, NULL));
    }
    checkLuaType(env, L, fieldType, luaObject);

    SET_FIELD();
    return 0;


}

int objectEquals(lua_State *L) {
    JavaObject *ob1 = (JavaObject *) testUData(L, 1, OBJECT_KEY);
    JavaObject *ob2 = (JavaObject *) testUData(L, 2, OBJECT_KEY);
    if (ob1 == nullptr || ob2 == nullptr) {
        lua_pushboolean(L, false);
    } else lua_pushboolean(L, getContext(L)->env->IsSameObject(ob1->object, ob2->object));
    return 1;
}

int concatString(lua_State *L) {
    size_t len1 = 0;
    size_t len2 = 0;
    const char *s1 = luaL_tolstring(L, 1, &len1);
    const char *s2 = luaL_tolstring(L, 2, &len2);
    lua_pushfstring(L,"%s%s", s1,s2);
    return 1;
}

int javaTypeToString(lua_State *L) {
    JavaType *type = *(JavaType**)lua_touserdata(L,1);
    auto env=getContext(L)->env;
    lua_pushfstring(L,"Java Type:%s", type->name(env).str());
    return 1;
}

int javaObjectToString(lua_State *L) {
    JavaObject *ob = (JavaObject*)lua_touserdata(L,1);
    ThreadContext *context = getContext(L);
    auto env= context->env;
    JString str = env->CallObjectMethod(ob->object, objectToString);
    HOLD_JAVA_EXCEPTION(context,{throwJavaError(L,context);});
    lua_pushstring(L, str);
    return 1;
}

int funcWriter(lua_State *, const void *p, size_t sz, void *ud) {
    Vector<char> *holder = (Vector<char> *) ud;
    for (int i = 0; i < sz; ++i) {
        holder->push_back(((const char *) p)[i]);
    }
    return 0;
}

FuncInfo *saveLuaFunction(lua_State *L, ThreadContext *context, int funcIndex) {
    typedef Vector<std::pair<int, FuncInfo *>> MY_VECTOR;
    static thread_local MY_VECTOR *parsedFuncs;
    bool isOwner = false;
    if (parsedFuncs == nullptr) {
        parsedFuncs = new MY_VECTOR();
        isOwner = true;
    }
    for (auto &pair:*parsedFuncs) {
        if (lua_rawequal(L, -1, pair.first))
            return pair.second;
    }
    lua_pushvalue(L, funcIndex);
    bool isCFunc = lua_iscfunction(L, -1) == 1;
    FuncInfo *ret;
    if (!isCFunc) {
        Vector<char> holder;
#if LUA_VERSION_NUM >= 503
#define STRIP ,1
#else
#define STRIP
#endif
        if (unlikely(lua_dump(L, funcWriter, &holder STRIP) != 0)) {
            lua_pop(L, 1);
            return nullptr;//unknown error;
        }
#undef STRIP
        ret = new FuncInfo(Array<char>(holder));
    } else {
        ret = new FuncInfo(lua_tocfunction(L, -1));
    }
    lua_pop(L, 1);
    parsedFuncs->push_back(std::make_pair(funcIndex, ret));
    Vector<CrossThreadLuaObject> upvalues;
#if LUA_VERSION_NUM>502
    lua_rawgeti(L,LUA_REGISTRYINDEX,LUA_RIDX_GLOBALS);
#endif
    for (int i = 1;; ++i) {
        if ((lua_getupvalue(L, funcIndex, i)) == NULL)
            break;
        CrossThreadLuaObject object;
#if LUA_VERSION_NUM>=502
        if(lua_rawequal(L,-1,-2))
            ret->globalIndex=i;
        else
#endif
            parseCrossThreadLuaObject(L, context, lua_gettop(L), object);
        upvalues.push_back(std::move(object));
        lua_pop(L,1);
    }
#if LUA_VERSION_NUM>502
    lua_pop(L,1);
#endif
    ret->setUpValues(Array<CrossThreadLuaObject>(std::move(upvalues)));

    ret->setImport(context->getImport());
    if (isOwner) {
        delete parsedFuncs;
        parsedFuncs = nullptr;
    }
    return ret;
}

void loadLuaFunction(TJNIEnv *env, lua_State *L, const FuncInfo *info, ScriptContext *context) {
    typedef Map<const FuncInfo *, int> MY_MAP;
    static thread_local MY_MAP *loadedFuncs;
    bool isOwner = false;
    if (loadedFuncs == nullptr) {
        loadedFuncs = new MY_MAP();
        isOwner = true;
    }
    const auto &iter = loadedFuncs->find(info);
    if (iter != loadedFuncs->end()) {
        lua_pushvalue(L, (*iter).second);
        return;
    }
    if (info->isCFunc) {
        uint32_t count = info->getUpValues().size();
        for (int i = 0; i < count; ++i) {
            lua_pushnil(L);
        }
        lua_pushcclosure(L, info->cFunc, count);
    } else {
        luaL_loadbuffer(L, &info->funcData.at(0), info->funcData.size(), "");
    }
    int funcIndex = lua_gettop(L);
    loadedFuncs->emplace(info, funcIndex);
    auto &upvalues = info->getUpValues();

    for (int i = upvalues.size() - 1; i >= 0; --i) {
        CrossThreadLuaObject &luaObject = upvalues[i];
        if (!pushLuaObject(env, L, context, luaObject))continue;
        lua_setupvalue(L, funcIndex, i + 1);//start at the second;
    }
#if LUA_VERSION_NUM>502
    if(info->globalIndex>0){
        lua_rawgeti(L,LUA_REGISTRYINDEX,LUA_RIDX_GLOBALS);
        lua_setupvalue(L,funcIndex,info->globalIndex);
    }
#endif
    if (isOwner) {
        delete loadedFuncs;
        loadedFuncs = nullptr;
    }
}

jobject LazyTable::asInterface(ThreadContext *context, JavaType *main) {
    Vector<JavaType *> interfaces;
    Vector<std::unique_ptr<BaseFunction>> luaFuncs;
    Vector<JObject> agentMethods;
    if (!readProxyMethods(L, context, interfaces, main, luaFuncs, agentMethods))
        return INVALID_OBJECT;
    return context->proxy(main, nullptr, agentMethods, luaFuncs);
}

LuaTable<ValidLuaObject> *LazyTable::getTable(ThreadContext *context) {
    if (table != nullptr)
        return table;
    lua_pushvalue(L, index);
    lua_rawget(L, LUA_REGISTRYINDEX);
    LuaTable<ValidLuaObject> *luaTable;
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        luaTable = new LuaTable<ValidLuaObject>();
        int len =(int) lua_rawlen(L, index);
        luaTable->get().reserve(len);
        lua_pushvalue(L, index);
        lua_pushlightuserdata(L, luaTable);
        lua_rawset(L, LUA_REGISTRYINDEX);
        lua_pushnil(L);
        while (lua_next(L, index)) {
            ValidLuaObject key;
            ValidLuaObject value;
            bool ok = parseLuaObject(L, context, -2, key) &&
                      parseLuaObject(L, context, -1, value);
            lua_pop(L, 1);
            if (ok)
                luaTable->get().push_back({std::move(key), std::move(value)});
            else {
                lua_pushvalue(L, index);
                lua_pushnil(L);
                lua_rawset(L, LUA_REGISTRYINDEX);
                delete luaTable;
                return nullptr;
            }
        }
        lua_pushvalue(L, index);
        lua_pushnil(L);
        lua_rawset(L, LUA_REGISTRYINDEX);
    } else {
        luaTable = static_cast<LuaTable<ValidLuaObject> *>(lua_touserdata(L, -1));
        lua_pop(L, -1);
    }
    table = luaTable;
    return luaTable;
}


void ScriptContext::pushAddedObject(TJNIEnv *env, lua_State *L, const char *name, const char *methodName,
                                    jobject obj) {
    if (methodName == nullptr || methodName[0] == 0) {
        if (obj == nullptr) {
            lua_pushnil(L);
        } else {
            pushJavaObject(L, env, this, obj, nullptr);
        }
        lua_setglobal(L, name);
    } else if (obj != nullptr) {
        int top = lua_gettop(L);
        bool isStatic = false;
        if (env->IsInstanceOf(obj, classType)) {
            JavaType *type = ensureType(env, (jclass) obj);
            *(JavaType **) lua_newuserdata(L, sizeof(JavaType *)) = type;
            isStatic = true;
        } else {
            pushJavaObject(L, env, this, obj, nullptr);
        }
        lua_pushstring(L, methodName);
        pushMember(getThreadContext(), L, isStatic, 0, true);

        lua_setglobal(L, name);
        lua_settop(L, top);
    }
}

static void registerNativeMethods(JNIEnv *env) {
    jclass scriptClass = env->FindClass("com/oslorde/luadroid/ScriptContext");
    env->RegisterNatives(scriptClass, nativeMethods,
                         sizeof(nativeMethods) / sizeof(JNINativeMethod));
    int sdk=getSDK();
    if(sdk>25){
        DexResolver::init();
        JNINativeMethod method[]={JNINativeMethod{"getBootClassList","()[[Ljava/lang/String;",(void*)DexResolver::getAllBootClasses}};
        env->RegisterNatives(scriptClass,method,1);
    }
    if(sdk>=21){
        JNINativeMethod method[]={JNINativeMethod{"getClassList","(Ljava/lang/Object;)[[Ljava/lang/String;",(void*)DexResolver::getClassList}};
        env->RegisterNatives(scriptClass,method,1);
    }
    env->DeleteLocalRef(scriptClass);
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *) {
    ::vm = vm;
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }
    registerNativeMethods(env);

    return JNI_VERSION_1_4;
}

jlong nativeOpen(TJNIEnv *env, jobject object, jboolean importAll) {
    ScriptContext *context = new ScriptContext(env, object, importAll);
    requireLogger([context, env](const char *data, bool isErr) {
        context->writeLog(data, isErr);
    });
    return reinterpret_cast<long>(context);
}

void registerLogger(TJNIEnv *env, jclass, jlong ptr, jobject out, jobject err) {
    ScriptContext *context = (ScriptContext *) ptr;
    if (context != nullptr) {
        context->registerLogger(env, out, err);
    };
}

void nativeClose(JNIEnv *, jclass, jlong ptr) {
    ScriptContext *context = (ScriptContext *) ptr;
    if (context != nullptr) {
        delete context;
        dropLogger();
    }
}

void nativeClean(JNIEnv *, jclass, jlong ptr) {
    ScriptContext *context = (ScriptContext *) ptr;
    if (context != nullptr)context->clean();
}

void releaseFunc(JNIEnv *, jclass, jlong ptr) {
    BaseFunction *info = (BaseFunction *) ptr;
    if (--info->javaRefCount == 0) {
        delete info;
    }
}

jint getClassType(TJNIEnv * env, jclass, jlong ptr,jclass clz) {
    ScriptContext *context = (ScriptContext *) ptr;
    return context->ensureType(env,clz)->getTypeID();
}

void addJavaObject(TJNIEnv *env, jclass, jlong ptr, jstring _name, jobject obj, jboolean local) {
    ScriptContext *context = (ScriptContext *) ptr;
    JString name(env, _name);
    context->addJavaObject(name, nullptr, env->NewGlobalRef(obj), local);
    name.invalidate();
}

void addJavaMethod(TJNIEnv *env, jclass, jlong ptr, jstring jname, jstring method, jobject jinst,
                   jboolean local) {
    if (jinst == nullptr) return;
    ScriptContext *context = (ScriptContext *) ptr;
    JString name(env, jname);
    JString methodName(env, method);
    context->addJavaObject(name, methodName, env->NewGlobalRef(jinst), local);
    name.invalidate();
    methodName.invalidate();
}

jlong compile(TJNIEnv *env, jclass, jlong ptr, jstring script, jboolean isFile) {
    ScriptContext *scriptContext = (ScriptContext *) ptr;
    ThreadContext* context=scriptContext->getThreadContext();
    auto old=context->changeScriptContext(scriptContext);
    auto L = scriptContext->getLua();
    JString s(env, script);
    int ret;
    if (isFile)ret = luaL_loadfile(L, s);
    else ret = luaL_loadstring(L, s);
    jlong retVal = 0;
    if (ret != LUA_OK) {
        context->setPendingException("Failed to load");
        recordLuaError(context, L, ret);
        context->changeScriptContext(old);
        context->throwToJava();
    } else {
        FuncInfo *info = saveLuaFunction(L, context, -1);
        info->javaRefCount++;
        retVal = reinterpret_cast<jlong >(info);
    }
    lua_pushcfunction(L, luaFullGC);
    lua_pcall(L, 0, 0, 0);
    s.invalidate();
    context->changeScriptContext(old);
    return retVal;
}

jobjectArray runScript(TJNIEnv *env, jclass, jlong ptr, jobject script, jboolean isFile,
                       jobjectArray args) {
    ScriptContext *scriptContext = (ScriptContext *) ptr;
    ThreadContext* context=scriptContext->getThreadContext();
    Import *oldImport= nullptr;
    ScriptContext* oldScriptContext=context->changeScriptContext(scriptContext);
    if (_setjmp(errorJmp)) {
        context->restore(oldScriptContext,oldImport);
        return nullptr;
    }
    Import myIMport;
    oldImport = context->changeImport(&myIMport);
    auto L = scriptContext->getLua();
    auto top = lua_gettop(L);
    int ret;
    int argCount;
    jobjectArray result = nullptr;
    pushErrorHandler(L,context);
    int handlerIndex = lua_gettop(L);
    if (env->IsInstanceOf(script, stringType)) {
        JString s(env, static_cast<jstring>(script));
        if (isFile)ret = luaL_loadfile(L, s);
        else {
            ret = luaL_loadstring(L, s);
        }
        s.invalidate();
    } else {
        FuncInfo *info = reinterpret_cast<FuncInfo *>(env->CallLongMethod(script, longValue));
        ret = luaL_loadbuffer(L, &info->funcData[0], info->funcData.size(), "");
    }
    if (unlikely(ret != LUA_OK)) {
        context->setPendingException("Failed to load");
        recordLuaError(context, L, ret);
        goto over;
    }
    argCount = args ?  env->GetArrayLength(args):0;
    for (int i = 0; i < argCount; ++i) {
        pushJavaObject(L, context, env->GetObjectArrayElement(args, i));
    }
    ret = lua_pcall(L, argCount, LUA_MULTRET, handlerIndex);
    if (unlikely(ret != LUA_OK)) {
        recordLuaError(context, L, ret);
        goto over;
    }
    {
        int resultSize = lua_gettop(L) - handlerIndex;
        if (resultSize > 0) {
            result = env->asJNIEnv()->NewObjectArray(resultSize, scriptContext->ObjectClass->getType(),
                                                     NULL);
            for (int i = resultSize - 1; i >= 0; --i) {
                ValidLuaObject object;
                parseLuaObject(L, context, handlerIndex + i + 1, object);
                jobject value = context->luaObjectToJObject(object);
                if (value != INVALID_OBJECT)
                    env->SetObjectArrayElement(result, i, JObject(env, value).get());
            }
        }
    }
    over:
    lua_settop(L, top);
    lua_pushcfunction(L, luaFullGC);
    lua_pcall(L, 0, 0, 0);

    context->restore(oldScriptContext,oldImport);
    return result;
}

jobject invokeLuaFunction(TJNIEnv *env, jclass, jlong ptr,
                          jboolean isInterface,
                          jlong funcRef, jobject proxy, jintArray argTypes, jobjectArray args) {
    ScriptContext *scriptContext = (ScriptContext *) ptr;
    ThreadContext* context=scriptContext->getThreadContext();
    Import *oldImport= nullptr;
    ScriptContext* oldScriptContext=context->changeScriptContext(scriptContext);
    if (_setjmp(errorJmp)) {
        context->restore(oldScriptContext,oldImport);
        return 0;
    }
    lua_State *L = scriptContext->getLua();
    pushErrorHandler(L,context);
    int handlerIndex = lua_gettop(L);
    if (!reinterpret_cast<BaseFunction *>(funcRef)->isLocal()) {
        FuncInfo *funcInfo = (FuncInfo *) funcRef;
        oldImport = context->changeImport(funcInfo->getImport());
        loadLuaFunction(env, L, funcInfo, scriptContext);
    } else {
        oldImport = context->getImport();
        LocalFunctionInfo *info = reinterpret_cast<LocalFunctionInfo *>(funcRef);
        lua_rawgetp(L, LUA_REGISTRYINDEX,info);
        if (unlikely(lua_isnil(L, -1))) {
            context->setPendingException(
                    "Local Function must run in the given thread it's extracted from");
            context->changeScriptContext(oldScriptContext);
            context->throwToJava();
            return 0;
        }
    }
    int len = env->GetArrayLength(argTypes);
    jint *arr = env->GetIntArrayElements(argTypes, nullptr);
    for (int i = 0; i < len; ++i) {
        switch (arr[i]) {
            case 0: {//char
                JObject character = env->GetObjectArrayElement(args, i);
                char16_t c = env->CallCharMethod(character, charValue);
                char charStr[4] ;
                strncpy16to8(charStr,&c, 1);
                lua_pushstring(L, charStr);
                break;
            }
            case 1: {//boolean
                JObject boolean = env->GetObjectArrayElement(args, i);
                jboolean b = env->CallBooleanMethod(boolean, booleanValue);
                lua_pushboolean(L, b);
                break;
            }
            case 2: {//integer
                JObject integer = env->GetObjectArrayElement(args, i);
                jlong intValue = env->CallLongMethod(integer, longValue);
                lua_pushinteger(L, intValue);
                break;
            }
            case 3: {//double
                JObject box = env->GetObjectArrayElement(args, i);
                jdouble floatValue = env->CallDoubleMethod(box, doubleValue);
                lua_pushnumber(L, floatValue);
                break;
            }
            default: {//object
                JObject obj = env->GetObjectArrayElement(args, i);
                if (obj == nullptr) {
                    lua_pushnil(L);
                } else pushJavaObject(L, context, obj);
                break;
            }
        }
    }
    env->ReleaseIntArrayElements(argTypes, arr, JNI_ABORT);

    pushJavaObject(L, context, proxy);
    if (!isInterface) {
        pushJavaObject(L, env, scriptContext, proxy, scriptContext->ensureType(
                        env, env->GetSuperclass(env->GetObjectClass(proxy))));
    }
    len += 1 + !isInterface;
    int err = lua_pcall(L, len, LUA_MULTRET, handlerIndex);
    jobject ret = nullptr;
    int retCount;
    if (err != LUA_OK)recordLuaError(context, L, err);
    else if ((retCount = lua_gettop(L) - handlerIndex) != 0) {
        bool multiRet =
                env->IsInstanceOf(proxy, context->FunctionType()->getType()) && isInterface;
        if (multiRet) {
            JObjectArray result(
                    env->NewObjectArray(retCount, scriptContext->ObjectClass->getType(), NULL));
            for (int i = handlerIndex + 1, top = lua_gettop(L), j = 0; i <= top; ++i, ++j) {
                ValidLuaObject object;
                parseLuaObject(L, context, i, object);
                jobject value = context->luaObjectToJObject( object);
                if (value != INVALID_OBJECT)
                    env->SetObjectArrayElement(result, j, JObject(env, value));
            }
            ret = result.invalidate();
        } else {
            ValidLuaObject object;
            parseLuaObject(L, context, -1, object);
            ret = context->luaObjectToJObject(object);
            if (ret == INVALID_OBJECT) ret = nullptr;
        }
    }
    lua_settop(L, handlerIndex - 1);
    lua_pushcfunction(L, luaFullGC);
    lua_pcall(L, 0, 0, 0);
    context->restore(oldScriptContext,oldImport);
    return ret;
}


