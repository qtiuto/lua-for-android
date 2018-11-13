
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

        ParameterizedType& operator=(ParameterizedType&& o){
            this->~ParameterizedType();
            rawType=o.rawType;
            realType=o.realType;;
            o.realType= nullptr;
            return *this;
        }
        ~ParameterizedType(){
            if(realType)
                _GCEnv->DeleteGlobalRef(realType);
        }
    };
    struct FieldInfo {
        jfieldID id;
        ParameterizedType type;
        FieldInfo()= default;
        FieldInfo(FieldInfo&& other):id(other.id),type(std::move(other.type)){};
        FieldInfo& operator=(FieldInfo&& other){
            this->~FieldInfo();
            id=other.id;
            type=std::move(other.type);
            return *this;
        };
    };
    struct MethodInfo {
        jmethodID id;
        ParameterizedType returnType;
        Array<ParameterizedType> params;
        MethodInfo()= default;
        MethodInfo(MethodInfo&& other):id(other.id),returnType(std::move(other.returnType)),params(std::move(other.params)){};
        MethodInfo& operator=(MethodInfo&& other){
            this->~MethodInfo();
            id=other.id;
            returnType=std::move(other.returnType);
            params=std::move(other.params);
            return *this;
        };
    };

    struct MockField{
        const char* getter;
        const char* setter;
    };

    typedef Array<MethodInfo> MethodArray;
    typedef Array<FieldInfo> FieldArray;
    //Should I optimize for field of length 1, it seems failed
    struct Member{
        MethodArray methods;
        FieldArray fields;
        Member(){}
        Member(Member&& other):methods(std::move(other.methods)),fields(std::move(other.fields)){};
        Member& operator=(Member&& other){
            this->~Member();
            methods=std::move(other.methods);
            fields=std::move(other.fields);
            return *this;
        };
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
    typedef Map<String, Member> MemberMap;


    friend class ScriptContext;
    friend class ThreadContext;

    static jmethodID sGetComponentType;
    static jmethodID sFindMembers;
    static jmethodID sFindMockName;
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
    int8_t _isStringAssignable = -1;
    int8_t _isThrowableType=-1;
    TYPE_ID typeID=OBJECT;

    MemberMap staticMembers;
    MemberMap objectMembers;
    Map<String,MockField> mockFields;
    jmethodID singleInterface = invalid<jmethodID>();
    JavaType *componentType = invalid<JavaType *>();
    jmethodID boxMethod= nullptr;

    JavaType(JNIEnv *env, jclass type, ScriptContext *context) : context(context) {
        this->type = (jclass) env->NewGlobalRef(type);
    }

    inline static JClass getComponentType(TJNIEnv *env, jclass type) {
        return (JClass) env->CallObjectMethod(type, sGetComponentType);
    }

    uint weightObject(TJNIEnv* env,JavaType *target, JavaType *from);


public:
    jobject newObject(ThreadContext *context, Vector<JavaType *> &types, Vector<ValidLuaObject> &params);

    jarray newArray(ThreadContext *context,jint size, Vector<ValidLuaObject> &params);
    Member* ensureMember(TJNIEnv *env, const String &name, bool isStatic);

    const MethodInfo *findMethod(TJNIEnv* env,const String &name, bool isStatic, Vector<JavaType *> &types,
                                 Vector<ValidLuaObject> *arguments);
    const char* findMockName(TJNIEnv* env,const String& name, bool getter);
    const FieldInfo *findField(TJNIEnv* env,const String &name, bool isStatic, JavaType *type);
    MethodArray *ensureMethod(TJNIEnv* env,const String &s, bool isStatic){
        auto members= ensureMember(env, s, isStatic);
        if(members&&members->methods.size())
            return &members->methods;
        return nullptr;
    }

    FieldArray *ensureField(TJNIEnv* env,const String &s, bool isStatic){
        auto members= ensureMember(env, s, isStatic);
        if(members&&members->fields.size())
            return &members->fields;
        return nullptr;
    }


    jclass getType() const { return type; }

    JString name(TJNIEnv* env) { return (JString) env->CallObjectMethod(type, classGetName); }


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

    JavaType* toBoxedType(){
        switch (typeID){
            case CHAR:
                return context->CharacterClass;
            case INT:
                return context->IntegerClass;
            case LONG:
                return context->LongClass;
            case BOOLEAN:
                return context->BooleanClass;
            case FLOAT:
                return context->FloatClass;
            case DOUBLE:
                return context->DoubleClass;
            case BYTE:
                return context->ByteClass;
            case SHORT:
                return context->ShortClass;
            default:
                return nullptr;
        }
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

    bool isStringAssignable(TJNIEnv *env) {
        if(_isStringAssignable==-1)
            _isStringAssignable = env->IsAssignableFrom(stringType, type);
        return _isStringAssignable;
    }

    bool isThrowable(TJNIEnv *env) {
        if(_isThrowableType==-1)
            _isThrowableType = env->IsAssignableFrom(type, throwableType);
        return _isThrowableType;
    }

    bool isString() {
        return _isString;
    }

    inline TYPE_ID  getTypeID(){ return typeID;}

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

    const bool isInterface(TJNIEnv* env) {
        return env->CallBooleanMethod(type, sIsInterface);
    }

    ~JavaType() {
        //run in gc thread
        _GCEnv->DeleteGlobalRef(type);
    }
};

#endif //LUADROID_JAVA_TYPE_H
