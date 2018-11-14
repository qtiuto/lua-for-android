
#ifndef LUADROID_SPINLOCK_H
#define LUADROID_SPINLOCK_H

#include <atomic>
#include <unistd.h>
#include <mutex>


class ReSpinLock {
    std::atomic_flag locked = ATOMIC_FLAG_INIT;
    int count = 0;
    pthread_t holder = 0;
public:
    volatile void lock() {
      pthread_t self=  pthread_self();
        if (self != holder) {
            while (locked.test_and_set(std::memory_order_acquire)) {
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
            locked.clear(std::memory_order_release);
        }

    }
};
class SpinLock {
    std::atomic_flag locked = ATOMIC_FLAG_INIT;
public:
    volatile void lock() {
            while (locked.test_and_set(std::memory_order_acquire)) {
                struct ::timespec req;
                req.tv_sec = 0;
                req.tv_nsec = 1000;
                nanosleep(&req,NULL);
            }

    }

    volatile void unlock() {
        locked.clear(std::memory_order_release);
    }
};
typedef std::lock_guard<SpinLock> ScopeLock;
typedef  std::lock_guard<ReSpinLock> ReScopeLock;
#endif //LUADROID_SPINLOCK_H
