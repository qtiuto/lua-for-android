
#ifndef LUADROID_SPINLOCK_H
#define LUADROID_SPINLOCK_H

#include <unistd.h>
#include "atomic.h"
#include "pthread.h"

class ReSpinLock {
    volatile at_flag locked = 0;
    int count = 0;
    pthread_t holder = 0;
public:
    volatile void lock() {
      pthread_t self=  pthread_self();
        if (self != holder) {
            while (at_flag_test_and_set(&locked)) {
                struct ::timespec req;
                req.tv_sec = 0;
                req.tv_nsec = 1000;
                nanosleep(&req,NULL);
            }
            holder = self;
        }
        ++count;
    }

    volatile void unlock() {
        if (pthread_self() != holder)
            return;
        --count;
        if (count == 0) {
            holder = 0;
            at_flag_clear(&locked);
        }

    }
};
class SpinLock {
    volatile at_flag locked = 0;
public:
    volatile void lock() {
            while (at_flag_test_and_set(&locked)) {
                struct ::timespec req;
                req.tv_sec = 0;
                req.tv_nsec = 1000;
                nanosleep(&req,NULL);
            }

    }

    volatile void unlock() {
        at_flag_clear(&locked);
    }
};
template <class mutex>
class lock_guard{
     mutex* m;
public:
    lock_guard(mutex& _m):m(&_m){
        _m.lock();
    }
    lock_guard()= delete;
    lock_guard(const lock_guard&)= delete;
    ~lock_guard(){
        m->unlock();
    }
};

typedef lock_guard<SpinLock> ScopeLock;
typedef  lock_guard<ReSpinLock> ReScopeLock;
#endif //LUADROID_SPINLOCK_H
