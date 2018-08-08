

#include "log_wrapper.h"
#include "SpinLock.h"
#include "common.h"
#include <atomic>
#include <cassert>
#include <cstdio>
#include <poll.h>
#include <time.h>
#include <thread>

static volatile int refCount = 0;
static SpinLock mutex;
static volatile bool loggerRunning = false;//avoid bug in loop
static struct pollfd fds[2]{
        {0, POLLIN, 0},
        {0, POLLIN, 0}
};

void requireLogger(std::function<void(const char *, bool)> logger) {

    ScopeLock sentry(mutex);
    if (refCount++ == 0) {
        setvbuf(stdout, 0, _IOLBF, 0);
        setvbuf(stderr, 0, _IONBF, 0);
        int stdOutFd[2];
        int stdErrFd[2];
        pipe(stdOutFd);

        pipe(stdErrFd);
        dup2(stdOutFd[1], STDOUT_FILENO);
        dup2(stdErrFd[1], STDERR_FILENO);

        close(stdOutFd[1]);
        close(stdErrFd[1]);

        fds[0].fd = stdOutFd[0];
        fds[1].fd = stdErrFd[0];
        auto loggerThread = std::thread([logger](void *) {

            JNIEnv *env;
            vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_4);
            vm->AttachCurrentThread(&env, NULL);
            char *buffer = new char[1024];
            ssize_t nBytes;
            while (refCount > 0) {
                int nfds = poll(fds, 2, 100);
                if (nfds > 0) {
                    if (fds[0].revents & (POLLIN)) {
                        nBytes = read(fds[0].fd, buffer, 1023);
                        if (nBytes < 0) {
                            break;
                        }
                        buffer[nBytes] = 0;
                        logger(buffer, false);
                    }
                    if (fds[1].revents & (POLLIN)) {
                        nBytes = read(fds[1].fd, buffer, 1023);
                        if (nBytes < 0)
                            break;
                        buffer[nBytes] = 0;
                        logger(buffer, true);
                    }
                } else if ((nfds < 0 && errno != EAGAIN)) break;
            }
            close(fds[0].fd);
            close(fds[1].fd);
            fds[0].fd = 0;
            fds[1].fd = 0;
            loggerRunning = false;
            vm->DetachCurrentThread();
            delete[] buffer;
        }, nullptr);
        loggerRunning = true;
        loggerThread.detach();
    }
}

void dropLogger() {
    ScopeLock sentry(mutex);
    if (!loggerRunning)
        return;
    --refCount;
    if (refCount == 0) {
        while (loggerRunning);
    }
}