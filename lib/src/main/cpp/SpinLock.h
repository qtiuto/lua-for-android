
#ifndef LUADROID_SPINLOCK_H
#define LUADROID_SPINLOCK_H

#include <atomic>
#include <unistd.h>
#include <mutex>


class SpinLock {
    std::atomic_flag locked = ATOMIC_FLAG_INIT;
    int count = 0;
    pthread_t holder = 0;
public:
    volatile void lock() {
        if (pthread_self() != holder) {
            while (locked.test_and_set(std::memory_order_acquire)) { ; }
            holder = pthread_self();
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

typedef std::lock_guard<SpinLock> ScopeLock;
#endif //LUADROID_SPINLOCK_H
