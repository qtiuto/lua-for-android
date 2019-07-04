
#ifndef LUADROID_COMMON_H
#define LUADROID_COMMON_H

#include <jni.h>
#include <time.h>
#include "lua.hpp"
#include "jtype.h"
#include "TJNIEnv.h"
#include "macros.h"
#include "HashMap.h"
extern JavaVM *vm;
extern TJNIEnv* _GCEnv;
extern jclass stringType;
extern jclass classType;
extern jclass throwableType;
extern jclass contextClass;
extern jclass loaderClass;
extern jmethodID objectHash;
extern jmethodID classGetName;
extern jmethodID objectToString;
extern jmethodID charValue;
extern jmethodID booleanValue;
extern jmethodID longValue;
extern jmethodID doubleValue;
#ifdef __cplusplus
extern "C"
#else
extern
#endif


typedef std::MString String;

template<class _Key, typename _Value,
        class _Hash=std::hash <_Key>, class _Equal=std::equal_to <_Key>>
using Map=std::HMap<_Key, _Value, _Hash, _Equal>;

template<typename _Tp>
inline void forceRelease(_Tp &t) {
    t.~_Tp();
};

template<typename _Tp, typename ...Args>
inline void forceRelease(_Tp &t, Args &&... args) {
    t.~_Tp();
    forceRelease(std::forward<Args>(args)...);
}

template<typename T>
inline T invalid() {
    return reinterpret_cast<T>(-1);
}
template<typename T>
inline T ptrTypeJudge(T*) {
    abort();
}
#define PTR_TYPE(v) decltype(ptrTypeJudge(v))
struct lua_State;
class FakeString {
    String::size_type __cap_;
    String::size_type __size_;
    const char *pointer;
public:

    FakeString(const char *s) : __cap_(1), __size_(strlen(s)), pointer(s) {
        //__cap_=(__size_&1)==1?__size_:__size_+1;
    }
    FakeString():__cap_(1){}

    FakeString(lua_State* L,int i);

    void set(lua_State* L,int i);

    String::size_type size(){
        return __size_;
    }

    operator const String &() const {
        return *reinterpret_cast<const String *>(this);
    }

    operator const char *() const {
        return pointer;
    }

    const char *data() const { return pointer; }

    const char *c_str() const { return pointer; }

    const char * * operator&() {
        return &pointer;
    }
    char operator[](String::size_type i) {
        return pointer[i];
    }
};
template <char s1,char s2>
inline void replaceAll(String& c){
    uint32_t pos = 0;
    while ((pos = c.find(s1, pos)) != String::npos) {
        c[pos] = s2;
    }
}

inline long long getTime() {
    struct timespec tv;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tv);
    return tv.tv_sec*1000000000+tv.tv_nsec;
}

#endif //LUADROID_COMMON_H
