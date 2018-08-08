
#ifndef LUADROID_COMMON_H
#define LUADROID_COMMON_H

#include <sstream>
#include <jni.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "lua.hpp"
#include <android/log.h>
#include "jtype.h"

#define LOGE(msg, ...) __android_log_print(ANDROID_LOG_ERROR,"Lua",msg,##__VA_ARGS__)
#define LOGV(msg, ...) __android_log_print(ANDROID_LOG_VERBOSE,"Lua",msg,##__VA_ARGS__)
#define LOGI(msg, ...) __android_log_print(ANDROID_LOG_INFO,"Lua",msg,##__VA_ARGS__)
#define LOGW(msg, ...) __android_log_print(ANDROID_LOG_WARN,"Lua",msg,##__VA_ARGS__)
#if !defined(__GNUC__)
#define __thread thread_local
#endif
extern JavaVM *vm;
extern jclass stringType;
extern jclass classType;
extern jclass throwableType;
extern jclass contextClass;
extern jmethodID objectHash;
extern jmethodID classGetName;
extern jmethodID objectToString;
extern jmethodID charValue;
extern jmethodID booleanValue;
extern jmethodID longValue;
extern jmethodID doubleValue;
extern jmethodID sLength;

typedef std::string String;

template<class _Key, typename _Value,
        class _Hash=std::hash <_Key>, class _Equal=std::equal_to <_Key>>
using Map=std::unordered_map<_Key, _Value, _Hash, _Equal>;
template<typename _Tp>
using Vector=std::vector<_Tp>;

bool testType(lua_State *L, int objIndex, const char *typeName);

int reportError(lua_State *L);

inline void __doNotCall(std::stringstream &sstream) {};

template<typename T1, typename ...T2>
inline void __doNotCall(std::stringstream &sstream, T1 &&arg, T2 &&...args) {
    sstream << arg;
    __doNotCall(sstream, std::forward<T2>(args)...);
};
//#pragma clang diagnostic pop

template<typename... T2>
std::string formMessage(T2 &&... args) {
    std::stringstream sstream;
    __doNotCall(sstream, std::forward<T2>(args)...);
    return sstream.str();
}

template<typename _Tp>
inline void forceRelease(_Tp &t) {
    _Tp(std::move(t));
};

template<typename _Tp, typename ...Args>
inline void forceRelease(_Tp &t, Args &&... args) {
    _Tp(std::move(t));
    forceRelease(std::forward<Args>(args)...);
}

template<typename T>
inline T invalid() {
    return reinterpret_cast<T>(-1);
}

class FakeString {
    String::size_type __cap_;
    String::size_type __size_;
    const char *pointer;
public:
    FakeString(const char *s) : __cap_(1), __size_(strlen(s)), pointer(s) {
        //__cap_=(__size_&1)==1?__size_:__size_+1;
    }

    operator const String &() const {
        return *reinterpret_cast<const String *>(this);
    }

    operator const char *() const {
        return pointer;
    }

    const char *data() const { return pointer; }

    const char *c_str() const { return pointer; }
};

inline long long getTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

#endif //LUADROID_COMMON_H
