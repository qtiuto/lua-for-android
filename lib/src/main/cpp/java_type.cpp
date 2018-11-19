
#include "java_type.h"
#include "utf8.h"

jmethodID JavaType::sGetComponentType;
jmethodID JavaType::sFindMembers;
jmethodID JavaType::sFindMockName;
jmethodID JavaType::sWeightObject ;
jmethodID JavaType::sGetSingleInterface ;
jmethodID JavaType::sIsTableType;
jmethodID JavaType::sTableConvert;
jmethodID JavaType::sIsInterface;

jobject JavaType::newObject(ThreadContext *context, Vector<JavaType *> &types, Vector<ValidLuaObject> &params) {
    TJNIEnv *env = context->env;
    const MethodInfo *info = findMethod(env, FakeString("<init>"), false, types, &params);
    if(unlikely(info== nullptr)){
        context->setPendingException("No matched constructor found");
        return nullptr;
    }
    uint32_t size = (uint32_t) types.size();
    jvalue argList[size];
    for (int i = size-1; i !=-1; --i) {
        ValidLuaObject &object = params[i];
        ParameterizedType &tp = info->params[i];
        argList[i] = context->luaObjectToJValue( object, tp.rawType,tp.realType);
        if ((object.type == T_TABLE || object.type == T_FUNCTION) &&
            argList[i].l == INVALID_OBJECT) {
            cleanArgs(argList, size, params, env);
            return nullptr;
        }
    }
    jobject ret = env->NewObjectA(type, info->id, argList);
    HOLD_JAVA_EXCEPTION(context, {});
    cleanArgs(argList, size, params, env);
    return ret;
}

jarray JavaType::newArray(ThreadContext *context,jint size, Vector<ValidLuaObject> &params) {
    uint32_t len = (uint32_t) params.size();
    auto  env=context->env;
#define ArrayHandle(typeID,jtype, jName, NAME) \
         case JavaType::typeID:{\
            j##jtype arr[len];\
            for (uint32_t i = 0; i < len; ++i) {\
                arr[i]=(j##jtype)params[i].NAME;\
            }\
            j##jtype##Array ret=env->New##jName##Array(size).invalidate();\
            env->Set##jName##ArrayRegion(ret,0,len,arr);\
            return ret;\
        }
#define IntegerArrayHandle(typeId,jtype, jName) ArrayHandle(typeId,jtype,jName,integer)
#define DoubleArrayHandle(typeId,jtype, jName) ArrayHandle(typeId,jtype,jName,number)
switch (typeID){
     IntegerArrayHandle(INT,int, Int)
     IntegerArrayHandle(BYTE,byte, Byte)
     IntegerArrayHandle(LONG,long, Long)
     DoubleArrayHandle(DOUBLE,double, Double)
     DoubleArrayHandle(FLOAT,float, Float)
     IntegerArrayHandle(SHORT,short, Short)
     ArrayHandle(BOOLEAN,boolean, Boolean, isTrue)
#undef ArrayHandle
#undef IntegerArrayHandle
#undef DoubleArrayHandle
    case CHAR: {
        String st;
        for (uint32_t i = 0; i < len; ++i) {
            st +=  + params[i].string;
        }
        len= static_cast<uint32_t>(strlen8to16(st.data()));
        if(len>size) return nullptr;
        jcharArray ret = env->NewCharArray(size).invalidate();
        char16_t *buf=(char16_t*)env->GetPrimitiveArrayCritical(ret,NULL);
        strcpy8to16(buf,st.data(),NULL);
        env->ReleasePrimitiveArrayCritical(ret,buf,0);
        return ret;
    }
    default:{
        jobjectArray ret = env->NewObjectArray(size, type, nullptr).invalidate();
        for (uint32_t i = 0; i < len; ++i) {
            ValidLuaObject &luaObject = params[i];
            if (luaObject.type == T_NIL) {
                env->SetObjectArrayElement(ret, i, NULL);
            } else {
                jobject value = context->luaObjectToJValue(luaObject, this).l;
                if (value == INVALID_OBJECT) return nullptr;
                env->SetObjectArrayElement(ret, i, value);
                cleanArg(env, value, luaObject.shouldRelease);
            }
        }
        return ret;
    }
}
return nullptr;//unreachable statement;
}


JavaType::Member* JavaType::ensureMember(TJNIEnv *env, const String &name, bool isStatic) {
    auto &&map = isStatic ? staticMembers : objectMembers;
    auto &&iter = map.find(name);
    if (iter != map.end()){
        return &iter->second;
    }
    JString str = env->NewStringUTF(&name[0]);
    Member member;
    if(name!="<init>"){
        JObjectArray fieldInfos = (JObjectArray) env->CallStaticObjectMethod(contextClass, sFindMembers,
                                                                             type, str.get(), true,
                                                                             isStatic);
        int count = fieldInfos == nullptr ? 0 : env->GetArrayLength(fieldInfos);
        if (count == 0) {
            goto FILL_METHOD;
        }
        FieldArray fieldArray((uint32_t) (count /3));
        for (int i = 0; i < count; i += 3) {
            FieldInfo &info = fieldArray[i /3];
            JObject&& field = env->GetObjectArrayElement(fieldInfos, i);
            info.id = env->FromReflectedField(field);
            info.type.rawType= context->ensureType(
                    env, (JClass) env->GetObjectArrayElement(fieldInfos, i + 1));
            auto&& realType = env->GetObjectArrayElement(fieldInfos, i + 2);
            info.type.realType= realType== nullptr? nullptr:env->NewGlobalRef (realType);
        }
        member.fields=std::move(fieldArray);
    }
    FILL_METHOD:
    {
        JObjectArray methodInfoArray = (JObjectArray) env->CallStaticObjectMethod(
                contextClass, sFindMembers, type, str.get(), false, isStatic);
        int count = methodInfoArray != nullptr ? env->GetArrayLength(methodInfoArray) : 0;
        if (count == 0) {
            goto END;
        }
        MethodArray methodArray((uint32_t) (count /5));
        for (int i = 0; i < count; i += 5) {
            MethodInfo &info = methodArray[i/5];
            JObject&& method = env->GetObjectArrayElement(methodInfoArray, i);
            info.id = env->FromReflectedMethod(method);
            info.returnType.rawType =  context->ensureType(
                    env, (JClass) env->GetObjectArrayElement(methodInfoArray, i + 1));
            auto&& realRetType = env->GetObjectArrayElement(methodInfoArray, i + 2);
            info.returnType.realType= realRetType== nullptr? nullptr: env->NewGlobalRef (realRetType);
            JObjectArray&& params =env->GetObjectArrayElement(methodInfoArray, i + 3);
            JObjectArray&& genericParams =env->GetObjectArrayElement(methodInfoArray, i + 4);
            int paramLen = env->GetArrayLength(params);
            Array<ParameterizedType> paramArray((uint32_t) paramLen);
            for (int j = 0; j < paramLen; ++j) {
                paramArray[j].rawType = context->ensureType(
                        env, (JClass) env->GetObjectArrayElement(params, j));
                auto&& obj = env->GetObjectArrayElement(genericParams, j);
                paramArray[j].realType = obj== nullptr? nullptr: env->NewGlobalRef(obj);
            }
            info.params = std::move(paramArray);
        }
        member.methods=std::move(methodArray);
    }
    END:
    if(member.fields.size()||member.methods.size())
    return &map.emplace(name, std::move(member)).first->second;
    else return nullptr;
}


const JavaType::MethodInfo *JavaType::findMethod(TJNIEnv* env,
        const String &name, bool isStatic,
        Vector<JavaType *> &types,
        Vector<ValidLuaObject> *arguments) {
    auto &&array = ensureMethod(env,name, isStatic);
    if (unlikely(!array)) return nullptr;
    int paramsLen = (int) types.size();
    const MethodInfo *select = nullptr;
    uint scores[paramsLen];
    uint cacheScores[paramsLen];
    memset(scores, 0, sizeof(int) * paramsLen);
    memset(cacheScores, 0, sizeof(int) * paramsLen);
    for (const MethodInfo &info:*array) {
        if (info.params.size() != paramsLen) goto bail;

        if (arguments == nullptr) {
            for (int i = paramsLen - 1; i >= 0; --i) {
                JavaType *expected = types[i];
                if (expected != nullptr && info.params[i].rawType != expected)
                    goto bail;
            }
            goto over;
        }
        for (int i = paramsLen - 1; i >= 0; --i) {
            const ValidLuaObject &luaObject = arguments->at(i);
            JavaType *toCheck = info.params[i].rawType;
            JavaType *provided = types[i];
            switch (luaObject.type) {
                case T_NIL:
                    if (toCheck->isPrimitive()) goto bail;
                    if (provided&& !env->IsAssignableFrom(
                            provided->getType(), toCheck->getType()))
                        goto bail;
                    break;
                case T_BOOLEAN:
                    if(provided&&provided->isBoxedBool())
                        goto Handle_OBJ;
                    if (toCheck ->isBool())
                        cacheScores[i]=1;
                    else if(scores[i]==1||!toCheck ->isBoxedBool())
                        goto bail;
                    else cacheScores[i]=0;
                    break;
                case T_CHAR:{
                    if(provided&&provided->isBoxedChar())
                        goto Handle_OBJ;
                    if (toCheck ->isChar())
                        cacheScores[i]=1;
                    else if(scores[i]==1||!toCheck ->isBoxedChar())
                        goto bail;
                    else cacheScores[i]=0;
                    break;
                }
                case T_STRING: {
                    if (provided == nullptr) {
                        if (strlen8to16(luaObject.string) == 1){
                            if (toCheck->isChar()) {
                                cacheScores[i] = 3;
                                continue;
                            } else if (scores[i] == 3) goto bail;
                            else if(toCheck->isBoxedChar()){
                                cacheScores[i] = 2;
                                continue;
                            }
                        }
                        if(scores[i]>1) continue;
                        if (toCheck->isString())cacheScores[i] = 1;
                        else if(!toCheck->isStringAssignable(env)){
                            goto bail;
                        } else cacheScores[i]=0;
                    } else if (provided->isString()||provided->isStringAssignable(env)) {
                        if (provided != toCheck) goto bail;
                    } else goto bail;
                    break;
                }
                case T_OBJECT: {
                    Handle_OBJ:
                    JavaType *real = provided ? provided : luaObject.objectRef->type;
                    if (toCheck->isPrimitive()){
                        if(scores[i]>5||(!real->_isBox))
                            goto bail;
                        uint score=0;
                        switch (real->typeID){
                            case BOX_BOOLEAN:
                                if(!toCheck->isBool()) goto bail;
                                continue;
                            case BOX_CHAR:
                                if(!toCheck->isChar()) goto bail;
                                continue;
                            case BOX_BYTE:
                                switch (toCheck->typeID){
                                    case BYTE:score=5;
                                        break;
                                    case SHORT:score=4;
                                        break;
                                    case INT:score=3;
                                        break;
                                    case LONG:score=2;
                                        break;
                                    case FLOAT:score=1;
                                        break;
                                    case DOUBLE:score=0;
                                        break;
                                    default:goto bail;
                                }
                                break;
                            case BOX_SHORT:
                                switch (toCheck->typeID){
                                    case SHORT:score=4;
                                        break;
                                    case INT:score=3;
                                        break;
                                    case LONG:score=2;
                                        break;
                                    case FLOAT:score=1;
                                        break;
                                    case DOUBLE:score=0;
                                        break;
                                    default:goto bail;
                                }
                                break;
                            case BOX_INT:
                                switch (toCheck->typeID){
                                    case INT:score=3;
                                        break;
                                    case LONG:score=2;
                                        break;
                                    case FLOAT:score=1;
                                        break;
                                    case DOUBLE:score=0;
                                        break;
                                    default:goto bail;
                                }
                                break;
                            case BOX_LONG:
                                switch (toCheck->typeID){
                                    case LONG:score=2;
                                        break;
                                    case FLOAT:score=1;
                                        break;
                                    case DOUBLE:score=0;
                                        break;
                                    default:goto bail;
                                }
                                break;
                            case BOX_FLOAT:
                                switch (toCheck->typeID){
                                    case FLOAT:score=1;
                                        break;
                                    case DOUBLE:score=0;
                                        break;
                                    default:goto bail;
                                }
                                break;
                            case BOX_DOUBLE:
                                if(toCheck->typeID!=DOUBLE)
                                   goto bail;
                                break;
                            default:
                                break;
                        }
                        if (score < scores[i]) goto bail;
                        else cacheScores[i] = score;
                        break;
                    }
                    if(provided&&provided->isPrimitive()) goto bail;
                    uint weight = weightObject(env,toCheck, real);
                    if (scores[i] > weight||!weight) goto bail;
                    cacheScores[i] = weight;
                    break;
                }
                case T_INTEGER: {
                    long long v = luaObject.integer;
                    INT_TYPE intType;
                    if (provided == nullptr) {
                        if (!toCheck->_isInteger && !toCheck->_isFloat&&
                            !toCheck->isObjectClass()&&!toCheck->isNumberClass()) goto bail;
                        if (v >= INT8_MIN && v <= INT8_MAX) {
                            intType = J_BYTE;
                        } else if (v >= INT16_MIN && v <= INT16_MAX) {
                            intType = J_SHORT;
                        }else if (v >= INT32_MIN && v <= INT32_MAX) {
                            intType = J_INT;
                        } else {
                            intType = J_LONG;
                        }
                    } else if (provided->isInteger()) {
                        intType= static_cast<INT_TYPE>(provided->typeID);//since they have the same order
                    }else if(provided->_isBox){
                        goto Handle_OBJ;
                    } else goto bail;
                    uint score = 0;
                    switch (intType) {
                        case J_BYTE: {
                            switch (toCheck->typeID){
                                case BYTE:score = 14;
                                    break;
                                case SHORT:score = 13;
                                    break;
                                case INT:score = provided?12:15;
                                    break;
                                case LONG:score = 11;
                                    break;
                                case FLOAT:score = 10;
                                    break;
                                case DOUBLE:score = 9;
                                    break;
                                case BOX_BYTE:score = 8;
                                    break;
                                case BOX_SHORT:score = 7;
                                    break;
                                case BOX_INT:score = 6;
                                    break;
                                case BOX_LONG:score = 5;
                                    break;
                                case BOX_FLOAT:score = 4;
                                    break;
                                case BOX_DOUBLE:score = 3;
                                    break;
                                case BOX_NUMBER: score= 2;
                                    break;
                                case BOX_OBJECT: score=1;
                                    break;
                                default:  goto bail;
                            }
                            break;
                        }
                        case J_SHORT: {
                            switch (toCheck->typeID){
                                case SHORT:score = 12;
                                    break;
                                case INT: score = provided?11:13;
                                    break;
                                case LONG:score = 10;
                                    break;
                                case FLOAT:score = 9;
                                    break;
                                case DOUBLE:score = 8;
                                    break;
                                case BOX_SHORT:score = 7;
                                    break;
                                case BOX_INT:score = 6;
                                    break;
                                case BOX_LONG:score = 5;
                                    break;
                                case BOX_FLOAT:score = 4;
                                    break;
                                case BOX_DOUBLE:score = 3;
                                    break;
                                case BOX_NUMBER: score= 2;
                                    break;
                                case BOX_OBJECT: score=1;
                                    break;
                                default:  goto bail;
                            }
                            break;
                        }
                        case J_INT: {
                            switch (toCheck->typeID){
                                case INT:score = 10;
                                    break;
                                case LONG:score = 9;
                                    break;
                                case FLOAT:score = 8;
                                    break;
                                case DOUBLE:score = 7;
                                    break;
                                case BOX_INT:score = 6;
                                    break;
                                case BOX_LONG:score = 5;
                                    break;
                                case BOX_FLOAT:score = 4;
                                    break;
                                case BOX_DOUBLE:score = 3;
                                    break;
                                case BOX_NUMBER: score= 2;
                                    break;
                                case BOX_OBJECT: score=1;
                                    break;
                                default:  goto bail;
                            }
                            break;
                        }
                        case J_LONG: {
                            switch (toCheck->typeID){
                                case LONG:score = 8;
                                    break;
                                case FLOAT:score = 7;
                                    break;
                                case DOUBLE: score = 6;
                                    break;
                                case BOX_LONG:score = 5;
                                    break;
                                case BOX_FLOAT:score = 4;
                                    break;
                                case BOX_DOUBLE:score = 3;
                                    break;
                                case BOX_NUMBER: score= 2;
                                    break;
                                case BOX_OBJECT: score=1;
                                    break;
                                default:  goto bail;
                            }
                            break;
                        }
                    }
                    if (score < scores[i]) goto bail;
                    else cacheScores[i] = score;
                    break;
                }
                case T_FLOAT: {
                    if (provided == nullptr) {
                        uint score;
                        switch (toCheck->typeID){
                            case DOUBLE:score = 6;
                                break;
                            case FLOAT: score = 5;
                                break;
                            case BOX_DOUBLE:score = 4;
                                break;
                            case BOX_FLOAT:score = 3;
                                break;
                            case BOX_NUMBER: score=2;
                                break;
                            case BOX_OBJECT: score=1;
                                break;
                            default:
                                goto bail;
                        }
                        if (score < scores[i]) goto bail;
                        else cacheScores[i] = score;
                    } else if(provided->isFloat()) {
                        if (provided->typeID == DOUBLE) {
                            if (toCheck->typeID ==DOUBLE)
                                cacheScores[i]=1;
                            else if(scores[i]==1||toCheck->typeID !=BOX_DOUBLE)
                                goto bail;
                            else cacheScores[i]=0;
                        } else {
                            uint score;
                            if (toCheck->typeID == FLOAT) score = 3;
                            else if (toCheck->typeID == DOUBLE) score = 2;
                            else score = 1;
                            if (score < scores[i]) goto bail;
                            else cacheScores[i] = score;
                        }
                    } else goto Handle_OBJ;
                    break;
                }
                case T_FUNCTION: {
                    if (provided != nullptr) {
                        if (!env->IsAssignableFrom(provided->getType(), toCheck->getType()))
                            goto bail;
                    } else {
                        if (!toCheck->isSingleInterface(env)) goto bail;
                    }
                    break;
                }
                case T_TABLE:
                    if (provided != nullptr) {
                        if (!env->IsAssignableFrom(provided->getType(), toCheck->getType()))
                            goto bail;
                    } else {
                        uint score;
                        if (toCheck->isTableType(env)) score = 2;
                        else if (toCheck->isInterface(env) && luaObject.lazyTable->isInterface()) {
                            score = 1;
                        } else score = 0;
                        if (score < scores[i]) goto bail;
                        else cacheScores[i] = score;
                    }
                    break;
                default:
                    goto bail;
            }
        }
        over:
        select = &info;
        memcpy(scores, cacheScores, paramsLen * sizeof(scores[0]));
        continue;
        bail:
        //memcpy(cacheScores, scores, paramsLen * sizeof(scores[0]));
        continue;
    }
    return select;
}

const char* JavaType::findMockName(TJNIEnv* env,const String& name, bool get){
    auto && iter=mockFields.find(name);
    if(iter==mockFields.end())
    {
        JString jname(env->NewStringUTF(name.data()));
        JObjectArray names(env->CallStaticObjectMethod(contextClass,sFindMockName,type,jname.get()));
        JString getter(env->GetObjectArrayElement(names,0));
        JString setter(env->GetObjectArrayElement(names,1));
        if(getter.get()== nullptr&&setter.get()== nullptr)
            return nullptr;
        const char* gett=getter.get()== nullptr? nullptr:getter.str();
        const char* sett=setter.get()== nullptr? nullptr:setter.str();
        if(gett){
            FakeString tmp(gett);
            ensureMember(env,tmp,false);
            gett=objectMembers.find(tmp)->first.data();
        }
        if(sett){
            FakeString tmp(sett);
            ensureMember(env,tmp,false);
            sett=objectMembers.find(tmp)->first.data();
        }
        mockFields.emplace(name,MockField{gett,sett});
        return get?gett:sett;
    } else{
        return get?iter->second.getter:iter->second.setter;
    }
}
const JavaType::FieldInfo *JavaType::findField(TJNIEnv* env,const String &name, bool isStatic, JavaType *type) {
    auto &&array = ensureField(env,name, isStatic);
    if (unlikely(array == nullptr))
        return nullptr;
    if (type == nullptr) {
        if (likely(array->size() == 1)) return &(*array)[0];
        else return nullptr;
    }
    for (const FieldInfo &info:*array) {
        if (info.type.rawType == type) return &info;
    }
    return nullptr;
}


uint JavaType::weightObject(TJNIEnv* env,JavaType *target, JavaType *from) {
    auto key = std::make_pair<>(target, from);
    auto weightMap = context->weightMap;
    context->weightLock.lock();
    auto iter = weightMap.find(key);
    context->weightLock.unlock();
    if (iter != weightMap.end()) {
        return iter->second;
    }
    uint ret = uint(env->CallStaticIntMethod(contextClass, sWeightObject, target->type, from->type));
    context->weightLock.lock();
    weightMap.emplace(key, ret);
    context->weightLock.unlock();
    return ret;
}

JObject JavaType::getSingleInterface(TJNIEnv* env) {
    if (singleInterface == invalid<jmethodID>()) {
        JObject ret = env->CallStaticObjectMethod(contextClass, sGetSingleInterface, type);
        if (ret != nullptr) singleInterface = env->FromReflectedMethod(ret);
        return ret;
    } else return env->ToReflectedMethod(type, singleInterface, JNI_FALSE);
}
