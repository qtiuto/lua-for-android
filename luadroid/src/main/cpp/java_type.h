
#ifndef LUADROID_JAVA_TYPE_H
#define LUADROID_JAVA_TYPE_H

#include <string>
#include <jni.h>
#include <unordered_map>
#include <cstdlib>
#include "myarray.h"
#include "lua_object.h"
#include "java_object.h"
#include "luadroid.h"
#include "jtype.h"
#include "TJNIEnv.h"

#define JAVA_TYPE "java_type"

class ScriptContext;

class JavaType {
public:
    struct FieldInfo {
        jfieldID id;
        JavaType *type;
    };
    struct MethodInfo {
        jmethodID id;
        JavaType *returnType;
        Array<JavaType *> params;
    };
private:
    enum INT_TYPE {
        J_BYTE,
        J_SHORT,
        J_INT,
        J_LONG,
        J_FLOAT,
        J_DOUBLE
    };
    typedef Array<MethodInfo> MethodArray;
    typedef Array<FieldInfo> FieldArray;
    typedef Map<String, MethodArray> MethodMap;
    typedef Map<String, FieldArray> FieldMap;
    typedef Map<String, bool> InvalidMap;

    friend class ScriptContext;

    static jmethodID sGetComponentType;
    static jmethodID sFindMembers;
    static jmethodID sWeightObject;
    static jmethodID sGetSingleInterface;
    static jmethodID sGetParameterTypes;
    static jmethodID sIsInterface;
    static jmethodID sIsTableType;
    static jmethodID sTableConvert;
    jclass type;
    bool primitive;
    bool isInteger;
    bool _isString;
    bool _isStringAssignable;
    ScriptContext *context;
    MethodMap staticMethods;
    MethodMap objectMethods;
    FieldMap staticFields;
    FieldMap objectFields;
    InvalidMap invalidFields;
    InvalidMap invalidMethods;
    jmethodID singleInterface = invalid<jmethodID>();
    JavaType *componentType = invalid<JavaType *>();

    JavaType(JNIEnv *env, jclass type, ScriptContext *context) : context(context) {
        this->type = (jclass) env->NewGlobalRef(type);
        _isStringAssignable = env->IsAssignableFrom(stringType, type);
        _isString = env->IsSameObject(type, stringType);
        primitive=false;
        isInteger=false;
    }

    inline static JClass getComponentType(TJNIEnv *env, jclass type) {
        return (JClass) env->CallObjectMethod(type, sGetComponentType);
    }

    int weightObject(TJNIEnv* env,JavaType *target, JavaType *from);

public:
    jobject newObject(TJNIEnv* env,Vector<JavaType *> &types, Vector<ValidLuaObject> &params);

    jarray newArray(TJNIEnv* env,jint size, Vector<ValidLuaObject> &params);

    jclass getType() const { return type; }

    JString name(TJNIEnv* env) { return (JString) env->CallObjectMethod(type, classGetName); }

    MethodArray *ensureMethod(TJNIEnv* env,const String &s, bool isStatic);

    FieldArray *ensureField(TJNIEnv* env,const String &s, bool isStatic);

    bool isTableType(TJNIEnv* env) {
        return env->CallBooleanMethod(context->javaRef, sIsTableType, type);
    }

    jobject convertTable(TJNIEnv* env,jobject map) {
        return env->asJNIEnv()->CallObjectMethod(context->javaRef, sTableConvert, map, type);
    }

    bool isArray(TJNIEnv* env) {
        return getComponentType(env) != NULL;
    }

    JavaType *getComponentType(TJNIEnv* env) {
        if (componentType == invalid<JavaType *>()) {
            auto cType = getComponentType(env, type);
            componentType = cType ? context->ensureType(env, cType) : nullptr;
        }
        return componentType;
    }

    bool isPrimitive() {
        return primitive;
    }

    bool isLuaInteger() {
        return isInteger;
    }

    bool isChar() {
        return this == context->charClass;
    }

    bool isBool() {
        return this == context->booleanClass;
    }

    bool isFloat() {
        return this == context->doubleClass || this == context->floatClass;
    }

    bool isVoid() {
        return this == context->voidClass;
    }

    bool isStringAssignable() {
        return _isStringAssignable;
    }

    bool isString() {
        return _isString;
    }

#define FLOAT_MAX ((1<<24))
#define FLOAT_MIN (-(1<<24))
#define DOUBLE_MAX (((long long)1<<54))
#define DOUBLE_MIN (-((long long)1<<54))

    const MethodInfo *findMethod(TJNIEnv* env,const String &name, bool isStatic, Vector<JavaType *> &types,
                                 Vector<ValidLuaObject> *arguments);

    const Array<MethodInfo> *findAllObjectMethod(TJNIEnv* env,const String &name) {
        return ensureMethod(env,name, false);
    }

    JObject getSingleInterface(TJNIEnv* env);

    bool isSingleInterface(TJNIEnv* env) {
        if (singleInterface == invalid<jmethodID>()) {
            JObject ret = env->CallStaticObjectMethod(contextClass, sGetSingleInterface, type);
            if (ret != nullptr)
                singleInterface = env->FromReflectedMethod(ret);
            else singleInterface = nullptr;
        }
        return singleInterface != nullptr;
    }

    const FieldInfo *findField(TJNIEnv* env,const String &name, bool isStatic, JavaType *type);

    int getFieldCount(TJNIEnv* env,const String &name, bool isStatic) {
        auto &&array = ensureField(env,name, isStatic);
        return array ? array->size() : 0;
    }

    const bool isInterface(TJNIEnv* env) {
        return env->CallBooleanMethod(type, sIsInterface);
    }

    ~JavaType() {
        //run in gc thread
        AutoJNIEnv()->DeleteGlobalRef(type);
    }
};

#endif //LUADROID_JAVA_TYPE_H
