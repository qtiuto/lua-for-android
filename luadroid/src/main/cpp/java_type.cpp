
#include "java_type.h"
#include "utf8.h"

jmethodID JavaType::sGetComponentType = nullptr;
jmethodID JavaType::sGetParameterTypes = nullptr;
jmethodID JavaType::sFindMembers = nullptr;
jmethodID JavaType::sWeightObject = nullptr;
jmethodID JavaType::sGetSingleInterface = nullptr;
jmethodID JavaType::sIsTableType = nullptr;
jmethodID JavaType::sTableConvert = nullptr;
jmethodID JavaType::sIsInterface = nullptr;

jobject JavaType::newObject(TJNIEnv* env,Vector<JavaType *> &types, Vector<ValidLuaObject> &params) {
    const MethodInfo *info = findMethod(env,FakeString("<init>"), false, types, &params);
    uint32_t size = (uint32_t) types.size();
    jvalue argList[size];
    for (int i = 0; i < size; ++i) {
        ValidLuaObject &object = params[i];
        argList[i] = context->luaObjectToJValue(env, object, info->params[i]);
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

jarray JavaType::newArray(TJNIEnv* env,jint size, Vector<ValidLuaObject> &params) {
    uint32_t len = (uint32_t) params.size();

#define ArrayHandle(jtype, jName, NAME) \
        if(this==context->jtype##Class){\
            j##jtype arr[len];\
            for (uint32_t i = 0; i < len; ++i) {\
                arr[i]=(j##jtype)params[i].NAME;\
            }\
            j##jtype##Array ret=env->New##jName##Array(size).invalidate();\
            env->Set##jName##ArrayRegion(ret,0,len,arr);\
            return ret;\
        }
#define IntegerArrayHandle(jtype, jName) ArrayHandle(jtype,jName,integer)
#define DoubleArrayHandle(jtype, jName) ArrayHandle(jtype,jName,number)

    IntegerArrayHandle(int, Int)
    else IntegerArrayHandle(byte, Byte)
    else IntegerArrayHandle(long, Long)
    else DoubleArrayHandle(double, Double)
    else DoubleArrayHandle(float, Float)
    else IntegerArrayHandle(short, Short)
    else ArrayHandle(boolean, Boolean, isTrue)
#undef ArrayHandle
#undef IntegerArrayHandle
#undef DoubleArrayHandle
    else if (isChar()) {
        String st;
        for (uint32_t i = 0; i < len; ++i) {
            st = st + params[i].string;
        }
        char16_t *buf = strdup8to16(st.c_str(), nullptr);
        jcharArray ret = env->NewCharArray(size).invalidate();
        env->SetCharArrayRegion(ret, 0, len, (const jchar *) buf);
        free(buf);
        return ret;
    } else {
        jobjectArray ret = env->NewObjectArray(size, type, nullptr).invalidate();
        for (uint32_t i = 0; i < len; ++i) {
            ValidLuaObject &luaObject = params[i];
            if (luaObject.type == T_NIL) {
                env->SetObjectArrayElement(ret, i, NULL);
            } else {
                jobject value = context->luaObjectToJValue(env, luaObject, this).l;
                if (value == INVALID_OBJECT) return nullptr;
                env->SetObjectArrayElement(ret, i, value);
                cleanArg(env, value, luaObject.shouldRelease);
            }
        }
        return ret;
    }
    return nullptr;//unreachable statement;

}

JavaType::MethodArray *JavaType::ensureMethod(TJNIEnv* env,const String &s, bool isStatic) {
    auto &&map = isStatic ? staticMethods : objectMethods;
    auto &&iter = map.find(s);
    if (iter != map.end())
        return &iter->second;
    auto &&invalidMember = invalidMethods.find(s);
    if (invalidMember != invalidMethods.end() && invalidMember->second == isStatic)
        return nullptr;
    JString str = env->NewStringUTF(&s[0]);
    JObjectArray methodInfoArray = (JObjectArray) env->CallStaticObjectMethod(
            contextClass, sFindMembers, type, str.get(), false, isStatic);
    int count = methodInfoArray != nullptr ? env->GetArrayLength(methodInfoArray) : 0;
    if (count == 0) {
        invalidFields.emplace(s, isStatic);
        return nullptr;
    }

    bool notInit = s != "<init>";
    MethodArray methodArray((uint32_t) (count >> notInit));
    for (int i = 0; i < count; i += 1 + notInit) {
        MethodInfo &info = methodArray[i >> notInit];
        JObject method = env->GetObjectArrayElement(methodInfoArray, i);
        info.id = env->FromReflectedMethod(method);
        info.returnType = notInit ? context->ensureType(
                env, (JClass) env->GetObjectArrayElement(methodInfoArray, i + 1))
                                  : context->voidClass;
        JObjectArray params = (JObjectArray) env->CallStaticObjectMethod(
                contextClass, sGetParameterTypes, method.get());
        int paramLen = env->GetArrayLength(params);
        Array<JavaType *> paramArray((uint32_t) paramLen);
        for (int j = 0; j < paramLen; ++j) {
            paramArray[j] = context->ensureType(
                    env, (JClass) env->GetObjectArrayElement(params, j));
        }
        info.params = std::move(paramArray);
    }
    return &map.emplace(s, std::move(methodArray)).first->second;

}

JavaType::FieldArray *JavaType::ensureField(TJNIEnv* env,const String &s, bool isStatic) {
    auto &&map = isStatic ? staticFields : objectFields;
    auto &&iter = map.find(s);
    if (iter != map.end())
        return &iter->second;
    auto &&invalidMember = invalidFields.find(s);
    if (invalidMember != invalidFields.end() && invalidMember->second == isStatic)
        return nullptr;
    JString str = env->NewStringUTF(&s[0]);
    JObjectArray fieldInfos = (JObjectArray) env->CallStaticObjectMethod(contextClass, sFindMembers,
                                                                         type, str.get(), true,
                                                                         isStatic);
    int count = fieldInfos == nullptr ? 0 : env->GetArrayLength(fieldInfos);
    if (count == 0) {
        invalidFields.emplace(s, isStatic);
        return nullptr;
    }
    FieldArray fieldArray((uint32_t) (count >> 1));
    for (int i = 0; i < count; i += 2) {
        FieldInfo &info = fieldArray[i >> 1];
        JObject field = env->GetObjectArrayElement(fieldInfos, i);
        info.id = env->FromReflectedField(field);
        info.type = context->ensureType(
                env, (JClass) env->GetObjectArrayElement(fieldInfos, i + 1));
    }
    return &map.emplace(s, std::move(fieldArray)).first->second;
}

const JavaType::MethodInfo *JavaType::findMethod(TJNIEnv* env,
        const String &name, bool isStatic,
        Vector<JavaType *> &types,
        Vector<ValidLuaObject> *arguments) {
    auto &&array = ensureMethod(env,name, isStatic);
    if (!array) return nullptr;
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
                if (expected != nullptr && info.params[i] != expected)
                    goto bail;
            }
            goto over;
        }
        memcpy(cacheScores, scores, paramsLen * sizeof(scores[0]));
        for (int i = paramsLen - 1; i >= 0; --i) {
            const ValidLuaObject &luaObject = arguments->at((unsigned long) i);
            JavaType *toCheck = info.params[i];
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
                    break;
                case T_CHAR:{
                    if(provided&&provided->isBoxedChar())
                        goto Handle_OBJ;
                    if (toCheck ->isChar())
                        cacheScores[i]=1;
                    else if(scores[i]==1||!toCheck ->isBoxedChar())
                        goto bail;
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
                        else if(!toCheck->isStringAssignable()){
                            goto bail;
                        }
                    } else if (provided->isString()||provided->isStringAssignable()) {
                        if (provided != toCheck) goto bail;
                    } else goto bail;
                    break;
                }
                case T_OBJECT: {
                    Handle_OBJ:
                    JavaType *from = provided ? provided : luaObject.objectRef->type;
                    if (toCheck->isPrimitive()){
                        if(scores[i]>5||!from->_isBox)
                            goto bail;
                        if(from==context->BooleanClass){
                            if(toCheck!=context->booleanClass) goto bail;
                        } else if(from==context->CharacterClass){
                            if(toCheck->isChar()) goto bail;
                        } else if(from==context->DoubleClass){
                            if(toCheck!=context->doubleClass) goto bail;
                        }else if(from==context->LongClass){
                            if(toCheck==context->longClass)
                                cacheScores[i]=2;
                            else if(scores[i]>1)
                                goto bail;
                            else if(toCheck==context->floatClass)
                                cacheScores[i]=1;
                            else if(scores[i]>0||toCheck!=context->doubleClass)
                                goto bail;
                        }else if(from==context->IntegerClass){
                            if(toCheck==context->intClass)
                                cacheScores[i]=3;
                            else if(scores[i]>2)
                                goto bail;
                            else if(toCheck==context->longClass)
                                cacheScores[i]=2;
                            else if(scores[i]>1)
                                goto bail;
                            else if(toCheck==context->floatClass)
                                cacheScores[i]=1;
                            else if(scores[i]>0||toCheck!=context->doubleClass)
                                goto bail;
                        }else if(from==context->FloatClass){
                            if(toCheck==context->floatClass)
                                cacheScores[i]=1;
                            else if(scores[i]>0||toCheck!=context->doubleClass)
                                goto bail;
                        }else if(from==context->ShortClass){
                            if(toCheck==context->shortClass)
                                cacheScores[i]=4;
                            else if(scores[i]>3)
                                goto bail;
                            else if(toCheck==context->intClass)
                                cacheScores[i]=2;
                            else if(scores[i]>2)
                                goto bail;
                            else if(toCheck==context->longClass)
                                cacheScores[i]=2;
                            else if(scores[i]>1)
                                goto bail;
                            else if(toCheck==context->floatClass)
                                cacheScores[i]=1;
                            else if(scores[i]>0||toCheck!=context->doubleClass)
                                goto bail;
                        }else if(from==context->ByteClass){
                            if(toCheck==context->byteClass)
                                cacheScores[i]=5;
                            else if(scores[i]>4)
                                goto bail;
                            else if(toCheck==context->shortClass)
                                cacheScores[i]=4;
                            else if(scores[i]>3)
                                goto bail;
                            else if(toCheck==context->intClass)
                                cacheScores[i]=3;
                            else if(scores[i]>2)
                                goto bail;
                            else if(toCheck==context->longClass)
                                cacheScores[i]=2;
                            else if(scores[i]>1)
                                goto bail;
                            else if(toCheck==context->floatClass)
                                cacheScores[i]=1;
                            else if(scores[i]>0||toCheck!=context->doubleClass)
                                goto bail;
                        }
                        break;
                    }
                    if(provided&&provided->isPrimitive()) goto bail;
                    uint weight = weightObject(env,toCheck, from);
                    if (scores[i] > weight||!weight) goto bail;
                    cacheScores[i] = weight;
                    break;
                }
                case T_INTEGER: {
                    if (!toCheck->_isInteger && !toCheck->_isFloat) goto bail;
                    long long v = luaObject.integer;
                    INT_TYPE intType = J_INT;
                    if (provided == nullptr) {
                        if (v >= INT8_MIN && v <= INT8_MAX) {
                            intType = J_BYTE;
                        } else if (v >= INT16_MIN && v <= INT16_MAX) {
                            intType = J_SHORT;
                        }else if (v >= INT32_MIN && v <= INT32_MAX) {
                            intType = J_INT;
                        }  else {
                            intType = J_LONG;
                        }
                    } else if (provided == context->byteClass) {
                        intType = J_BYTE;
                    } else if (provided == context->shortClass) {
                        intType = J_SHORT;
                    } else if (provided == context->intClass) {
                        intType = J_INT;
                    } else if (provided == context->longClass) {
                        intType = J_LONG;
                    }else if(provided->_isBox){
                        goto Handle_OBJ;
                    }
                    uint score = 0;
                    switch (intType) {
                        case J_BYTE: {
                            if (toCheck == context->intClass) score = provided?10:13;
                            else if (toCheck == context->byteClass) score = 12;
                            else if (toCheck == context->shortClass) score = 11;
                            else if (toCheck == context->longClass) score = 9;
                            else if (toCheck == context->floatClass) score = 8;
                            else if (toCheck == context->doubleClass) score = 7;
                            else if (toCheck == context->ByteClass) score = 6;
                            else if (toCheck == context->ShortClass) score = 5;
                            else if (toCheck == context->IntegerClass) score = 4;
                            else if (toCheck == context->LongClass) score = 3;
                            else if (toCheck == context->FloatClass) score = 2;
                            else if (toCheck == context->DoubleClass) score = 1;
                            else goto bail;
                            break;
                        }
                        case J_SHORT: {
                            if (toCheck == context->intClass) score = provided?8:11;
                            else if (toCheck == context->shortClass) score = 10;
                            else if (toCheck == context->longClass) score = 9;
                            else if (toCheck == context->floatClass) score = 7;
                            else if (toCheck == context->doubleClass) score = 6;
                            else if (toCheck == context->ShortClass) score = 5;
                            else if (toCheck == context->IntegerClass) score = 4;
                            else if (toCheck == context->LongClass) score = 3;
                            else if (toCheck == context->FloatClass) score = 2;
                            else if (toCheck == context->DoubleClass) score = 1;
                            else goto bail;
                            break;
                        }
                        case J_INT: {
                            if (toCheck == context->intClass) score = 8;
                            else if (toCheck == context->longClass) score = 7;
                            else if (toCheck == context->floatClass) score = 6;
                            else if (toCheck == context->doubleClass) score = 5;
                            else if (toCheck == context->IntegerClass) score = 4;
                            else if (toCheck == context->LongClass) score = 3;
                            else if (toCheck == context->FloatClass) score = 2;
                            else if (toCheck == context->DoubleClass) score = 1;
                            else goto bail;
                            break;
                        }
                        case J_LONG: {
                            if (toCheck == context->longClass) score = 6;
                            else if (toCheck == context->floatClass) score = 5;
                            else if (toCheck == context->doubleClass) score = 4;
                            else if (toCheck == context->LongClass) score = 3;
                            else if (toCheck == context->FloatClass) score = 2;
                            else if (toCheck == context->DoubleClass) score = 1;
                            else goto bail;
                            break;
                        }
                    }
                    if (score < scores[i]) goto bail;
                    else cacheScores[i] = score;
                    break;
                }
                case T_FLOAT: {
                    if (!toCheck->isFloat()) goto bail;
                    if (provided == nullptr) {
                        uint score;
                        if (toCheck == context->doubleClass) score = 4;
                        else if (toCheck == context->floatClass) score = 3;
                        else if (toCheck == context->DoubleClass) score = 2;
                        else score = 1;
                        if (score < scores[i]) goto bail;
                        else cacheScores[i] = score;
                    } else if(!provided->_isBox) {
                        if (provided == context->doubleClass) {
                            if (toCheck ==context->doubleClass)
                                cacheScores[i]=1;
                            else if(scores[i]==1||toCheck !=context->DoubleClass)
                                goto bail;
                        } else {
                            uint score;
                            if (toCheck == context->floatClass) score = 3;
                            else if (toCheck == context->doubleClass) score = 2;
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
        continue;
    }
    return select;
}

const JavaType::FieldInfo *JavaType::findField(TJNIEnv* env,const String &name, bool isStatic, JavaType *type) {
    auto &&array = ensureField(env,name, isStatic);
    if (array == nullptr)
        return nullptr;
    if (type == nullptr) {
        if (array->size() == 1) return &(*array)[0];
        else return nullptr;
    }
    for (const FieldInfo &info:*array) {
        if (info.type == type) return &info;
    }
    return nullptr;
}


uint JavaType::weightObject(TJNIEnv* env,JavaType *target, JavaType *from) {
    auto key = std::make_pair<>(target, from);
    auto weightMap = context->weightMap;
    auto iter = weightMap.find(key);
    if (iter != weightMap.end()) {
        return iter->second;
    }
    uint ret = uint(env->CallStaticIntMethod(contextClass, sWeightObject, target->type, from->type));
    weightMap.emplace(key, ret);
    return ret;
}

JObject JavaType::getSingleInterface(TJNIEnv* env) {
    if (singleInterface == nullptr) {
        JObject ret = env->CallStaticObjectMethod(contextClass, sGetSingleInterface, type);
        if (ret != nullptr) singleInterface = env->FromReflectedMethod(ret);
        return ret;
    } else return env->ToReflectedMethod(type, singleInterface, JNI_FALSE);
}
