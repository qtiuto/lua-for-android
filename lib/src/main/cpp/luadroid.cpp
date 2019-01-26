
#include "luadroid.h"
#include "SpinLock.h"
#include "myarray.h"
#include "java_type.h"
#include "utf8.h"
#include "java_member.h"
#include "lua_object.h"
#include "log_wrapper.h"
#include "lfs.h"
#include "FakeVector.h"
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <setjmp.h>
#include <lua.h>
#include <dex/DexResolver.h>
#include <assert.h>
#include <dlfcn.h>
#if LUA_VERSION_NUM < 503
#include "int64_support.h"
#endif
#define TopErrorHandle(fmt, ...)  \
    ({lua_pushfstring(L,fmt,##__VA_ARGS__);\
    goto __ErrorHandle;})
#define ERROR(fmt, ...)  \
    ({lua_pushfstring(L,fmt,##__VA_ARGS__);\
    lua_error(L);})
#define SetErrorJMP()({if(false){\
__ErrorHandle:\
        lua_error(L);\
return 0;\
}})
#define luaL_isstring(L, i) (lua_type(L,i)==LUA_TSTRING)

static int javaType(lua_State *L);

static int javaInstanceOf(lua_State *L);

static int javaNew(lua_State *L);

static int javaNewArray(lua_State *L);

static int javaImport(lua_State *L);

static int javaUsing(lua_State*L);

static int javaIterate(lua_State*L);

static int javaCharValue(lua_State *L);

static int javaCharString(lua_State *L);

static int javaToJavaObject(lua_State *L);

static int javaProxy(lua_State *L);

static int javaSync(lua_State *L);

static int javaThrow(lua_State *L);

static int javaTry(lua_State *L);

static int javaUnBox(lua_State *L);

static int javaTypeOf(lua_State *L);

static int javaSuper(lua_State *L);

static int javaPut(lua_State *L);

static int javaGet(lua_State *L);

static int concatString(lua_State *L);

static int objectEquals(lua_State *L);

static int getObjectLength(lua_State *L);

static int javaTypeToString(lua_State *L) ;

static int javaObjectToString(lua_State *L);

static int getFieldOrMethod(lua_State *L);

static int setFieldOrArray(lua_State *L);

static int setField(lua_State *L);

static int getField(lua_State *L);

static int callMethod(lua_State *L);

static int callInitializer(lua_State *L);

static ThreadContext *getContext(lua_State *L);

static FuncInfo *saveLuaFunction(lua_State *L, ThreadContext *context, int funcIdx);

static bool parseLuaObject(lua_State *L, ThreadContext *context, int idx,
                           ValidLuaObject &luaObject);

static bool parseCrossThreadLuaObject(lua_State *L, ThreadContext *infcontext, int idx,
                                      CrossThreadLuaObject &luaObject);

static bool pushLuaObject(TJNIEnv *env, lua_State *L, ThreadContext *context,
                          const CrossThreadLuaObject &luaObject);

static inline void pushJavaType(lua_State *L,JavaType* type);

static void
loadLuaFunction(TJNIEnv *env, lua_State *L, const FuncInfo *info, ThreadContext *context);

static void checkLuaType(TJNIEnv *env, lua_State *L, JavaType *expected, ValidLuaObject &luaObject);

extern bool changeClassName(String &className);

static void pushMember(ThreadContext *context, lua_State *L,const Member* member,int toIndex, bool isStatic, int fieldCount, bool isMethod);

static void
pushArrayElement(lua_State *L, ThreadContext *context, const JavaObject *obj,
                 JavaType *component);

static void
readArguments(lua_State *L, ThreadContext *context, FakeVector<JavaType *> &types, FakeVector<ValidLuaObject> &objects,
              int start, int end);

static void parseTryTable(lua_State *L);

static void recordLuaError(ThreadContext *context, lua_State *L, int ret);

static LocalFunctionInfo *saveLocalFunction(lua_State *L, int i);

static bool
readProxyMethods(lua_State *L, ThreadContext *context, Vector<JavaType *> &interfaces,
                 JavaType *main, Vector<std::unique_ptr<BaseFunction>> &luaFuncs,
                 Vector<JObject> &agentMethods);

extern "C" {
jlong compile(TJNIEnv *env, jclass thisClass, jlong ptr, jstring script, jboolean isFile);
jlong nativeOpen(TJNIEnv *env, jobject object, jboolean importAll);
void registerLogger(TJNIEnv *, jclass, jlong ptr, jobject out, jobject err);
void nativeFlushLog(TJNIEnv *, jclass);
void nativeClose(JNIEnv *env, jclass thisClass, jlong ptr);
void referFunc(JNIEnv *env, jclass thisClass, jlong ptr, jboolean deRefer);
jint getClassType(TJNIEnv * env, jclass, jlong ptr,jclass clz);
jboolean sameSigMethod(JNIEnv* env,jclass,jobject f,jobject s,jobject caller);
void addJavaObject(TJNIEnv *env, jclass thisClass, jlong ptr, jstring _name, jobject obj,
                   jboolean local);
void addJavaMember(TJNIEnv *env, jclass thisClass, jlong ptr, jstring jname, jstring member,
                   jobject jinst, jclass type, jboolean local);
jobjectArray runScript(TJNIEnv *env, jclass thisClass, jlong ptr, jobject script,
                       jboolean isFile,
                       jobjectArray args);
jobject constructChild(TJNIEnv *env, jclass thisClass, jlong ptr, jclass target,
                       jlong nativeInfo);
jobject invokeSuper(TJNIEnv* env,jclass c,jobject thiz,jobject method,jint id,jobjectArray args);
jobject invokeLuaFunction(TJNIEnv *env, jclass thisClass, jlong ptr, jlong funcRef, jboolean multiRet, jobject proxy,
                          jstring methodName, jintArray argTypes, jobjectArray args);
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
         {"super",javaSuper},
         {"typeof",javaTypeOf},
         {nullptr,      nullptr}};

static const JNINativeMethod nativeMethods[] =
        {{"nativeOpen",        "(ZZ)J",                            (void *) nativeOpen},
         {"nativeClose",       "(J)V",                             (void *) nativeClose},
         //{"nativeClean",       "(J)V",                             (void *) nativeClean},
         {"registerLogger",    "(JLcom/oslorde/luadroid/Logger;"
                                       "Lcom/oslorde/luadroid/Logger;)V", (void *) registerLogger},
         {"nativeFlushLog","()V",(void*) nativeFlushLog},
         {"compile",           "(JLjava/lang/String;Z)J",          (void *) compile},
         {"runScript",         "(JLjava/lang/Object;Z"
                                       "[Ljava/lang/Object;"
                                       ")[Ljava/lang/Object;",     (void *) runScript},
         {"addObject",         "(JLjava/lang/String;"
                                       "Ljava/lang/Object;Z)V",    (void *) addJavaObject},
         {"addMember",         "(JLjava/lang/String;"
                                       "Ljava/lang/String;"
                                       "Ljava/lang/Object;"
                                       "Ljava/lang/Class;Z)V",     (void *) addJavaMember},
         {"constructChild",    "(JLjava/lang/Class;J)"
                                       "Ljava/lang/Object;",       (void *) constructChild},
         {"referFunc",       "(JZ)V",                             (void *) referFunc},
         {"getClassType",      "(JLjava/lang/Class;)I",            (void *) getClassType},
         {"sameSigMethod","(Ljava/lang/reflect/Method;Ljava/lang/reflect/Method;Ljava/lang/reflect/Method;)Z",(void *)sameSigMethod},
         {"invokeSuper","(Ljava/lang/Object;Ljava/lang/reflect/Method;I[Ljava/lang/Object;)Ljava/lang/Object;",(void*)invokeSuper},
         {"invokeLuaFunction", "(JJZLjava/lang/Object;Ljava/lang/String;"
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
#define JAVA_MEMBER "java_member"
class RegisterKey{};
static const RegisterKey* OBJECT_KEY= reinterpret_cast<const RegisterKey *>(javaInterfaces);
static const RegisterKey* TYPE_KEY=OBJECT_KEY+1;
static const RegisterKey* MEMBER_KEY=OBJECT_KEY+2;


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

static inline void lua_rawgetp(lua_State* L,int index,const void* p){
#ifndef __LP64__
    lua_rawgeti(L,index,(int)p);
#else
    lua_pushlightuserdata(L,(void*)p);
    lua_rawget(L,index);
#endif
}
static inline void lua_rawsetp(lua_State* L,int index,const void* p){
#ifndef __LP64__
    lua_rawseti(L,index,(int)p);
#else
    if(index<0&&index>LUA_REGISTRYINDEX){
        index=lua_gettop(L)+index+1;
    }
    lua_pushlightuserdata(L,(void*)p);
    lua_pushvalue(L,-2);
    lua_rawset(L,index);
    lua_pop(L,1);
#endif
}
static inline size_t lua_rawlen(lua_State*L,int index){
    return lua_objlen(L,index);
}
#define lua_setuservalue lua_setfenv
#define lua_getuservalue lua_getfenv

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
            ERROR( "'__tostring' must return a string");
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
        ERROR("Expected " JAVA_OBJECT ",but got %s",luaL_tolstring(L,idx,NULL));
    return ret;
}
static JavaType* checkJavaType(lua_State* L,int idx){
    JavaType** ret= static_cast<JavaType **>(testUData(L, idx, TYPE_KEY));
    if(unlikely(ret== nullptr))
        ERROR("Expected " JAVA_TYPE ",but got %s",luaL_tolstring(L,idx,NULL));
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
    lua_newtable(L);  /* create metatable */
    if(tname){
        lua_pushstring(L, tname);
        lua_setfield(L, -2, "__name");  /* metatable.__name = tname */
    }
    lua_pushvalue(L, -1);
    lua_rawsetp(L, LUA_REGISTRYINDEX, key);
    return true;
}
static int safeGC(lua_State*L){
    lua_gc(L, LUA_GCCOLLECT, 0);
    return 1;
}

static inline void luaFullGC(lua_State *L) {
    do{
        lua_pushcfunction(L,safeGC);
    } while(lua_pcall(L,0,0,0)!=LUA_OK);
    return ;
}

static void pushJavaObject(lua_State *L, TJNIEnv *env, ScriptContext *context, jobject obj, JavaType *given) {
#ifndef NDEBUG
    if(obj== nullptr){
        LOGE("Error put object");
        return ;
    }
#endif
    JavaObject *objectRef = (JavaObject *) lua_newuserdata(L, sizeof(JavaObject));
    objectRef->object = env->NewGlobalRef(obj);
    objectRef->type = given?given:context->ensureType(env, (JClass) env->GetObjectClass(obj));
    setMetaTable(L, OBJECT_KEY);
}

static inline  void pushJavaObject(lua_State *L, ThreadContext *context, jobject obj,JavaType* given= nullptr){

    if(((++context->pushedCount)&0x1FF)==0){// multi of 512
        luaFullGC(L);
    }

    if(context->pushedCount>>13){//>=8192
        lua_pushnil(L);
        return;
    }
    pushJavaObject(L, context->env, context->scriptContext, obj,given);
}

static void appendInt(String& str,int i){
    char tmp[8];
    snprintf(tmp,7,"%d",i);
    str.append(tmp);
}

static String traceback(lua_State *L, int level) {
    lua_Debug ar;
    String ret;
    ret.reserve(1024);
    ret+="\nstack traceback:";
    bool isOk= false;
    while (lua_getstack(L, level++, &ar)) {
        lua_getinfo(L, "Slnt", &ar);
        if (ar.currentline > 0){
            ret.append("\n\tat line ");
            appendInt(ret,ar.currentline)/*<<':'*/;
            if (*ar.namewhat != '\0')  /* is there a name from code? */
                ret.append(" in ").append( ar.namewhat).append(ar.name) ;  /* use it */
            else if (*ar.what == 'm')  /* main? */
                ret.append(" in main chunk");
            else if (*ar.what != 'C') {
                /* for Lua functions, use <file:line> */
                ret.append(" in function <").append(ar.short_src).push_back(':');
                appendInt(ret,ar.linedefined);
                ret.push_back('-');
                appendInt(ret,ar.lastlinedefined);
                ret.push_back('>');
            }
            if (ar.istailcall)
                ret.append("\n\t(...tail calls...)");
            isOk=true;
        }
    }
    if(isOk)
    return ret;
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


static int getLinkableLib(lua_State *L){
    const char *fullName = lua_tostring(L, 1);
    String name;
    const char* p=strchr(fullName,'.');
    void* handle= nullptr;
    if(p== nullptr){
        name=fullName;
    } else{
        name.append(fullName);
        replaceAll<'.','/'>(name);
        handle=dlopen(name.data(),RTLD_NOW);
        if(!handle){
            uint pos=name.rfind('/');
            name.insert(pos+1,"lib");
            handle=dlopen(name.data(),RTLD_NOW);
        }
        if(handle== nullptr){
            name.clear();
            name.append(fullName,0,p-fullName);
        }
    }
    if(!handle){
        name = "lib" + name + ".so";
        handle = dlopen(name.data(), RTLD_NOW);
    }
    if(handle== nullptr){
        name.erase(0,3);
        handle=dlopen(name.data(),RTLD_NOW);
    }
    if(handle== nullptr){
        lua_pushfstring(L,"\n\topen library %s failed",fullName);
        return 1;
    }
    String funcName;
    lua_CFunction  function= nullptr;
    const char* mark=strchr(fullName,'-');
    if(mark){
        funcName.append(fullName,0,mark-fullName);
        if(p) replaceAll<'.','_'>(funcName);
        funcName="luaopen_" + funcName;
        function= (lua_CFunction)(dlsym(handle, funcName.data()));
        if(function== nullptr){
            lua_pushfstring(L,"\n\tno function %s in library %s",funcName.data(),name.data());
            funcName.clear();
            funcName.append(mark+1);
        }
    } else funcName=fullName;
    if(!function){
        if(p) replaceAll<'.','_'>(funcName);
        funcName=("luaopen_" + funcName);
        function = (lua_CFunction)(dlsym(handle,funcName.data()));
    }
    if(function== nullptr){
        lua_pushfstring(L,"\n\tno function %s in library %s",funcName.data(),name.data());
        return 1;
    }
    lua_pushcfunction(L,function);
    return 1;
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
    lua_pushlightuserdata(L, context);//for panic and clean
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
        lua_setglobal(L, "type_");
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
        
        lua_pushlightuserdata(L,context);
        lua_pushcclosure(L, callInitializer,1);
        lua_setfield(L, index, "__call");
        //concat is simple
        lua_pushcfunction(L, concatString);
        lua_setfield(L, index, "__concat");

        lua_pushlightuserdata(L,context);
        lua_pushcclosure(L, JavaObject::objectGc,1);
        lua_setfield(L, index, "__gc");
    }
    for (auto &pair:addedMap) {
        pushAddedObject(context->env, L, pair.first.data(), pair.second);
    }
    pushJavaType(L,context->ensureType("com.oslorde.luadroid.ClassBuilder"));
    lua_pushvalue(L, -1);
    lua_setglobal(L, "ClassBuilder");

    luaL_openlibs(L);
    luaL_requiref(L,LFS_LIBNAME,luaopen_lfs, true);

    const char* loaderName;
#if LUA_VERSION_NUM>=502
    loaderName="searchers";
#else
    loaderName="loaders";
#endif
    lua_getglobal(L,"package");
    lua_getfield(L,-1,loaderName);
    int nextSearcher=lua_rawlen(L,-1);
    lua_pushlightuserdata(L,context);
    lua_pushcclosure(L, getLinkableLib, 1);
    lua_rawseti(L,-2,nextSearcher);
    lua_settop(L, top);
}

int JavaObject::objectGc(lua_State *L) {
    auto context=(ThreadContext*) lua_touserdata(L, lua_upvalueindex(1));
    auto ref = (JavaObject *) lua_touserdata(L, -1);
    context->env->DeleteGlobalRef(ref->object);
    context->pushedCount--;
    return 0;
}

ThreadContext::~ThreadContext() {
    if (scriptContext != nullptr) {
        JNIEnv* v;
        int err=vm->GetEnv((void**)&v,JNI_VERSION_1_4);
        if(likely(err==JNI_EDETACHED)){//most likely java thread detached first
            err=vm->AttachCurrentThread(&v,NULL);
            if(likely(!err)){
                env=(TJNIEnv*)v;
                scriptContext->removeCurrent();
                scriptContext = nullptr;
                vm->DetachCurrentThread();
            } else{//TODO:should I solve the problem? It doesn't seems to happen.
                LOGE("Failed to attach at exit=%d",err);
            }
        } else{
            scriptContext->removeCurrent();
            scriptContext = nullptr;
        }

    }
}

ScriptContext::~ScriptContext() {
    AutoJNIEnv env;
    _GCEnv=env;
    for (auto &&pair :typeMap) {
        delete pair.second;
    }
    ScopeLock sentry(gcLock);
    for (auto &&pair :stateMap){
        lua_State *L = pair.second;
        lua_getfield(L,LUA_REGISTRYINDEX,JAVA_CONTEXT);
        ThreadContext* context=(ThreadContext *)lua_touserdata(L, -1);
        context->scriptContext= nullptr;//to mark it as freed
        lua_close(L);
    }

    for (auto &&object:addedMap) {
        env->DeleteGlobalRef(object.second.obj);
    }
    env->DeleteWeakGlobalRef(javaRef);
}

static inline void pushJavaType(lua_State *L,JavaType* type){
    lua_rawgetp(L, LUA_REGISTRYINDEX, type);
    if( lua_isnil(L,-1)){
        lua_pop(L,1);
        *((JavaType **) lua_newuserdata(L, sizeof(JavaType *))) = type;
        setMetaTable(L, TYPE_KEY);
        lua_pushvalue(L,-1);
        lua_rawsetp(L,LUA_REGISTRYINDEX,type);
    }

}

static inline ThreadContext *getContext(lua_State *L) {
    return  (ThreadContext *) lua_touserdata(L, lua_upvalueindex(1));
}

static inline MemberInfo* getMemberInfo(lua_State* L){
    MemberInfo *ret = (MemberInfo *) lua_touserdata(L, FLAG_INDEX);
    return ret?ret:(MemberInfo*)lua_touserdata(L,1);

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
            if(expected->isBoxedFloat()||expected->isObjectClass()||expected->isNumberClass()){
                return false;
            }
        } else if(luaObject.type==T_INTEGER){
            if(expected->isBoxedInteger()||expected->isBoxedFloat()||expected->isObjectClass()||expected->isNumberClass())
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
    lua_pushfstring(L, "Expected type is %s,but receives %s",
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
            ERROR( "Arg unexpected");
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
    if (unlikely(!isnum)) ERROR( "Invalid index for an array");
    if (unlikely(index > INT32_MAX))ERROR( "Index out of range:%d", index);
    auto  env=context->env;
    switch (componentType->getTypeID()){
#define PushChar(c) ({ \
        char s[4];\
        strncpy16to8(s,(const char16_t *) &c, 1);\
        lua_pushstring(L,s);\
        break;\
        })
#define ArrayGet(typeID,jtype, jname, TYPE)\
            case JavaType::typeID:{\
                j##jtype buf;\
                env->Get##jname##ArrayRegion((j##jtype##Array) obj->object, (jsize) index, 1, &buf);\
                lua_push##TYPE(L,buf);\
                break;\
            }
#define IntArrayGet(typeID,jtype, jname) ArrayGet(typeID,jtype,jname,integer)
#if LUA_VERSION_NUM>=503
#define LongArrayGet() IntArrayGet(LONG,long, Long)
#else
#define LongArrayGet() case JavaType::LONG:{\
                jlong buf;\
                env->GetLongArrayRegion((jlongArray) obj->object, (jsize) index, 1, &buf);\
                if(int64_t(double(v))!=v)\
                    lua_pushnumber(L,v);\
                else Integer64::pushLong(L,v);\
                break;\
            }
#endif
#define FloatArrayGet(typeID,jtype, jname) ArrayGet(typeID,jtype,jname,number)
        IntArrayGet(BYTE,byte, Byte)
        IntArrayGet(SHORT,short, Short)
        IntArrayGet(INT,int, Int)
        LongArrayGet()
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

static inline void pushRawMethod(lua_State *L,int toIndex) {
    lua_pushvalue(L, toIndex);
    lua_pushcclosure(L, callMethod, 2);
}

void pushMember(ThreadContext *context, lua_State *L,const Member* member,int tOIndex, bool isStatic, int fieldCount, bool isMethod) {
    MemberInfo *flag = (MemberInfo *) lua_newuserdata(L, sizeof(MemberInfo));
    if(isStatic) flag->type=*(JavaType**)lua_touserdata(L,tOIndex);
    else flag->object=(JavaObject*)lua_touserdata(L,tOIndex);
    flag->member=member;
    flag->context=context;
    flag->isStatic = isStatic;
    flag->isNotOnlyMethod = fieldCount!=0;
    flag->isDuplicatedField = fieldCount > 1;
    if (fieldCount == 0) {
        pushRawMethod(L,tOIndex);
        return;
    }
#if LUA_VERSION_NUM>=502
    lua_pushvalue(L,tOIndex);
    lua_setuservalue(L,-2);
    if(newMetaTable(L,MEMBER_KEY,JAVA_MEMBER))
#else
    lua_createtable(L, 1, 3);//metatable;
    lua_pushvalue(L,tOIndex);
    lua_rawseti(L,-2,1);
#endif
    {
        int tableIndex = lua_gettop(L);
        if (fieldCount > 0) {
            lua_pushstring(L, "__index");
            lua_pushcclosure(L, getField, 0);
            lua_rawset(L, tableIndex);

            lua_pushstring(L, "__newindex");
            lua_pushcclosure(L, setField, 0);
            lua_rawset(L, tableIndex);
        }

        if (isMethod) {
            lua_pushstring(L, "__call");
            lua_pushcclosure(L, callMethod, 0);
            lua_rawset(L, tableIndex);
        }

#if LUA_VERSION_NUM >=502 //It ok to alert for once init
        lua_pushstring(L,"__metatable");
        lua_pushstring(L,"Can't change java metatable");
        lua_rawset(L,tableIndex);
#endif
    }


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
                luaTable->get().reserve(uint(lua_rawlen(L, idx)));
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
static void pushTable(TJNIEnv *env, lua_State *L, ThreadContext *context, LuaTable<CrossThreadLuaObject> *table);

static inline void pushMetaTable(TJNIEnv *env, lua_State *L, ThreadContext *context,
                                 LuaTable<CrossThreadLuaObject> *metaTable) {
    pushTable(env, L, context, metaTable);
    lua_setmetatable(L, -2);
}

static void pushTable(TJNIEnv *env, lua_State *L, ThreadContext *context, LuaTable<CrossThreadLuaObject> *table) {
    lua_rawgetp(L, LUA_REGISTRYINDEX, table);
    if (lua_isnil(L, -1)) {
        lua_pop(L, -1);
        lua_createtable(L, 0, table->get().size());
        lua_pushlightuserdata(L, table);
        lua_pushvalue(L, -2);
        lua_rawset(L, LUA_REGISTRYINDEX);
        for (const auto &pair:table->get()) {
            const CrossThreadLuaObject &key = pair.first;
            pushLuaObject(env, L, context, key);
            const CrossThreadLuaObject &value = pair.second;
            pushLuaObject(env, L, context, value);
            lua_rawset(L, -3);
        }
        if (table->metaTable != nullptr) {
            pushMetaTable(env, L, context, table->metaTable);
        }
        lua_pushlightuserdata(L, table);
        lua_pushnil(L);
        lua_rawset(L, LUA_REGISTRYINDEX);
    }
}
bool pushLuaObject(TJNIEnv *env, lua_State *L, ThreadContext *context,
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
            pushJavaObject(L, context, luaObject.objectRef->object);
            break;
        case T_FUNCTION:
            loadLuaFunction(env, L, luaObject.func, context);
            break;
        case T_TABLE:
            pushTable(env, L, context, luaObject.table);
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
        ERROR( "Invalid type=%s", luaL_tolstring(L, i,NULL));
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
    std::unique_ptr<BaseFunction> defaultFunc;
    lua_getfield(L, 1, "methods");
    if (lua_istable(L, -1)) {
        if (!readProxyMethods(L, context, interfaces, main, luaFuncs,
                              agentMethods)) {
            goto __ErrorHandle;
        }
    }
    lua_getfield(L,1,"all");
    if (lua_isfunction(L, -1)) {
        defaultFunc=context->isLocalFunction()?(BaseFunction*)saveLocalFunction(L,-1):saveLuaFunction(L,context,-1);
        defaultFunc->javaRefCount++;
    }
    lua_getfield(L, 1, "shared");
    bool shared;
    if (lua_isnil(L, -1)) shared = false;
    else if (lua_isboolean(L, -1)) shared = lua_toboolean(L, -1) != 0;
    else if (lua_isnumber(L, -1)) shared = lua_tointeger(L, -1) != 0;
    else shared = true;
    jobject proxy;
    if(superObject){
       proxy = context->proxy( main, &interfaces, agentMethods, luaFuncs,defaultFunc.get(), shared,
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
        proxy = context->proxy( main, &interfaces, agentMethods, luaFuncs,defaultFunc.get(), shared,
                               (long) &constructInfo);
    }
    if (proxy == INVALID_OBJECT) {
        pushJavaException(L,context);
        goto __ErrorHandle;
    }
    for (auto &ptr:luaFuncs) {
        ptr.release();
    }
    defaultFunc.release();
    pushJavaObject( L, context, JObject(env, proxy));
    return 1;
}

bool
readProxyMethods(lua_State *L, ThreadContext *context, Vector<JavaType *> &interfaces,
                 JavaType *main, Vector<std::unique_ptr<BaseFunction>> &luaFuncs,
                 Vector<JObject> &agentMethods) {
    uint expectedLen = (uint)lua_rawlen(L, -1);
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
    std::unique_ptr<BaseFunction> defaultFunc;
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
            } else{
                if(curMethodTypes.size()>0)
                    TopErrorHandle("No name specified for the proxy method");
                else break;
            }
            BaseFunction *function=isLocalFunction?(BaseFunction*)saveLocalFunction(L,j):saveLuaFunction(L,context,j);
            function->javaRefCount++;
            luaFuncs.emplace_back(function);
            curMethodTypes.clear();
            i = j + 1;//add stack index on success;
        } else
            break;
    }
    if(lua_isfunction(L,i)){
        defaultFunc=isLocalFunction?(BaseFunction*)saveLocalFunction(L,i):saveLuaFunction(L,context,i);
        defaultFunc->javaRefCount++;
        ++i;
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
        proxy = context->proxy(main, &interfaces, agentMethods, luaFuncs,defaultFunc.get(), shared,
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
        proxy = context->proxy( main, &interfaces, agentMethods, luaFuncs,defaultFunc.get(), shared,
                               (long) &constructInfo);
    }
    if (proxy == INVALID_OBJECT) {
        pushJavaException(L,context);
        goto __ErrorHandle;
    }
    for (auto &ptr:luaFuncs) {
        ptr.release();
    }
    defaultFunc.release();
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
                 import->stubbed[name]={type, DeleteOrNotString(pack)};
            } else {
                type=iter->second.type;
#ifndef NDEBUG
                if(strcmp(iter->second.pack,pack)!=0){
                    printf("name %s overloaded for new %s and old %s\n",name.data(),pack,iter->second.pack.get());
                }
#endif
            }
            if(!qualifyJavaName(name))continue;
            pushJavaType(L,type);
            lua_setglobal(L,name.data());
        }
        
    } else {
        JavaObject * loader=checkJavaObject(L,1);
        if(loaderClass== nullptr)
            loaderClass= static_cast<jclass>(env->NewGlobalRef(env->FindClass("java/lang/ClassLoader")));
        if(env->IsInstanceOf(loader->object,loaderClass)){
            static jmethodID  loadClassLoader=env->GetMethodID(contextClass,"loadClassLoader","(Ljava/lang/ClassLoader;)V");
            env->CallVoidMethod(context->scriptContext->javaRef,loadClassLoader,loader->object);
            HOLD_JAVA_EXCEPTION(context,{ return 0;});
            import->externalLoaders.push_back(env->NewGlobalRef(loader->object));
        }
    }
    return 0;
}

int javaImport(lua_State *L) {
    ThreadContext *context = getContext(L);
    SetErrorJMP();
    auto env=context->env;
    if (!luaL_isstring(L, -1)) ERROR( "Should pass a string for import");
    auto&& s=lua_tostring(L, -1);
    Import *import = context->getImport();
    if(s[0]=='[') TopErrorHandle("Don't import array type!");
    const char* separate = strrchr(s,'.');
    String pack;
    if (separate!= nullptr) {
        auto len = separate - s + 1;
        pack.resize(uint(len));
        memcpy(&pack[0],s,len);
    }
    size_t len=strlen(s);
    if (s[len-1]=='*'&&separate==s+len-2) {//ends with .*
        import->packages.insert(std::move(pack));
    } else {
        String name=separate?separate+1:s;
        auto &&iter = import->stubbed.find(name);
        if (iter != nullptr) {
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
            Import::TypeInfo info;
            info.type=type;
            auto&& existedPack=import->packages.find(pack);
            if(existedPack==nullptr){
                info.pack=DeleteOrNotString(pack,true);
            } else info.pack=DeleteOrNotString(*existedPack);
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
        ERROR( "Not a string but %s", luaL_typename(L, lua_type(L, -1)));
    }
    const char *s = lua_tostring(L, -1);
    if (strlen8to16(s) == 1) {
        char16_t ret;
        strcpy8to16(&ret, s, nullptr);
        lua_pushinteger(L, ret);
    } else ERROR( "the string has more than one char:%s", s);
    return 1;
}

int javaCharString(lua_State *L) {
    int isnum;
    jlong v = lua_tointegerx(L, -1, &isnum);
    if (!isnum)
        ERROR( "Not a integer");
    if (v > UINT16_MAX) ERROR( "Not a char value");
    char16_t s = (char16_t) v;
    char ret[4];
    strncpy16to8(ret,&s, 1);
    lua_pushstring(L, ret);
    return 1;
}

int newArray(lua_State *L, int index, ThreadContext *context, JavaType *type,JavaType* arrayType= nullptr) {
    jlong size = 0;
    if (type->isVoid()) {
        ERROR( "Type Error:array for void.class can't be created");
    }
    int isnum;
    size = lua_tointegerx(L, index++, &isnum);
    if (!isnum) {
        ERROR( "Type Error: not a integer but %s", luaL_typename(L, index));
    } else if (size > INT32_MAX || size < -1) {
        ERROR( "integer overflowed");
    }
    int top = lua_gettop(L);
    if (size!=-1&&top - index > size) {
        ERROR( "%d elements is too many for an array of size %d", top - index, size);
    }
    Vector<ValidLuaObject> elements;
    elements.reserve(static_cast<uint >(top - index + 1));
    for (; index <= top; ++index) {
        ValidLuaObject object;
        if (!parseLuaObject( L, context, index, object)) {
            forceRelease(elements);
            ERROR( "Arg unexpected for array");
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
    pushJavaObject(L, context, ret,arrayType);
    return 1;
}
int javaNew(lua_State *L) {
    ThreadContext *context = getContext(L);
    JavaType *type = checkJavaType(L,1);
    auto env=context->env;
    auto component = type->getComponentType(env);
    if (component != nullptr) {
        return newArray(L, 2, context, component,type);
    } else if(!type->isPrimitive()){
        int top=lua_gettop(L);
        uint expectedSize = uint (top - 1);
        JavaType* _types[expectedSize];
        ValidLuaObject _objects[expectedSize];
        FakeVector<JavaType *> types(_types,expectedSize);
        FakeVector<ValidLuaObject> objects(_objects,expectedSize);
        readArguments(L, context, types, objects, 2, top);
        JObject obj = JObject(env, type->newObject(context,types, objects));
        if (context->hasErrorPending()) {
            forceRelease(obj);
            types.release();
            throwJavaError(L,context);
        }
        pushJavaObject(L, context, obj.get(), type);
    } else
        ERROR("Primitive type can't make a new instance");
    return 1;
}

static int javaUnBox(lua_State* L){
    ThreadContext *context = getContext(L);
    int n=lua_gettop(L);
    for (int i = 1; i <=n ; ++i) {
        JavaObject* object=(JavaObject*) testUData(L,i,OBJECT_KEY);
        if(object== nullptr) continue;
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
    }
    return n;
}

static int javaTypeOf(lua_State *L){
    JavaObject* obj=checkJavaObject(L,1);
    pushJavaType(L,obj->type);
    return 1;
}

static int javaSuper(lua_State *L){
    JavaObject* obj=checkJavaObject(L,1);
    ThreadContext* context=getContext(L);
    ScriptContext *scriptContext = context->scriptContext;
    TJNIEnv *env = context->env;
    pushJavaObject(L, context, obj->object,
                   scriptContext->ensureType(env, env->GetSuperclass(obj->type->getType())));
    return 1;
}

static int javaNext(lua_State* L){
    ThreadContext *context = getContext(L);
    JavaObject* object= static_cast<JavaObject *>(lua_touserdata(L, 1));
    TJNIEnv *env = context->env;
    static jmethodID hasNext;
    static jmethodID nextEntry;
    if(hasNext== nullptr){
        JClass type(env->FindClass("com/oslorde/luadroid/MapIterator"));
        hasNext = env->GetMethodID(type, "hasNext", "()Z");
        nextEntry = env->GetMethodID(type, "nextEntry", "()[Ljava/lang/Object;");
    }
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
        lua_pushinteger(L,key+1);
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
        ERROR("Bad argument for iterator:%s",luaL_tolstring(L,1, nullptr));
    }
    lua_pushlightuserdata(L,context);
    lua_pushcclosure(L,javaNext,1);
    pushJavaObject(L,context,iterator);
    lua_pushinteger(L,-1);
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
    uint expectedSize=(uint)lua_gettop(L);
    JavaType* _types[expectedSize];
    ValidLuaObject _objects[expectedSize];
    FakeVector<JavaType *> types(_types,expectedSize);
    FakeVector<ValidLuaObject> luaObjects(_objects,expectedSize);
    readArguments(L, context, types, luaObjects, 1, expectedSize);
    int len=luaObjects.asVector().size();
    int i;
    for (i = 0; i < len; ++i) {
        if(_objects[i].type==T_NIL){
            lua_pushnil(L);
            continue;
        }
        JavaType* type=_types[i];
        if(type== nullptr){
            jobject obj=context->luaObjectToJObject(_objects[i]);
            if(likely(obj!=INVALID_OBJECT))
                pushJavaObject(L,context,JObject(env,obj));
            else{
                throwJavaError(L,context);
            }
            continue;
        } else {
            if(type->isPrimitive()){
                type=type->toBoxedType();
            }
            jvalue v=context->luaObjectToJValue(_objects[i],type);
            if(likely(v.l!=INVALID_OBJECT)){
                pushJavaObject(L,context,v.l);
                cleanArg(env,v.l,_objects[i].shouldRelease);
            }else{
                throwJavaError(L,context);
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
    if (top == 0) ERROR( "No args");
    bool noCatch = false;
    int finallyIndex = 0;
    if (top == 1&&lua_istable(L, 1))
        parseTryTable(L);
    if (lua_isnil(L, 1))
        ERROR( "No try body");
    top = lua_gettop(L);
    int catchAllIndex = 0;
    if (top == 1) noCatch = true;
    else if (top == 2) {
        noCatch = true;
        finallyIndex = 2;
    } else {
        int i;
        auto env=context->env;
        JavaType** typeRef;
        for (i = 2; i < top ; i += 2) {
            if ((typeRef=(JavaType**)testUData(L, i, TYPE_KEY))!= nullptr&&(*typeRef)->isThrowable(env)) {
                    continue;
            } else if (luaL_isstring(L, i)) {
                const char *thrType = lua_tostring(L, i);
                if(strcmp(thrType, "all") == 0) {
                    if (catchAllIndex == 0) catchAllIndex = i;
                    else
                        ERROR("More than one catch all functions");
                }else{
                    JavaType *type = context->ensureType(thrType);
                    if(type== nullptr||!type->isThrowable(env))
                        ERROR("Not an exception type: %s",thrType);
                    pushJavaType(L,type);
                    lua_replace(L,i);
                }
            } else {
                ERROR("Not an exception type:%s",luaL_tolstring(L,i,NULL));
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
        ERROR( "Invalid object %s with name %s to be pushed to cross thread table", s, name);
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
    if (object == nullptr||!pushLuaObject(context->env, L, context, *object)) {
        lua_pushnil(L);
    }
    return 1;
}


static JString getMemberName(TJNIEnv *env, const JObject &member) {
    static jmethodID getName=env->GetMethodID(env->GetObjectClass(member), "getName", "()Ljava/lang/String;");
    return (JString) env->CallObjectMethod(member,getName);
}

static JString getMethodName(TJNIEnv* env, jclass c, jmethodID id, jboolean isStatic){
    JObject member=env->ToReflectedMethod(c,id,isStatic);
    return getMemberName(env, member);
}
int callMethod(lua_State *L) {
    MemberInfo *memberInfo =getMemberInfo(L);
    ThreadContext* context=memberInfo->context;
    SetErrorJMP();
    bool isStatic = memberInfo->isStatic;
    JavaObject *objRef = isStatic ? nullptr : memberInfo->object;
    JavaType *type = isStatic ? memberInfo->type : objRef->type;
    int start=1 + memberInfo->isNotOnlyMethod;
    int top=lua_gettop(L);
    uint expectedSize = uint(top - (memberInfo->isNotOnlyMethod));
    JavaType* _types[expectedSize];
    ValidLuaObject _objects[expectedSize];
    FakeVector<JavaType *> types(_types,expectedSize);
    FakeVector<ValidLuaObject> objects(_objects,expectedSize);
    readArguments(L, context, types, objects, start,top);
    auto env=context->env;
    auto&& array=memberInfo->member->methods;
    bool gotVarMethod;
    auto info = type->deductMethod(env,&array, types, &objects.asVector(),&gotVarMethod);
    if (unlikely(info == nullptr)) {
        TopErrorHandle("No matched found for the method %s;->%s",type->name(env).str(),
                       getMethodName(env,type->getType(),array[0].id,isStatic).str());
    }
    int argCount = info->params.size();
    jvalue args[argCount];
    for (int i = argCount-gotVarMethod ; i-- !=0; ) {
        ValidLuaObject &object = _objects[i];
        ParameterizedType &tp = info->params[i];
        args[i] = context->luaObjectToJValue(object, tp.rawType,tp.realType);
        if (!tp.rawType->isPrimitive() && args[i].l == INVALID_OBJECT) {
            cleanArgs(args, argCount, objects, env);
            pushJavaException(L,context);
            goto __ErrorHandle;
        }
    }
    if(gotVarMethod){
        uint varCount=types.asVector().size()-argCount+1;
        FakeVector<ValidLuaObject> varArgs(_objects+argCount-1,varCount,varCount);
        jarray arr = info->varArgType.rawType->newArray(context, varCount, varArgs);
        if(arr== nullptr){
            cleanArgs(args, argCount-1, objects, env);
            pushJavaException(L,context);
            goto __ErrorHandle;
        }
        args[argCount - 1].l= arr;
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
    cleanArgs(args, argCount-gotVarMethod, objects, env);
    if(gotVarMethod) env->DeleteLocalRef(args[argCount-1].l);
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
        ERROR("invalid map value");
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
    lua_insert(L,1);
    lua_call(L,len,1);//fast call the type;
    return 1;
}

static int callExtendingMethod(lua_State* L){
    lua_pushvalue(L,lua_upvalueindex(1));
    lua_pushvalue(L,lua_upvalueindex(2));
    lua_insert(L,1);
    lua_insert(L,1);
    lua_call(L,lua_gettop(L)-1,1);
    return 1;
}

static inline void saveExistedMember(lua_State* L, bool force){
    lua_getuservalue(L,1);
    if(lua_isnil(L,-1)){
        if(!force)
            return;
        lua_pop(L,1);
        lua_newtable(L);
        lua_pushvalue(L,-1);
        lua_setuservalue(L,1);

    }
    lua_pushvalue(L,2);
    lua_pushvalue(L,3);
    lua_rawset(L,-3);
    lua_pop(L,1);
}


static inline bool tryExistedMember(lua_State* L,JavaType* type,bool isStatic){
    lua_getuservalue(L,1);

    if(lua_isnil(L,-1)){
        lua_pop(L,1);
        goto Handle_Object;
    }

    lua_pushvalue(L,2);
    lua_rawget(L,-2);
    if(lua_isnil(L,-1)){
        lua_pop(L,2);//nil and table
        goto Handle_Object;
    }
    return true;
    Handle_Object:
    if(isStatic)
        return false;
    lua_rawgetp(L,LUA_REGISTRYINDEX,reinterpret_cast<char *>(type)+1);
    if(lua_isnil(L,-1)){
        lua_pop(L,1);//nil
        return false;
    }
    lua_pushvalue(L,2);
    lua_rawget(L,-2);
    if(lua_isnil(L,-1)){
        lua_pop(L,2);//nil and table
        return false;
    }
    lua_pushvalue(L,1);
    lua_pushcclosure(L,callExtendingMethod,2);
    lua_remove(L,-2);
    saveExistedMember(L, true);
    return true;
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
                    goto MemberStart;
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
        if(!isStatic){
            return pushMapValue(L,context,env,obj->object);
        }
        ERROR( "Invaild type to get a field or method:%s", luaL_typename(L, 2));
    }
    MemberStart:
    if(tryExistedMember(L,type,isStatic))
        return 1;
    FakeString name(lua_tostring(L, 2));
    auto member=type->ensureMember(env,(const String&)name,isStatic);
    FieldArray* fieldArr=nullptr;
    bool isMethod= false;
    int fieldCount=0;
    if(member){
        fieldArr = &member->fields ;
        isMethod =member->methods.size() != 0 ;
        fieldCount = fieldArr->size();
    }
    if (fieldCount == 1 && !isMethod) {
        auto&& info = fieldArr->begin();
        JavaType *fieldType = info->type.rawType;
#define GetField(typeID,jtype, jname, TYPE)\
        case JavaType::typeID:{\
            lua_push##TYPE(L,isStatic?env->GetStatic##jname##Field(type->getType()\
                    ,info->id):env->Get##jname##Field(obj->object,info->id));\
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
                    ,info->id):env->GetLongField(obj->object,info->id);\
        if(int64_t(double(v))!=v)\
            lua_pushnumber(L,v);\
        else Integer64::pushLong(L,v);\
        break;\
    }
#endif


#define PushField()\
        switch(fieldType->getTypeID()){\
            GetIntegerField(INT,int,Int)\
            GetIntegerField(BYTE,byte,Byte)\
            GetInteger64Field()\
            GetIntegerField(SHORT,short,Short)\
            GetFloatField(FLOAT,float,Float)\
            GetFloatField(DOUBLE,double,Double)\
            GetField(BOOLEAN,boolean,Boolean,boolean)\
            case JavaType::CHAR:{\
            jchar c=isStatic?env->GetStaticCharField(type->getType(),info->id):env->GetCharField(obj->object,info->id);\
            PushChar(c);\
            }\
            default:{\
            JObject object=isStatic?env->GetStaticObjectField(type->getType(),info->id):env->GetObjectField(obj->object,info->id);\
            if(object==nullptr) lua_pushnil(L);else pushJavaObject(L,context,object);break;\
            }}
        PushField();
    } else {
        bool force= true;
        if (!isMethod && unlikely(fieldCount == 0)) {
            if(strcmp(name,"class")==0){
                pushJavaObject(L,context,type->getType());
                goto SAVE_AND_EXIT;
            }else if(!isStatic){
                if(strcmp(name,"instanceof")==0){
                    lua_pushlightuserdata(L,context);
                    lua_pushvalue(L,1);
                    lua_pushcclosure(L,isInstanceOfCall,2);
                    force=false;
                    goto SAVE_AND_EXIT;
                }else  if(strcmp(name,"super")==0){
                    pushJavaObject(L,context,obj->object,
                                   context->scriptContext->ensureType(env,env->GetSuperclass(type->getType())));
                    goto SAVE_AND_EXIT;
                }
                
            }else {
                if(strcmp(name,"new")==0){
                    lua_pushvalue(L,1);
                    lua_pushcclosure(L,newCall,1);
                    goto SAVE_AND_EXIT;
                }else if(strcmp(name,"assignableFrom")==0){
                    lua_pushlightuserdata(L,context);
                    lua_pushlightuserdata(L,type);
                    lua_pushcclosure(L,isAssignableFromCall,2);
                    goto SAVE_AND_EXIT;
                }
            }
            if(isStatic)
                ERROR( "No static member is named %s in class %s", name.data(),
                            type->name(env).str());
            else{
                auto getter= type->findMockMember(env, name, true);
                if(getter){
                    pushMember(context,L,getter,1,false,0,true);
                    lua_call(L,0,1);
                    return 1;
                } else
                    return pushMapValue(L,context,env,obj->object);
            }
            return 0;
        }
        pushMember(context, L,member,1, isStatic, fieldCount, isMethod);
        SAVE_AND_EXIT:
        saveExistedMember(L,force);
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
    MemberInfo *memberInfo =getMemberInfo(L);
    SetErrorJMP();
#ifndef NDEBUG
    if (unlikely(!memberInfo->isField)) {
        LOGE( "Not a field");
    }
#endif
    bool isStatic = memberInfo->isStatic;
    JavaObject *obj = isStatic ? nullptr :memberInfo->object;
    JavaType *type = isStatic ? memberInfo->type : obj->type;

    auto env=memberInfo->context->env;
    JavaType **fieldTypeRef;
    if (unlikely(memberInfo->isDuplicatedField) && (fieldTypeRef = (JavaType **) testUData(L, 2, TYPE_KEY)) ==
                                   nullptr) {
        TopErrorHandle( "The class has duplicated field named %s while no type specified", getMemberName(env, env->
                ToReflectedField(type->getType(),memberInfo->member->fields[0].id,isStatic)).str());
    }
    auto&& info = type->deductField(env,&memberInfo->member->fields,
                                memberInfo->isDuplicatedField ? *fieldTypeRef:nullptr);
    if(info== nullptr){
        TopErrorHandle( "The class doesn't have a field name %s with type %s", getMemberName(env, env->
                ToReflectedField(type->getType(),memberInfo->member->fields[0].id,isStatic)).str(),(*fieldTypeRef)->name(env).str());
    }
    JavaType *fieldType = info->type.rawType;
    auto context=memberInfo->context;
    PushField();
    return 1;
}

int setField(lua_State *L) {
    MemberInfo* memberInfo=getMemberInfo(L);
    ThreadContext* context=memberInfo->context;
    SetErrorJMP();
#ifndef NDEBUG
    if (unlikely(!memberInfo->isField)) {
        LOGE( "Not a field");
    }
#endif
    bool isStatic = memberInfo->isStatic;
    JavaObject *objRef = isStatic ? nullptr : memberInfo->object;
    JavaType *type = isStatic ? memberInfo->type : objRef->type;

    auto env=context->env;
    JavaType **fieldTypeRef;
    if (unlikely(memberInfo->isDuplicatedField) && (fieldTypeRef = (JavaType **)
            testUData(L, 2, TYPE_KEY)) == nullptr) {
        TopErrorHandle( "The class has duplicated field named %s while no type specified", getMemberName(env, env->
                ToReflectedField(type->getType(),memberInfo->member->fields[0].id,isStatic)).str());
    }
    auto info = type->deductField(env,&memberInfo->member->fields,
                                memberInfo->isDuplicatedField ? *fieldTypeRef  :nullptr);
    if(info== nullptr){
        TopErrorHandle( "The class doesn't have a field name %s with type %s", getMemberName(env, env->
                ToReflectedField(type->getType(),memberInfo->member->fields[0].id,isStatic)).str(),(*fieldTypeRef)->name(env).str());
    }
    JavaType *fieldType = info->type.rawType;
    ValidLuaObject luaObject;
    if (unlikely(!parseLuaObject(L, context, 3, luaObject))) {
        ERROR( "Invalid value passed to java as a field with type:%s", luaL_typename(L, 3));
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
#define SET_FIELD()\
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
    if(type->isPrimitive())
        ERROR("Primitive type is not allowed to be set");
    if (!isStatic) {
        JavaType *component = objRef->type->getComponentType(env);
        if (component != nullptr) {
            type = component;
            int isnum;
            jlong index = lua_tointegerx(L, 2, &isnum);
            if (unlikely(!isnum))
                ERROR( "Invalid Value to set a array:%s", luaL_tolstring(L, 2, nullptr));
            if (index < 0 || index > INT32_MAX) ERROR( "Index out of range:%lld", index);
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
        ERROR( "Invalid index for a field member:%s",
                   luaL_tolstring(L, 2, NULL));
    }

    const FakeString name(lua_tostring(L, 2));
    auto arr=type->ensureField(env,name, isStatic);
    if (arr== nullptr){
        if(!isStatic){
            auto setter= type->findMockMember(env, name, false);
            if(setter){
                pushMember(context, L, setter, 1, false, 0, true);
                lua_pushvalue(L,3);
                lua_call(L,1,0);
                return 0;
            }
            setMapValue(L,context,env,objRef->object);
            return 0;
        }else if(lua_isfunction(L,3)&&type->ensureMethod(env,name,false)== nullptr){
            char *key = reinterpret_cast<char*>(type) + 1;
            lua_rawgetp(L, LUA_REGISTRYINDEX, key);
            if(lua_isnil(L,-1)){
                lua_pop(L,1);
                lua_newtable(L);
                lua_pushvalue(L,-1);
                lua_rawsetp(L,LUA_REGISTRYINDEX,key);
            }
            lua_replace(L,1);
            lua_rawset(L,1);
            return 0;
        }
        ERROR("No such field");
    }
    if (arr->size() > 1) ERROR("The name %s represents not only one field", name.data());
    auto &&info = arr->begin();
    JavaType *fieldType = info->type.rawType;
    ValidLuaObject luaObject;
    if (unlikely(!parseLuaObject(L, context, 3, luaObject))) {
        ERROR("Invalid value passed to java as a field with type:%s",
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

static int callInitializer(lua_State *L){
    if(lua_gettop(L)!=2) return 0;
    if(!lua_istable(L,2)) return 0;
    lua_pushnil(L);
    while (lua_next(L,2)){
        lua_pushvalue(L,-2);
        lua_pushvalue(L,-2);
        lua_settable(L,1);
        lua_pop(L,1);
    }
    lua_pop(L,1);
    return 1;
}

int funcWriter(lua_State *, const void *p, size_t sz, void *ud) {
    Vector<char,0> *holder = (Vector<char,0> *) ud;
    for (int i = 0; i < sz; ++i) {
        holder->push_back(((const char *) p)[i]);
    }
    return 0;
}

FuncInfo *saveLuaFunction(lua_State *L, ThreadContext *context, int funcIndex) {
    typedef Vector<std::pair<int, FuncInfo *>> MY_VECTOR;
    bool isOwner = false;
    MY_VECTOR* current= context->getValue<MY_VECTOR>(ContextStorage::PARSED_FUNC);
    if (current == nullptr) {
        current = new MY_VECTOR(128);
        isOwner = true;
        context->setValue(ContextStorage::PARSED_FUNC, current);
    }
    funcIndex=funcIndex<0?lua_gettop(L)+funcIndex+1:funcIndex;
    for (auto &&pair:*current) {
        if (lua_rawequal(L, -1, pair.first))
            return pair.second;
    }
    lua_pushvalue(L, funcIndex);
    bool isCFunc = lua_iscfunction(L, -1) == 1;
    FuncInfo *ret;
    if (!isCFunc) {
        Vector<char> holder(256);
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
    current->push_back(std::make_pair(funcIndex, ret));
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
        delete current;
        context->setValue(ContextStorage::PARSED_FUNC, nullptr);
    }
    return ret;
}

void loadLuaFunction(TJNIEnv *env, lua_State *L, const FuncInfo *info, ThreadContext *context) {
    typedef Map<const FuncInfo *, int> MY_MAP;
    bool isOwner = false;
    MY_MAP* current= context->getValue<MY_MAP>(ContextStorage::LOADED_FUNC);
    if (current==nullptr) {
        current = new MY_MAP();
        context->setValue(ContextStorage::LOADED_FUNC, current);
        isOwner = true;
    }
    const auto &iter = current->find(info);
    if (iter != current->end()) {
        lua_pushvalue(L, iter->second);
        return;
    }
    if (info->isCFunc) {
        uint32_t count = info->getUpValues().size();
        for (int i = 0; i < count; ++i) {
            lua_pushnil(L);
        }
        lua_pushcclosure(L, info->cFunc, count);
    } else {
        luaL_loadbuffer(L, info->funcData.begin(), info->funcData.size(), "");
    }
    int funcIndex = lua_gettop(L);
    current->emplace(info, funcIndex);
    auto &upvalues = info->getUpValues();

    for (int i = upvalues.size(); i--;) {
        CrossThreadLuaObject &luaObject = upvalues[i];
        if (!pushLuaObject(env, L, context, luaObject))continue;
        lua_setupvalue(L, funcIndex, i + 1);//start at the second;
    }
#if LUA_VERSION_NUM>=502
    if(info->globalIndex>0){
        lua_rawgeti(L,LUA_REGISTRYINDEX,LUA_RIDX_GLOBALS);
        lua_setupvalue(L,funcIndex,info->globalIndex);
    }
#endif
    if (isOwner) {
        delete current;
        context->setValue(ContextStorage::LOADED_FUNC, nullptr);
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


void ScriptContext::pushAddedObject(TJNIEnv *env, lua_State *L, const char *name,const AddInfo& info) {
    if (info.member== nullptr) {
        if (info.obj == nullptr) {
            lua_pushnil(L);
        } else {
            pushJavaObject(L, env, this, info.obj, nullptr);
        }
        lua_setglobal(L, name);
    } else {
        int top = lua_gettop(L);
        bool isStatic = !info.obj;
        if (isStatic) {
            pushJavaType(L,info.type);
        } else {
            pushJavaObject(L, env, this, info.obj, info.type);
        }
        pushMember(getThreadContext(), L,info.member,top+1, isStatic, info.member->fields.size(), info.member->methods.size()>0);

        lua_setglobal(L, name);
        lua_settop(L, top);
    }
}

static void registerNativeMethods(JNIEnv *env) {
    jclass scriptClass = env->FindClass("com/oslorde/luadroid/ScriptContext");
    env->RegisterNatives(scriptClass, nativeMethods,
                         sizeof(nativeMethods) / sizeof(JNINativeMethod));
    int sdk=getSDK();
    if(sdk>=23){
        DexResolver::init();
        JNINativeMethod method[]={JNINativeMethod{"getBootClassList","()[[Ljava/lang/String;",(void*)DexResolver::getAllBootClasses}};
        env->RegisterNatives(scriptClass,method,1);
    }

    JNINativeMethod method[]={JNINativeMethod{"getClassList","(Ljava/lang/Object;)[[Ljava/lang/String;",(void*)DexResolver::getClassList}};
    env->RegisterNatives(scriptClass,method,1);

    env->DeleteLocalRef(scriptClass);
}
JNIEXPORT FILE* tmpfile(void){
    static const char* tmpDir;
    if(tmpDir== nullptr){
        AutoJNIEnv env;
        auto&& c=env->FindClass("java/lang/System");
        jmethodID mid=env->GetStaticMethodID(c,"getProperty","(Ljava/lang/String;)Ljava/lang/String;");
        JString key=env->NewStringUTF("java.io.tmpdir");
        JString s=env->CallStaticObjectMethod(c,mid,key.get());
        if(s.get()== nullptr){
            tmpDir="/data/local/tmp";
            if(access(tmpDir,W_OK)!=0){
                return NULL;
            }
        } else{
           char* tmp=new char[env->GetStringUTFLength(s)];
           strcpy(tmp,s.str());
           tmpDir=tmp;
        }
    }

    size_t dirLen = strlen(tmpDir);
    char tp[dirLen + 12];
    memcpy(tp,tmpDir,dirLen);
    tp[dirLen]='/';
    memcpy(tp+dirLen+1,"tmp_XXXXXX",11);
    int fd=mkstemp(tp);
    if(fd==-1){
        return nullptr;
    }
    return fdopen(fd,"r+");
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

static void loggerCallback(JNIEnv*env,const char* data,bool isErr,void* arg){
    ((ScriptContext *) arg)->writeLog((TJNIEnv *) env, data, isErr);
}

jlong nativeOpen(TJNIEnv *env, jobject object, jboolean importAll) {
    ScriptContext *context = new ScriptContext(env, object, importAll);
    requireLogger(loggerCallback,context);
    return reinterpret_cast<long>(context);
}
void nativeFlushLog(TJNIEnv *, jclass){
    fflush(stdout);
    fflush(stderr);
    fdatasync(STDOUT_FILENO);
    fdatasync(STDERR_FILENO);
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

void referFunc(JNIEnv *, jclass, jlong ptr, jboolean deRefer) {
    BaseFunction *info = (BaseFunction *) ptr;
    if(!deRefer){
      info->javaRefCount++;
    } else if (--info->javaRefCount == 0) {
        delete info;
    }
}
jboolean sameSigMethod(JNIEnv* env,jclass,jobject f,jobject s,jobject caller){
    static jmethodID mid=env->FromReflectedMethod(caller);
    return env->CallBooleanMethod(f,mid,s);
}

jint getClassType(TJNIEnv * env, jclass, jlong ptr,jclass clz) {
    ScriptContext *context = (ScriptContext *) ptr;
    return context->ensureType(env,clz)->getTypeID();
}

void addJavaObject(TJNIEnv *env, jclass, jlong ptr, jstring _name, jobject obj, jboolean local) {
    ScriptContext *context = (ScriptContext *) ptr;
    if(_name== nullptr) return;
    JString name(env, _name);
    context->addJavaObject(env, name, nullptr, env->NewGlobalRef(obj), nullptr, local);
    name.invalidate();
}

void addJavaMember(TJNIEnv *env, jclass, jlong ptr, jstring jname, jstring member, jobject jinst, jclass type,
                   jboolean local) {
    if ((!jinst&&!type)||member== nullptr||jname== nullptr) return;
    ScriptContext *context = (ScriptContext *) ptr;
    JString name(env, jname);
    JString memberName(env, member);
    context->addJavaObject(env, name, memberName, env->NewGlobalRef(jinst), context->ensureType(env,type), local);
    name.invalidate();
    memberName.invalidate();
}

jlong compile(TJNIEnv *env, jclass, jlong ptr, jstring script, jboolean isFile) {
    ScriptContext *scriptContext = (ScriptContext *) ptr;
    ThreadContext* context=scriptContext->getThreadContext();
    auto L = scriptContext->getLua();
    JString s(env, script);
    int ret;
    if (isFile)ret = luaL_loadfile(L, s);
    else ret = luaL_loadstring(L, s);
    jlong retVal = 0;
    if (ret != LUA_OK) {
        context->setPendingException("Failed to load");
        recordLuaError(context, L, ret);
        context->throwToJava();
    } else {
        FuncInfo *info = saveLuaFunction(L, context, -1);
        info->javaRefCount++;
        retVal = reinterpret_cast<jlong >(info);
    }
    luaFullGC(L);
    s.invalidate();
    return retVal;
}

jobject invokeSuper(TJNIEnv* env,jclass c,jobject thiz,jobject method,jint id,jobjectArray args){
    jmethodID mid = env->FromReflectedMethod(method);
    JClass superclass(env->GetSuperclass(env->GetObjectClass(thiz)));
    switch (id) {
        case 1://toString
            return env->asJNIEnv()->CallNonvirtualObjectMethod(thiz, superclass, mid);
        case 2://hashCode
        {
            jintArray ret = env->asJNIEnv()->NewIntArray(1);
            int retVal = env->CallNonvirtualIntMethod(thiz, superclass, mid);
            env->SetIntArrayRegion(ret,0,1,&retVal);
            return ret;
        }
        case 3://equals
        {
            jbooleanArray ret = env->asJNIEnv()->NewBooleanArray(1);
            jboolean retVal = env->CallNonvirtualBooleanMethod(thiz, superclass, mid,env->GetObjectArrayElement(args,0).get());
            env->SetBooleanArrayRegion(ret,0,1,&retVal);
            return ret;
        }
        default:
            return nullptr;
    }
}

jobjectArray runScript(TJNIEnv *env, jclass, jlong ptr, jobject script, jboolean isFile,
                       jobjectArray args) {
    ScriptContext *scriptContext = (ScriptContext *) ptr;
    ThreadContext* context=scriptContext->getThreadContext();
    Import *oldImport= nullptr;
    if (_setjmp(errorJmp)) {
        context->restore(oldImport);
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
            for (int i = resultSize ; i--;) {
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
    context->restore(oldImport);
    return result;
}


jobject invokeLuaFunction(TJNIEnv *env, jclass, jlong ptr, jlong funcRef, jboolean multiRet, jobject proxy, jstring methodName,
                          jintArray argTypes, jobjectArray args) {
    ScriptContext *scriptContext = (ScriptContext *) ptr;
    ThreadContext* context=scriptContext->getThreadContext();
    Import *oldImport= nullptr;
    if (_setjmp(errorJmp)) {
        context->restore(oldImport);
        return 0;
    }
    auto L=scriptContext->getLua();
    pushErrorHandler(L,context);
    int handlerIndex = lua_gettop(L);
    if (!reinterpret_cast<BaseFunction *>(funcRef)->isLocal()) {
        FuncInfo *funcInfo = (FuncInfo *) funcRef;
        oldImport = context->changeImport(funcInfo->getImport());
        loadLuaFunction(env, L, funcInfo, context);
    } else {
        oldImport = context->getImport();
        LocalFunctionInfo *info = reinterpret_cast<LocalFunctionInfo *>(funcRef);
        lua_rawgetp(L, LUA_REGISTRYINDEX,info);
        if (unlikely(lua_isnil(L, -1))) {
            context->setPendingException(
                    "Local Function must run in the given thread it's extracted from");
            context->throwToJava();
            return 0;
        }
    }
    pushJavaObject(L, context, proxy);
    JString name(env,methodName);
    lua_pushstring(L,name.str());
    name.invalidate();
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

    len += 2;
    int err = lua_pcall(L, len, LUA_MULTRET, handlerIndex);
    jobject ret = nullptr;
    int retCount;
    if (err != LUA_OK)recordLuaError(context, L, err);
    else if ((retCount = lua_gettop(L) - handlerIndex) != 0) {
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
            parseLuaObject(L, context, handlerIndex+1, object);//only return the first result
            ret = context->luaObjectToJObject(object);
            if (ret == INVALID_OBJECT) ret = nullptr;
        }
    }
    lua_settop(L, handlerIndex - 1);
    context->restore(oldImport);
    return ret;
}


