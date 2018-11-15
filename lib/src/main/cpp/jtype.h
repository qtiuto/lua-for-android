
#ifndef LUADROID_JTYPE_H
#define LUADROID_JTYPE_H

#include <jni.h>

template<typename Tp>
class JType {
protected:
    Tp j = nullptr;
    JNIEnv *env = nullptr;
public:
    JType() {};

    template<typename Sp>
    JType(JNIEnv *env, Sp sp):JType(env, Tp(sp)) {
    }

    template<typename Sp>
    JType(JType<Sp> &&sp):JType(sp.getEnv(), Tp(sp.invalidate())) {
    }

    JType(JNIEnv *env, Tp j) : j(j), env(env) {
    }

    JType(JType &&type) : j(type.j), env(type.env) {
        type.j = nullptr;
    }

    JType(const JType &j) = delete;

    JType &operator=(JType &&type) {
        release();
        j = type.j;
        env = type.env;
        type.j = nullptr;
        return *this;
    };

    JType &operator=(const JType &j)= delete;

    operator Tp() const {
        return j;
    }

    Tp get() const {
        return j;
    }

    Tp invalidate() {
        Tp ret = j;
        j = nullptr;
        return ret;
    }

    JNIEnv *getEnv() { return env; }

    ~JType() {
        release();
    }

    void release() const {
        if (j != nullptr) {
            env->DeleteLocalRef(j);
        }
    }
};
template <typename Tp>
inline bool operator==(JType<Tp> j,Tp p){
    return j.get()==p;
}
template <typename Tp>
inline bool operator==(Tp p,JType<Tp> j){
    return j.get()==p;
}
template <typename Tp>
inline bool operator!=(JType<Tp> j,Tp p){
    return j.get()!=p;
}
template <typename Tp>
inline bool operator!=(Tp p,JType<Tp> j){
    return j.get()!=p;
}

typedef JType<jclass> JClass;
typedef JType<jobject> JObject;
typedef JType<jarray> JArray;
typedef JType<jlongArray> JLongArray;
typedef JType<jobjectArray> JObjectArray;

#define INVALID (reinterpret_cast<char *const>(-1))

template<>
class JType<jstring> {
    jstring j;
    JNIEnv *env;
    const char *chars = INVALID;
public:
    JType() : j(nullptr), chars(nullptr) {};

    JType(JObject &&sp) : JType(sp.getEnv(), jstring(sp.invalidate())) {
    }

    JType(JNIEnv *env, jstring j) : j(j), env(env) {
        if (j == nullptr) chars = nullptr;
    }

    JType(JType &&type) : j(type.j), env(type.env), chars(type.chars) {
        type.j = nullptr;
        type.chars = nullptr;
    }

    JType(const JType &j) = delete;

    JType &operator=(JType &&type) {
        release();
        j = type.j;
        env = type.env;
        chars = type.chars;
        type.j = nullptr;
        type.chars = nullptr;
        return *this;
    };

    JType &operator=(const JType &j)= delete;

    operator jstring() {
        return j;
    }

    operator const char *() {
        return str();
    }

    jstring get() {
        return j;
    }

    jstring invalidate() {
        jstring ret = j;
        if (chars != nullptr && chars != INVALID) {
            env->ReleaseStringUTFChars(j, chars);
            chars = INVALID;
        }
        j = nullptr;
        return ret;
    }

    const char *str() {
        if (chars == INVALID) {
            chars = env->GetStringUTFChars(j, NULL);
        }
        return chars;
    }

    JNIEnv *getEnv() { return env; }

    ~JType() {
        release();
    }

private:
    void release() const {
        if (chars != nullptr && chars != INVALID) {
            env->ReleaseStringUTFChars(j, chars);
        }
        if (j != nullptr) {
            env->DeleteLocalRef(j);
        }
    }
};

typedef JType<jstring> JString;
#endif //LUADROID_JTYPE_H