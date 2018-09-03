
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


class ScriptContext;

class JavaType {
public:
    struct ParameterizedType{
        JavaType* rawType;
        jobject realType= nullptr;
        ParameterizedType(){}
        ParameterizedType(const ParameterizedType&)= delete;
        ParameterizedType(ParameterizedType&& o):rawType(o.rawType),realType(o.realType){
            o.realType= nullptr;
        }
        ~ParameterizedType(){
            if(realType)
                _GCEnv->DeleteGlobalRef(realType);
        }
    };
    struct FieldInfo {
        jfieldID id;
        ParameterizedType type;
    };
    struct MethodInfo {
        jmethodID id;
        ParameterizedType returnType;
        Array<ParameterizedType> params;
    };
    enum TYPE_ID{
        BYTE,
        SHORT,
        INT,
        LONG,
        FLOAT,
        DOUBLE,
        CHAR,
        BOOLEAN,
        VOID,
        BOX_BYTE,
        BOX_SHORT,
        BOX_INT,
        BOX_LONG,
        BOX_FLOAT,
        BOX_DOUBLE,
        BOX_CHAR,
        BOX_BOOLEAN,
        OBJECT
    };
private:
    enum INT_TYPE {
        J_BYTE,
        J_SHORT,
        J_INT,
        J_LONG,
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
    static jmethodID sIsInterface;
    static jmethodID sIsTableType;
    static jmethodID sTableConvert;
    jclass type;
    ScriptContext *context;

    bool primitive = false;
    bool _isInteger = false;
    bool _isFloat = false;
    bool _isBox = false;
    bool _isString = false;
    bool _isStringAssignable = false;
    TYPE_ID typeID=OBJECT;

    MethodMap staticMethods;
    MethodMap objectMethods;
    FieldMap staticFields;
    FieldMap objectFields;
    InvalidMap invalidFields;
    InvalidMap invalidMethods;
    jmethodID singleInterface = invalid<jmethodID>();
    JavaType *componentType = invalid<JavaType *>();
    jmethodID boxMethod= nullptr;

    JavaType(JNIEnv *env, jclass type, ScriptContext *context) : context(context) {
        this->type = (jclass) env->NewGlobalRef(type);
        _isStringAssignable = env->IsAssignableFrom(stringType, type);
    }

    inline static JClass getComponentType(TJNIEnv *env, jclass type) {
        return (JClass) env->CallObjectMethod(type, sGetComponentType);
    }

    uint weightObject(TJNIEnv* env,JavaType *target, JavaType *from);
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

    jobject convertTable(TJNIEnv* env,jobject map,jobject realType) {
        return env->asJNIEnv()->CallObjectMethod(context->javaRef, sTableConvert, map, type,realType);
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

    bool isInteger() {
        return _isInteger&&!_isBox;
    }
    bool isBoxedInteger(){
        return _isInteger&&_isBox;
    }

    bool isChar() {
        return typeID==CHAR;
    }
    bool isBoxedChar() {
        return typeID==BOX_CHAR;
    }
    bool isBool() {
        return typeID==BOOLEAN;
    }
    bool isBoxedBool() {
        return typeID==BOX_BOOLEAN;
    }

    bool canAcceptBoxedNumber(JavaType* boxed){
        if(!_isBox||isBoxedChar()||isBoxedBool()) return false;
        if(typeID==DOUBLE)
            return true;
        if(boxed->typeID==BOX_DOUBLE)
            return false;
        if(typeID==FLOAT)
            return true;
        if(boxed->typeID==BOX_FLOAT)
            return false;
        if(typeID==LONG)
            return true;
        if(boxed->typeID==BOX_LONG)
            return false;
        if(typeID==INT)
            return true;
        if(boxed->typeID==BOX_INT)
            return false;
        if(typeID==SHORT)
            return true;
        return boxed->typeID != BOX_SHORT;
    }

    jmethodID getBoxMethodForBoxType(TJNIEnv *env){
        if(!_isBox) return nullptr;
        if(boxMethod)
            return boxMethod;
        auto&& array=ensureMethod(env,FakeString("valueOf"), true);
        for (const MethodInfo& info:*array){
            if(info.params[0].rawType->isPrimitive())
                return (boxMethod=info.id,boxMethod);
        }
        return nullptr;
    }

    bool isFloat() {
        return _isFloat&&!_isBox;
    }
    bool isBoxedFloat() {
        return _isFloat&&_isBox;
    }

    bool isVoid() {
        return typeID==VOID;
    }

    bool isStringAssignable() {
        return _isStringAssignable;
    }

    bool isString() {
        return _isString;
    }

    inline TYPE_ID  getTypeID(){ return typeID;}

    inline ScriptContext* getContext(){ return context; }

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
        _GCEnv->DeleteGlobalRef(type);
    }
};

#endif //LUADROID_JAVA_TYPE_H
