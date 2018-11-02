
#include <jni.h>
#include <sstream>
#include <android/log.h>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include "common.h"
#include "SpinLock.h"
#include "myarray.h"
#include "jtype.h"
#include "lua_object.h"
#include "func_info.h"
#include "lua.hpp"
#include "AutoJNIEnv.h"
#include "TJNIEnv.h"
#include <thread>

#ifndef LUADROID_LUADROID_H
#define LUADROID_LUADROID_H

#define HOLD_JAVA_EXCEPTION(context, code)({\
if(unlikely(env->ExceptionCheck())){\
    jthrowable t=env->ExceptionOccurred();\
    env->ExceptionClear();\
    context->setPendingException(t);\
    env->DeleteLocalRef(t);\
    code\
}})

#define INVALID_OBJECT reinterpret_cast<jobject >(-1)

inline void cleanArgs(jvalue *args, int argSize, Vector<ValidLuaObject> &arr, JNIEnv *env) {
    for (int i =argSize-1; i !=-1; --i) {
        if (arr[i].shouldRelease) {
            jobject ref = args[i].l;
            if (ref != INVALID_OBJECT)
                env->DeleteLocalRef(ref);
            else break;
        }
    }
}

inline void cleanArg(JNIEnv *env, const jobject j, bool should) {
    if (should) {
        env->DeleteLocalRef(j);
    }
}


namespace std {
    template<>
    struct hash<jclass> : public unary_function<jclass, int> {
        int operator()(const jclass c) const noexcept {
            return AutoJNIEnv()->CallIntMethod(c, objectHash);
        }
    };

    template<>
    struct equal_to<jclass> : public binary_function<jclass, jclass, bool> {
        bool operator()(const jclass &c1, const jclass &c2) const noexcept {//must not null both
            if (c1 == nullptr) return false;
            if (c2 == nullptr) return false;
            return c1 == c2 || AutoJNIEnv()->IsSameObject(c1, c2);
        }
    };

    template<>
    struct hash<std::pair<JavaType *, JavaType *>> {
        size_t operator()(const std::pair<JavaType *, JavaType *> &__v) const noexcept {
            return reinterpret_cast<size_t>(__v.first) + reinterpret_cast<size_t>(__v.second);
        }
    };

}
class JavaType;
class ScriptContext;
struct ThreadContext{
    TJNIEnv* env;
    ScriptContext* scriptContext;
private:
    Import* import;
    jthrowable pendingJavaError;
    inline JClass getTypeNoCheck(const String &className) const;
    inline JavaType* ensureShortArrayType(const char *typeName) ;
public:

    void setPendingException( const String &msg);

    void setPendingException(const char *msg) {
        FakeString m(msg);
        const String &k = m;
        setPendingException(k);
    }

    void setPendingException(jthrowable throwable) {
        if (throwable != nullptr) {
            if (pendingJavaError != nullptr) {
                if (env->IsSameObject(throwable, pendingJavaError)) return;
                env->DeleteLocalRef(pendingJavaError);
            }
            pendingJavaError = (jthrowable) env->NewLocalRef(throwable);
        }
    }

    jthrowable transferJavaError(){
        auto ret=pendingJavaError;
        pendingJavaError= nullptr;
        return ret;
    }

    void restore(ScriptContext* oldContext,Import* oldImport){
        changeScriptContext(oldContext);
        changeImport(oldImport);
        if(hasErrorPending())
            throwToJava();
    }

    void throwToJava() {
        AutoJNIEnv env;
        jthrowable p = (jthrowable) pendingJavaError;
        pendingJavaError = nullptr;
        env->Throw(p);
        env->DeleteLocalRef(p);
    }
    ScriptContext *changeScriptContext(ScriptContext *newScriptContext) {
        ScriptContext *old = scriptContext;
        scriptContext = newScriptContext;
        return old;
    }


    Import *changeImport(Import *newImport) {
        Import *old = import;
        import = newImport;
        return old;
    }

    Import *getImport() {
        return import;
    }

    bool hasErrorPending() {
        return pendingJavaError != nullptr;
    }

    jthrowable getPendingJavaError() {
        return pendingJavaError;
    }

    JClass findClass(String&& str){
        return findClass(str);
    }

    JClass findClass(String& str);

    JavaType *ensureType(const char *typeName);
    jobject proxy(JavaType *main, Vector<JavaType *> *interfaces,
                  const Vector<JObject> &principal, Vector<std::unique_ptr<BaseFunction>> &proxy,
                  bool shared= false, long nativeInfo=0,jobject superObject= nullptr);
    jvalue luaObjectToJValue( ValidLuaObject &luaObject, JavaType *type,jobject real= nullptr);

    jobject luaObjectToJObject( ValidLuaObject &luaObject);

    JavaType *HashMapType();

    JavaType *FunctionType();

    bool isLocalFunction();

};

class ScriptContext {
    static jmethodID sMapPut;
    static thread_local ThreadContext threadContext;
    static jmethodID sWriteBytes;
    static int (*sThreadTest)(intptr_t tid);
    static intptr_t (*sThreadID)();
    friend class ThreadContext;
    typedef Map<jclass, JavaType *> TypeMap;
    typedef Map<intptr_t , lua_State *> StateMap;
    typedef Map<String, CrossThreadLuaObject> CrossThreadMap;
    typedef Map<String, std::pair<String, jobject>> AddedMap;
    const bool importAll;
    const bool localFunction;
    TypeMap typeMap;
    StateMap stateMap;
    AddedMap addedMap;
    SpinLock lock;
    SpinLock mapLock;
    CrossThreadMap crossThreadMap;

    JavaType *HashMapClass = nullptr;
    JavaType *FunctionClass = nullptr;
    jweak outLogger = nullptr;
    jweak errLogger = nullptr;

    JavaType *getVoidClass(TJNIEnv *env);
    JavaType *const byteClass;
    JavaType *const shortClass;
    JavaType *const intClass;
    JavaType *const longClass;
    JavaType *const booleanClass;
    JavaType *const charClass;
    JavaType *const floatClass;
    JavaType *const doubleClass;
    JavaType *const voidClass;
public:
    JavaType *const ByteClass;
    JavaType *const ShortClass;
    JavaType *const IntegerClass;
    JavaType *const LongClass;
    JavaType *const BooleanClass;
    JavaType *const CharacterClass;
    JavaType *const FloatClass;
    JavaType *const DoubleClass;
    Map<std::pair<JavaType *, JavaType *>, uint> const weightMap;
    jobject const javaRef;
    JavaType *const ObjectClass;

    JavaType *ensureType(TJNIEnv *env, jclass type);

    ScriptContext(TJNIEnv *env, jobject con, bool importAll = true, bool localFunction = false);

    lua_State *getLua();

    void saveLuaObject(CrossThreadLuaObject &object, const char *name) {
        ScopeLock sentry(mapLock);
        crossThreadMap[name]=std::move(object);
    }

    void clean() {
        ScopeLock sentry(lock);
        for (auto&& pair = stateMap.begin(); pair != stateMap.end();) {
            int status = sThreadTest(pair->first);
            if (status == 0) {
                ++pair;
                continue;
            }
            lua_close(pair->second);
            stateMap.erase(pair++);
        }
    }


    CrossThreadLuaObject *getLuaObject(const char *name) {
        ScopeLock sentry(mapLock);
         auto &&iter = crossThreadMap.find(String(name));
        if (iter == crossThreadMap.end()) return nullptr;
        else return &(iter->second);
    }

    void deleteLuaObject(const char *name) {
        ScopeLock sentry(mapLock);
        crossThreadMap.erase(String(name));
    }
    ThreadContext* getThreadContext(){
        return &threadContext;
    }


    void addJavaObject(const char *name, const char *methodName, jobject object,
                       bool currentOnly);

    void pushAddedObject(TJNIEnv *env, lua_State *L, const char *name, const char *methodName,
                         jobject obj);

    void registerLogger(TJNIEnv *env, jobject out, jobject err);

    void writeLog(const char *data, bool isError);

    ~ScriptContext();


private:
    void config(lua_State *L);

    char *checkLineEndForLogcat(const char *data) const;

    static void init(TJNIEnv *env, jobject const javaObject);

};



#endif //LUADROID_LUADROID_H
