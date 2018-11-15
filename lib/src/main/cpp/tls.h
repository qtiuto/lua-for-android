

#ifndef LUADROID_TLS_H
#define LUADROID_TLS_H

#include "pthread.h"

template <typename T,bool release=false>
class ThreadLocal{
    pthread_key_t key;

    typedef void(*ReleaseFunc)(void*);
    template <typename Tp,bool re>
    struct Release{
        static constexpr inline ReleaseFunc get(){
            return nullptr;
        }
    };

    template <typename Tp>
    struct Release<Tp,true>{
        static void deconstruct(void* p){
            delete (Tp*)p;
        }
        static inline ReleaseFunc get(){
            return deconstruct;
        }
    };
public:
    ThreadLocal() {
         pthread_key_create(&key, Release<T,release>::get());
    }

    void set(T* newValue){
        T* old=(T*)pthread_getspecific(key);
        if(old!= nullptr){
            delete old;
        }
        pthread_setspecific(key,newValue);
    }

    void rawSet(T* newValue){
        pthread_setspecific(key,newValue);
    }

    T *get(){
        return (T*)pthread_getspecific(key);
    }

    ~ThreadLocal(){
        pthread_key_delete(key);
    }

    ThreadLocal& operator=(T* other){
        set(other);
        return *this;
    }
    friend bool operator==(T* p1,ThreadLocal& p2){
        return p2.get()==p1;
    }
    friend bool operator!=(T* p1,ThreadLocal& p2){
        return p2.get()!=p1;
    }
    friend bool operator==(ThreadLocal& p2,T* p1){
        return p2.get()==p1;
    }
    friend bool operator!=(ThreadLocal& p2,T* p1){
        return p2.get()!=p1;
    }
    T& operator*(){
        return *get();
    }

    operator T*(){
        return get();
    }

    T* operator->(){
        return get();
    }


};
#endif //LUADROID_TLS_H
