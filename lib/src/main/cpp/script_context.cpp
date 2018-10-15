
#include "luadroid.h"
#include "jtype.h"
#include "java_type.h"
#include "utf8.h"
#include <grp.h>

bool changeClassName(String &className) noexcept;
thread_local ThreadContext ScriptContext::threadContext ;
jmethodID ScriptContext::sMapPut = nullptr;
jmethodID ScriptContext::sWriteBytes;
static jmethodID sProxy;
TJNIEnv* _GCEnv;
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
        addedMap[name]= std::make_pair(methodName == nullptr ? String() : methodName, object);
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
    int self = gettid();
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

#define BOX_INIT(Type) Type##Class(ensureType(env,env->FindClass("java/lang/"#Type)))
ScriptContext::ScriptContext(TJNIEnv *env, jobject javaObject, bool importAll, bool localFunction) :
        importAll((init(env, javaObject),importAll)), localFunction(localFunction),
        javaRef( env->NewWeakGlobalRef(javaObject)),
        byteClass(ensureType(env, JavaType::getComponentType(env, env->FindClass("[B")))),
        shortClass(ensureType(env, JavaType::getComponentType(env, env->FindClass("[S")))),
        intClass(ensureType(env, JavaType::getComponentType(env, env->FindClass("[I")))),
        longClass(ensureType(env, JavaType::getComponentType(env, env->FindClass("[J")))),
        booleanClass(ensureType(env, JavaType::getComponentType(env, env->FindClass("[Z")))),
        charClass(ensureType(env, JavaType::getComponentType(env, env->FindClass("[C")))),
        floatClass(ensureType(env, JavaType::getComponentType(env, env->FindClass("[F")))),
        doubleClass(ensureType(env, JavaType::getComponentType(env, env->FindClass("[D")))),
        voidClass(getVoidClass(env)),ObjectClass(ensureType(env,env->GetSuperclass(stringType))),
        BOX_INIT(Byte),BOX_INIT(Character), BOX_INIT(Boolean),BOX_INIT(Integer),BOX_INIT(Long),
        BOX_INIT(Short),BOX_INIT(Float), BOX_INIT(Double){
    JavaType* StringClass=ensureType(env,stringType);
    StringClass->_isString=true;
    byteClass->_isInteger = true;
    byteClass->primitive = true;
    shortClass->_isInteger = true;
    shortClass->primitive = true;
    intClass->primitive = true;
    intClass->_isInteger = true;
    longClass->primitive = true;
    longClass->_isInteger = true;
    floatClass->primitive = true;
    doubleClass->primitive = true;
    floatClass->_isFloat = true;
    doubleClass->_isFloat = true;
    voidClass->primitive = true;
    ByteClass->_isBox=true;
    ShortClass->_isBox=true;
    IntegerClass->_isBox=true;
    LongClass->_isBox=true;
    FloatClass->_isBox=true;
    DoubleClass->_isBox=true;
    CharacterClass->_isBox=true;
    BooleanClass->_isBox=true;
    ByteClass->_isInteger=true;
    ShortClass->_isInteger=true;
    IntegerClass->_isInteger=true;
    LongClass->_isInteger=true;
    FloatClass->_isFloat = true;
    DoubleClass->_isFloat = true;
    byteClass->typeID=JavaType::BYTE;
    shortClass->typeID=JavaType::SHORT;
    intClass->typeID=JavaType::INT;
    longClass->typeID=JavaType::LONG;
    floatClass->typeID=JavaType::FLOAT;
    doubleClass->typeID=JavaType::DOUBLE;
    charClass->typeID=JavaType::CHAR;
    booleanClass->typeID=JavaType::BOOLEAN;
    voidClass->typeID=JavaType::VOID;
    ByteClass->typeID=JavaType::BOX_BYTE;
    ShortClass->typeID=JavaType::BOX_SHORT;
    IntegerClass->typeID=JavaType::BOX_INT;
    LongClass->typeID=JavaType::BOX_LONG;
    FloatClass->typeID=JavaType::BOX_FLOAT;
    DoubleClass->typeID=JavaType::BOX_DOUBLE;
    CharacterClass->typeID=JavaType::BOX_CHAR;
    BooleanClass->typeID=JavaType::BOX_BOOLEAN;
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
        sProxy = env->GetMethodID(cl, "proxy", "(Ljava/lang/Class;[Ljava/lang/Class;"
                "[Ljava/lang/reflect/Method;[JZJLjava/lang/Object;)Ljava/lang/Object;");
        sLength = env->GetStaticMethodID(cl, "length", "(Ljava/lang/Object;)I");
        JavaType::sFindMembers = env->GetStaticMethodID(cl, "findMembers"
                , "(Ljava/lang/Class;Ljava/lang/String;ZZ)[Ljava/lang/Object;");
        JavaType::sWeightObject = env->GetStaticMethodID(cl, "weightObject",
                                                         "(Ljava/lang/Class;Ljava/lang/Class;)I");
        JavaType::sGetSingleInterface = env->GetStaticMethodID(cl, "getSingleInterface"
                , "(Ljava/lang/Class;)Ljava/lang/reflect/Method;");
        JavaType::sIsTableType = env->GetMethodID(cl, "isTableType", "(Ljava/lang/Class;)Z");
        JavaType::sTableConvert = env->GetMethodID(cl, "convertTable", "(Ljava/util/Map;"
                "Ljava/lang/Class;Ljava/lang/reflect/Type;)Ljava/lang/Object;");
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
    int tid = gettid();
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
JavaType* ensureShortArrayType(ThreadContext* info,const char *typeName){
    TJNIEnv* env=info->env;
    size_t nameLen=strchr(typeName,'[')-typeName;
    String trueType(typeName,nameLen);
    if(trueType=="int") trueType="I";
    else if(trueType=="byte") trueType="B";
    else if(trueType=="char") trueType="C";
    else if(trueType=="float") trueType="F";
    else if(trueType=="boolean") trueType="Z";
    else if(trueType=="long") trueType="J";
    else if(trueType=="double") trueType="D";
    else if(trueType=="short") trueType="S";
    else{
        const Import *import =info-> getImport();
        for (auto &&pack:import->packages) {
            String full(pack + trueType);
            changeClassName(full);
            env->FindClass(&full[0]);
            if (env->ExceptionCheck()) {
                env->ExceptionClear();
                continue;
            }
            trueType='L'+full+';';
            break;
        }
    }
    size_t arrDepth=(strlen(typeName)-nameLen)>>1;
    char* legalName=new char[trueType.size()+arrDepth+1];
    memset(legalName,'[',arrDepth);
    memcpy(legalName+arrDepth,trueType.data(),trueType.length()+1);
    JClass type = env->FindClass(legalName);
    delete [] legalName;
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        return nullptr;
    }
    JavaType *ret = info->scriptContext->ensureType(env, type);
    info->getImport()->stubbed.emplace(typeName,ret);
    return ret;
}


JClass ThreadContext::findClass(String className){
    if(!changeClassName(className))
        return JClass();
    JClass type = env->FindClass(className.data());
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        if(import->externalLoaders.size()>0){
            static jmethodID loadClass=env->GetMethodID(loaderClass,"loadClass","(Ljava/lang/String;)Ljava/lang/Class;");
            JString jclassName=env->NewStringUTF(className.data());
            for(auto loader:import->externalLoaders){
                type=env->CallObjectMethod(loader,loadClass,jclassName.get());
                if(env->ExceptionCheck()){
                    env->ExceptionClear();
                } else break;
            }
        }

    }
    return type;
}

JavaType *ThreadContext::ensureType(const char *typeName) {
#define MatchPrimitive(type)\
    ({if(strcmp(typeName,#type)==0){\
        return scriptContext->type##Class;\
    }})

    MatchPrimitive(byte);
    MatchPrimitive(int);
    MatchPrimitive(void);
    MatchPrimitive(boolean);
    MatchPrimitive(long);
    MatchPrimitive(double);
    MatchPrimitive(char);
    MatchPrimitive(float);
    MatchPrimitive(short);


    JClass type;
    if (strchr(typeName, '.') == nullptr&&typeName[0]!='[') {
        if (strchr(typeName, '/') != nullptr) return nullptr;
        const Import *import = getImport();
        auto&& iter = import->stubbed.find(FakeString(typeName));
        if (iter != import->stubbed.end()) return iter->second;
        if(typeName[strlen(typeName)-1]==']'){
            return ensureShortArrayType(this,typeName);
        }
        for (auto &&pack:import->packages) {
            String full(pack + typeName);
            type = findClass(full);
            if(type== nullptr) continue;
            return scriptContext->ensureType(env, type);
        }
    }
    String qul = typeName;
    type = findClass(qul);
    if (type== nullptr) {
        return nullptr;
    }
    JavaType *ret = scriptContext->ensureType(env, type);
    return ret;

}

void ThreadContext::setPendingException(const String &msg) {
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

jvalue ThreadContext::luaObjectToJValue(ValidLuaObject &luaObject, JavaType *type,jobject real) {
    jvalue ret;
    if (type->isChar()) {
        if(unlikely(luaObject.type==T_OBJECT)){
            ret.c=env->CallCharMethod(luaObject.objectRef->object,charValue);
        } else if(luaObject.type==T_CHAR)
            ret.c=luaObject.character;
        else strcpy8to16((char16_t *) &ret.c, luaObject.string, nullptr);
    } else if (type->isBool()) {
        if(unlikely(luaObject.type==T_OBJECT)){
            ret.z=env->CallBooleanMethod(luaObject.objectRef->object,booleanValue);
        } else ret.z = luaObject.isTrue;
    } else if (type->isInteger()) {
        if(unlikely(luaObject.type==T_OBJECT)){
            ret.j=env->CallLongMethod(luaObject.objectRef->object,longValue);
        } else ret.j = luaObject.integer;
    } else if (type->typeID == JavaType::FLOAT) {
        if(unlikely(luaObject.type==T_OBJECT)){
            ret.f= float(env->CallDoubleMethod(luaObject.objectRef->object, doubleValue));
        } else ret.f = (float) luaObject.number;
    } else if (type->typeID == JavaType::DOUBLE) {
        if(unlikely(luaObject.type==T_OBJECT)){
            ret.d= env->CallDoubleMethod(luaObject.objectRef->object, doubleValue);
        } else ret.d = luaObject.number;
    } else {
        if (luaObject.type == T_FUNCTION) {
            luaObject.shouldRelease=true;
            JObject method = type->getSingleInterface(env);
            Vector<JObject> methods;
            methods.push_back(std::move(method));
            BaseFunction *info = luaObject.func;
            luaObject.func = nullptr;//avoid
            Vector<std::unique_ptr<BaseFunction>> func;
            func.emplace_back(info);
            info->javaRefCount++;
            ret.l = proxy( type, nullptr, methods, func);

            if (ret.l != INVALID_OBJECT) {
                func.begin()->release();
            }
        } else if (luaObject.type == T_TABLE) {
            luaObject.shouldRelease=true;
            if (type->isTableType(env)) {
                static thread_local Map<LazyTable*,jobject>* parsedTable= nullptr;
                bool isOwner=false;
                if(parsedTable== nullptr){
                    parsedTable=new PTR_TYPE(parsedTable)();
                    isOwner=true;
                }
                auto&& iter=parsedTable->find(luaObject.lazyTable);
                if(iter==parsedTable->end()){
                    JavaType *mapType = HashMapType();
                    ValidLuaObject lenObject;
                    lenObject.type = T_INTEGER;
                    auto &&table = luaObject.lazyTable->getTable(this)->get();
                    lenObject.integer = static_cast<lua_Integer>(table.size());
                    Vector<JavaType *> types{nullptr};
                    Vector<ValidLuaObject> args;
                    args.push_back(std::move(lenObject));
                    JObject map = JObject(env, mapType->newObject(this,types, args));
                    parsedTable->emplace(luaObject.lazyTable,map.get());
                    for (auto &&pair:table) {
                        jobject key = luaObjectToJObject(std::move(pair.first));
                        if (key == INVALID_OBJECT) goto ERROR_HANDLE;
                        jobject value = luaObjectToJObject(std::move(pair.second));
                        if (value == INVALID_OBJECT) {
                            env->DeleteLocalRef(key);
                            goto ERROR_HANDLE;
                        }
                        env->CallObjectMethod(map, ScriptContext::sMapPut, JObject(env, key).get(),
                                              JObject(env, value).get());
                    }
                    parsedTable->erase(luaObject.lazyTable);
                    ret.l = type->convertTable(env,map,real);
                } else ret.l=type->convertTable(env,iter->second,real);
                if(isOwner) {
                    delete parsedTable;
                    parsedTable= nullptr;
                }
            } else if (type->isInterface(env)) {
                ret.l = luaObject.lazyTable->asInterface(this, type);
            }
            HOLD_JAVA_EXCEPTION(this, {
                goto ERROR_HANDLE;
            });
        } else if (luaObject.type == T_NIL) {
            ret.l = nullptr;
        } else if (luaObject.type == T_STRING) {
            luaObject.shouldRelease=true;
            ret.l = env->NewStringUTF(luaObject.string).invalidate();
        } else if (luaObject.type == T_OBJECT) {
            ret.l = luaObject.objectRef->object;
        } else {
            luaObject.shouldRelease=true;
            switch (type->typeID){
                case JavaType::BOX_CHAR:
                    if (luaObject.type == T_STRING)
                        strcpy8to16(&luaObject.character, luaObject.string, NULL);
                    ret.l = env->CallStaticObjectMethod(type->getType(), type->
                            getBoxMethodForBoxType(env), luaObject.character).invalidate();
                    break;
                case JavaType::BOX_BOOLEAN:
                    ret.l = env->CallStaticObjectMethod(type->getType(), type->
                            getBoxMethodForBoxType(env), luaObject.isTrue).invalidate();
                    break;
                case JavaType::BOX_DOUBLE:
                    ret.l = env->CallStaticObjectMethod(type->getType(), type->getBoxMethodForBoxType(env)
                            , luaObject.type == T_INTEGER ? (double) luaObject.integer:
                              luaObject.number).invalidate();
                    break;
                case JavaType::BOX_FLOAT:
                    ret.l = env->CallStaticObjectMethod(type->getType(), type->getBoxMethodForBoxType(env),
                                                        luaObject.type == T_INTEGER ? (float) luaObject.integer :
                                                        (float) luaObject.number).invalidate();
                    break;
                default:
                    if (likely(type->_isBox)) {
                        ret.l = env->CallStaticObjectMethod(type->getType(), type->
                                getBoxMethodForBoxType(env), luaObject.integer).invalidate();
                    } else {
                        luaObject.shouldRelease = false;
                        ret.l = INVALID_OBJECT;
                    }
                    break;
            }
        }
    }
    return ret;
    ERROR_HANDLE:
    ret.l = INVALID_OBJECT;
    return ret;
}

JavaType *ThreadContext::HashMapType() {
    if (scriptContext->HashMapClass == nullptr) {
        scriptContext->HashMapClass = ensureType("LinkedHashMap");
        ScriptContext::sMapPut = env->GetMethodID(scriptContext->HashMapClass->getType(), "put", "(Ljava/lang/Object;"
                "Ljava/lang/Object;)Ljava/lang/Object;");
    }
    return scriptContext->HashMapClass;
}

JavaType *ThreadContext::FunctionType() {
    if (scriptContext->FunctionClass == nullptr) {
        scriptContext->FunctionClass = ensureType("com.oslorde.luadroid.LuaFunction");
    }
    return scriptContext->FunctionClass;
}

jobject ThreadContext::luaObjectToJObject(ValidLuaObject &&luaObject) {
    switch (luaObject.type) {
        case T_NIL:
            return nullptr;
        case T_INTEGER: {
            return env->CallStaticObjectMethod(scriptContext->LongClass->getType(), scriptContext->LongClass->
                    getBoxMethodForBoxType(env), luaObject.integer).invalidate();
        }
        case T_BOOLEAN: {
            return env->CallStaticObjectMethod(scriptContext->BooleanClass->getType(), scriptContext->BooleanClass->
                    getBoxMethodForBoxType(env), luaObject.integer).invalidate();
        }
        case T_FLOAT: {
            return env->CallStaticObjectMethod(scriptContext->DoubleClass->getType(), scriptContext->DoubleClass->
                    getBoxMethodForBoxType(env), luaObject.integer).invalidate();
        }
        case T_OBJECT: {
            jvalue v = luaObjectToJValue(luaObject,HashMapType());
            v.l = env->NewLocalRef(v.l);
            return v.l;
        }
        case T_STRING:
        case T_TABLE:
        case T_FUNCTION: {
            jvalue v = luaObjectToJValue(luaObject, FunctionType());
            return v.l;
        }
        case T_CHAR:{
            return env->CallStaticObjectMethod(scriptContext->CharacterClass->getType(), scriptContext->CharacterClass->
                    getBoxMethodForBoxType(env), luaObject.integer).invalidate();
        }
    }
    return nullptr;
}

ScriptContext::~ScriptContext() {
    AutoJNIEnv env;
    _GCEnv=env;
    for (auto &&pair :typeMap) {
        delete pair.second;
    }
    ScopeLock sentry(lock);
    for (auto &&pair :stateMap)
        lua_close(pair.second);
    for (auto &&object:addedMap) {
        env->DeleteGlobalRef(object.second.second);
    }
    env->DeleteWeakGlobalRef(javaRef);
}

jobject ThreadContext::proxy(JavaType *main, Vector<JavaType *> *interfaces,
                             const Vector<JObject> &principal,
                             Vector<std::unique_ptr<BaseFunction>> &proxy, bool shared,
                             long nativeInfo,jobject superObject) {
    int interfaceCount;
    JObjectArray interfaceArray;
    if (interfaces != nullptr && (interfaceCount =  interfaces->size())) {
        interfaceArray = env->NewObjectArray(interfaceCount, classType, nullptr);
        for (int i = interfaceCount - 1; i >= 0; --i) {
            env->SetObjectArrayElement(interfaceArray, i,
                                       interfaces->at(i)->getType());
        }
    }
    int principalCount =  principal.size();
    JObjectArray principalArray;
    if (principalCount > 0)
        principalArray = env->NewObjectArray(principalCount, env->GetObjectClass(principal[0]), nullptr);
    for (int i = principalCount - 1; i >= 0; --i) {
        env->SetObjectArrayElement(principalArray, i, principal[i]);
    }
    int proxyCount = proxy.size();
    jlong buf[proxyCount];
    for (jsize i = proxyCount - 1; i >= 0; --i) {
        buf[i] = (jlong) proxy[i].get();
    }
    JType<jlongArray> proxyArray = env->NewLongArray(proxyCount);
    env->SetLongArrayRegion(proxyArray, 0, proxyCount, buf);
    jvalue args[]={{.l=main->getType()}, {.l=interfaceArray.get()},
                   {.l=principalArray.get()}, {.l=proxyArray.get()},
                   {.z=(jboolean)shared}, {.j=nativeInfo}, {.l=superObject}};
    jobject ret = env->asJNIEnv()->CallObjectMethodA(
            scriptContext->javaRef, sProxy,args);//use jvalue to avoid stack limit in 32 bit mode
    HOLD_JAVA_EXCEPTION(this, {
        return INVALID_OBJECT;
    });
    return ret;
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