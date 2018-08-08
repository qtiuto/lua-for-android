
#ifndef LUADROID_TJNIENV_H
#define LUADROID_TJNIENV_H

#include "jtype.h"
#include <jni.h>

class TJNIEnv : public JNIEnv {
public:
    JNIEnv *asJNIEnv() { return this; }

    JClass FindClass(const char *name) { return JClass(this, functions->FindClass(this, name)); }

    JClass GetObjectClass(jobject obj) {
        return JClass(this, functions->GetObjectClass(this, obj));
    }

    JClass GetSuperclass(jclass clazz) {
        return JClass(this, functions->GetSuperclass(this, clazz));
    }

    JObject GetObjectField(jobject obj, jfieldID fieldID) {
        return JObject(this, functions->GetObjectField(this, obj, fieldID));
    }

    JObject GetStaticObjectField(jclass clazz, jfieldID fieldID) {
        return JObject(this, functions->GetStaticObjectField(this, clazz, fieldID));
    }

    JString NewStringUTF(const char *bytes) {
        return JString(this, functions->NewStringUTF(this, bytes));
    }

    JObject CallStaticObjectMethodV(jclass clazz, jmethodID methodID,
                                    va_list args) {
        return JObject(this, functions->CallStaticObjectMethodV(this, clazz, methodID,
                                                                args));
    }

    JObject CallStaticObjectMethodA(jclass clazz, jmethodID methodID,
                                    jvalue *args) {
        return JObject(this, functions->CallStaticObjectMethodA(this, clazz, methodID,
                                                                args));
    }

    JObject CallStaticObjectMethod(jclass clazz, jmethodID methodID,
                                   ...) {
        va_list args;
        va_start(args, methodID);
        JObject result(this, functions->CallStaticObjectMethodV(this, clazz, methodID,
                                                                args));
        va_end(args);
        return result;
    }

    JObject CallObjectMethodV(jobject object, jmethodID methodID,
                              va_list args) {
        return JObject(this, functions->CallObjectMethodV(this, object, methodID,
                                                          args));
    }

    JObject CallObjectMethod(jobject object, jmethodID methodID,
                             ...) {
        va_list args;
        va_start(args, methodID);
        JObject result(this, functions->CallObjectMethodV(this, object, methodID,
                                                          args));
        va_end(args);
        return result;
    }

    JObject CallNonvirtualObjectMethodV(jobject object, jclass clazz, jmethodID methodID,
                                        va_list args) {
        return JObject(this, functions->CallNonvirtualObjectMethod(this, object, clazz, methodID,
                                                                   args));
    }

    JObject CallNonvirtualObjectMethod(jobject object, jclass clazz, jmethodID methodID,
                                       ...) {
        va_list args;
        va_start(args, methodID);
        JObject result(this, functions->CallNonvirtualObjectMethodV(this, object, clazz, methodID,
                                                                    args));
        va_end(args);
        return result;
    }

    JObject CallNonvirtualObjectMethodA(jobject object, jclass clazz, jmethodID methodID,
                                        jvalue *params) {
        return JObject(this, functions->CallNonvirtualObjectMethodA(this, object, clazz, methodID,
                                                                    params));
    }

    JObject GetObjectArrayElement(jobjectArray array, jsize index) {
        return JObject(this, functions->GetObjectArrayElement(this, array, index));
    }

    JObjectArray NewObjectArray(jsize length, jclass elementClass,
                                jobject initialElement) {
        return JObjectArray(this, functions->NewObjectArray(this, length, elementClass,
                                                            initialElement));
    }

    JObject ToReflectedMethod(jclass cls, jmethodID methodID, jboolean isStatic) {
        return JObject(this, functions->ToReflectedMethod(this, cls, methodID, isStatic));
    }

    JObject ToReflectedField(jclass cls, jfieldID fieldID, jboolean isStatic) {
        return JObject(this, functions->ToReflectedField(this, cls, fieldID, isStatic));
    }

    JType<jbooleanArray> NewBooleanArray(jsize length) {
        return JType<jbooleanArray>(this, functions->NewBooleanArray(this, length));
    }

    JType<jbyteArray> NewByteArray(jsize length) {
        return JType<jbyteArray>(this, functions->NewByteArray(this, length));
    }

    JType<jcharArray> NewCharArray(jsize length) {
        return JType<jcharArray>(this, functions->NewCharArray(this, length));
    }

    JType<jshortArray> NewShortArray(jsize length) {
        return JType<jshortArray>(this, functions->NewShortArray(this, length));
    }

    JType<jintArray> NewIntArray(jsize length) {
        return JType<jintArray>(this, functions->NewIntArray(this, length));
    }

    JType<jlongArray> NewLongArray(jsize length) {
        return JType<jbooleanArray>(this, functions->NewLongArray(this, length));
    }

    JType<jfloatArray> NewFloatArray(jsize length) {
        return JType<jfloatArray>(this, functions->NewFloatArray(this, length));
    }

    JType<jdoubleArray> NewDoubleArray(jsize length) {
        return JType<jdoubleArray>(this, functions->NewDoubleArray(this, length));
    }
};

#endif //LUADROID_TJNIENV_H
