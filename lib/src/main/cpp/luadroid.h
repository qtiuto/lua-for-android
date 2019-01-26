
#include <jni.h>
#include <android/log.h>
#include "common.h"
#include "SpinLock.h"
#include "myarray.h"
#include "jtype.h"
#include "lua_object.h"
#include "func_info.h"
#include "lua.hpp"
#include "AutoJNIEnv.h"
#include "TJNIEnv.h"
#include "tls.h"

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
    for (int i =argSize; i--; ) {
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
    struct hash<std::pair<JavaType *, JavaType *>> {
        size_t operator()(const std::pair<JavaType *, JavaType *> &__v) const noexcept {
            return reinterpret_cast<size_t>(__v.first) + reinterpret_cast<size_t>(__v.second);
        }
    };

}
enum class ContextStorage{
    PARSED_FUNC,
    LOADED_FUNC,
    PARSED_TABLE,
    LEN
};
//class JavaType;
class Member;
class ScriptContext;
struct ThreadContext{
    TJNIEnv* env;
    ScriptContext* scriptContext;
    int pushedCount;
private:
    Import* import;
    jthrowable pendingJavaError;
    void* storage[(int)ContextStorage::LEN];
    inline JClass getTypeNoCheck(const String &className) const;
    inline JavaType* ensureShortArrayType(const char *typeName) ;
public:

    template <typename T>
    T* getValue(ContextStorage index){
        return (T*)storage[(int)index];
    }

    void setValue(ContextStorage index, void *value){
        storage[(int)index]=value;
    }

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

    void restore(Import* oldImport){
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

    JClass findClass(String&& str){
        return findClass(str);
    }

    JClass findClass(String& str)  ;

    JavaType *ensureType(const char *typeName);
    jobject proxy(JavaType *main, Vector<JavaType *> *interfaces,
                  const Vector<JObject> &principal, Vector<std::unique_ptr<BaseFunction>> &proxy,
                  BaseFunction* defaultFunc= nullptr,
                  bool shared = false, long nativeInfo = 0, jobject superObject = nullptr);
    jvalue luaObjectToJValue( ValidLuaObject &luaObject, JavaType *type,jobject real= nullptr);

    jobject luaObjectToJObject( ValidLuaObject &luaObject);

    JavaType *MapType();

    JavaType *FunctionType();

    bool isLocalFunction();

    ~ThreadContext();
};

struct AddInfo{
    JavaType* type;
    jobject obj;
    Member* member;
};
class ScriptContext {

    struct hashJClass {
        int operator()(const jclass c) const noexcept {
            return ScriptContext::sTypeEnv->CallIntMethod(c, objectHash);
        }
    };

    struct equalJClass{
        bool operator()(const jclass &c1, const jclass &c2) const noexcept {//must not null both
            if (c1 == nullptr) return false;
            if (c2 == nullptr) return false;
            return c1 == c2 || ScriptContext::sTypeEnv->IsSameObject(c1, c2);
        }
    };
    static jmethodID sWriteLog;
    static TJNIEnv* sTypeEnv;
    friend class ThreadContext;
    typedef Map<jclass, JavaType *,hashJClass,equalJClass> TypeMap;
    typedef Map<intptr_t , lua_State *> StateMap;
    typedef Map<String, CrossThreadLuaObject> CrossThreadMap;
    typedef Map<String, AddInfo> AddedMap;
    const bool importAll;
    const bool localFunction;
    ThreadLocal<ThreadContext,true> threadContext;
    TypeMap typeMap;
    StateMap stateMap;
    AddedMap addedMap;
    SpinLock typeLock;
    SpinLock gcLock;
    SpinLock crossLock;
    SpinLock addLock;
    SpinLock loggerLock;
    CrossThreadMap crossThreadMap;

    JavaType *HashMapClass = nullptr;
    JavaType *FunctionClass = nullptr;
    jweak outLogger = nullptr;
    jweak errLogger = nullptr;
    char16_t * javaLogBuffer= nullptr;

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
    JavaType *const ObjectClass;
    Map<std::pair<JavaType *, JavaType *>, uint> const weightMap;
    SpinLock weightLock;
    jobject const javaRef;

    JavaType *ensureType(TJNIEnv *env, jclass type);

    ScriptContext(TJNIEnv *env, jobject con, bool importAll = true, bool localFunction = false);

    lua_State *getLua();

    void saveLuaObject(CrossThreadLuaObject &object, const char *name) {
        ScopeLock sentry(crossLock);
        crossThreadMap[name]=std::move(object);
    }

    void removeCurrent(){
        ScopeLock sentry(gcLock);
        auto id=pthread_self();
        auto L=stateMap.find(id)->second;
        stateMap.erase(id);
        lua_close(L);
    }


    CrossThreadLuaObject *getLuaObject(const char *name) {
        ScopeLock sentry(crossLock);
         auto &&iter = crossThreadMap.find(String(name));
        if (iter == crossThreadMap.end()) return nullptr;
        else return &(iter->second);
    }

    void deleteLuaObject(const char *name) {
        ScopeLock sentry(crossLock);
        crossThreadMap.erase(String(name));
    }
    ThreadContext* getThreadContext(){
        ThreadContext* context= threadContext.get();
        if(context==nullptr){
            context=new ThreadContext();
            context->scriptContext=this;
            threadContext.rawSet(context);
        }
        return context;
    }

    void addJavaObject(TJNIEnv *env, const char *name, const char *methodName, jobject object, JavaType *type,
                           bool currentOnly);

    void pushAddedObject(TJNIEnv *env, lua_State *L, const char *name,const AddInfo& addInfo);

    void registerLogger(TJNIEnv *env, jobject out, jobject err);

    void writeLog(TJNIEnv *env, const char *data, bool isError);

    ~ScriptContext();


private:
    void config(lua_State *L);

    static void init(TJNIEnv *env, jobject const javaObject);

};



#endif //LUADROID_LUADROID_H
