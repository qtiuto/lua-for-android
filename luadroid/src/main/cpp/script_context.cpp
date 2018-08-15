
#include "luadroid.h"
#include "jtype.h"
#include "java_type.h"
#include "utf8.h"
#include <grp.h>

__thread jthrowable ScriptContext::pendingJavaError = nullptr;
__thread Import *ScriptContext::import = nullptr;
jmethodID ScriptContext::sMapPut = nullptr;
jmethodID ScriptContext::sWriteBytes;
static jmethodID sProxy;
jmethodID charValue;
jmethodID booleanValue;
jmethodID longValue;
jmethodID doubleValue;
static struct {
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    ScriptContext *context;
    const char *name;
    const char *methodName;
    jobject object;
} sAddInfo;
#define ADD_SIG (SIGRTMAX-11)

static void handleAdd(int, siginfo *, void *) {
    pthread_mutex_lock(&sAddInfo.mutex);
    const char *name = sAddInfo.name;
    assert(name != nullptr);
    AutoJNIEnv env;
    ScriptContext *context = sAddInfo.context;
    lua_State *L = context->getLua();
    const char *methodName = sAddInfo.methodName;
    jobject obj = sAddInfo.object;
    context->pushAddedObject(env, L, name, methodName, obj);
    pthread_mutex_unlock(&sAddInfo.mutex);
}


void ScriptContext::addJavaObject(const char *name, const char *methodName, jobject object,
                                  bool currentOnly) {
    lock.lock();
    if (object == nullptr) {
        addedMap.erase(name);
    } else if(!currentOnly)
        addedMap.emplace(name,
                         std::make_pair(methodName == nullptr ? String() : methodName, object));
    lock.unlock();
    pthread_mutex_lock(&sAddInfo.mutex);
    struct sigaction sig, old;
    sigemptyset(&sig.sa_mask);
    sig.sa_sigaction = handleAdd;
    sig.sa_flags = 0;
    sigaction(ADD_SIG, &sig, &old);
    sAddInfo.context = this;
    sAddInfo.name = name;
    sAddInfo.methodName = methodName;
    sAddInfo.object = object;
    lock.lock();
    pthread_t self = pthread_self();
    if (currentOnly && (stateMap.find(self) != stateMap.end())) {
        pthread_mutex_unlock(&sAddInfo.mutex);
        handleAdd(ADD_SIG, nullptr, nullptr);
        pthread_mutex_lock(&sAddInfo.mutex);
    } else {
        for (auto key:stateMap) {
            if (key.first == self) {
                pthread_mutex_unlock(&sAddInfo.mutex);
                handleAdd(ADD_SIG, nullptr, nullptr);
                pthread_mutex_lock(&sAddInfo.mutex);
            } else {
                lock.unlock();
                int status = pthread_kill(key.first, ADD_SIG);
                if (status == 0)pthread_cond_wait(&sAddInfo.cond, &sAddInfo.mutex);
                lock.lock();
            }
        }
    }

    lock.unlock();
    sigaction(ADD_SIG, &old, &sig);
    sAddInfo.name = sAddInfo.methodName = nullptr;
    sAddInfo.object = nullptr;
    pthread_mutex_unlock(&sAddInfo.mutex);

}

bool changeClassName(String &className) noexcept;

ScriptContext::ScriptContext(TJNIEnv *env, jobject javaObject, bool importAll, bool localFunction) :
        importAll(importAll), localFunction(localFunction),
        javaRef((init(env, javaObject), env->NewWeakGlobalRef(javaObject))),
        byteClass(ensureType(env, JavaType::getComponentType(env, env->FindClass("[B")))),
        shortClass(ensureType(env, JavaType::getComponentType(env, env->FindClass("[S")))),
        intClass(ensureType(env, JavaType::getComponentType(env, env->FindClass("[I")))),
        longClass(ensureType(env, JavaType::getComponentType(env, env->FindClass("[J")))),
        booleanClass(ensureType(env, JavaType::getComponentType(env, env->FindClass("[Z")))),
        charClass(ensureType(env, JavaType::getComponentType(env, env->FindClass("[C")))),
        floatClass(ensureType(env, JavaType::getComponentType(env, env->FindClass("[F")))),
        doubleClass(ensureType(env, JavaType::getComponentType(env, env->FindClass("[D")))),
        voidClass(getVoidClass(env)) {
    byteClass->isInteger = true;
    byteClass->primitive = true;
    shortClass->isInteger = true;
    shortClass->primitive = true;
    intClass->primitive = true;
    intClass->isInteger = true;
    longClass->primitive = true;
    longClass->isInteger = true;
    floatClass->primitive = true;
    floatClass->isInteger = false;
    doubleClass->primitive = true;
    doubleClass->isInteger = false;
    voidClass->primitive = true;
    voidClass->isInteger = false;
}

void ScriptContext::init(TJNIEnv *env, const jobject javaObject) {
    if (stringType == nullptr) {
        JClass cStr = env->FindClass("java/lang/String");
        stringType = (jclass) env->NewGlobalRef(cStr);
    }
    if (throwableType == nullptr) {
        JClass cThr = env->FindClass("java/lang/Throwable");
        throwableType = (jclass) env->NewGlobalRef(cThr);
    }
    if (classType == nullptr) {
        JClass cClass = env->GetObjectClass(stringType);
        classType = (jclass) env->NewGlobalRef(cClass);
        classGetName = env->GetMethodID(classType, "getName", "()Ljava/lang/String;");
        JavaType::sGetComponentType = env->GetMethodID(classType, "getComponentType",
                                                       "()Ljava/lang/Class;");
        JavaType::sIsInterface = env->GetMethodID(classType, "isInterface", "()Z");
    }
    if (sProxy == nullptr) {
        JClass cl = env->GetObjectClass(javaObject);
        contextClass = (jclass) env->NewGlobalRef(cl);
        sProxy = env->GetMethodID(cl, "proxy", "(JLjava/lang/Class;[Ljava/lang/Class;"
                "[Ljava/lang/reflect/Method;[JZJLjava/lang/Object;)Ljava/lang/Object;");
        sLength = env->GetStaticMethodID(cl, "length", "(Ljava/lang/Object;)I");
        JavaType::sFindMembers = env->GetStaticMethodID(cl, "findMembers"
                , "(Ljava/lang/Class;Ljava/lang/String;ZZ)[Ljava/lang/Object;");
        JavaType::sWeightObject = env->GetStaticMethodID(cl, "weightObject",
                                                         "(Ljava/lang/Class;Ljava/lang/Class;)I");
        JavaType::sGetSingleInterface = env->GetStaticMethodID(cl, "getSingleInterface"
                , "(Ljava/lang/Class;)Ljava/lang/reflect/Method;");
        JavaType::sGetParameterTypes = env->GetStaticMethodID(cl, "getParameterTypes"
                , "(Ljava/lang/Object;)[Ljava/lang/Class;");
        JavaType::sIsTableType = env->GetMethodID(cl, "isTableType", "(Ljava/lang/Class;)Z");
        JavaType::sTableConvert = env->GetMethodID(cl, "convertTable", "(Ljava/util/Map;"
                "Ljava/lang/Class;)Ljava/lang/Object;");
    }
    if (charValue == nullptr) {
        JClass cChar = env->FindClass("java/lang/Character");
        charValue = env->GetMethodID(cChar, "charValue", "()C");

        JClass cBoolean = env->FindClass("java/lang/Boolean");
        booleanValue = env->GetMethodID(cBoolean, "booleanValue", "()Z");

        JClass cNumber = env->FindClass("java/lang/Number");
        longValue = env->GetMethodID(cNumber, "longValue", "()J");
        doubleValue = env->GetMethodID(cNumber, "doubleValue", "()D");
    }
    if (objectHash == nullptr) {
        JClass cObject = env->FindClass("java/lang/Object");
        objectHash = env->GetMethodID(cObject, "hashCode", "()I");
        objectToString = env->GetMethodID(cObject, "toString", "()Ljava/lang/String;");
    }
}

lua_State *ScriptContext::getLua() {
    ScopeLock sentry(lock);
    pthread_t tid = pthread_self();
    const auto &iter = stateMap.find(tid);
    lua_State *state;
    if (iter == stateMap.end()) {
        state = luaL_newstate();
        config(state);
        luaL_openlibs(state);
        stateMap.emplace(tid, state);
    } else state = (*iter).second;
    return state;
}


JavaType *ScriptContext::ensureType(TJNIEnv *env, jclass type) {
    const auto &iter = typeMap.find(type);
    if (iter != typeMap.end()) {
        return (*iter).second;
    }
    JavaType *ret = new JavaType(env, type, this);
    typeMap.emplace(ret->type, ret);
    return ret;
}

JavaType *ScriptContext::getVoidClass(TJNIEnv *env) {
    JClass Void = env->FindClass("java/lang/Void");
    jfieldID mid = env->GetStaticFieldID(Void, "TYPE", "Ljava/lang/Class;");
    return ensureType(env, (JClass) env->GetStaticObjectField(Void, mid));

}

JavaType *ScriptContext::ensureType(TJNIEnv *env, const char *typeName) {
#define MatchPrimitive(type)\
    ({if(strcmp(typeName,#type)==0){\
        return type##Class;\
    }})

    MatchPrimitive(byte);
    MatchPrimitive(short);
    MatchPrimitive(int);
    MatchPrimitive(long);
    MatchPrimitive(boolean);
    MatchPrimitive(char);
    MatchPrimitive(float);
    MatchPrimitive(double);
    MatchPrimitive(void);


    JClass type;
    if (strchr(typeName, '.') == nullptr) {
        if (strchr(typeName, '/') != nullptr) return nullptr;
        const Import *import = getImport();
        const auto &iter = import->stubbed.find(FakeString(typeName));
        if (iter != import->stubbed.end()) return iter->second;
        for (auto &pack:import->packages) {
            String full(pack + typeName);
            changeClassName(full);
            type = env->FindClass(&full[0]);
            if (env->ExceptionCheck()) {
                env->ExceptionClear();
                continue;
            }
            return ensureType(env, type);
        }
    }
    String qul = typeName;
    if (!changeClassName(qul)) return nullptr;
    type = env->FindClass(&qul[0]);
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        return nullptr;
    }
    JavaType *ret = ensureType(env, type);
    return ret;

}

void ScriptContext::setPendingException(TJNIEnv *env, const String &msg) {

    if (pendingJavaError == nullptr) {
        static jclass luaExceptionType = (jclass) env->NewGlobalRef(
                env->FindClass("com/oslorde/luadroid/LuaException"));
        jmethodID con = env->GetMethodID(throwableType, "<init>", "()V");
        pendingJavaError = ((jthrowable) env->NewObject(luaExceptionType, con));

    }
    static jfieldID id = env->GetFieldID(throwableType, "detailMessage", "Ljava/lang/String;");
    JString oldMsg = (JString) env->GetObjectField(pendingJavaError, id);
    JString jmsg = env->NewStringUTF(
            oldMsg.str() != nullptr && strlen(oldMsg) > 0 ? formMessage(oldMsg.str(), "\n\t",
                                                                        msg).data() : msg.c_str());
    env->SetObjectField(pendingJavaError, id, jmsg.get());
}

jvalue ScriptContext::luaObjectToJValue(TJNIEnv *env, ValidLuaObject &luaObject, JavaType *type) {
    jvalue ret;
    if (type->isChar()) {
        strcpy8to16((char16_t *) &ret.c, luaObject.string, nullptr);
    } else if (type == booleanClass) {
        ret.b = luaObject.isTrue;
    } else if (type->isLuaInteger()) {
        ret.j = luaObject.integer;
    } else if (type == floatClass) {
        ret.f = (float) luaObject.number;
    } else if (type == doubleClass) {
        ret.d = luaObject.number;
    } else {
        if (luaObject.type == T_FUNCTION) {
            JObject method = type->getSingleInterface(env);
            Vector<JObject> methods;
            methods.push_back(std::move(method));
            BaseFunction *info = luaObject.func;
            luaObject.func = nullptr;//avoid
            Vector<std::unique_ptr<BaseFunction>> func;
            func.emplace_back(info);
            info->javaRefCount++;
            ret.l = proxy(env, type, nullptr, methods, func);

            if (ret.l != INVALID_OBJECT) {
                func.begin()->release();
            }
        } else if (luaObject.type == T_TABLE) {
            if (type->isTableType(env)) {
                JavaType *mapType = HashMapType(env);
                ValidLuaObject lenObject;
                lenObject.type = T_INTEGER;
                auto &&table = luaObject.lazyTable->getTable(env, this)->get();
                lenObject.integer = static_cast<lua_Integer>(table.size());
                Vector<JavaType *> types{nullptr};
                Vector<ValidLuaObject> args;
                args.push_back(std::move(lenObject));
                JObject map = JObject(env, mapType->newObject(env,types, args));
                for (auto &&pair:table) {
                    jobject key = luaObjectToJObject(env, std::move(pair.first));
                    if (key == INVALID_OBJECT) goto ERROR_HANDLE;
                    jobject value = luaObjectToJObject(env, std::move(pair.second));
                    if (value == INVALID_OBJECT) {
                        env->DeleteLocalRef(key);
                        goto ERROR_HANDLE;
                    }
                    env->CallObjectMethod(map, sMapPut, JObject(env, key).get(),
                                          JObject(env, value).get());
                }
                ret.l = type->convertTable(env,map);
            } else if (type->isInterface(env)) {
                ret.l = luaObject.lazyTable->asInterface(env, this, type);
            }

            HOLD_JAVA_EXCEPTION(this, {
                goto ERROR_HANDLE;
            });
        } else if (luaObject.type == T_NIL) {
            ret.l = nullptr;
        } else if (luaObject.type == T_STRING) {
            ret.l = env->NewStringUTF(luaObject.string).invalidate();
        } else if (luaObject.type == T_OBJECT) {
            ret.l = luaObject.objectRef->object;
        } else {
            ret.l = INVALID_OBJECT;
        }
    }
    return ret;
    ERROR_HANDLE:
    ret.l = INVALID_OBJECT;
    return ret;
}


ScriptContext::~ScriptContext() {
    for (auto &pair :typeMap) {//ide support for libstdc++  bug fix
        delete pair.second;
    }
    ScopeLock sentry(lock);
    for (auto &pair :stateMap)
        lua_close(pair.second);
    AutoJNIEnv env;
    for (auto &object:addedMap) {
        env->DeleteGlobalRef(object.second.second);
    }
    env->DeleteWeakGlobalRef(javaRef);
}

jobject ScriptContext::proxy(TJNIEnv *env, JavaType *main, Vector<JavaType *> *interfaces,
                             const Vector<JObject> &principal,
                             Vector<std::unique_ptr<BaseFunction>> &proxy, bool shared,
                             long nativeInfo,jobject superObject) {
    jsize interfaceCount;
    JObjectArray interfaceArray;
    if (interfaces != nullptr && (interfaceCount = (jsize) interfaces->size())) {
        interfaceArray = env->NewObjectArray(interfaceCount, classType, nullptr);
        for (jsize i = interfaceCount - 1; i >= 0; --i) {
            env->SetObjectArrayElement(interfaceArray, i,
                                       interfaces->at((unsigned long) i)->getType());
        }
    }
    jsize principalCount = (jsize) principal.size();
    JClass methodType = env->GetObjectClass(principal[0]);
    JObjectArray principalArray = env->NewObjectArray(principalCount, methodType, nullptr);
    for (jsize i = principalCount - 1; i >= 0; --i) {
        env->SetObjectArrayElement(principalArray, i, principal[i]);
    }
    jsize proxyCount = (jsize) proxy.size();
    jlong buf[proxyCount];
    for (jsize i = proxyCount - 1; i >= 0; --i) {
        buf[i] = (jlong) proxy[i].get();
    }
    JType<jlongArray> proxyArray = env->NewLongArray(proxyCount);
    env->SetLongArrayRegion(proxyArray, 0, proxyCount, buf);
    jobject ret = env->asJNIEnv()->CallObjectMethod(
            javaRef, sProxy, (jlong) this, main->getType(), interfaceArray.get(),
            principalArray.get(), proxyArray.get(), shared, nativeInfo,superObject);
    HOLD_JAVA_EXCEPTION(this, {
        return INVALID_OBJECT;
    });
    return ret;
}

JavaType *ScriptContext::HashMapType(TJNIEnv *env) {
    if (HashMapClass == nullptr) {
        HashMapClass = ensureType(env, "HashMap");
        sMapPut = env->GetMethodID(HashMapClass->getType(), "put", "(Ljava/lang/Object;"
                "Ljava/lang/Object;)Ljava/lang/Object;");
    }
    return HashMapClass;
}

JavaType *ScriptContext::FunctionType(TJNIEnv *env) {
    if (FunctionClass == nullptr) {
        FunctionClass = ensureType(env, "com.oslorde.luadroid.LuaFunction");
    }
    return FunctionClass;
}

jobject ScriptContext::luaObjectToJObject(TJNIEnv *env, ValidLuaObject &&luaObject) {
    switch (luaObject.type) {
        case T_NIL:
            return nullptr;
        case T_INTEGER: {
            JavaType *type = LongType(env);
            Vector<ValidLuaObject> args;
            args.push_back(std::move(luaObject));
            Vector<JavaType *> types{longClass};
            return type->newObject(env,types, args);
        }
        case T_BOOLEAN: {
            JavaType *type = BooleanType(env);
            Vector<ValidLuaObject> args;
            args.push_back(std::move(luaObject));
            Vector<JavaType *> types{booleanClass};
            return type->newObject(env,types, args);
        }
        case T_FLOAT: {
            JavaType *type = DoubleType(env);
            Vector<ValidLuaObject> args;
            args.push_back(std::move(luaObject));
            Vector<JavaType *> types{doubleClass};
            return type->newObject(env,types, args);
        }
        case T_OBJECT: {
            jvalue v = luaObjectToJValue(env, luaObject, FunctionType(env));
            v.l = env->NewLocalRef(v.l);
            return v.l;
        }
        case T_STRING:
        case T_TABLE:
        case T_FUNCTION: {
            jvalue v = luaObjectToJValue(env, luaObject, FunctionType(env));
            return v.l;
        }
    }
    return nullptr;
}

void ScriptContext::registerLogger(TJNIEnv *env, jobject out, jobject err) {
    if (outLogger)
        env->DeleteWeakGlobalRef(outLogger);
    if (errLogger)
        env->DeleteWeakGlobalRef(errLogger);

    outLogger = out ? env->NewWeakGlobalRef(out) : nullptr;
    errLogger = err ? env->NewWeakGlobalRef(err) : nullptr;
    if ((out || err) && !sWriteBytes) {
        sWriteBytes = env->GetMethodID(env->GetObjectClass(out ? out : err), "write", "([B)V");
    }
}

void ScriptContext::writeLog(const char *data, bool isError) {
    JType<jbyteArray> bytes;
    AutoJNIEnv env;
    if (outLogger || errLogger) {
        int len = static_cast<int>(strlen(data));
        bytes = env->NewByteArray(len);
        env->SetByteArrayRegion(bytes, 0, len, reinterpret_cast<const jbyte *>(data));
    }
    if (isError) {
        if (errLogger)
            env->CallVoidMethod(errLogger, sWriteBytes, bytes.get());
        else {
            char *str = checkLineEndForLogcat(data);
            __android_log_write(ANDROID_LOG_ERROR, "stderr", str);
        }

    } else {
        if (outLogger)
            env->CallVoidMethod(outLogger, sWriteBytes, bytes.get());
        else {
            char *str = checkLineEndForLogcat(data);
            __android_log_write(ANDROID_LOG_VERBOSE, "stdout", str);
        }
    }
}

char *ScriptContext::checkLineEndForLogcat(const char *data) const {
    char *str = const_cast<char *>(data);
    size_t pos = strlen(data) - 2;
    if (str[pos] == '\n')
        str[pos] = 0;
    return str;
}


bool changeClassName(String &className) noexcept {
    if (className.find('/') != String::npos) {
        return false;
    }
    size_t pos = 0;
    while ((pos = className.find('.', pos)) != String::npos) {
        className[pos] = '/';
    }
    return true;
}