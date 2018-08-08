
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

jobject JavaType::newObject(Vector<JavaType *> &types, Vector<ValidLuaObject> &params) {
    const MethodInfo *info = findMethod(FakeString("<init>"), false, types, &params);
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

jarray JavaType::newArray(jint size, Vector<ValidLuaObject> &params) {
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
    else if (this == context->charClass) {
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
                cleanArg(env, value, luaObject.type);
            }
        }
        return ret;
    }
    return nullptr;//unreachable statement;

}

JavaType::MethodArray *JavaType::ensureMethod(const String &s, bool isStatic) {
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

JavaType::FieldArray *JavaType::ensureField(const String &s, bool isStatic) {
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

const JavaType::MethodInfo *JavaType::findMethod(
        const String &name, bool isStatic,
        Vector<JavaType *> &types,
        Vector<ValidLuaObject> *arguments) {
    auto &&array = ensureMethod(name, isStatic);
    if (!array) return nullptr;
    int paramsLen = (int) types.size();
    const MethodInfo *select = nullptr;
    int scores[paramsLen];
    int cacheScores[paramsLen];
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
                    if (provided != nullptr &&
                        !env->IsAssignableFrom(provided->getType(), toCheck->getType()))
                        goto bail;
                    break;
                case T_BOOLEAN:
                    if (toCheck != context->booleanClass) goto bail;
                    break;
                case T_STRING: {
                    if (provided == nullptr) {
                        if (strlen8to16(luaObject.string) == 1)
                            if (toCheck->isChar()) {
                                cacheScores[i] = 3;
                                continue;
                            }
                        if (scores[i] == 3) goto bail;
                        if (toCheck->isString())cacheScores[i] = 2;
                        else {
                            if (scores[i] == 2) goto bail;
                            if (toCheck->isStringAssignable()) cacheScores[i] = 1;
                            else goto bail;
                        }
                    } else if (provided->isChar() || provided->isString()) {
                        if (provided != toCheck) goto bail;
                    } else goto bail;
                    break;
                }
                case T_OBJECT: {
                    if (toCheck->isPrimitive()) goto bail;
                    JavaType *from = provided != nullptr ? provided : luaObject.objectRef->type;
                    int weight = weightObject(toCheck, from);
                    if (scores[i] > weight) goto bail;
                    cacheScores[i] = weight;
                    break;
                }
                case T_USER_DATA:
                case T_LIGHT_USER_DATA:
                case T_INTEGER: {
                    bool isInteger = toCheck->isLuaInteger();
                    if (!isInteger && !toCheck->isFloat()) goto bail;
                    long long v = luaObject.integer;
                    INT_TYPE intType = J_INT;
                    if (provided == nullptr) {
                        if (v >= INT8_MIN && v <= INT8_MAX) {
                            intType = J_BYTE;
                        } else if (v >= INT16_MIN && v <= INT16_MAX) {
                            intType = J_SHORT;
                        } else if (v >= FLOAT_MIN && v <= FLOAT_MAX) {
                            intType = J_FLOAT;
                        } else if (v >= INT32_MIN && v <= INT32_MAX) {
                            intType = J_INT;
                        } else if (double(v) == v) {
                            intType = J_DOUBLE;
                        } else {
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
                    }
                    int score = 0;
                    switch (intType) {
                        case J_BYTE: {
                            if (toCheck == context->intClass) score = 6;
                            else if (toCheck == context->byteClass) score = 5;
                            else if (toCheck == context->shortClass) score = 4;
                            else if (toCheck == context->longClass) score = 3;
                            else if (toCheck == context->floatClass) score = 2;
                            else if (toCheck == context->doubleClass) score = 1;
                            else goto bail;
                            break;
                        }
                        case J_SHORT: {
                            if (toCheck == context->intClass) score = 5;
                            else if (toCheck == context->shortClass) score = 4;
                            else if (toCheck == context->longClass) score = 3;
                            else if (toCheck == context->floatClass) score = 2;
                            else if (toCheck == context->doubleClass) score = 1;
                            else goto bail;
                            break;
                        }
                        case J_FLOAT: {
                            if (toCheck == context->intClass) score = 4;
                            else if (toCheck == context->longClass) score = 3;
                            else if (toCheck == context->floatClass) score = 2;
                            else if (toCheck == context->doubleClass) score = 1;
                            else goto bail;
                            break;
                        }
                        case J_INT: {
                            if (toCheck == context->intClass) score = 3;
                            else if (toCheck == context->longClass) score = 2;
                            else if (toCheck == context->doubleClass) score = 1;
                            else goto bail;
                            break;
                        }
                        case J_DOUBLE: {
                            if (toCheck == context->longClass) score = 2;
                            else if (toCheck == context->doubleClass) score = 1;
                            else goto bail;
                            break;
                        }
                        case J_LONG: {
                            if (toCheck == context->longClass) score = 1;
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
                    if (provided != nullptr) {
                        if (provided == context->doubleClass) {
                            if (toCheck != provided) goto bail;
                        } else {
                            int score;
                            if (toCheck == context->floatClass) score = 2;
                            else score = 1;
                            if (score < scores[i]) goto bail;
                            else cacheScores[i] = score;
                        }
                    } else {
                        int score;
                        if (toCheck == context->doubleClass) score = 2;
                        else score = 1;
                        if (score < scores[i]) goto bail;
                        else cacheScores[i] = score;
                    }
                    break;
                }
                case T_FUNCTION: {
                    if (provided != nullptr) {
                        if (!env->IsAssignableFrom(provided->getType(), toCheck->getType()))
                            goto bail;
                    } else {
                        if (!toCheck->isSingleInterface()) goto bail;
                    }
                    break;
                }
                case T_TABLE:
                    if (provided != nullptr) {
                        if (!env->IsAssignableFrom(provided->getType(), toCheck->getType()))
                            goto bail;
                    } else {
                        int score;
                        if (toCheck->isTableType()) score = 2;
                        else if (toCheck->isInterface() && luaObject.lazyTable->isInterface()) {
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

const JavaType::FieldInfo *JavaType::findField(const String &name, bool isStatic, JavaType *type) {
    auto &&array = ensureField(name, isStatic);
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


int JavaType::weightObject(JavaType *target, JavaType *from) {
    auto key = std::make_pair<>(target, from);
    auto weightMap = context->weightMap;
    auto iter = weightMap.find(key);
    if (iter != weightMap.end()) {
        return iter->second;
    }
    int ret = env->CallStaticIntMethod(contextClass, sWeightObject, target->type, from->type);
    weightMap.emplace(key, ret);
    return ret;
}

JObject JavaType::getSingleInterface() {
    if (singleInterface == nullptr) {
        JObject ret = env->CallStaticObjectMethod(contextClass, sGetSingleInterface, type);
        if (ret != nullptr) singleInterface = env->FromReflectedMethod(ret);
        return ret;
    } else return env->ToReflectedMethod(type, singleInterface, JNI_FALSE);
}
